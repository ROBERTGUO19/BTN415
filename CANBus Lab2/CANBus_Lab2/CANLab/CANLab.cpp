#include <iostream>
#include <thread>
#include "CanIF.h"

int main(int argc, char* argv[])
{
	CanIF* CanNode;							//The CANBus Node
	bool bDone = false;						//Program termination flag

	while (!bDone)
	{
		char input;
		std::cin >> input;
	 if (input == 'd')
			bDone = true;

	};		//Loop until program terminated

	return 1;
}