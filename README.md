# minishell

A robust, POSIX-compliant UNIX command interpreter built in C.

Minishell is a complete recreation of core GNU Bash functionality. The project implements a deterministic command lifecycle: lexical analysis, quote-aware tokenization, dynamic environment expansion, syntax validation, multi-stage inter-process communication (IPC), stream redirection, heredoc buffering, and asynchronous signal trapping.

Developed in accordance with the rigorous 42 Network C programming standards (Norminette).

---

## Architectural Lifecycle

The shell operates as an infinite read-eval-print loop (REPL) that preserves runtime state consistency across nested pipelines, child process terminations, and builtin mutations.

```
                      [ User Input via readline() ]
                                    │
                                    ▼
┌───────────────────────────────────────────────────────────────────────┐
│ 1. LEXICAL ANALYSIS & TOKEN STREAM GENERATION (cmd1.c, cmd2.c)        │
│    ├── Single Quote ('...') Masking: Preserves raw literals           │
│    ├── Double Quote ("...") Processing: Enables \$VAR & \$? expansion  │
│    ├── Operator Classification: PIPE, INPUT, HEREDOC, WRITE, REWRITE │
│    └── Linked List Construction: Builds deterministic t_cmd stream   │
└───────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌───────────────────────────────────────────────────────────────────────┐
│ 2. SYNTAX VALIDATION & HEREDOC PRE-PROCESSING (token_control.c)       │
│    ├── Grammar Validation: Rejects consecutive pipes & unclosed ops   │
│    └── Heredoc Processing (<<): Pre-reads delimiter streams into temp │
│        pipe descriptors before command execution                      │
└───────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌───────────────────────────────────────────────────────────────────────┐
│ 3. STREAM REDIRECTION & DESCRIPTOR MAPPING (redirection.c)            │
│    ├── Input Redirection (<): dup2(fd, STDIN_FILENO)                  │
│    ├── Output Truncate (>): O_WRONLY | O_CREAT | O_TRUNC              │
│    ├── Output Append (>>): O_WRONLY | O_CREAT | O_APPEND              │
│    └── Heredoc Binding (<<): Injects buffered pipe fds into STDIN     │
└───────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌───────────────────────────────────────────────────────────────────────┐
│ 4. PIPELINE ORCHESTRATION & EXECUTION (exec.c, path.c)                │
│    ├── Builtin Dispatcher: In-process execution (cd, export, unset...)│
│    ├── Multi-Command Pipe Chaining: fork() + pipe() + dup2() matrix   │
│    ├── PATH Resolution: Scans \$PATH directories for executable bins   │
│    ├── Binary Invocation: execve() with synchronized envp             │
│    └── Exit Status Collection: waitpid() harvesting exit codes to \$?  │
└───────────────────────────────────────────────────────────────────────┘
```

---

## Core Technical Modules

### 1. Lexer & Quotation State Machine
- **Quote Masking**: Differentiates between hard quotes (`'...'`) where metacharacters are treated as literal characters, and soft quotes (`"..."`) where variable expansion is actively evaluated.
- **Dynamic Parameter Expansion**: Resolves `$VAR` references against the in-memory environment store (`t_env`) and expands `$?` to the exit code of the most recent pipeline foreground process.
- **Token Classification**: Categorizes input fragments into `enum e_token`:
  - `NONE`: Standard command names and arguments.
  - `PIPE` (`|`): Pipeline separator triggering inter-process piping.
  - `INPUT` (`<`): Input file redirection.
  - `WRITE` (`>`): Truncating output file redirection.
  - `REWRITE` (`>>`): Appending output file redirection.
  - `HEREDOC` (`<<`): Inline delimiter-based input buffer.

### 2. Concrete Data Structures (`minishell.h`)

#### Command Token Node (`t_cmd`)
```c
typedef struct s_cmd
{
    char            *value;  /* Raw string token or argument */
    int             token;  /* Token type (NONE, PIPE, INPUT, HEREDOC, WRITE, REWRITE) */
    struct s_cmd    *next;   /* Pointer to next token node */
}   t_cmd;
```

#### Environment Variable Node (`t_env`)
```c
typedef struct s_env
{
    char            *key;    /* Environment variable identifier (e.g., "PATH") */
    char            *value;  /* Environment variable value (e.g., "/usr/bin:/bin") */
    struct s_env    *next;   /* Pointer to next environment node */
}   t_env;
```

