#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <tchar.h>

#include "execute_function.h"
#include "process.h"

int kill_process(DWORD pid)
    {
	    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        TerminateProcess(hProcess, 1);
        return 0;
    }

int main()
{
    kill_process(13464);
    return 0;
}