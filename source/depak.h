#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <Windows.h>

#define SUPPORTED_GAMES 2

enum eGames {
	GAME_B3D,
	GAME_POL
};

enum eModes {
	MODE_EXTRACT,
	MODE_CREATE
};

class EPakFile {
private:
	std::wstring   inPath, outPath;
	bool           encrypted;
	std::ifstream  pFile;
	HWND* progressBar;
	HWND* filename;
	wchar_t  progressBuffer[256];
public:
	std::vector<std::string> pakEntries;
	EPakFile(std::wstring in, std::wstring out,bool enc);
	bool Process();
	void AttachProgressBar(HWND* bar);
	void AttachFilenameText(HWND* txt);
};


std::wstring   SetPathFromButton(wchar_t* filter, wchar_t* ext, HWND hWnd);
std::wstring   SetSavePathFromButton(wchar_t* filter, wchar_t* ext, HWND hWnd);
std::wstring   SetFolderFromButton(HWND hWnd);