// ENGINE.h : Include file for standard system include files,
// or project specific include files.
 
#pragma once

#include <iostream>

class Test
{
public:
	Test(std::string sample);
	~Test() = default;

	void PrintSample();

private:
	std::string printable;
};
