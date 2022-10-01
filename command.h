#ifndef COMMAND_H
#define COMMAND_H

typedef struct Command 
{
    char* cmd;
    char** args;
    int args_len;
} Command;

Command* create_command(const char* command_string);

#endif