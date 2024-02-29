#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "utils.h"
#include "console.h"
#include "parse.h"


int main(int argc, char* argv[])
{
	enum modes { def, html, json, help } mode = def;
	bool knowInput = false;
	bool knowOutput = false;

	std::string inputFile;
	std::string outputFile;

	if (argc > 0)
	{
		for (int i = 0; i < argc; i++)
		{
			if (i == 0) continue;

			if (mode == def)
			{
				if (!strcmp(argv[i], "--html") || !strcmp(argv[i], "-h"))
				{
					mode = html;
				}
				if (!strcmp(argv[i], "--json") || !strcmp(argv[i], "-j"))
				{
					mode = json;
				}
				if (!strcmp(argv[i], "--help"))
				{
					mode = help;
				}
			}

			if (argv[i][0] != '-')
			{
				if (!knowInput)
				{
					inputFile = argv[i];
					knowInput = true;
				}
				else if (!knowOutput)
				{
					outputFile = argv[i];
					knowOutput = true;
				}
			}
		}
	}

	while (mode == def)
	{
		PrintConsole("Mode? (json/html/help): ");
		std::string response = ReadConsole();

		if (response == "json")
		{
			mode = json;
			break;
		}
		if (response == "html")
		{
			mode = html;
			break;
		}
		if (response == "help")
		{
			mode = help;
			break;
		}
	}

	if (mode == help)
	{
		PrintConsole("\nUSAGE:\n");
		PrintConsole("readable[.exe] [ARGS] [INPUT] [OUTPUT]\n");

		PrintConsole("\nARGS:\n");
		PrintConsole("-h, --html: Use HTML mode.\n");
		PrintConsole("-j, --json: Use JSON mode.\n");
		PrintConsole("--help: Print help.\n");

		PrintConsole("\nINPUT:\n");
		PrintConsole("Existing input file.\n");

		PrintConsole("\nOUTPUT:\n");
		PrintConsole("Output file name.\n");

		PrintConsole("\n");

		getc(stdin);
		return 0;
	}

	// -------------------------------------------------------

	if (!knowInput)
	{
		PrintConsole("Input file: ");
		inputFile = ReadConsole();
	}
	if (!knowOutput)
	{
		// outputFile is set automatically to:
		// inputFileBasename + '_readable' + inputFileExtension
		//
		// (where inputFileExtension is dot-inclusive)


		outputFile = "";
		std::string basename = "";
		std::string ext = "";

		std::string currentFragment = "";
		for (int i = 0; i < inputFile.size(); i++)
		{
			if (inputFile[i] == '.')
			{
				basename += currentFragment;
				currentFragment = "";
			}
			currentFragment += inputFile[i];
		}
		if (basename.empty())
		{
			basename += currentFragment;
		}
		else
		{
			ext += currentFragment;
		}

		outputFile = basename + "_readable" + ext;
	}

	// -------------------------------------------------------

	std::string data = "";
	if (GetFileData(toWide(inputFile).c_str(), &data))
	{
		PrintConsole("Failed to open file '" + inputFile + "'.\n");

		getc(stdin);
		return 1;
	}

	if (mode == html) ReadableHTML(&data);
	if (mode == json) ReadableJSON(&data);
	PrintConsole(data);

	ClearFileData(toWide(outputFile).c_str());
	WriteDataToFile(data, toWide(outputFile).c_str());

	PrintConsole("\nWrote " + std::to_string(data.size()) + " characters to '" + outputFile + "'.\n");

	getc(stdin);
	return 0;
}

