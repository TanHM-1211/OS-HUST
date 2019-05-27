#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <sys/types.h>
#include <psapi.h>
#include <tlhelp32.h>
// #include <processthreadsapi.h>
#include <string>
#include <tchar.h>
#include <signal.h>

#include "basic_commands.h"
#include "read_input.h"

void sigintHandler(int );
int process_works(char **);
int create_process(char **, int);
DWORD find_process_id(const char *);
int get_all_processes();
int get_child_processes(DWORD );
int suspend_process(DWORD );
int resume_process(DWORD );
int kill_process(DWORD );
int get_thread_list(DWORD );


HANDLE fore = NULL; 

/* Ctrl C handler
*/
void sigintHandler(int sig_num) 
{ 
	if (fore != NULL) 
    {
        TerminateProcess(fore, 1);
        fore = NULL;
    }
	// printf("\nKeyboardInterrupt");
	std::cin.clear();
	// fflush(stdout);
    signal(SIGINT, sigintHandler);  
}

int process_works(char** args)
{
	if (len(args, 2, 0))
    {
        printf("Too few arguments!");
        return EXIT_FAILURE;
    }
	else if (!strcmp(args[1], "-all"))
	{
		if (len(args, 3, 0)) 
		{
			return get_all_processes();
		}
		printf("Unknown option!");
		return EXIT_FAILURE;
	}
	else if (len(args, 3, 0))
	{
		printf("Too few arguments!");
		return EXIT_FAILURE;
	}
	else if (!strcmp(args[1], "-b"))
	{
		char* path = combine_line(args, 3);
		strcpy(args[1], path);
		return create_process(args, 0);
	}
	else if (!strcmp(args[1], "-f"))
	{
		char* path = combine_line(args, 3);
		strcpy(args[1], path);
		return create_process(args, INFINITE);
	}
	else if (len(args, 4, 0))
	{
		if (!strcmp(args[1], "-id"))
		{
			return find_process_id(args[2]);
		}
		else
		{
			int id = atoi(args[2]);
			if (strcmp(args[2], "0") && !id) // process id is not a number
			{
				printf("Failed!");
				return EXIT_FAILURE;
			}
			if (!strcmp(args[1], "-child"))
			{
				return get_child_processes(id);
			}
			else if (!strcmp(args[1], "-suspend"))
			{
				return suspend_process(id);
			}
			else if (!strcmp(args[1], "-resume"))
			{
				return resume_process(id);
			}
			else if (!strcmp(args[1], "-kill"))
			{
				return kill_process(id);
			}
			else if (!strcmp(args[1], "-thread"))
			{
				return get_thread_list(id);
			}
		}
	}
	else 
	{
		printf("Unknown option!");
		return EXIT_FAILURE;
	}
	// return EXIT_SUCCESS ;

}

int create_process(char **args, int wait_time)
{
    // int wait_time = INFINITE ;
    // if (len(args, 2, 0))
    // {
    //     printf("Too few arguments!");
    //     return EXIT_FAILURE;
    // }
    // else if (len(args, 4, 1))
    // {
    //     printf("Invalid command!");
    //     return EXIT_FAILURE;
    // }
    // else if (!strcmp(args[1], "-b")) { // run task in background mode
    //     strcpy(args[1], args[2]);
    //     wait_time = 0;
    // }
    STARTUPINFO si;
	PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.wShowWindow = SW_SHOW;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.lpTitle = args[1];
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process.
    if (!CreateProcess(NULL,               // No module name (use command line)
                       args[1],            // Command line
                       NULL,               // Process handle not inheritable
                       NULL,               // Thread handle not inheritable
                       FALSE,              // Set handle inheritance to FALSE
                       CREATE_NEW_CONSOLE, // Create new console
                       NULL,               // Use parent's environment block
                       NULL,               // Use parent's starting directory
                       &si,                // Pointer to STARTUPINFO structure
                       &pi)                // Pointer to PROCESS_INFORMATION structure
    )
    {
        int error = GetLastError();
        printf("CreateProcess failed (%d).\n", error);
        if (error == 2)
            printf("\"%s\" not found.\n", args[1]);
        return EXIT_FAILURE;
    }

    // display process in new window
    // HWND console_name = FindWindow(NULL, NULL);
    // if (console_name)
    // {
    //     ShowWindow(console_name, SW_SHOW);
    // }

	if (wait_time != 0) 
	{
		fore = pi.hProcess;
	}
    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, wait_time);
    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return EXIT_SUCCESS;
}

