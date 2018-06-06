/**************************************************************************************
Module Name:	CanIF.h
Author:			Dr. Elliott Coleshill
Date:			May 2018
Description:	This module has been written to provide a sample CANBus interface
				for demo/teaching purposes at Seneca College

Change History:
May 2018: Initial Release
*************************************************************************************/
#pragma once


/*
This solution space have been configured for a standard installation of the ESD CAN Bus
drivers.  If your solution is not compiling properly the following actions will need to be taken:

Open the CANLab Project Properties and update the following information
- Linker --> General:   Make sure the Additional Library Directories as the correct directory for the driver
- Linker --> Input:		Make sure the Additional Dependencies contains ntcan.lib

Verify the ntcan.h include (below) is the correct directory for the *.h file
*/
#include "c:\Program Files\ESD\CAN\SDK\include\ntcan.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <time.h>
using namespace std;

#define CAN_BUFFER_SIZE 25				//Max number of CAN Bus Rx/Tx buffer

class CanIF
{
	int m_net;						/* logical net number (here: 0) */
	uint32_t m_mode;				/* mode used for canOpen() */
	int32_t m_txqueuesize;			/* size of transmit queue */
	int32_t m_rxqueuesize;			/* size of receive queue */
	int32_t m_txtimeout;			/* timeout for transmit operations in ms */
	int32_t m_rxtimeout;			/* timeout for receive operations in ms */
	uint32_t m_baud;				/* configured CAN baudrate (here: 500 kBit/s.) */
	CMSG m_cmsg[CAN_BUFFER_SIZE];	/* can message buffer */
	int m_rtr;						/* rtr bit */
	int m_i;						/* loop counter */
	int32_t m_len;					/* # of CAN messages */
	NTCAN_HANDLE m_Canhandle;		/* can handle returned by canOpen() */
	NTCAN_RESULT m_retvalue;		/* return values of NTCAN API calls */
	bool bExtended;					// flag stating if the extended header is enabled or not

public:
	CanIF(int net,
		uint32_t mode,
		int32_t txqueuesize,
		int32_t rxqueuesize,
		int32_t txtimeout,
		int32_t rxtimeout);

	NTCAN_RESULT OpenCANConnection();
	NTCAN_RESULT SetBaudRate(uint32_t);
	NTCAN_RESULT SetExtendedHeader();
	NTCAN_RESULT AddCanID(int32_t IDToAdd);
	NTCAN_RESULT WriteDataFrame(char *, unsigned int);
	NTCAN_RESULT WriteDataFrame(unsigned int, char *, unsigned int, unsigned int);
	NTCAN_RESULT ReadCANMessage();
	int GetCANMessageBuffer(CMSG MsgBuffer[8]);
	NTCAN_RESULT CloseCANConnection();
	bool CheckRTR(int);
};