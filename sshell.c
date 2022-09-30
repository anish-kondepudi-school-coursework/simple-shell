#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "command.h"

#define CMDLINE_MAX 512

int main(void)
{
        char command_input[CMDLINE_MAX];

        while (1) 
        {
                char *nl;
                int retval;

                /* Print prompt */
                printf("sshell@ucd$ ");
                fflush(stdout);

                /* Get command line */
                fgets(command_input, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) 
                {
                        printf("%s", command_input);
                        fflush(stdout);
                }

                /* Replace trailing newline from command line with null terminator */
                nl = strchr(command_input, '\n');
                if (nl) 
                {
                        *nl = '\0';
                }

                /* Exit Condition */
                if (!strcmp(command_input, "exit")) 
                {
                        fprintf(stderr, "Bye...\n");
                        break;
                }

                if (!strcmp(command_input, "pwd")) 
                {
                        char cwd[512];
                        getcwd(cwd, sizeof(cwd));
                        printf("'%s'\n", cwd );
                }
                
                // Parses command_input to create Command object
                Command command = { .cmd = NULL, .args = NULL };
                populate_command(&command, command_input);


                if (!strcmp(command.cmd, "cd"))
                {

                        if (fork() == 0) 
                        {
                                //child process
                                char *argv[3] = {command.cmd, *command.args, NULL };
                                execvp(command.cmd, argv);
                                perror("execvp");
                                exit(1);
                        } 
                        else
                        {
                                // parent process
                                wait(&retval);
                                if (WIFEXITED(retval))
                                {
                                        chdir(command.args[0]);
                                        fprintf(stderr, "+ completed '%s' [%d]\n",
                                        command_input, retval);
                                }
                                else
                                {
                                        fprintf(stderr, "Child did not terminate with exit\n");
                                }

                        }

                }
                else
                {
                        // Complete Child Process First
                        if (fork() == 0) 
                        {
                                // child process
                                char *argv[3] = {command.cmd, *command.args, NULL };

                                execvp(command.cmd, argv);
                                perror("execvp");
                                exit(1);
                        }       
                        else
                        {
                                // parent process
                                wait(&retval);
                                if (WIFEXITED(retval))
                                {
                                        fprintf(stderr, "+ completed '%s' [%d]\n",
                                        command_input, retval);
                                }
                                else
                                {
                                        fprintf(stderr, "Child did not terminate with exit\n");
                                }

                        }      
                }

        }

        return EXIT_SUCCESS;
}