DWORD find_process_id(const char *process_name)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    DWORD result = NULL;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hProcessSnap) return(EXIT_FAILURE);

    pe32.dwSize = sizeof(PROCESSENTRY32); // <----- IMPORTANT

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);          // clean the snapshot object
        printf("!!! Failed to gather information on system processes! \n");
        return(NULL);
    }
	printf("%-50s%-20s%-20s\n", "Process Name", "Process ID", "Parent Process ID");
	printf("%-50s%-20s%-20s\n", "----------------------------------", "----------", "-----------");
    do
    {
        // printf("Checking process %ls\n", pe32.szExeFile);
        if (0 == strcmp(process_name, pe32.szExeFile))
        {
            result = pe32.th32ProcessID;
			printf("%-50s%-20d%-20d\n", pe32.szExeFile, pe32.th32ProcessID, pe32.th32ParentProcessID);
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    return result;
}

int get_all_processes()
{
	HANDLE h_process_snap;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	h_process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (h_process_snap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot Fail %d\n", GetLastError());
		return EXIT_FAILURE;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(h_process_snap, &pe32))
	{
		printf("Process32First Fail %d\n", GetLastError()); // show cause of failure
		CloseHandle(h_process_snap);          // clean the snapshot object
		return EXIT_FAILURE;
	}
	// Retrieve the priority class.
	printf("%-50s%-20s%-20s\n", "Process Name", "Process ID", "Parent Process ID");
	printf("%-50s%-20s%-20s\n", "----------------------------------", "----------", "-----------");
	do
	{
		printf("%-50s%-20d%-20d\n", pe32.szExeFile, pe32.th32ProcessID, pe32.th32ParentProcessID);
	} while (Process32Next(h_process_snap, &pe32));
	CloseHandle(h_process_snap);
	return(EXIT_SUCCESS);
}

int get_child_processes(DWORD pid)
{
    // DWORD pid = find_process_id(process_name) ;
	HANDLE h_process_snap;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	h_process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (h_process_snap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot Fail %d\n", GetLastError());
		return EXIT_FAILURE;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(h_process_snap, &pe32))
	{
		printf("Process32First Fail %d\n", GetLastError()); // show cause of failure
		CloseHandle(h_process_snap);          // clean the snapshot object
		return EXIT_FAILURE;
	}
	// Retrieve the priority class.
	printf("%-50s%-20s%-20s\n", "Process Name", "Process ID", "Parent Process ID");
	printf("%-50s%-20s%-20s\n", "----------------------------------", "----------", "-----------");
	do
	{
		if (pe32.th32ParentProcessID == pid)
			printf("%-50s%-20d%-20d\n", pe32.szExeFile, pe32.th32ProcessID, pe32.th32ParentProcessID);
	} while (Process32Next(h_process_snap, &pe32));
	CloseHandle(h_process_snap);
	return(EXIT_SUCCESS);
}

int suspend_process(DWORD pid)
{
	// Take a snapshot of all threads in the process.
	// DWORD pid = find_process_id(process_name) ;
	HANDLE h_thread_snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	THREADENTRY32 th32;
	HANDLE h_thread;
	if (h_thread_snap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot Fail %d\n", GetLastError());
		return EXIT_FAILURE;
	}
	// Set the size of the structure before using it.
	th32.dwSize = sizeof(THREADENTRY32);
	// Retrieve information about the first thread,
	if (!Thread32First(h_thread_snap, &th32))
	{
		printf("Thread32First Fail %d\n", GetLastError()); // show cause of failure
		CloseHandle(h_thread_snap);          // clean the snapshot object
		return EXIT_FAILURE;
	}
	// Walk other threads
	do
	{
		if (th32.th32OwnerProcessID == pid)
		{
			h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
			if (SuspendThread(h_thread) == -1)
			{
				return EXIT_FAILURE;
			}
		}
	} while (Thread32Next(h_thread_snap, &th32));
	CloseHandle(h_thread_snap);
	return EXIT_SUCCESS;
}

int resume_process(DWORD pid)
{
	// Take a snapshot of all threads in the process.
	// DWORD pid = find_process_id(process_name) ;
	HANDLE h_thread_snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	THREADENTRY32 th32;
	HANDLE h_thread;
	if (h_thread_snap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot Fail %d\n", GetLastError());
		return EXIT_FAILURE;
	}
	// Set the size of the structure before using it.
	th32.dwSize = sizeof(THREADENTRY32);
	// Retrieve information about the first thread,
	if (!Thread32First(h_thread_snap, &th32))
	{
		printf("Thread32First Fail %d\n", GetLastError()); // show cause of failure
		CloseHandle(h_thread_snap);          // clean the snapshot object
		return EXIT_FAILURE;
	}
	// Walk other threads
	do
	{
		if (th32.th32OwnerProcessID == pid)
		{
			h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
			if (ResumeThread(h_thread) == -1)
			{
				return EXIT_FAILURE;
			}
		}
	} while (Thread32Next(h_thread_snap, &th32));
	CloseHandle(h_thread_snap);
	return EXIT_SUCCESS;
}

// // for terminate process
// void UpdatePrivilege(void)
// {
//     HANDLE hToken;
//     TOKEN_PRIVILEGES tp;
//     LUID luid;

//     if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken))
//     {
//        LookupPrivilegeValue(NULL,SE_DEBUG_NAME, &luid);

//        tp.PrivilegeCount = 1;
//        tp.Privileges[0].Luid = luid;
//        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

//        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
//     }
// }

int kill_process(DWORD pid)
{
	// UpdatePrivilege();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL)
	{
		printf("Failed with error code: %d", GetLastError());
		return EXIT_FAILURE;
	}
	if (!TerminateProcess(hProcess, 0))
	{
		printf("Failed with error code: %d", GetLastError());
		return EXIT_FAILURE ;
	}
	return EXIT_SUCCESS;
}

int get_thread_list(DWORD pid)
{
	// Take a snapshot of all threads in the process.
	HANDLE h_thread_snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	THREADENTRY32 th32;
	if (h_thread_snap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot Fail %d\n", GetLastError());
		return EXIT_FAILURE;
	}
	// Set the size of the structure before using it.
	th32.dwSize = sizeof(THREADENTRY32);
	// Retrieve information about the first thread,
	if (!Thread32First(h_thread_snap, &th32))
	{
		printf("Thread32First Fail %d\n", GetLastError()); // show cause of failure
		CloseHandle(h_thread_snap);          // clean the snapshot object
		return EXIT_FAILURE;
	}
	// Walk other threads
	printf("%-50s%-20s\n", "Thread ID", "OwnerProcessID");
	printf("%-50s%-20s\n", "----------------------------------", "----------");
	do
	{
		if (th32.th32OwnerProcessID == pid)
			printf("%-50d%-20d\n", th32.th32ThreadID, th32.th32OwnerProcessID);
	} while (Thread32Next(h_thread_snap, &th32));
	CloseHandle(h_thread_snap);
	return EXIT_SUCCESS;
}