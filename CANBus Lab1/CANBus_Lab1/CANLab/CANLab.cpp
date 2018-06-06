#include <iostream>
#include <thread>
#include "CanIF.h"

void processSync(CanIF* canBus, bool& flag)
{
	std::cout << "Starting processSync thread" << std::endl;

	//TBD:   ADD YOUR CODE HERE <================================

	std::cout << "Thread Terminating" << std::endl;
}

int main(int argc, char* argv[])
{
	CanIF* CanNode;							//The CANBus Node
	bool bDone = false;						//Program termination flag

	//TBD:   ADD YOUR CODE HERE <================================
	
	while (!bDone)
	{
		char input;
		std::cin >> input;
		if (input == 'r')
		{
			//TBD:   ADD YOUR CODE HERE <================================
		}
		else if (input == 'd')
			bDone = true;

	};		//Loop until program terminated

	return 1;
}