#### Master Runtime State (`t_ms`)
```c
typedef struct s_ms
{
    t_cmd   *cmd;            /* Active tokenized command stream */
    t_cmd   *main_cmd;       /* Head pointer for memory lifecycle reclamation */
    t_env   *env;            /* In-memory environment linked list */
    char    *input;          /* Raw readline buffer */
    char    *pwd;            /* Current working directory */
    char    *home;           /* User home directory */
    char    *path;           /* Resolved command binary path */
    int     **heredocs;      /* Matrix of active heredoc pipe descriptors */
    int     status;          /* Latest command exit status code ($?) */
    int     num_of_pipe;     /* Count of pipeline stages */
}   t_ms;
```

### 3. Pipeline IPC & Process Management (`exec.c`)
- **Linear Pipe Chaining**: Implements robust multi-command piping for arbitrary sequence lengths (`cmd1 | cmd2 | ... | cmdN`).
- **File Descriptor Isolation**: Properly manages pipe ends in parent and child processes to prevent file descriptor leaks, hanging processes, or broken pipe signals.
- **Process Synchronization**: Parents track all forked child PIDs and collect status flags via `waitpid()`, extracting POSIX exit codes via `WIFEXITED` and `WEXITSTATUS` macros.

### 4. Heredoc Engine (`heredoc.c`)
- Pre-processes all heredocs (`<< DELIMITER`) prior to executing any command in the pipeline.
- Captures interactive multi-line terminal input via temporary inter-process pipes.
- Supports variable expansion inside heredoc bodies unless the delimiter string is quoted.

### 5. Signal Trapping Matrix (`signal.c`)
The shell maintains strict signal state transitions across interactive and execution modes:

| Signal | Interactive Prompt | Active Child Process | Heredoc Prompt |
| :--- | :--- | :--- | :--- |
| **SIGINT (Ctrl+C)** | Clears line, outputs newline, redraws prompt (`rl_on_new_line`) | Forwards interrupt to child group; sets `$? = 130` | Aborts heredoc input loop cleanly |
| **SIGQUIT (Ctrl+\)** | Ignored (`SIG_IGN`) | Forwards quit to child; dumps core message; sets `$? = 131` | Ignored (`SIG_IGN`) |

---

## Native Builtin Implementations

Built-in commands execute directly within the parent process to allow persistent state mutations:

- **`cd [path]`**: Updates `PWD` and `OLDPWD` environment variables. Supports relative paths, absolute paths, home resolution (`cd` without arguments), and previous directory navigation (`cd -`).
- **`echo [-n / -nnn...] [args]`**: Outputs arguments separated by spaces. Supports standard and repeated `-n` flag variations to suppress trailing newlines.
- **`pwd`**: Prints the canonical absolute path of the current working directory.
- **`export [KEY=VALUE]`**: Assigns or updates environment variables. When invoked without arguments, outputs the entire environment list formatted and sorted in alphabetical order.
- **`unset [KEY...]`**: Removes key-value pairs from the in-memory environment store.
- **`env`**: Dumps all active exported environment variables.
- **`exit [code]`**: Reclaims all allocated runtime memory and terminates the shell with the specified numeric exit code, properly wrapping overflow values within `0-255`.

---

## Compilation & Build Targets

### Dependencies
- C Compiler: `gcc` or `clang`
- Standard POSIX C Library
- GNU `readline` development library (`libreadline-dev`)

### Makefile Commands

```bash
# Compile the production binary
make

# Launch interactive minishell
./minishell

# Clean recompile, run shell, and auto-cleanup on exit
make run

# Execute under Valgrind memory analyzer
make v

# Run 42 Norminette code style & syntax compliance checker
make n

# Remove object files and binary artifacts
make fclean

# Full re-compilation
make re
```

---

## Memory Discipline & Valgrind Verification

- **Zero Memory Leaks**: Every allocated token, environment node, string buffer, and file descriptor is tracked and freed upon cycle completion or syntax error (`free1.c`, `free2.c`).
- **Leak-Check Command**:
  ```bash
  valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell
  ```
- **Norminette Compliant**: Strictly follows 42 Network C coding norms (maximum 25 lines per function, 5 functions per file, explicit variable definitions, no memory leaks).

---

## License

Developed as part of the 42 Network curriculum. Released under the MIT License.
