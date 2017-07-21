#include <iostream>
#include "CmdProc.h"
#include "BatchFiles.h"

void showMenu();
void Help(char * Cmd);

///////////////////////// STUBS ////////////////////

void Write(char*Buf)
{
	std::cout << Buf << std::endl;
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

void showMenu()
{
	printf("***************************************************************\n");
	printf("*                     T C P   U t i l s                       *\n");
	printf("*                     Beta Release 0.0.4                      *\n");
	printf("*                     Copyright 2010-2017                     *\n");
	printf("* ------------------------------------------------------------*\n");
	printf("*                        Command List                         *\n");
	printf("*                                                             *\n");
	printf("*             open         menu         quit      channel(s)  *\n");
	printf("*             display      help         clear     connect     *\n");
	printf("*             set          trigger      log       receive     *\n");
	printf("*             history      send         close     checksum    *\n");
	printf("*             listen       bind                               *\n");
	printf("*                                                             *\n");
	printf("***************************************************************\n\n");
	printf(">"); fflush(stdout);
}

void Help(char * Cmd)
{
	char Buf[64];

	if (strcmp(Cmd, "open") == 0)
	{
		printf("\nUsage: open <-fFILENAME|TYPE ADDRESS PORT>\n");
		printf("If the first argument is \'-f\' followed by a filename then\n");
		printf("the second argument must be either \'-s\' or \'-e\'. A \'-e\' tells\n");
		printf("TCPUtils to execute the file while -s simply reports its size.\n");
		printf("Using \'open -f<filename>\' opens an existing local\n");
		printf("file. Filenames ending with 'tcp' extensions are treated\n");
		printf("as TCPUtils batch files containing commands and parameters.\n");
		printf("For other file types, TCPUtils reads the contents and reports\n");
		printf("the total number of bytes within the file which is usefull\n");
		printf("for obtaining the byte count within pattern files.\n");
		printf("If the first argument is not the \'-f\' option and a filename\n");
		printf("then the 1st argument must be TYPE which is is either udp,tcp or tcpl.\n");
		printf("where tcpl is means \'connect and listen\' for connection request\n");
		printf("The second argument is then ADDRESS which is an IPv4 or MACADDRESS\n");
		printf("address of the remote computer and PORT is the remote port ID. \n");
		printf("\n");
		printf("Example: open udp 172.126.62.116 2015\n");
		printf("         open tcp 172.126.62.116 2015\n");
		printf("         open tcpl 172.126.62.116 2015\n");
		printf("         open tcp aa:bb:cc:dd:ee:ff 2015\n");
		printf("         open -fMyFile.utl -e\n");
		printf("         open -fMyFile.dat -s\n");

	}
	else if (strcmp(Cmd, "close") == 0)
	{
		printf("\nUsage: close [OPTIONS]\n");
		printf("Where OPTIONS is either \'log\', \'channel\' or \'all\' which\n");
		printf("either closes the current channel, log file or all channels\n");
		printf("and the active log file.\n");
	}
	else if (strcmp(Cmd, "clear") == 0)
	{
		printf("\nUsage: clear triggers <TRIGGERID|all>\n");
		printf("Clears the specified TRIGGERID or if \'all\' is specified\n");
		printf("then all triggers will be cleared.\n");
	}
	else if (strcmp(Cmd, "send") == 0)
	{
		printf("Usage is:\n");
		printf("send ADDR PORT [-s|-fPATTERN]\n");
		printf("send ADDR PORT [ITERATION DELAY] -s|-fPATTERN\n");
		//     printf("send ADDR PORT ITERATION [MIN_DELAY MAX_DELAY] [MIN_SIZE MAX_SIZE] -s|-fPATTERN\n\n");
		printf("send ADDR PORT ITERATION [MIN_DELAY MAX_DELAY] [MIN_SIZE MAX_SIZE] TYPE\n");

		printf("Where ADDR is the IPv4 or MAC address, PORT is the Port ID\n");
		printf("PATTERN is the string pattern (preceeded with -s no space) or the name\n");
		printf("of a file (preceeded with -f no space) contained the data to send.\n\n");
		printf("An optional ITERATION and DELAY specifies the total number\n");
		printf("of times to transmit the specified data and the time\n");
		printf("in milliseconds to wait between consecutive retransmissions.\n\n");

		printf("Upon entering \'send ADDR PORT\' and the <RETURN> key\n");
		printf("or \'send ADDRESS PORT INTERATION DELAY\' and the <RETURN> key\n");
		printf("the user will be prompted to enter PATTERN or -fFILENAME.\n\n");

		printf("-sPATTERN can be any string of ASCII characters upto 4000 bytes.\n");
		printf("PATTERN can be formatted using 32 bit hex notation in the form\n");
		printf("of 0x00000001 0xffffffff (which represents two 32 bit values.\n");
		printf("Each hex value is seperated by the previous using a space.\n");
		printf("-fPATTERN is the name of an optional binary or text file\n");
		printf("containing the data to be sent.\n\n");

		printf("Instead of supplying PATTERN, a file TYPE can be specified\n");
		printf("of t for text or b for binary. Specifying a file TYPE invokes\n");
		printf("the pattern auto generation feature\n");

		printf("Example: send 170.126.62.164 2015 <RETURN>\n");
		printf("         0x00000001 0x0000000F\n");
		printf("Example: send ca:b7:45:0d:ee:3f 2015 10 1000 <RETURN>\n");
		printf("         -fMyFile.txt\n");
		printf("Example: send 127.62.68.16 64010 -fMyFile.txt\n");
		printf("Example: send 127.62.68.16 64010 -sThis is a test\n");
		printf("Example: send 127.62.68.16 64010 10 500 -fMyFile.txt\n");
		printf("Example: send 127.62.68.16 64010 10 500 -sThis is a test\n");
		printf("Example: send 127.62.68.16 64010 10 500 -fFileName.txt\n");
		printf("Example: send 127.62.68.16 64010 10 200 500 64 100 t\n");

	}
	else if (strcmp(Cmd, "listen") == 0)
	{
		printf("\nListens for a connection request on the address & \n");
		printf("port specified in last OPEN request.The last port\n");
		printf("created using OPEN must be a TCP port. (Listen is\n");
		printf("typically used by servers).You can also specify \"“tcpl\"”\n");
		printf("for tcp + listen as the mode parameter to the open command\n");
		printf("as in : Open tcpl 169.12.134.20 2010\n");
	}

	else if (strcmp(Cmd, "connect") == 0)
	{
		printf("\nUsage is \"connect <ADDRESS> <PORT>\"\n");
		printf("ADDRESS is IPv4 or MACADDRESS address of the remote terminal\n");
		printf("and PORT is the destination terminal's Port ID.\n");
		printf("Connect will send a request for a TCP connection to port\n");
		printf("id PORT located at ADDRESS. A connection request prompts\n");
		printf("the remote terminal to establish a reliable comminucations\n");
		printf("link. Authorization is indicated by the remote terminal\n");
		printf("sending back an \'accept\' message. Once the host terminal\n");
		printf("sees the accept message a two way link is established.\n");
	}
	else if (strcmp(Cmd, "receive") == 0)
	{
		printf("\nUsage: receive\n");
		printf("Tee receive command allows a UDP port to receive incomming\n");
		printf("data streams. If receiving data on a UDP port is desired, \n");
		printf("the Receive command must be execute before the Send command.\n");
		printf("Once UDP data has been sent on the port, the Receive command\n");
		printf("will fail execution. In such circumstances the port must be\n");
		printf("closed and reopend. Once opened \'receive\' can be issued.\n");
	}
	else if (strcmp(Cmd, "history") == 0)
	{
		printf("\nUsage is: \'history\' or \'!![index]\' or \'!<index> \'\n");
		printf("Using \'history\' or \'!!\' shows the last 100\n");
		printf("recognized commands issued at the command line.\n");
		printf("Issuing the command \'!<index>\' where <index>\n");
		printf("is the index value associated with a command history\n");
		printf("entry as shown to the left of a history list, will\n");
		printf("reenter the indicated command and wait for the user\n\n");
		printf("to make additions and press the enter key.\n");
		printf("Issuing the command \'!!<index>\' will immediatly\n");
		printf("execute the indicated command\n\n");
	}
	else if (strcmp(Cmd, "display") == 0)
	{
		printf("\nUsage: set display [BYTES|off|hex|bin]\n");
		printf("Bytes restricts messages from being displayed which\n");
		printf("are larger than BYTES long. Setting BYTES to 0 filters out\n");
		printf("all received messages. Error messages and byte received count\n");
		printf("are still shown. The default value is 4096 characters.\n");
		printf("'off' turns the display off. No erros or warnings are printed.\n");
		printf("'hex' converts binary data to hex format of ease of viewing.\n");
		printf("'bin' displays received binary data in raw format.\n");
		printf("\n");
		printf("Example: set display 80\n");
		printf(" NOT YET WORKING        set display bin\n");
		printf(" NOT YET WORKING        set display hex\n");
		printf(" NOT YET WORKING  set display off\n");
	}
	else if (strncmp(Cmd, "set", 3) == 0)
	{
		if (strlen(Cmd) == 3)
		{
			printf("\nType \'log\'  \'checksum\' \'channel\'  \'display\' \'trigger\'\n");
			std::cin.getline(Cmd, sizeof(Cmd));
			printf("\n");
			Help(Cmd); // Recursive call here
		}
		else // set has parameters such as log, checksum etc 
			Help(Cmd + 4); // Recursive call here
	}
	else if (strcmp(Cmd, "log") == 0)
	{
		printf("\nUsage is: set log filename [-ton|-toff][-o]\n");
		printf("where filename is the name of the log file\n");
		printf("-ton appends transmission ID to outgoing messages\n");
		printf("-toff turns off the logging of the transmissions ID.\n");
		printf("If a log file with an identical name already exist, a\n");
		printf("warning is issued and the operations is aborted. The\n");
		printf("overwrite option \'-o\' will supress the warning and\n");
		printf("overwrite the existing file. All original data is destroyed.\n");
		printf("You can also use \'set log -c\', which closes the logfile.\n");
	}
	else if (strcmp(Cmd, "help") == 0)
	{
		printf("\nUsage is: help [COMMAND] where COMMAND is a TCPUtils recognized\n");
		printf("command.\n");
	}
	else if (strcmp(Cmd, "checksum") == 0)
	{
		printf("\nUsage is: set checksum OPTION\n");
		printf("Where OPTIONS is <on|off>. The set command turns on/off the\n");
		printf("calculation of checksum values for all transmitted and\n");
		printf("received messages.\n");
	}
	else if (strcmp(Cmd, "channel") == 0)
	{
		printf("\nUsage is: set channel chanID\n");
		printf("          : channels\n");
		printf("Where chanID is a value 0-15 inclusive.\n");
		printf("Each channel is associated with a unique socket.\n");
		printf("The \'set channel\' allows the use of multiple sockets.\n");
		printf("Sockets are defined for a channel via \'open\' command.\n");
		printf("First set the channel then issue the \'open\' command.\n");
		printf("Issuing the \'channels\' command without any parameters\n");
		printf("will show the address and port values assigned to all of the\n");
		printf("channels and indicate the socket type associcated with the\n");
		printf("channel (\'U\'DP, \'T\'CP, or \'R\'aw. The active channel\n");
		printf("is shown using \'<===\'.\n");
	}
	else if (strcmp(Cmd, "trigger") == 0)
	{
		printf("\nUsage: set trigger TRIGGERID<ENTER>\n");
		printf("       CONDITION<ENTER>\n");
		printf("       [ACTION]<ENTER>\n");
		printf("       [ADDRESS]<ENTER>\n");
		printf("Where TRIIGERID is an integer value from 0-15 indicating\n");
		printf("which of the 16 triggers available triggers the definition\n");
		printf("applies to. CONDITION is the string that the trigger will\n");
		printf("listen for, ACTION is the string pattern that the trigger\n");
		printf("will send back. An optional Ipv4 or MACADDRESS ADDRESS can\n");
		printf("specified which causes the trigger to forward ACTION\n");
		printf("to ADDRESS.  When ADDRESS is specified the orginal sender\n");
		printf("of CONDITION does not receive a response back\n");
		printf("\n");
		printf("Upon entering \'set TRIGGERID<ENTER>\' the command will\n");
		printf("for an assertion an assertion CONDITION which can be any\n");
		printf("typed ASCII characters.  Users can also enter a series of\n");
		printf("4 byte hex pattern in the form of 0x00000001 0xFFFFFFFF\n");
		printf("with each hex value seperated by the previous using a space\n");
		printf("The hex values type will be converted to their binary\n");
		printf("equivilent values. Once the user enters CONDITION followed\n");
		printf("the <ENTER> hey, the user will be prompted to enter an\n");
		printf("optional ACTION pattern. Simply press the <RETURN> key\n");

		printf("\nPress <ENTER>\n");
		std::cin.getline(Buf, sizeof(Buf));

		printf("if no response is desired. Triggers will make log entries\n");
		printf("upon assertion even if a ACTION patterns is not entered.\n");
		printf("Once the user enters Triggers followed by the <ENTER> key\n");
		printf("the user will be prompted to enter an optional ADDRESS.\n");
		printf("If a IPv4 or MACADDRESS address is not entered, the default\n");
		printf("behavior is to send ACTION string back to the sender.\n");
		printf("\n");
		printf("Upon trigger assertion, a text message will be presented\n");
		printf("announcing assertion of the trigger. If a response was\n");
		printf("was specified then the response pattern is sent.\n");
		printf("Reponse patterns are always sent using the protocol of\n");
		printf("the arriving message.\n");
		printf("\n");
		printf("Triggers can be deactivated using the \'clear\' command\n");
		printf("\'clear TRIGGERID\' clears the assertion pattern of the\n");
		printf("trigger identified by TRIGGERID. Once cleared, trigger ID\n");
		printf("will no longer be asserted until redefined. Using command\n");
		printf("\'clear trigger all\' will clear the assertion pattern \n");
		printf("of all triggers.\n");

		printf("\nPress <ENTER>\n");
		std::cin.getline(Buf, sizeof(Buf));

		printf("\n");
		printf("Example: Set trigger 0\n");
		printf("        >Enter condition: ping\n");
		printf("        >Enter action: pong\n");
		printf("        >Enter response address:<enter>\n");
		printf("        >Trigger set\n");
		printf("\n");
		printf("The above sets trigger 0 to assert upon receiving the \n");
		printf("string \'ping\'. Upon assertion, the trigger will respond\n");
		printf("with \'pong\'\n");
		printf("\n");
		printf("Example: set trigger 3 I say PING,I say PONG,120 64010\n\n");
		printf("The above shows how the CONDITION and ACTION strings\n");
		printf("can contain multiple words forming a phrase. This is allowed\n");
		printf("when the CONDITION and ACTION strings are separated using\n");
		printf("commas. No spaces should appear immediately following the\n");
		printf("commas or they will be interpreted as a place holder for\n");
		printf("for a missing next arguments.\n");
	}
	else
	{
		std::cerr << "Command: " << Cmd << " is not a recognized command" << std::endl;
		showMenu();
	}
	printf(">"); fflush(stdout);
}
