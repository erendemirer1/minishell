# minishell

A lightweight, POSIX-compliant UNIX command interpreter built in C.

This project recreates the fundamental architecture of GNU Bash, covering lexical analysis, AST tokenization, environment state management, multi-stage process pipelines, file descriptor redirection, and asynchronous signal handling.

---

## Architecture Overview

Minishell operates as an event loop that reads, parses, transforms, and executes commands while maintaining runtime state consistency across child processes and builtins.

```
                  [ User Input (readline) ]
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│ 1. LEXICAL ANALYSIS & TOKEN GENERATION                      │
│    ├── Quote state machine (single & double quotes)         │
│    ├── Dynamic variable expansion ($VAR, $?)                │
│    └── Linked-list token stream (t_cmd)                     │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. SYNTAX VALIDATION & PRE-EXECUTION                        │
│    ├── Grammar check (consecutive pipes, unclosed tokens)   │
│    └── Heredoc pre-buffering (<<) via pipe file descriptors │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. PIPELINE & EXECUTION ENGINE                              │
│    ├── Builtin dispatcher (cd, echo, pwd, export, unset...) │
│    ├── Fork / Pipe / Dup2 process chaining                  │
│    └── PATH lookup and external binary execution (execve)   │
└─────────────────────────────────────────────────────────────┘
```

---

## Core Components

### 1. Lexer & Parsing Pipeline (`cmd1.c`, `cmd2.c`, `token_control.c`)
- Converts raw input into a singly linked list (`t_cmd`), categorizing tokens into arguments (`NONE`) and control operators (`PIPE`, `INPUT`, `HEREDOC`, `WRITE`, `REWRITE`).
- Handles quote masking rules: preserves literals inside single quotes (`'...'`) and enables variable expansion within double quotes (`"..."`).
- Expands `$VAR` and exit status `$?` dynamically before grammar evaluation.

### 2. Environment Store (`env1.c`, `env2.c`, `env3.c`)
- Maintains an in-memory key-value linked list (`t_env`) synchronized with the host environment.
- Mutated dynamically at runtime by `export`, `unset`, and directory navigation (`cd` updating `PWD` and `OLDPWD`).

### 3. Process Chaining & IPC (`exec.c`, `redirection.c`)
- Implements linear command chaining across arbitrary pipe lengths (`cmd1 | cmd2 | ... | cmdN`).
- Manages bidirectional file descriptor manipulation using `dup2()`, isolating standard streams and resolving input/output redirections (`<`, `>`, `>>`, `<<`).
- Collects child termination statuses via `waitpid()`, mapping standard UNIX exit codes to `$?`.

### 4. Asynchronous Signal Handling (`signal.c`)
- Configures non-interactive and interactive signal handlers for `SIGINT` (Ctrl+C) and `SIGQUIT` (Ctrl+\).
- Prevents prompt corruption during blocking child process execution and active heredoc inputs.

### 5. Built-in Commands
Implements native in-process command execution without spawning child processes:
- `echo` (with multi-flag support: `-n`, `-nnn`)
- `cd` (relative, absolute, home, and oldpwd navigation)
- `pwd` (current working directory resolution)
- `export` (alphabetically sorted environment dump or variable assignment)
- `unset` (variable removal)
- `env` (environment inspection)
- `exit` (clean state termination with custom numeric exit codes)

---

## Compilation & Usage

### Prerequisites
- GCC / Clang
- GNU Make
- `libreadline` development headers

### Build Targets

```bash
# Compile standard executable
make

# Run interactive shell
./minishell

# Compile and run with automatic cleanup
make run

# Run with Valgrind memory leak verification
make v

# Run 42 Norminette compliance check
make n

# Clean build artifacts
make fclean
```

---

## Memory Lifecycle & Rigor

Minishell is engineered with strict dynamic memory management practices:
- Heap allocations for token streams, temporary environment clones, and execution buffers are explicitly reclaimed on command completion and syntax errors (`free1.c`, `free2.c`).
- Zero memory leaks across interactive sessions and child termination, verified through Valgrind instrumentation.
- Fully compliant with 42 Network C coding standards (Norminette).

---

## License

Developed as part of the 42 Network curriculum. Released under the MIT License.
