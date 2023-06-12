#include <stdio.h>
#include <stdlib.h>
#include <string>

std::wstring toWide(std::string str)
{
	std::wstring wide;
	for (auto letter : str)
	{
		wide += letter;
	}
	return wide;
}

std::string EncodeToUTF8(std::wstring str)
{
	std::string encoded = "";
	for (int i = 0; i < str.size(); i++)
	{
		wchar_t character = str[i];
		if (character <= 0x7F) encoded += (unsigned char)character;
		else
		{
			if (character >= 0x80 && character <= 0x7FF)
			{
				encoded += (unsigned char)(((0xC0 << 8) + ((character << 2) & (0x1F << 8))) >> 8);
				encoded += (unsigned char)(((0x80) + (character & 0x3F)));
			}
			else if (character >= 0x800 && character <= 0xFFFF)
			{
				encoded += (unsigned char)(((0xE0 << 16) + ((character << 4) & (0x0F << 16))) >> 16);
				encoded += (unsigned char)(((0x80 << 8) + ((character << 2) & (0x3F << 8))) >> 8);
				encoded += (unsigned char)(((0x80) + (character & 0x3F)));
			}
			else if (character >= 0x10000)
			{
				// wchar_t is 16-bit on Windows, 0x10000+ code points are not supported
			}
		}
	}
	return encoded;
}

std::wstring DecodeFromUTF8(std::string str)
{
	std::wstring decoded = L"";
	for (int i = 0; i < str.size(); i++)
	{
		wchar_t currentChar = str[i];
		if ((currentChar & 128) == 128 && (currentChar & 64) == 64)
		{
			if ((currentChar & 32) == 0 && i + 1 < str.size())
			{
				// 2-Byte encoded character
				// 110xxxxx ...
				unsigned char Byte[] ={ (unsigned char)currentChar & 31, str[i + 1] & 63 };
				i++;

				// 16-bit decoded character:
				currentChar = (Byte[1]) + (((unsigned short)Byte[0] << 8) >> 2);
			}
			else if ((currentChar & 32) == 32)
			{
				if ((currentChar & 16) == 0 && i + 2 < str.size())
				{
					// 3-Byte encoded character
					// 1110xxxx ...
					unsigned char Byte[] ={ (unsigned char)currentChar & 15, str[i + 1] & 63, str[i + 2] & 63 };
					i++;
					i++;

					// 16-bit decoded character:
					currentChar = (Byte[2]) + (((unsigned short)Byte[1] << 8) >> 2) + (((unsigned int)Byte[0] << 16) >> 4);
				}
				else if ((currentChar & 16) == 16 && i + 3 < str.size())
				{
					if ((currentChar & 8) == 0)
					{
						//4-Byte encoded character
						// 11110xxx ...
						for (int i = 0; i < 3; i++) i++;

						// 16-bit substitue character:
						currentChar = 'e';
					}
				}
			}
		}

		decoded += currentChar;
	}
	return decoded;
}

void PrintConsole(std::wstring output)
{
	printf("%ls", output.c_str());
}

void PrintConsole(std::string output)
{
	printf("%s", output.c_str());
}

int GetFileData(const wchar_t* filename, std::string* returnData = nullptr)
{
	std::string data = "";

	FILE* file;
	_wfopen_s(&file, filename, L"rb");
	if (file == nullptr)
	{
		PrintConsole(L"Cannot open: " + std::wstring(filename) + L'\n');
		return 1;
	}

	int currentChar;
	do
	{
		currentChar = getc(file);
		if (currentChar != EOF) data += (unsigned char)currentChar;
	} while (currentChar != EOF);
	fclose(file);

	if (returnData != nullptr) *returnData = data;
	return 0;
}

int WriteDataToFile(std::string data, const wchar_t* filename)
{
	FILE* file;
	_wfopen_s(&file, filename, L"wb");
	if (file == nullptr)
	{
		PrintConsole(L"Cannot open: " + std::wstring(filename) + L'\n');
		return 1;
	}

	for (int i = 0; i < data.size(); i++)
	{
		putc((unsigned char)data[i], file);
	}
	fclose(file);

	return 0;
}

void ClearFile(std::wstring filename)
{
	std::string empty_data = "";
	WriteDataToFile(empty_data, filename.c_str());
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


int main()
{
	std::string data = "";
	
	std::string dir = "D:/conversion/tbdl/";
	std::string inputFile = "parsed_regex_sample.txt";
	std::string outputFile = "parsed_regex_sample_output.txt";

	GetFileData(toWide(dir + inputFile).c_str(), &data);

	modifyData(&data);
	PrintConsole(data);

	ClearFile(toWide(dir + outputFile));
	WriteDataToFile(data, toWide(dir + outputFile).c_str());

	while (getc(stdin) != 'x') { };
	return 0;
}
