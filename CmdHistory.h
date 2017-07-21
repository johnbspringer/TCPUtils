


/************************************
COMMAND HISTORY PROCESSING
*************************************/
struct _CmdHistoryDef
{
	static const int MAX_COMMANDS = 100;
	static const int MAX_COMMAND_LEN = 256;
};


class _CmdHistory : public _CmdHistoryDef
{
	// Command history.  100 commands 256 bytes max for each each
	char CmdHist[MAX_COMMANDS][MAX_COMMAND_LEN];
	int HistRetrieveID;
	int CmdHistIndex;
public:
	inline int GetCmdHistIndex(void){ return CmdHistIndex; }
	inline void SetCmdHistIndex(int val){ CmdHistIndex = val; }

	// CmdHistIndex is an index into the command
	// history buffer CmdHist[]. The CmdHistIndex
	// is used as a pointer to the active slot
	// and is incremented once per command execution.
	_CmdHistory(void)
	{
		HistRetrieveID = 0;
		CmdHistIndex = 0;
		for (int i = 0; i < MAX_COMMANDS; i++)
			CmdHist[i][0] = (char)0;
	}

	void addCmdToHistory(const char *UserInput);
	void processCmd(char *UserInput, int size);
	void _CmdHistory::addCmdToHistory(const char *UserInput, int len);
	int process_Not_Not_cmd(const char *UserInput, char *cmdOut, int size);
	int process_Not_cmd(const char *UserInput, char *cmdOut, int size);
};

