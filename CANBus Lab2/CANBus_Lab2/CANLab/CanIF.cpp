/**************************************************************************************
Module Name:	CanIF.cpp
Author:			Dr. Elliott Coleshill
Date:			May 2018
Description:	This module has been written to provide a sample CANBus interface
				for demo/teaching purposes at Seneca College

Change History:
May 2018: Initial Release
*************************************************************************************/
#include "CanIF.h"


/*
Function:	Constructor
Inputs:
			net:			CANBus Controller ID
			mode:			The mode for the controller -- Read only, passive, etc...
			txqueuesize:	The number of bytes in the drivers Tx Queue
			rxqueuesize:	The number of bytes in the drivers Rx Queue
			txtimeout:		The number of milliseconds for the driver to wait for Tx
			rxtimeout:		The number of milliseconds for the driver to wait for Rx
Output:		CANBus Controller Result
Purpose:	This function sets the objects state member variables to be used for all CANBus operations
*/
CanIF::CanIF(int net, uint32_t mode, int32_t txqueuesize, int32_t rxqueuesize, int32_t txtimeout, int32_t rxtimeout)
{
	m_net = net;
	m_mode = mode;
	m_txqueuesize = txqueuesize;
	m_rxqueuesize = rxqueuesize;
	m_txtimeout = txtimeout;
	m_rxtimeout = rxtimeout;
	bExtended = false;					//By default the CAN Bus protocol is set to 11-bit standard header
}

/*
Function:	OpenCANConnection
Inputs:		
			None
Outputs:	CAN Bus Controller Result
Purpose:	This function calls the canOpen library function and initializes the CANBus USB controller selected
			for the current object.   Note:  The m_net value needs to be set to the CANBus ID on the physical
			controller
*/
NTCAN_RESULT CanIF::OpenCANConnection()
{
	m_retvalue = canOpen(m_net, m_mode, m_txqueuesize, m_rxqueuesize, m_txtimeout, m_rxtimeout, &m_Canhandle);

	return(m_retvalue);
}

/*
Function:	SetBaudRate
Intputs:
			BaudRate:		The baud rate in bps to configure the CANBus controller for
Output:		CAN Bus Controller Result
Purpose:	This function sets the objects BaudRate to the selected value -- 10bps upto 1000bps.  It then
			sets the physical hardwards baud rate by calling the canSetBaudrate library function
*/
NTCAN_RESULT CanIF::SetBaudRate(uint32_t BaudRate)
{
	m_baud = BaudRate;		//store the baud rate in the objects location
	m_retvalue = canSetBaudrate(m_Canhandle, m_baud);

	return(m_retvalue);
}

/*
Function:	SetExtendedHeader
Inputs:
			None
Output:		CAN Bus Controller Result
Purpose:	This functions configures the 29-bit extended header for the CAN Protocol using the canIoctl 
			library function.  If not called the CAN Bus will default to the 11-bit standard header
*/
NTCAN_RESULT CanIF::SetExtendedHeader()
{
	uint32_t mask29Bit;
	mask29Bit = 0x1FFFFFFF;
	m_retvalue = canIoctl(m_Canhandle, NTCAN_IOCTL_SET_20B_HND_FILTER, &mask29Bit);

	//Verify the 29-bit extended header was enabled before setting the objects state
	if (m_retvalue == NTCAN_SUCCESS)
		bExtended = true;

	return(m_retvalue);
}

/*
Function:	AddCanID
Inputs:
			IDToAdd:		The unique CAN Bus ID to register with the controller
Output:		CAN Bus Controller Result
Purpose:	This function adds a CANId to the current CAN Bus driver to monitor by calling
			the canIdAdd library function
*/
NTCAN_RESULT CanIF::AddCanID(int32_t IDToAdd)
{
	m_retvalue = canIdAdd(m_Canhandle, IDToAdd);

	return(m_retvalue);
}

/*
Function:	WriteDataFrame
Inputs:
			data:		A pointer to the data that needs to be transmitted
			size:		The number of bytes to be transmitted
			rtr:		Flag (1 or 0) to set the RTR bit in the transmission frame
Outputs:	CAN Bus Controller Result
Purpose:	This function will generate 8-byte CAN Bus frames to transmit all the data, upto
			a maximum of 32 8-byte CAN Bus frames.   If the ammount of data is > 32 frames
			the Tx operation will be aborted

			NOTE:  This function will transmit the data using the objects CAN Bus ID in the
					arbitration field
*/
NTCAN_RESULT CanIF::WriteDataFrame(char *data, unsigned int size)
{
	unsigned int NumOfFrames = (unsigned int)(size / 8);				//calc the number of CANBus frames required to send message
	unsigned int Remainder = size % 8;									//Get the number of left over bytes
	char *ptr = data;													//Make a local copy for pointer arithmetic

	if (Remainder > 0)
		NumOfFrames++;		//add an extra frame for the left over data

	//Validate to make sure buffer sizes are not exceeded
	if (NumOfFrames > CAN_BUFFER_SIZE)
		return NTCAN_OPERATION_ABORTED;
	else
		m_len = NumOfFrames;

	//Generate all the CANBus frames
	for (unsigned int x = 0; x < NumOfFrames; x++)
	{
		m_cmsg[x].id = m_net;
		
		//Handle special case -- Last CANBus frame won't be a full 8-bytes
		if (x == NumOfFrames - 1)
		{
			m_cmsg[x].len = Remainder;
			m_cmsg[x].len |= m_cmsg[x].len + (0 << 4);
			memcpy(m_cmsg[x].data, ptr, Remainder);
		}
		else
		{
			m_cmsg[x].len = 8;
			m_cmsg[x].len |= m_cmsg[x].len + (0 << 4);
			memcpy(m_cmsg[x].data, ptr, 8);
		}

		ptr += 8;	//move the raw pointer 8-bytes in memory
	}

	m_retvalue = canWrite(m_Canhandle, &m_cmsg[0], &m_len, nullptr);
	return(m_retvalue);
}

