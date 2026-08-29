# minishell

This project is a small shell implementation built around core POSIX behavior.  
The goal is not only to execute commands, but to manage lexer/parsing, process flow, redirections, heredoc, and environment mutations in a consistent execution pipeline.

## Scope

This minishell covers:

- Interactive command line (`readline` + history)
- Token generation and command representation on a linked list (`t_cmd`)
- Multi-process execution with pipe chains (`|`)
- Redirection operators:
  - `<`
  - `>`
  - `>>`
  - `<<` (heredoc)
- Environment variable expansion (`$VAR`, `$?`)
- Builtin commands:
  - `cd`
  - `pwd`
  - `echo`
  - `env`
  - `export`
  - `unset`
  - `exit`
- `PATH` resolution and external command execution with `execve`
- `SIGINT` / `SIGQUIT` signal handling

## Build and Run

```bash
make
./minishell
```

Shortcuts:

```bash
make run    # re + run + fclean
make v      # run with valgrind
make n      # run norminette checks
```

> Note: Build depends on the `readline` library.

## Architecture Summary

### 1) Input and Prompt

On each loop iteration in `minishell.c`:

1. Collects current working directory and HOME information
2. Builds the prompt string
3. Reads user input through `readline`
4. Pushes non-empty inputs to history

### 2) Parsing Layer

The command line is transformed into a `t_cmd` linked list via `create_cmd`:

- Regular words are stored as `token = NONE`
- Operators are stored as separate nodes (`PIPE`, `INPUT`, `HEREDOC`, `WRITE`, `REWRITE`)
- Quote handling and `$` expansion are processed during parsing

This structure simplifies token consumption during redirection and execution.

### 3) Syntax Checks + Heredoc Preparation

Before execution, there are two critical steps:

- Detect invalid token sequences (e.g. consecutive `|` or missing operands)
- Pre-read heredoc blocks and bind them to pipe fds

This ensures input sources are prepared before actual command execution begins.

### 4) Redirection Application

In `redirection.c`, the command list is traversed to:

- Open target files/fds
- Apply required `dup2` mappings
- Remove consumed redirection token nodes from the list

After that, only executable command arguments remain.

### 5) Execution Model

In `exec.c`:

- If piping exists, processes are chained with `fork + pipe + dup2`
- Builtins and external commands are separated
- External command paths are resolved and executed via `execve`
- Parent process collects exit status with `waitpid`

The `$?` value is updated from this status.

## Data Structures

### `t_cmd`
Singly linked list for tokenized command flow.

### `t_env`
Key/value list for environment variables; actively mutated by builtins such as `export`, `unset`, and `cd`.

### `t_ms`
Carries the full runtime state of the shell:

- active command list
- environment list
- heredoc fds
- latest status code
- temporary parsing buffers

## Behavior Notes

- `cd` updates `PWD` and `OLDPWD`
- `echo` supports repeated `-n` variants (`-n`, `-nnn`, ...)
- `export` without arguments prints environment entries in sorted form
- Commands not found in PATH return the expected error status

## Technical Focus

This repo targets much more than “read input and run command.”  
Its core focus is modeling shell behavior as a deterministic and manageable state machine inside a compact implementation.
