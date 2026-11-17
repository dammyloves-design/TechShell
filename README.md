
TechShell

TechShell is a simple command line shell implemented in C.
Techshell accepts user commands, executes programs supports fuile input and output redirection.

Features
    - can perform basic unix commands like pwd, ls, and echo etc.
    - supports built-in commands like cd, history and exit.
    - Utilises system calls fork(), execvp(), waitpid() to create and manage processes.
    - Displays the current working directory with $
    - Exit: exit terminates the shell
    - Output redirection (>) - redirects command output into a file
    - Append Redirection(>>) - appends command output into a  file
    - Input Redirection (<) - reads command inpout froma file
    - Supports combined redirection - supports both input and output redirection in the same command
    - Error Handling: Displays errors 

Possible Future Implementation
    - Navigting the history uisng the up and down arrows.
    - save command history between shell sessions

Requirements
    - Ubuntu
    - gcc compiler
    - Git
To Clone:
    git clone ...

To Compile and Run:
    gcc techshell.c -o techshell
    ./techshell
