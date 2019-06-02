#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <time.h>

#include "read_input.h"

int help(char **);
int cd(char **);
int dir(char **);
int get_time(char **);
int get_date(char **);
int shell_exit(char **);

/* Compare len(args) with len, use 2 option: 1 for >= and 0 for <
*/
int len(char **args, int len, int option)
{
    int i;
    for (i = 0; i < MAX_TOKENS; i++)
    {
        if (args[i] == NULL)
            break;
    }
    if (option)
    {
        if (i >= len)
            return 1;
        else
            return 0;
    }
    else
    {
        if (i < len)
            return 1;
        else
            return 0;
    }
}

//show builtin commands
int help(char **args)
{
    if (len(args, 3, 1))
    {
        printf("Too many arguments!");
        return EXIT_FAILURE;
    }
    else if (len(args, 2,0)) 
    {
        printf("Type \"help -[command]\" for more information about a specific command.\nEXAMPLE: help ps\n");
        printf("\nSuppoted commands:\n cd\n date\n time\n dir\n ps\n enva\n exit\n");
        printf("\nUsage:\n <command> -[option]\nEXAMPLE: ps -all\n");
    }
    else if (!strcmp(args[1], "-cd"))
    {
        // printf("Commands:\n");
        printf("Change the current directory. You must write the new directory after this command.\nEXAMPLE: \"cd C:/\"\n");
        printf("This command does not support any options.\n");
    }
    else if (!strcmp(args[1], "-time"))
    {
        printf("Print current time to screen.\n");
        printf("This command does not support any options.\n");
    }
    else if (!strcmp(args[1], "-date"))
    {
        printf("Print current date to screen.\n");
        printf("This command does not support any options.\n");
    }
    else if (!strcmp(args[1], "-dir"))
    {
        printf("Show list of files or folders in current folder or specific folder (write path after this command).\n");
        printf("EXAMPLES: \"dir\", \"dir C:/\"\n");
    }
    else if (!strcmp(args[1], "-ps"))
    {
        printf("Supported options:\n");
        printf("%-20s%s", " all", "Show list of all running processes.\n");
        printf("%-20s%s", " b", "Run a program in background mode (by path to program).\n");
        printf("%-20s%s", " f", "Run a program in foregound mode (by path to program).\n");
        printf("%-20s%s", " id", "Get pid of specific program(s) (by name).\n");
        printf("%-20s%s", " child", "Show list of child processes of a process with specific pid.\n");
        printf("%-20s%s", " suspend", "Suspend a program (by pid).\n");
        printf("%-20s%s", " resume", "Resume a program (by pid).\n");
        printf("%-20s%s", " kill", "Terminate a program (by pid).\n");
        printf("%-20s%s", " thread", "Get all child processes id of a specific process(by pid).\n");
    }
    else if (!strcmp(args[1], "enva"))
    {
        printf("Supported options:\n");
        printf("%-20s%s", " all", "Show list of all environment variables.\n");
        printf("%-20s%s", " get", "Get value of a specific environment variable.\n");
        printf("%-20s%s", " set", "Set value to a specific environment variable.\n");
    }
    else if (!strcmp(args[1], "exit"))
    {
        printf("Exit shell.\n");
        printf("This command does not support any options.\n");
    }
    
    else
    {
        printf("Command not found!");
        return EXIT_FAILURE;
    }
    

    // printf("Usage:\n <command> [option]\n\n");
    // printf("Commands:\n");
    // printf("%-30s%s\n%-30s%s", " cd",
    //        "Change the current directory. You must write the new directory after this command.",
    //        " ", "EXAMPLE: \"cd C:/\"\n");
    // printf("%-30s%s\n%-30s%s", " dir",
    //        "Show list of files or folders in current folder or specific folder (write path after this command)",
    //        " ", "EXAMPLES: \"dir\", \"dir C:/\"\n");
    // printf("%-30s%s\n%-30s%s\n%-30s%s", " run",
    //        "Create new foreground process.", " ",
    //        "You must enter name of file (if file is in current directory) or enter full path to file after this command.",
    //        " ", "EXAMPLE: \"run test.exe\"\n");
    // printf("%-30s%s", " exit", "Exit this tiny shell :(\n");

    return EXIT_SUCCESS;
}

//change directory
int cd(char **args)
{
    char *path = combine_line(args, 2);
    if (!SetCurrentDirectory(path))
    {
        //append path to the end of current directory
        int i, pos;
        char current_dir[MAX_DIR_LEN];
        GetCurrentDirectory(MAX_DIR_LEN, current_dir);

        for (pos = 0; pos < MAX_DIR_LEN; pos++)
        {
            if (current_dir[pos] == '\0')
                break;
        }

        for (i = pos; i < MAX_DIR_LEN; i++)
        {
            current_dir[i] = path[i - pos];
        }

        current_dir[i] = '\0';
        if (!SetCurrentDirectory(current_dir))
        {
            printf("Can't change directory !");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

//show list of file in current or specific directory
int dir(char **args)
{
    char *dir;
    dir = (char *)malloc(MAX_DIR_LEN * sizeof(char));
    //current directory
    if (len(args, 2, 0))
    {
        GetCurrentDirectory(MAX_DIR_LEN, dir);
    }
    else if (len(args, 3, 0))
    {
        strcpy(dir, args[1]);
    }
    else
    {
        printf("Invalid command!");
        return EXIT_FAILURE;
    }

    const char add[] = "//*";
    dir = strcat(dir, add);
    WIN32_FIND_DATA data;
    HANDLE han = FindFirstFile(dir, &data);
    if (han != INVALID_HANDLE_VALUE)
    {
        do
        {
            printf("%s\n", data.cFileName);
        } while (FindNextFileA(han, &data) != 0);
        FindClose(han);
        return EXIT_SUCCESS;
    }
    else
        return EXIT_FAILURE;
}

int get_date(char **args)
{
    if (args[1] != NULL)
    {
        std::cout << "Command \"date\" does not support any option !";
        return EXIT_FAILURE;
    }
    time_t t = time(0);
    struct tm *now = localtime(&t);
    std::cout << "Current Date: " << now->tm_mday << '/'
              << (now->tm_mon + 1) << '/'
              << (now->tm_year + 1900)<<"\n";
    return EXIT_SUCCESS;
    ;
}

int get_time(char **args)
{
    if (args[1] != NULL)
    {
        std::cout << "Command \"time\" does not support any option !";
        return EXIT_FAILURE;
    }
    time_t t = time(0);
    struct tm *now = localtime(&t);
    std::cout << "Current time: " << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec<<"\n";
    return EXIT_SUCCESS;
}

int shell_exit(char **args)
{
    if (len(args, 2, 1))
    {
        printf("Command \"%s\" does not support any option !\n", args[0]);
        return (EXIT_FAILURE);
    }

    return -1;
}