#include "CmdProc.h"

int _cmdProcessor::ParamStrLen(int ParamID)
{
	switch (ParamID)
	{
	case 1:
		return strnlen((char*)Param1, sizeof(Param1));
		break;
	case 2:
		return strnlen((char*)Param2, sizeof(Param2));
		break;
	case 3:
		return strnlen((char*)Param3, sizeof(Param3));
		break;
	case 4:
		return strnlen((char*)Param4, sizeof(Param4));
		break;
	case 5:
		return strnlen((char*)Param5, sizeof(Param5));
		break;
	case 6:
		return strnlen((char*)Param6, sizeof(Param6));
		break;
	case 7:
		return strnlen((char*)Param7, sizeof(Param7));
		break;
	case 8:
		return strnlen((char*)Param8, sizeof(Param8));
		break;
	case 9:
		return strnlen((char*)Param9, sizeof(Param9));
		break;
	}
	return 0;
}


void _cmdProcessor::GetParam(int ParamID, char *dataOut,int size)
{
	switch (ParamID)
	{
	    case 1:
			strncpy(dataOut, (char*)Param1, size);
		break;
		case 2:
			strncpy(dataOut, (char*)Param2, size);
		break;
		case 3:
			strncpy(dataOut, (char*)Param3, size);
		break;
		case 4:
			strncpy(dataOut, (char*)Param4, size);
		break;
		case 5:
			strncpy(dataOut, (char*)Param5, size);
		break;
		case 6:
			strncpy(dataOut, (char*)Param6, size);
		break;
		case 7:
			strncpy(dataOut, (char*)Param7, size);
		break;
		case 8:
			strncpy(dataOut, (char*)Param8, size);
		break;
		case 9:
			strncpy(dataOut, (char*)Param9, size);
		break;
	}
}


// Extended parameters are those that contain spaces within in string.
// Extended parameters must be the last parameter within a command
void _cmdProcessor::GetExtParam(unsigned char *DataOut, unsigned char *DataIn)
{
	int i = 0;

	while ((*(DataIn + i) != '\n') && i<strlen((char*)DataIn))
	{
		*(DataOut + i) = *(DataIn + i);
		i++;
	}

	*(DataOut + i) = 0;
}


void _cmdProcessor::ExtractParamFromInput(int ParamID, unsigned char *DataIn, unsigned char TermChar)
{
	int i = 0;
	unsigned char *Param;

	switch (ParamID)
	{
	   case  1:
		  Param = Param1;
	   break;
	   case  2:
		   Param = Param2;
		   break;
	   case  3:
		   Param = Param3;
		   break;
	   case  4:
		   Param = Param4;
		   break;
	   case  5:
		   Param = Param5;
		   break;
	   case  6:
		   Param = Param6;
		   break;
	   case  7:
		   Param = Param7;
		   break;
	   case  8:
		   Param = Param8;
		   break;
	}

	while ((*(DataIn + i) != TermChar) &&
		(*(DataIn + i) != '\n') &&
		i<strlen((char*)DataIn))
	{
		// 100 is just arbitrary just to keep from getting near the end of the buffer
		// Paramters buffers are no larger than PARAM_BUF_SIZE bytes
		if (i >= _cmdProcessorDefs::PARAM_BUF_SIZE - 1)
			return;
		*(Param + i) = *(DataIn + i);
		i++;
	}

	*(Param + i) = 0;
}



void _cmdProcessor::extractCmd(char *UserInput, int bufLen, char Cmd[])
{
	int i = 0;

	// Get the command
	while (UserInput[i] != ' ')
	{
		Cmd[i] = *(UserInput + i);
		if (i == bufLen)
			break;
		else i++;         // Increment i here
	}
	Cmd[i] = 0;        // NUL_VALUE terminate
}