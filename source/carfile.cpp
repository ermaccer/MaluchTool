#include "carfile.h"
#include "depak.h"
#include <Windows.h>
#include <filesystem>


bool ECarFile::Process()
{
	std::wstring inPath = SetPathFromButton(L"Car Info(*.dat)\0*.dat\0All Files (*.*)\0*.*\0", L"dat", 0);
	FILE* pFile = _wfopen(inPath.c_str(), L"rb");

	if (!pFile && inPath.length() > 0)
	{
		MessageBox(0, L"Failed to open input file!", 0, MB_ICONWARNING);
		return false;
	}

	if (pFile)
	{
		int fileSize = (int)std::experimental::filesystem::file_size(inPath);

		std::wstring out = SetSavePathFromButton(L"INI(*.ini)\0*.ini\0All Files (*.*)\0*.*\0", L"ini", 0);
		FILE* oFile = _wfopen(out.c_str(), L"wb");

		char temp = 0;
		temp = fgetc(pFile) ^ 0x7B;
		for (int i = temp; !feof(pFile); i++)
		{
			fputc(temp, oFile);
			temp = i ^ fgetc(pFile);

		}
		fclose(pFile);
		fclose(oFile);
		return true;

	}
}

bool ECarFile::ProcessEncrypt()
{
	std::wstring inPath = SetPathFromButton(L"INI(*.ini)\0*.ini\0All Files (*.*)\0*.*\0", L"ini", 0);
	FILE* pFile = _wfopen(inPath.c_str(), L"rb");

	if (!pFile && inPath.length() > 0)
	{
		MessageBox(0, L"Failed to open input file!", 0, MB_ICONWARNING);
		return false;
	}

	if (pFile)
	{
		int fileSize = (int)std::experimental::filesystem::file_size(inPath);

		std::wstring out = SetSavePathFromButton(L"Car Info(*.dat)\0*.dat\0All Files (*.*)\0*.*\0", L"dat", 0);
		FILE* oFile = _wfopen(out.c_str(), L"wb");
		char temp = 0;
		temp = fgetc(pFile) ^ 0x7B;
		char og = temp;

		for (int i = 0; i < fileSize; i++)
		{
			fputc(og, oFile);
		}
		//char byte = fgetc(pFile);

		//temp = og ^ 0x7B ^ byte;


		fclose(pFile);
		fclose(oFile);
		return true;

	}
}