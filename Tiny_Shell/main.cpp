/*******************************************************
 * @author: Dang Lam San ---- Hoang Minh Tan 
 * @name of Program: Tiny Shell
 * @KSTN CNTT K62
 *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <tchar.h>

#include "execute_command.h"

int main()
{
    printf("%-20s***** Tiny Shell *****\n"," ");
    printf("Type \"help\" for more information\n");
    
    shell_loop();

    // printf("Press 'ENTER' key to exit : ...\n");
    // getchar();
    return EXIT_SUCCESS;
}