/*
Function:	WriteDataFrame
Inputs:
			ID:			A unique CAN Bus ID to use in the CAN Bus arbitration field
			data:		A pointer to the data that needs to be transmitted
			size:		The number of bytes to be transmitted
			rtr:		Flag (1 or 0) to set the RTR bit in the transmission frame
Outputs:	CAN Bus Controller Result
Purpose:	This function will generate 8-byte CAN Bus frames to transmit all the data, upto
			a maximum of 32 8-byte CAN Bus frames.   If the ammount of data is > 32 frames
			the Tx operation will be aborted

			NOTE:  This function will transmit the data using the provided CAN Bus ID in the
					arbitration field
*/
NTCAN_RESULT CanIF::WriteDataFrame(unsigned int ID, char *data, unsigned int size, unsigned int rtr)
{
	unsigned int NumOfFrames = (unsigned int)(size / 8);				//calc the number of CANBus frames required to send message
	unsigned int Remainder = size % 8;									//Get the number of left over bytes
	char *ptr = data;													//Make a local copy for pointer arithmetic

	//If the request is to send an RTR -- process and exit
	if (rtr == 1)
	{
		m_len = 1;

		m_cmsg[0].id = ID;
		m_cmsg[0].len = 0;
		m_cmsg[0].len |= m_cmsg[0].len + (rtr << 4);

		m_retvalue = canWrite(m_Canhandle, &m_cmsg[0], &m_len, nullptr);
		return(m_retvalue);
	}

	if (Remainder > 0)
		NumOfFrames++;		//add an extra frame for the left over data

							//Validate to make sure buffer sizes are not exceeded
	if (NumOfFrames > CAN_BUFFER_SIZE)
		return NTCAN_OPERATION_ABORTED;
	else
		m_len = NumOfFrames;

	//Generate all the CANBus frames
	for (unsigned int x = 0; x < NumOfFrames; x++)
	{
		m_cmsg[x].id = ID;

		//Handle special case -- Last CANBus frame won't be a full 8-bytes
		if (x == NumOfFrames - 1)
		{
			m_cmsg[x].len = Remainder;
			m_cmsg[x].len |= m_cmsg[x].len + (rtr << 4);
			memcpy(m_cmsg[x].data, ptr, Remainder);
		}
		else
		{
			m_cmsg[x].len = 8;
			m_cmsg[x].len |= m_cmsg[x].len + (rtr << 4);
			memcpy(m_cmsg[x].data, ptr, 8);
		}

		ptr += 8;	//move the raw pointer 8-bytes in memory
	}

	m_retvalue = canWrite(m_Canhandle, &m_cmsg[0], &m_len, nullptr);
	return(m_retvalue);
}

/*
Function:	ReadCANMessage
Inputs:
			None
Output:		CAN Bus Controller Result
Purpose:	This function will use the canTake library function to receive data on the
			bus.  Only frames that have the objects ID or IDs registered with the driver
			will be collected off the bus.   A maximum of 32 frames can be collected at
			any given time
*/
NTCAN_RESULT CanIF::ReadCANMessage()
{
	m_len = CAN_BUFFER_SIZE;
	m_retvalue = canTake(m_Canhandle, &m_cmsg[0], &m_len);

	return (m_retvalue);
}

/*
Function:	GetCANMessageBuffer
Inputs:
			MsgBuffer:		A reference to a message buffer where all the received data can be copied
Outputs:	m_len:			The total number of bytes copied into the buffer
Purpose:	This function copies the current content of the objects CAN Bus frame buffer into the
			memory space provided.
*/
int CanIF::GetCANMessageBuffer(CMSG MsgBuffer[CAN_BUFFER_SIZE])
{
	//Copy the cmsg buffer * the number of frames received to the msgbuffer passed into
	memcpy(&MsgBuffer[0], &m_cmsg[0], sizeof(CMSG)*m_len);
	return(m_len);		//Return the length of the data receive
}

/*
Function:	CloseCANConnection
Inputs:
			None
Outputs:	CAN Bus Controller Result
Purpose:	This function calls the canClose library function to clean up the objects active CAN Bus
			controller
*/
NTCAN_RESULT CanIF::CloseCANConnection()
{
	m_retvalue = canClose(m_Canhandle);

	return(m_retvalue);
}

/*
Function:	CheckRTR
Inputs:
			index:		The index of the objects CAN Bus frame buffer
Outputs:	bool:		Flag stating if the RTR bit is set (true) or not (false)
Purpose:	This function checks the RTR bit of the requested index to determine if the CAN Bus
			frame received is a request frame of command frame
*/
bool CanIF::CheckRTR(int index)
{
	uint8_t rtrExt = m_cmsg[index].len;
	rtrExt = ((rtrExt >> 4) && 0x01);
	if (rtrExt == 1)
		return true;
	else
		return false;
}