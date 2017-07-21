// CmdHistory.cpp : Defines the entry point for the console application.
//

#include <string.h> // for strcpy etc
#include <stdio.h>  // for fflush
#include <iostream>
#include "CmdHistory.h"

#define NUL_VALUE 0


// len is the size of the buffer pointed to by UserInput minus 1
// for a null terminator
void _CmdHistory::addCmdToHistory(const char *UserInput,int len)
{
	strncpy(CmdHist[CmdHistIndex], UserInput, len-1);
	CmdHist[CmdHistIndex][len] = 0;
	CmdHistIndex++;
}


//  Using !! with an index number retreives the command 
//  assigned to that index value and executes it.
// size = size of buffer pointed to by cmdOut
int _CmdHistory::process_Not_Not_cmd(const char *UserInput, char *cmdOut, int size)
{
	if ((*(UserInput + 2)) != 0) // !! with an index
	{
		// See if what follows is a number
		if (isdigit(*(UserInput + 2)) != 0)
		{
			// get the command & execute 
			HistRetrieveID = atoi((const char *)(UserInput + 2));
			strncpy((char *)cmdOut, CmdHist[HistRetrieveID], size - 1);
			*(cmdOut + size) = 0; // null termainte
			std::cout << cmdOut << std::endl;
			HistRetrieveID = -1; // No history has been requested
			return strlen(cmdOut);
		}
		else return -1;
	}
	else // just !! without an index
	{
		for (int c = CmdHistIndex; c >= 0; c--)
			fprintf(stdout, "[%d] %s\n", c, CmdHist[c]);

		return 0;
	}
	return -1;
}


//  Using ! without an index number executes the last command
//  Using ! with an index number retreives the command 
//  assigned to that index value but does not execute
// size = size of buffer pointed to by cmdOut
int _CmdHistory::process_Not_cmd(const char *UserInput, char *cmdOut, int size)
{
	if ((*(UserInput + 1)) != 0)
	{
		// See if what follows is a number
		if (isdigit(*(UserInput + 1)) != 0)
		{
			// get the command & wait for user to press enter
			HistRetrieveID = atoi((const char *)(UserInput + 1));

			strncpy((char *)cmdOut, CmdHist[HistRetrieveID], size - 1);
			*(cmdOut + size) = 0; // null termainte
			HistRetrieveID = -1; // No history has been requested
			return strlen(cmdOut);
		}
		else std::cerr << "Argument to \'!\' must be an index value" << std::endl;
	}
	else // Execute last command
	{
		if (CmdHistIndex)
			HistRetrieveID = CmdHistIndex;
		return 0;
	}
	return -1;
}
