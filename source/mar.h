#pragma once
#include <string>
#include <Windows.h>
#include <fstream>

struct mar_header {
	int header; //MAR 0x0
	int files;
	char pad[12] = {};
};

struct mar_entry {
	char name[260] = {};
	int  offset;
	int  size;
	int  pad;
};


class EMarFile {
private:
	std::wstring   inPath, outPath;
	int           mode;
	std::ifstream  pFile;
	HWND* progressBar;
	HWND* filename;
	wchar_t  progressBuffer[256];
public:
	EMarFile(std::wstring in, std::wstring out, int m);
	bool Process();
	void AttachProgressBar(HWND* bar);
	void AttachFilenameText(HWND* txt);
};