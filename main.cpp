#include <iostream>
#include "CmdProc.h"
#include "BatchFiles.h"


///////////////////////// STUBS ////////////////////

void Write(char*Buf)
{
	std::cout << Buf << std::endl;
}

void showMenu(void)
{
}

void Help(char* str)
{
	std::cout << "Help on "<< str << std::endl;
}

struct _Triggers
{
}Triggers;

void ShowAllTriggers(_Triggers *Triggers, int nTrigs)
{
}

void ClearTrigger(_Triggers *Triggers, int nTrigs)
{
}

void ClearAllTriggers(_Triggers *Triggers)
{
}

void ParseTrigCmdsFromCmdLine(char *UserInput
	, char *param4   // Pattern
	, char *param5   // Response
	, char *param6   // Ret/Fwd Addr
	, char *param7)  // Port
{
	printf("Pattern: %s Response: %s Addr: %s Ports %s\n", UserInput, param4, param5, param6, param7);
}

void SetTrigger(_Triggers* Triggers, int TrigID, char *param4, char *param5, char * param6, char *param7)
{
	printf("TriggerID: %d with pattern: %s Response: %s Addr: %s Ports %s\n", TrigID, param4, param5, param6, param7);
}

//////////////////////////////////////////////////////////////
struct  _MyAppDefs
{
	static const int SPACE = 1;
};

class _MyApp
{
	int GSelectedChannel=0;
public:
	char Buf[_cmdProcessorDefs::PARAM_BUF_SIZE];
	_cmdProcessor mCmdProc;
	_BatchFileProc mBatchFile;
	_MyApp()
	{
		memset(Buf, 0, sizeof(Buf));
	}
	void SetSelectedChannel(int chan) { GSelectedChannel = chan; }
	int GetSelectedChannel(void) { return GSelectedChannel; }
	unsigned char Cmd[_cmdProcessorDefs::PARAM_BUF_SIZE];
	void processCmd(char *UserInput, int size);
	void checkCmdHistory(char *UserInput, int size);
	void BatchCmd(char *UserInput);
	void DoHelp(char *UserInput);
	void DoClear(char *UserInput);
	void DoSet(char *UserInput);
	void DoSetChannel(char *UserInput, int CmdLen);
	void DoTrigger(char *UserInput);
};


void _MyApp::checkCmdHistory(char *UserInput, int size)
{
	char buf[256];
	char cmdOut[256];

	buf[0] = cmdOut[0] = (char)0;

	if (mCmdProc.GetCmdHistIndex() > mCmdProc.MAX_COMMANDS)
		mCmdProc.SetCmdHistIndex(0);

	//  Using !! without an index number shows history
	if ((strcmp("history", (const char *)UserInput) == 0) ||
		(strcmp("!!",      (const char *)UserInput) == 0))
	{
		if (mCmdProc.process_Not_Not_cmd(UserInput, UserInput, size) >= 0)
		{
			std::cout << cmdOut; // Send the daa to the user prompt
			std::cin.getline(buf, 256);// Allow the user to edit or accept as is
			strncat((char*)UserInput, (char *)buf, size - 1);
			*(UserInput + strlen(UserInput)) = 0; // null terminate
		}
		else
			std::cerr << "Argument to \'!!\' must be an index value" << std::endl;
	}
	else if (strncmp("!", (const char *)UserInput, 1) == 0)
	{
		mCmdProc.process_Not_cmd(UserInput, UserInput, size);
	}
	mCmdProc.addCmdToHistory(UserInput, size);
	printf(">"); fflush(stdout);
}


void _MyApp::DoClear(char *UserInput)
{
	char param1[64];
	char param2[64];

	int i = strlen((char*)Cmd);

	mCmdProc.ExtractParamFromInput(1, (unsigned char*)&UserInput[i + _MyAppDefs::SPACE], ' ');  // trigger
	mCmdProc.ExtractParamFromInput(2, (unsigned char*)&UserInput[i + _MyAppDefs::SPACE + mCmdProc.ParamStrLen(1) + _MyAppDefs::SPACE], ' ');  // ID

	mCmdProc.GetParam(1, param1, sizeof(param1));
	mCmdProc.GetParam(2, param2, sizeof(param2));

	if ((mCmdProc.ParamStrLen(1) == 0) || (mCmdProc.ParamStrLen(2) == 0))
	{
		//Param1[0] = Param2[0] = Param3[0] = Param4[0] = Param5[0] = 0;
		std::cerr << "Missing parameters:" << std::endl;
		Help((char*)Cmd);
		//goto START;
	}
	else if ((strncmp(param1, "trigger", 7)) == 0)// TRIGGER
	{
		// ALL
		if ((strncmp(param2, "all", 3)) == 0)
			ClearAllTriggers(&Triggers);       // TRIGGER_ID
		else
		{
			ClearTrigger(&Triggers, atoi(param2));
		}
	}
	//printf(">"); fflush(stdout);
}


