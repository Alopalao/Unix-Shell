# Unix-Shell
A command line interpreter in C
Commands: ">", ">>", "<" and "|"
">" Redirect standard output from a command to a file. Eg. ls > 1 \n
">>" Append standard output from a command to a file. Eg. "Hello world" >> 1
"<" Redirecct standard input to be from a file. Eg. sort < 1
"|" Pass the standard output of one command to another for further processing. Eg. ls | sort

Note: The program is written in C. To run it, first write "make" in the command line, then "./myshell"
