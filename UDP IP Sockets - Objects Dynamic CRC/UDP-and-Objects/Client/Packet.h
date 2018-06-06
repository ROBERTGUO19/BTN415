#pragma once
#include <memory>
#include <iostream>
#include <fstream>

const int EmptyPktSize = 6;					//Number of data bytes in a packet with no data field

class Packet
{
	struct Header
	{
		unsigned Source : 4;				//Source ID
		unsigned LineNumber : 4;			//Line number of the input file being transmitted
		char Length;							//Number of characters in the line
	} Head;
	char *Data;								//The data bytes
	unsigned int CRC;						//Cyclic Redundancy Check

	char *TxBuffer;

public:
	Packet() : Data(nullptr), TxBuffer(nullptr) { Head.Source = 2; };		//Default Constructor - Safe State
	void SetLineNumber(int value) { Head.LineNumber = value; };				//Sets the line number within the object
	void Display(std::ostream& os)
	{
		os << "Source:  " << (int)Head.Source << std::endl;
		os << "LineNum: " << (int)Head.LineNumber << std::endl;
		os << "Length:  " << (int)Head.Length << std::endl;
		os << "Msg:     " << Data << std::endl;
		os << "CRC:     " << std::hex << (unsigned int)CRC << std::endl;
	}
	
	/*******************************************************************************
	STUDENTS:  You are responsible for writing the logic for the following functions
	           as per the specifications provided
	*******************************************************************************/
	Packet(char* src)
	{

	}

	void SetData(char* srcData, int Size)
	{

	};

	char* SerializeData(int &TotalSize)
	{

	};

	unsigned int CalculateCRC()
	{

	}
};