// size = size of buffer pointed to by UserInput
void _MyApp::processCmd(char *UserInput, int size)
{
	int bufLen;

	checkCmdHistory(UserInput, size);
	bufLen = strlen((char*)UserInput);

	if (bufLen>0)
	{
		mCmdProc.extractCmd(UserInput, bufLen, (char*)Cmd);

		if (strncmp((char*)Cmd, "@", 1) == 0)
			BatchCmd(UserInput);
		else if (strncmp((char*)Cmd, "?", 1) == 0)
			showMenu();
		else if (strncmp((char*)Cmd, "menu", 4) == 0)
			showMenu();
		else if (strncmp((char*)Cmd, "help", 4) == 0)
			DoHelp(UserInput);
		else if ((strncmp((char*)Cmd, "triggers", 8)) == 0)// TRIGGER
		{
			//ShowAllTriggers(&Triggers, MAX_TRIGGERS);
			//printf(">"); fflush(stdout);
		}
		else if (strncmp((char*)Cmd, "trigger", 7) == 0)
			DoHelp((char*)"trigger");
		else if (strncmp((char*)Cmd, "display", 4) == 0)
			DoHelp((char*)"display");
		else if (strncmp((char*)Cmd, "log", 4) == 0)
			DoHelp((char*)"log");
		else if (strncmp((char*)Cmd, "clear", 5) == 0)
			DoClear(UserInput);
		else if (strncmp((char*)Cmd, "set", 3) == 0)
			DoSet(UserInput);
	}
}


void _MyApp::DoSet(char *UserInput)
{
	int i = strlen((char*)Cmd);
	char param1[64];

	// Get the parameter to help
	mCmdProc.ExtractParamFromInput(1, (unsigned char*)&UserInput[i + _MyAppDefs::SPACE], ' ');
	mCmdProc.GetParam(1, param1, sizeof(param1) - 1);

	// See if there are paramters to process, if not go back to START
	if ((param1[0] == (unsigned char)0))
	{
		std::cerr << "Missing parameters:" << std::endl;
		Help((char*)Cmd);
	    return;
	}
	else
	{
		if ((strncmp(param1, "channel", 7)) == 0)
			DoSetChannel(UserInput, i);
		else if ((strncmp(param1, "trigger", 7)) == 0)
			DoTrigger(UserInput);
	}
}


