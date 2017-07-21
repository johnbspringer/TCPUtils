#include <iostream>
#include "BatchFiles.h"

long _BatchFileProc::ProcessBatchFile(char *BatchFileName)
{
//	long BatchBufLen = -1;

	std::cout << "Attempting to Open file " << BatchFileName << std::endl;
	pBatchFile = fopen(BatchFileName, "r");

	if (pBatchFile != (FILE *)0)
	{
		fseek(pBatchFile, 0, SEEK_END);
		GBatchFileLen = ftell(pBatchFile);
		fseek(pBatchFile, 0, SEEK_SET);
		GBatchCommands = (char*)malloc(GBatchFileLen);
		BatchBufLen = GetBatchedCommands(pBatchFile, GBatchCommands, GBatchFileLen);
		fclose(pBatchFile);
		pBatchFile = (FILE*)0;
	}
	else std::cout << "Could not open file: '" << BatchFileName << "' " << strerror(errno) << std::endl;

	if (BatchBufLen>0)
		BatchFileMode = true;
	return BatchBufLen;
}


// pFile is the pointer to the opened batch file
// Bud is a ptr to allocated memory to hold the returned cmds
// Size is the length of buf in bytes
int _BatchFileProc::GetBatchedCommands(FILE* pFile, char *buf, long len)
{
	return fread(buf, sizeof(char), len, pFile);
}

// Returns the string pointed to by BatchCommands plus offset BatchCmdIndex 
// The string is returned to UserInput.
// The function will return a value equal to BatchBufLen if it determines that
// BatchCmdIndex is or will be greater than BatchBufLen.
void _BatchFileProc::GetStringFromBatchBuffer( char *UserInput)
{
	long u = 0;
	int i = 0;


	// Get off of the current CR/LF
	if ((*(GBatchCommands + BatchCmdIndex)) == '\n')
		i++;

	// Get the sentence length which is the distance to the next CR/LF
	while ((*(GBatchCommands + BatchCmdIndex + u + i)) != '\n')
	{
		if ((u + i) >= (BatchBufLen - BatchCmdIndex))
			break;
		u++;
	}
	// Copy from BatchCommands, beginning at BatchCmdIndex+i
	// which should be immediately beyond the first CR/LF
	// From this location we want to copy in the lenght of the
	// string of interest which should be 'u' length long.
	// This is the string the needs to be copied into UserInput
	strncpy((char*)UserInput, GBatchCommands + BatchCmdIndex + i, u);
	*(UserInput + u) = 0;
	BatchCmdIndex += (u + i);
}
