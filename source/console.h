#include <string>
#include <stdio.h>

inline void PrintConsole(std::wstring output)
{
	printf("%ls", output.c_str());
}

inline void PrintConsole(std::string output)
{
	printf("%s", output.c_str());
}

inline std::string ReadConsole()
{
	std::string response = "";
	char input;
	do
	{
		input = getc(stdin);
		response.push_back(input);
	} while (input != '\n');
	response.pop_back();

	return response;
}

