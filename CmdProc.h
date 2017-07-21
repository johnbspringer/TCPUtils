#include <stdlib.h>
#include <string.h>
#include "CmdHistory.h"

struct  _cmdProcessorDefs
{
	static const int PARAM_BUF_SIZE = 64;
	static const int MAX_UDP_PACKET_SIZE = 65535;
};


class _cmdProcessor:public _CmdHistory
{
	//class _cmdProcessorDefs mCmdProcDefs;

	unsigned char Param1[_cmdProcessorDefs::PARAM_BUF_SIZE];
	unsigned char Param2[_cmdProcessorDefs::PARAM_BUF_SIZE];
	// holds data pattern to send
	unsigned char Param3[_cmdProcessorDefs::MAX_UDP_PACKET_SIZE];
	unsigned char Param4[_MAX_PATH];
	unsigned char Param5[_cmdProcessorDefs::PARAM_BUF_SIZE];
	unsigned char Param6[_cmdProcessorDefs::MAX_UDP_PACKET_SIZE +
		_cmdProcessorDefs::PARAM_BUF_SIZE]; //Holds "-s" data, i.e. the data the payload data
	unsigned char Param7[_cmdProcessorDefs::PARAM_BUF_SIZE];
	unsigned char Param8[_cmdProcessorDefs::PARAM_BUF_SIZE];
	unsigned char Param9[5];

public:
	void extractCmd(char *UserInput, int buflen, char Cmd[]);
	void GetParam(int ParamID, char *dataOut, int size);
	void ExtractParamFromInput(int ParamID, unsigned char *DataIn, unsigned char TermChar);
	void GetExtParam(unsigned char *DataOut, unsigned char *DataIn);
	int ParamStrLen(int ParamID);
	_cmdProcessor(void )
	{
		memset(Param1, 0, sizeof(Param1));
		memset(Param2, 0, sizeof(Param2));
		memset(Param3, 0, sizeof(Param3));
		memset(Param4, 0, sizeof(Param4));
		memset(Param5, 0, sizeof(Param5));
		memset(Param6, 0, sizeof(Param6));
		memset(Param7, 0, sizeof(Param7));
		memset(Param8, 0, sizeof(Param8));
		memset(Param9, 0, sizeof(Param9));
	}
};


