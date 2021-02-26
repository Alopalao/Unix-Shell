# Unix-Shell
A command line interpreter in C <br />
Commands: ">", ">>", "<" and "|" <br />
">" Redirect standard output from a command to a file. Eg. ls > 1 <br />
">>" Append standard output from a command to a file. Eg. "Hello world" >> 1 <br />
"<" Redirecct standard input to be from a file. Eg. sort < 1 <br />
"|" Pass the standard output of one command to another for further processing. Eg. ls | sort <br />
<br />
Note: The program is written in C. To run it, first write "make" in the command line, then "./myshell" <br />
