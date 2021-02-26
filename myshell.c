#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 100
#define MAXARGS 20
#define MAXCMDS 10
//Separate command into tokens which are saved in word[]
int get_words(char * command, char * word[]){
	int i = 0;
	if((word[0] = strtok(command, "\n\t ")) == NULL){
		return 0;
	}
	while((word[++i] = strtok(NULL, "\n\t ")) != NULL){
		if(i >= MAXARGS){
			printf("Too many arguments\n");
			exit(1);
		}
	}
	return i;
}

//Recognize the command "|" which passes the standard output of one command to another for further processing.
//Separate line into commands which are saved in command[]
int get_commands(char * line, char * command[]){
	int i = 0;
	command[i] = strtok(line, "\n|");
	i++;
	while((command[i] = strtok(NULL, "\n|")) != NULL){
		i++;
	}
	return i;
}
//Recognize ">", ">>", "<" and "|". Also, creates input and output files if neccessary for the UNIX commands in command
void decipher(char * command){
	FILE * input;//input file
	FILE * output;//output file
	int i = 0;//index
	char * word[MAXARGS];//each word in command
	int nwords = get_words(command, word);//number of words in command
	int outfd;//output file descriptor
	int infd;//input file descriptor
	int err = 0;//Error
	while(i < nwords){
		if(!strcmp(word[i], ">")){//Redirect standard output from a command to a file
			if((output = fopen(word[i + 1], "w+")) != NULL){
				outfd = fileno(output);
				dup2(outfd, STDOUT_FILENO);
				word[i] = 0;
				word[i + 1] = 0;
				i = 1000;
			}
			else
				err++;
		}
		else if(!strcmp(word[i], ">>")){//Append standard output from a command to a file
			if((output = fopen(word[i + 1], "a+")) != NULL){
				outfd = fileno(output);
				dup2(outfd, STDOUT_FILENO);
				word[i] = 0;
				word[i + 1] = 0;
				i = 1000;
			}
			else
				err++;
		
		}
		else if(!strcmp(word[i], "<")){//Redirect the standard input to be from a file
			if((input = fopen(word[i + 1], "r")) != NULL){
				infd = fileno(input);
				dup2(infd, STDIN_FILENO);
				word[i] = 0;
				word[i + 1] = 0;
				i = 1000;
			}
			else
				err++;
		}
		i++;
	}
	if(err == 0){//There is no errors
		execvp(word[0], word);
	}
	else{
		printf("Error while writing or reading the file\n");
		exit(1);
	}
}
//In this function the commands are piped, if neccesary
void execute(char * line){
	int i;
	int pid1;
	char * command[MAXCMDS];
	int ncommand = get_commands(line, command);
	if(ncommand  <= 0){
		return;
	}
	if(!strcmp(command[0], "exit") || !strcmp(command[0], "logout")){//Exit shell
		exit(0);
	}
	pid1 = fork();
	if(pid1 == 0){
		if(ncommand == 1){
			decipher(command[0]);
		}
		else if(ncommand == 2){//For one "|"
			int pid2;
			int fd1[2];
			pipe(fd1);
			pid2 = fork();
			if(pid2 == 0){
				dup2(fd1[1], 1);
				close(fd1[0]);
				decipher(command[0]);
			}
			else if(pid2 > 0){
				dup2(fd1[0], 0);
				close(fd1[1]);
				decipher(command[1]);
			}
			close(fd1[0]);
			close(fd1[1]);
		}
		else if(ncommand == 3){//For two "|"
			int pid3;
			int fd1[2];
			int fd2[2];
			pipe(fd1);
			pid3 = fork();
			if(pid3 == 0){
				dup2(fd1[1], 1);
				close(fd1[0]);
				close(fd1[1]);
				decipher(command[0]);
			}
			pipe(fd2);
			pid3 = fork();
			if(pid3 == 0){
				dup2(fd1[0], 0);
				dup2(fd2[1], 1);
				close(fd1[0]);
				close(fd1[1]);
				close(fd2[0]);
				close(fd2[1]);
				decipher(command[1]);
			}
			close(fd1[0]);
			close(fd1[1]);
			pid3 = fork();
			if(pid3 == 0){
				dup2(fd2[0],0);
				close(fd2[0]);
				close(fd2[1]);
				decipher(command[2]);
			}
		}
	}
	else if(pid1 > 0){//wait for the terminated children
		waitpid(pid1, NULL, 0);
	}
	else{
		exit(1);
	}
} 
int main(int arg, char * argv[]){
	char line[SIZE];
	while(1){
		printf("COP4338$ ");
		if(fgets(line, SIZE, stdin) == NULL){
			perror("fgets failed\n");
			exit(1);
		}
		execute(line);
	}
	return 0;
}
