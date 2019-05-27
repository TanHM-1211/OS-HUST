#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <tchar.h>

#define MAX_BUFFER_SIZE 128
#define MAX_TOKENS 10
#define SPLIT_CHARS " \t"
#define MAX_DIR_LEN 1024

char *readline();
char **split_line(char *);
char *combine_line(char **);

char *readline()
{
    //read a line from stdin
    char *input = (char *)malloc(MAX_BUFFER_SIZE * sizeof(char));
    int pos = 0;

    if (input == NULL)
    {
        printf("Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if (pos > MAX_BUFFER_SIZE)
        {
            printf("Too long argument !\n");
            exit(EXIT_FAILURE);
        }
        char c = getchar();

        if (c == EOF || c == '\n')
        {
            input[pos] = '\0';
            break;
        }
        else
        {
            input[pos] = c;
        }
        pos++;
    }
    // fflush(stdin);
    // cin.getline(input,MAX_BUFFER_SIZE);

    return input;
}

char **split_line(char *input)
{
    int pos = 0;
    char **tokens = (char **)malloc(MAX_TOKENS * sizeof(char *));
    char *token;

    token = strtok(input, SPLIT_CHARS);
    while (token != NULL)
    {
        tokens[pos] = token;
        pos++;

        if (pos >= MAX_TOKENS)
        {
            printf("Too many arguments !\n");
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, SPLIT_CHARS);
    }

    tokens[pos] = NULL;
    return tokens;
}

char *combine_line(char **args, int pos)
{
    char *path = (char *)calloc(125, sizeof(char));
    char *pivot = (char *)" ";

    // Append argv[] and space char to path

    int i = pos;
    path = strcat(path, args[i-1]);
    while (args[i] != NULL)
    {
        path = strcat(path, pivot);
        path = strcat(path, args[i]);
        ++i;
    }

    return path;
}