// Name : Ayomide Oyekunle


TechShell

TechShell is a simple command line shell implemented in C.
Techshell accepts user commands, executes programs supports full input and output redirection.

Features
    - can perform basic unix commands like pwd, ls, and echo etc.
    - supports built-in commands like cd, history and exit.
    - Utilises system calls fork(), execvp(), waitpid() to create and manage processes.
    - Displays the current working directory with $
    - Exit: exit terminates the shell
    - Output redirection (>) - redirects command output into a file
    - Append Redirection(>>) - appends command output into a  file
    - Input Redirection (<) - reads command input from a file
    - Supports combined redirection - supports both input and output redirection in the same command
    - Error Handling: Displays errors 


Requirements
    - Ubuntu
    - gcc compiler
    - Git
To Clone:
    git clone https://github.com/dammyloves-design/TechShell

To Compile and Run:
    gcc techshell.c -o techshell
    ./techshell

Testing examples:
    pwd
    ls
    ls -l
    echo Hello World!
    cd ..
    pwd
    cat input.txt
    echo Hello > output.txt
    cat output.txt
    sort < input.txt > sorted.txt
    ls
    history
