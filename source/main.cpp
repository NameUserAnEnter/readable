#include <string>
#include "utils.h"
#include "console.h"
#include "readable.h"


int main(int argc, char* argv[])
{
	enum modes { def, html, json, help } mode = def;
	bool knowInput = false;
	bool knowOutput = false;

	std::string inputFile;
	std::string outputFile;

	// -------------------------------------------------------
	
	// Parse args if any have been passed.
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

	// -------------------------------------------------------
	
	// If not provided, ask for input file.
	if (!knowInput)
	{
		PrintConsole("Input file: ");
		inputFile = ReadConsole();
	}
	if (!FileExist(toWide(inputFile).c_str()))
	{
		PrintConsole("\nFailed to open file '" + inputFile + "'.\n");

		PrintConsole("Press ENTER to continue: ");
		ReadConsole();
		return 1;
	}


	if (!knowOutput)
	{
		// Attempt to set the output filename to:
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

		// To avoid collision with existing files, attempt to add an appendix number to the basename.
		unsigned long appendix = 1;
		while (FileExist(toWide(outputFile).c_str()))
		{
			// Ask user for output filename only if the maximum possible appendix number is reached and still colliding.
			if (appendix >= 0xFFFFFFFF)
			{
				PrintConsole("Output file: ");
				outputFile = ReadConsole();

				break;
			}

			appendix++;
			outputFile = basename + "_readable" + NumToStr(appendix) + ext;
		}

		// Attempt to automatically deduce mode from input file extension.
		// If not deduced ask the user for mode.
		if (ext == ".js" || ext == ".json") mode = json;
		else if (ext == ".html") mode = html;
		else
		{
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
		}
	}

	// -------------------------------------------------------
	// help mode

	if (mode == help)
	{
		PrintConsole("\nUSAGE:\n");
		PrintConsole("readable[.exe] [ARGS] [INPUT] [OUTPUT]\n");

		PrintConsole("\nARGS:\n");
		PrintConsole("-h, --html: Use HTML mode.\n");
		PrintConsole("-j, --json: Use JSON mode.\n");
		PrintConsole("--help: Print help.\n");
		PrintConsole("\nPass no args to run in interactive mode.\n");

		PrintConsole("\nINPUT:\n");
		PrintConsole("Existing input file.\n");

		PrintConsole("\nOUTPUT:\n");
		PrintConsole("Output file name.\n");

		PrintConsole("\n");

		PrintConsole("Press ENTER to continue: ");
		ReadConsole();
		return 0;
	}

	// -------------------------------------------------------
	// standard modes

	std::string data = "";
	if (GetFileData(toWide(inputFile).c_str(), &data))
	{
		PrintConsole("\nFailed to open file '" + inputFile + "'.\n");

		PrintConsole("Press ENTER to continue: ");
		ReadConsole();
		return 1;
	}

	if (mode == html) ReadableHTML(&data);
	if (mode == json) ReadableJSON(&data);
	PrintConsole(data);

	WriteDataToFile(data, toWide(outputFile).c_str());
	PrintConsole("\nWrote " + std::to_string(data.size()) + " characters to '" + outputFile + "'.\n");

	PrintConsole("Press ENTER to continue: ");
	ReadConsole();
	return 0;
}

