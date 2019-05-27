#pragma once

#include "read_input.h"
#include "basic_commands.h"
#include "process.h"
// #include "handler.h"
#include "path_works.h"

int execute(char **);
void shell_loop();

//Commands name
char const *builtin_str[] = {
    "help",
    "cd",
    "dir",
    "date",
    "time",
    "ps",
    "enva",
    "exit"};

//number of builtin commands
int num_builtins = sizeof(builtin_str) / sizeof(char *);

//functions name
int (*builtin_func[])(char **) = {
    &help,
    &cd,
    &dir,
    &get_date,
    &get_time,
    &process_works,
    &environment_variable_works,
    &shell_exit};

/*compare length of user arguments to specific length
* option 1: >=
* option 0: <
*/

//execute user's commands
int execute(char **args)
{
    int i;
    for (i = 0; i < num_builtins; i++)
    {
        if (!strcmp(args[0], builtin_str[i]))
        {
            return (*builtin_func[i])(args);
        }
    }

    printf("Command not found");
    return 0;
}

void shell_loop()
{
    // SetConsoleCtrlHandler(CtrlHandler, TRUE); // ctrl C handler
    signal(SIGINT, sigintHandler); 
    while (1)
    {
        char current_dir[MAX_DIR_LEN];
        GetCurrentDirectory(MAX_DIR_LEN, current_dir);
        printf("\n%s> ", current_dir);
        
        char *input = readline();
        char **args = split_line(input);
        if (args[0] == NULL) continue; // input is 'ENTER'
        // std::cout << combine_line(args);
        int status = execute(args);
        // get_child_processes("main.exe");

        if (status == -1)
        {
            break;
        }
        free(*args);
    }
}


