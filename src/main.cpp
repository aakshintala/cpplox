#include <iostream>

// We are using SYSEXITS exit codes

int runScript(char const *scriptName) {
	std::cout << "Running from a script is not implemented." << std::endl;
	return 70;
}

int runPrompt(char const *scriptName) {
	std::cout << "Running from a script is not implemented." << std::endl;
	return 70;
}

int main(int argc, char const *argv[])
{
	if (argc > 2) {
		std::cout << "Usage: ./cpplox <script.lox>" << std::endl;
		std::exit(64);
	} else if (2 == argc) {
		return runScript(argv[1]);
	} else {
		return runPrompt();
	}
	return 0;
}

