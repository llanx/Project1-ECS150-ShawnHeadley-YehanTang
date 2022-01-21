CC=gcc
CFLAGS=-I. -Wall -Wextra -Werror

sshell: sshell.o 
	$(CC) -o sshell sshell.o

clean:
	rm -rf sshell sshell.o