#pragma once

#include <windows.h>
#include <vector>

#include "read_input.h"

int get_all_env_vars();
int get_specific_env_var(char **);
int set_env_var(char **);
int environment_variable_works(char **);

// Get Full Environment Variable
int get_all_env_vars()
{
	const char *str = (char *)GetEnvironmentStrings();
	if (str == NULL)
	{
		printf("Error get all environment variable\n");
		return EXIT_FAILURE;

	}
	int prev = 0;
	std::vector<std::string> env_strings;
	for (int i = 0;; i++)
	{
		if (str[i] == '\0')
		{
			env_strings.push_back(std::string(str + prev, str + i));
			prev = i;
			if (str[i + 1] == '\0')
			{
				break;
			}
		}
	}
	for (unsigned int i = 0; i < env_strings.size(); i++)
	{
		std::cout << env_strings[i] << "\n";
	}
	return EXIT_SUCCESS;
}

// Get a specific environment variable
int get_specific_env_var(char **args)
{
	if (args[2] == NULL)
	{
		printf("To few argument\n");
		return EXIT_FAILURE;
	}
	if (args[3] != NULL)
	{
		printf("enva -get comment doesn't suppport any argument\n");
		return EXIT_FAILURE;
	}
	char *value = (char *)calloc(10000, sizeof(char));
	DWORD byte = 10000 * sizeof(char);
	if (GetEnvironmentVariableA(args[2], value, byte) == 0)
	{
		printf("Don't find any '%s' variable\n", args[2]);
	}
	else
	{
		printf("%s = %s\n", args[2], value);
	}
	delete value;
	return EXIT_SUCCESS;
}

/* Set a specific environment variable */
int set_env_var(char **args)
{
	if (args[3] == NULL)
	{
		printf("To few arguments\n");
		return EXIT_FAILURE;
	}
	char *new_value = combine_line(args, 3);
	// get evironment var
	char *value = (char *)calloc(10000, sizeof(char));
	char extension[] = ";";
	DWORD byte = 10000 * sizeof(char);
	// if variable doesn't exist, make a new variable. Else then add to existance value
	if (GetEnvironmentVariableA(args[2], value, byte) == 0)
	{
		if (!SetEnvironmentVariableA(args[2], new_value))
		{
			printf("Set environment variable ERROR\n");
				return EXIT_FAILURE;
		}
	}
	else
	{
		value = strcat(value, new_value);
		value = strcat(value, extension);
		if (!SetEnvironmentVariableA(args[2], value))
		{
			printf("Set environment variable ERROR\n");
			return EXIT_FAILURE;
		}
	}
	// reset value

	delete value;
	return EXIT_SUCCESS;


}

int environment_variable_works(char **args)
{
	if (args[1] == NULL)
	{
		printf("To few arguments.\n");
		return EXIT_FAILURE;
	}
	if (strcmp(args[1], "-all") == 0)
	{
		if (args[2] != NULL)
		{
			printf("Too much arguments\n");
			return EXIT_FAILURE;
		}
		else
		{
			get_all_env_vars();
			return EXIT_SUCCESS;
		}
	}
	else if (strcmp(args[1], "-get") == 0)
	{
		get_specific_env_var(args);
		return EXIT_SUCCESS;
	}
	else if (strcmp(args[1], "-set") == 0)
	{
		set_env_var(args);
		return EXIT_SUCCESS;
	}
	else
	{
		printf("enva comment doesn't support '%s' option.\n", args[1]);
		return EXIT_FAILURE;
	}
}