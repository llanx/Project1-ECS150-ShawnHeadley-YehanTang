#ifndef __SSHELL_H
#define __SSHELL_H 

#define CMDLINE_MAX 512
#define CMD_ARGS_MAX 16
#define TOKEN_CHARS_MAX 32

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Command Functions
int execute_command(char *);
void parse_command(char *** command_array, char *, int *, int *, char **);
int launch_command(char *** command_array, int, char *);

// Builtin Commands
int command_pwd();
int command_cd(char *** command_array, int);

#endif