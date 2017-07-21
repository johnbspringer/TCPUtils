#include <stdlib.h>
#include <stdio.h>

class _BatchFileProc
{
	int GBatchFileLen=0;
	bool BatchFileMode=false;
	int BatchCmdIndex = 0;
	FILE *pBatchFile = (FILE*)NULL;
	char *GBatchCommands; // Buffer for batch file data
	int BatchBufLen = 0;
public:
	_BatchFileProc()
	{
		GBatchFileLen = 0;
		BatchFileMode = false;
	}
	long ProcessBatchFile(char *BatchFileName);
	int GetBatchedCommands(FILE* pFile, char *buf, long len);
	inline bool  GetBatchFileMode(void) { return BatchFileMode; }
	inline void SetBatchFileMode(bool OnOff) { BatchFileMode = OnOff; }
	void GetStringFromBatchBuffer(char *UserInput);

};

