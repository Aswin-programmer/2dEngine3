// GetIntoGameDevOPENGL.cpp : Defines the entry point for the application.
//

#include "GetIntoGameDevOPENGL.h"

using namespace std;

int main()
{
	Test test("Hello World");
	test.PrintSample();

	Window::init("TESTING");

	while (!Window::shouldClose())
	{
		Window::clearScreen();
		Window::processInput();



		Window::update();
	}
	Window::cleanup();
	return 0;
}
