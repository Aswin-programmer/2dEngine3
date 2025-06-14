// ENGINE.cpp : Defines the entry point for the application.
// 

#include "ENGINE.h"

Test::Test(std::string sample)
	:
	printable(sample)
{

}

void Test::PrintSample()
{
	std::cout << printable << std::endl;
}