void _MyApp::DoTrigger(char *UserInput)
{
    char param2[64];
	char param3[64];
	char param4[64];
	char param5[64];
	char param6[64];
	char param7[64];
	int i = strlen((char*)Cmd);
	int TrigID;
	int index;


	// Get the trigger ID from the 2nd parameter
	index = i + _MyAppDefs::SPACE + mCmdProc.ParamStrLen(1) + _MyAppDefs::SPACE;
	mCmdProc.ExtractParamFromInput(2, (unsigned char*)&UserInput[index], ' ');
	mCmdProc.GetParam(2, param2, sizeof(param2) - 1);
	TrigID = atoi((char*)param2);

	// Get the trigger ID from the 3rd parameter
	index = i + _MyAppDefs::SPACE + strlen((char*)param2) + _MyAppDefs::SPACE + strlen((char*)param3) + _MyAppDefs::SPACE;
	mCmdProc.ExtractParamFromInput(3, (unsigned char*)&UserInput[index], ' ');
	mCmdProc.GetParam(3, param3, sizeof(param3) - 1);

	if (param3[0] == 0) // If user did not provide all required info, prompt for more
	{
		if (!mBatchFile.GetBatchFileMode())   // Prompt for data since there is no batch file
		{
			// Promt the user for the trigger assert pattern
			printf("Enter condition criteria: ");
			std::cin.getline(param4, 64);
		}
		else
			mBatchFile.GetStringFromBatchBuffer(param4); // Get assertion pattern

		if (!mBatchFile.GetBatchFileMode())
		{
			// Get response pattern
			printf("Enter action or <ENTER> for none: ");
			std::cin.getline(param5, 64); //Get response pattern
		}
		else
			mBatchFile.GetStringFromBatchBuffer(param5);  //Get response pattern

		if (param5[0] != 0)
		{
			if (!mBatchFile.GetBatchFileMode())
			{
				
				printf("Enter destination address or <ENTER> for none:");
				std::cin.getline(param6, 64); // Get destination address
			}
			else
			{
				// Get destination address
				mBatchFile.GetStringFromBatchBuffer(param6);
			}

			if (param6[0] != 0)
			{

				if (!mBatchFile.GetBatchFileMode())
				{
					// Get destination port only if we got address
					// from the above command
					printf("Enter destination port: ");
					std::cin.getline(param7, 64); // Get destination address
				}
				else // Get destination port only if we got address from the above command
					mBatchFile.GetStringFromBatchBuffer(param7);
			}
		}
	}  // end of "if(Param2[0]!=0)"
	else
		ParseTrigCmdsFromCmdLine(UserInput
								, param4   // Pattern
								, param5   // Response
								, param6   // Ret/Fwd Addr
								, param7); // Port

	SetTrigger(&Triggers, TrigID, param4, param5, param6, param7);

	Buf[0] = 0;
	sprintf((char*)Buf, "Setting trigger %d with pattern %s\n", TrigID, param4);
	Write((char*)Buf);
}				



void _MyApp::DoSetChannel(char *UserInput, int CmdLen)
{
	char param1[64];
	char param2[64];
	int i = CmdLen;

	// Get channel ID
	mCmdProc.ExtractParamFromInput(2, (unsigned char*)&UserInput[i + _MyAppDefs::SPACE + strlen((char*)param1) + _MyAppDefs::SPACE], ' ');
	mCmdProc.GetParam(2, param2, sizeof(param2) - 1);

	SetSelectedChannel(atoi((char*)param2));
	sprintf(Buf, "Channel now set to %d\n", GetSelectedChannel());
	//TCPPrint((char*)Buf, strnlen((const char*)Buf,sizeof(Buf)));
	//Write((char*)Buf);

	//Param1[0] = Param2[0] = Param3[0] = Param4[0] = (char)0;

}


void _MyApp::DoHelp(char *UserInput)
{
	int i = strlen((char*)Cmd);

	//Param1[0] = Param2[0] = Param3[0] = (unsigned char)0;
	//Param4[0] = Param5[0] = (unsigned char)0;

	if (strncmp(UserInput, "help", 4)==0)
	{
		// Get the parameter to help
		mCmdProc.ExtractParamFromInput(1, (unsigned char*)&UserInput[i + _MyAppDefs::SPACE], ' ');
		mCmdProc.GetParam(1, Buf, sizeof(Buf) - 1);
		Help(Buf);
	}
	else Help(UserInput); // Bad command. Invoke help
}


// Shortcut way to execute a batch file using
// @FileName works really well with filenames
// like 1,2,3 etc as in @1
void _MyApp::BatchCmd(char *UserInput)
{

	// Shortcut way to execute a batch file using
	// @FileName works really well with filenames
	// like 1,2,3 etc as in @1
	if (strncmp((char*)Cmd, "@", 1) == 0)
	{
		mCmdProc.ExtractParamFromInput(1, (unsigned char*)&UserInput[1], ' ');
		mCmdProc.GetParam(1, Buf, sizeof(Buf) - 1);
		mBatchFile.ProcessBatchFile(Buf);

		//Param1[0] = Param2[0] = Param3[0] = Param4[0] = Param5[0] = 0;
		//goto START;
	}
}



int main(int argc, char* argv[])
{
	_MyApp cMyApp;

	char buf[256];

	while (1)
	{
		memset(buf, 0, 256);
		std::cin.getline(buf, 256);
		cMyApp.processCmd(buf, 256);
		std::cout << buf << std::endl;
	}

	return 0;
}

