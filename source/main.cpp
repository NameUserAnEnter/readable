#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "utils.h"



void PrintConsole(std::wstring output)
{
	printf("%ls", output.c_str());
}

void PrintConsole(std::string output)
{
	printf("%s", output.c_str());
}



void modifyData(std::string* data)
{
	if (data == nullptr) return;
	std::string copy = *data;
	//					0123456789012345678901
	//std::string copy = "testDict = { testVar }";
	std::string output = "";

	int nest = 0;
	bool inQuotes = false;
	bool escaped = false;
	for (auto letter : copy)
	{
		if (letter == '"')
		{
			if (inQuotes)
			{
				if (!escaped)
				{
					inQuotes = !inQuotes;
				}
			}
			else
			{
				inQuotes = !inQuotes;
			}
		}

		if (inQuotes && letter == '\\') escaped = true;
		else escaped = false;

		if ((letter != ' ' && letter != '\t' && letter != '\n' && letter != '\r') || inQuotes)
		{
			if (!inQuotes)
			{
				if (letter == '}' || letter == ']')
				{
					output += "\r\n";
					nest--;
					for (int i = 0; i < nest; i++)
					{
						output += '\t';
					}
				}
				if (letter == '=')
				{
					output += ' ';
				}
			}
			output += letter;
			if (!inQuotes)
			{
				if (letter == '=') output += ' ';
				if (letter == ':') output += ' ';

				if (letter == ',' || letter == ';')
				{
					output += "\r\n";
					for (int i = 0; i < nest; i++)
					{
						output += '\t';
					}
				}
				if (letter == '{' || letter == '[')
				{
					output += "\r\n";
					nest++;
					for (int i = 0; i < nest; i++)
					{
						output += '\t';
					}
				}
			}
		}
	}


	*data = output;
}

void modifyData2(std::string* input)
{
	if (input == nullptr) return;

	std::string data = *input;
	std::string output = "";

	std::vector<std::string> lines;
	lines.clear();
	lines.push_back("");
	for (int i = 0; i < data.size(); i++)
	{
		if (data[i] == '<') lines.push_back("");
		lines.back() += data[i];
	}




	std::vector<std::string> outputLines;
	outputLines.resize(lines.size());

	auto getTagWithSlashOrNot = [](std::string line)
	{
		std::string tag = "";
		for (int i = 1; i < line.size(); i++)
		{
			if (line[i] == ' ' || line[i] == '>') break;
			tag += line[i];
		}
		return tag;
	};


	int nest = 0;
	std::string prevTag = "";
	for (int i = lines.size() - 1; i >= 0; i--)
	{
		std::string currentLine = lines[i];
		std::string currentTag = getTagWithSlashOrNot(currentLine);
		for (int j = 1; j < currentLine.size(); j++)
		{
			if (currentLine[0] == '<' && currentLine[1] != '/' && currentTag != "img")
			{
				nest--;
				break;
			}
		}

		//MessageDialog(NumToStr(nest) + ": " + getTagWithSlashOrNot(currentLine) + "\n" + currentLine);


		//MessageDialog(NumToStr(nest) + ": " + line);
		outputLines[i] = currentLine;
		for (int j = 0; j < nest; j++)
		{
			outputLines[i] = '\t' + outputLines[i];
		}

		for (int j = 0; j < currentLine.size(); j++)
		{
			if (j == 0 || j == 1) continue;
			if (currentLine[0] == '<' && currentLine[1] == '/')
			{
				nest++;
				break;
			}
		}
	}



	for (auto line : outputLines) output += line + '\n';
	*input = output;
}


int main()
{
	std::string data = "";
	
	std::string dir = "";
	std::string inputFile = "input.html";
	std::string outputFile = "output.html";

	GetFileData(toWide(dir + inputFile).c_str(), &data);

	modifyData2(&data);
	PrintConsole(data);

	ClearFileData(toWide(dir + outputFile).c_str());
	WriteDataToFile(data, toWide(dir + outputFile).c_str());

	while (getc(stdin) != 'x') { };
	return 0;
}
