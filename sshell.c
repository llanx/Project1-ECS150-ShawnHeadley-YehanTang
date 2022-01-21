
#include "sshell.h"


int main(void) {
        char cmd[CMDLINE_MAX];

        while (1) {
                char *nl;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl) {
                        *nl = '\0';
                }

                /* Builtin 'exit' command */
                if (strcmp(cmd, "exit") == 0) {
                        fprintf(stderr, "Bye...\n");
                        fprintf(stderr, "+ completed '%s' [0]\n", cmd);
                        break;
                }

                /* Execute command */
                execute_command(cmd);
        }

        return EXIT_SUCCESS;
}

int execute_command(char *command) {
        char **command_array;
        char *original_command;
        char *output_file;

        int ret_val = 0;
        int num_args = 0;
        int has_redirect = 0;

	// Check if input is empty
        if (strcmp(command, "") == 0) {
                return 0;
        }

        // Duplicate original command for output.
        original_command = (char *)malloc(sizeof(char) * (strlen(command) + 1));
        strcpy(original_command, command);

        // Parse the incoming command
        parse_command(&command_array, command, &num_args, &has_redirect, &output_file);

        // Validation
        if (num_args > CMD_ARGS_MAX) {
                printf("Error: too many process arguments\n");
                return -1;
        }

        if (has_redirect == 1 && output_file == NULL) {
                printf("Error: no output file\n");
                return -1;
        }

        if (has_redirect == 1 && access(output_file, F_OK) == 0 && access(output_file, W_OK) != 0) {
                printf("Error: cannot open output file\n");
                return -1;
        }

        // Execute Command
        if (strcmp(original_command, "pwd") == 0) {
                ret_val = command_pwd();
        } else if (strcmp(command_array[0], "cd") == 0) {
                ret_val = command_cd(&command_array, num_args);
        } else {
                ret_val = launch_command(&command_array, num_args, output_file);
        }

        // STDERR output
        fprintf(stderr, "+ completed '%s' [%d]\n", original_command, ret_val);

        // Free Memory and Return
        free(command_array);
        free(original_command);

        return ret_val;
}


void parse_command(char ***command_array, char *command, int *num_args, int *has_redirect, char **output_file) {
        int num_spaces = 0;
        char *token = strtok(command, " ");
        *command_array = NULL;
 
        // Handle Empty Commands
        if (token == NULL) {
                *num_args = 0;
                return;
        }

        // Tokenize command into command_array (Delimiting: " ")
        *has_redirect = 0;

        while (token) {

                // Look for redirect operator (and output file on subsequent call)
                if (strcmp(token, ">") == 0) {
                        *has_redirect = 1;
                        token = strtok(NULL, " ");
                } else if (*has_redirect == 1) {
                        *output_file = token;
                        break;
                } else {
                        *command_array = realloc(*command_array, sizeof(char *) * ++num_spaces);
            
                        if (*command_array == NULL) {
                                perror("FATAL: Memory Allocation Failed!");
                                exit(1);
                        }

                        (*command_array)[num_spaces - 1] = token;

                        token = strtok(NULL, " ");
                }
        }

        // Allocate and Insert NULL Character
        *command_array = realloc(*command_array, sizeof(char *) * (num_spaces + 1));
        (*command_array)[num_spaces] = 0;

        // Set number of arguments
        *num_args = num_spaces;
}


int launch_command(char ***command_array, int num_args, char *output_file) {
        pid_t pid;
        int status = 0;

        if (num_args != 0) {
  
                // Create Fork
                if ((pid = fork()) < 0) {
                        perror("FATAL: Failed to create fork()!");
                        exit(1);

                // Child Process
                } else if (pid == 0) {
                        // Setup Output Redirection
                        int fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);

                        dup2(fd, 1);   // Send STDOUT to FD
                        close(fd);     // Close Duped FD

                        // Launch process in Child
                        execvp((*command_array)[0], (*command_array));

                        // Kill process if it isn't a known command.
                        printf("Error: command not found\n");
                        free((*command_array));
                        exit(1);
      
                // Parent Process
                } else {
                        if ((pid = waitpid(pid, &status, 0)) < 0) {
                                perror("FATAL: Failed to wait for launched process!");
                                exit(1);
                        }
                }
        }

        return status;
}

int command_pwd() {
        char *curr_dir = (char *)malloc(1024);

        if (getcwd(curr_dir, 1024) != NULL) {
                fprintf(stdout, "%s\n", curr_dir);
        }

        return 0;
}

int command_cd(char ***command_array, int num_args) {
        int ret_val = 0;

        switch(num_args) {
                case 1:
                        return command_pwd();
                case 2:
                        ret_val = chdir((*command_array)[1]);

                        if (ret_val < 0) {
                                printf("Error: cannot cd into directory\n");
                        }
        }

        return ret_val;
}
