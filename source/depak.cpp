#include "depak.h"
#include <fstream>
#include "framework.h"
#include "filef.h"
#include <filesystem>
#include <memory>
#include <CommCtrl.h>
#include <shlobj.h>

EPakFile::EPakFile(std::wstring in, std::wstring out, bool enc)
{
	inPath = in;
	outPath = out;
	encrypted = enc;
}

bool EPakFile::Process()
{
	// reset
	//pFile.close();
	pakEntries.clear();

	pFile.open(inPath, std::ifstream::binary);
	SendMessage(*progressBar, PBM_SETPOS, 0, 0);
	SendMessage(*progressBar, PBM_SETSTATE, PBST_NORMAL, 0);

	if (!pFile.is_open())
	{
		MessageBox(0, L"Could not open input file!", 0, MB_ICONWARNING);
		return false;
	}
	if (pFile.is_open())
	{
		int key = 0x46A78C95;
		int mask = 0xBC1558BC;

		if (encrypted)
		{
			int offset = 0;
			pFile.read((char*)&offset, sizeof(int));
			if (offset >= 0)
			{
				MessageBox(0, L"Failed to extract! This file is most likely not encrypted", 0, MB_ICONWARNING);
				pFile.close();
				return false;
			}

			offset ^= mask;
			pFile.seekg((offset - 1), pFile.beg);



			int files = 0;

			pFile.read((char*)&files, sizeof(int));

			char temp = 0;
			char tempString[255];
			int strlen = 0;
			int fileSize = (int)std::experimental::filesystem::file_size(inPath);
			int headerSize = (int)std::experimental::filesystem::file_size(inPath) - (int)pFile.tellg() + 4;


			std::unique_ptr<char[]> headerBuff = std::make_unique<char[]>(headerSize);
			pFile.read(headerBuff.get(), headerSize);

			for (int i = 0; i < headerSize; i += 4)
			{
				*(int*)(&headerBuff[i]) ^= (int)key;
			}

			std::ofstream hdr("tmp.bin", std::ofstream::binary);


			hdr.write(headerBuff.get(), headerSize);
			hdr.close();

			std::ifstream pTmp("tmp.bin", std::ifstream::binary);

			if (!pTmp)
			{
				MessageBox(0, L"Could not open temporary file!", 0, MB_ICONWARNING);
				return false;
			}



			for (int i = 0; i < files * 3; i++)
			{

				while (temp != 0xD)
				{
					pTmp.read((char*)&temp, sizeof(char));
					tempString[strlen] = temp;
					strlen++;
				}
				std::string entry(tempString, strlen - 1);
				pakEntries.push_back(entry);
				strlen = 0;
				temp = 0;
				sprintf(tempString, "");
			}

			pTmp.close();
			std::experimental::filesystem::remove("tmp.bin");


			// update progress bar
			SendMessage(*progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, files));



			if (!std::experimental::filesystem::exists(outPath))
				std::experimental::filesystem::create_directory(outPath);

			std::experimental::filesystem::current_path(
				std::experimental::filesystem::system_complete(std::experimental::filesystem::path(outPath)));


			// extract
			pFile.clear();
			for (unsigned int i = 0; i < pakEntries.size(); i += 3)
			{
				int offset = 0, size = 0, buffer = 0;

				std::string out(pakEntries[i].c_str(), pakEntries[i].length());
				std::wstring wstr(out.length(), L' ');
				std::copy(out.begin(), out.end(), wstr.begin());


				SetWindowText(*filename, wstr.c_str());
				SendMessage(*progressBar, PBM_STEPIT, 0, 0);

				sscanf(pakEntries[i + 1].c_str(), "%d", &offset);
				sscanf(pakEntries[i + 2].c_str(), "%d", &size);

				pFile.seekg(offset, pFile.beg);


				std::ofstream oFile(wstr, std::ofstream::binary);


				std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(size);
				pFile.read(dataBuff.get(), size);

				for (int i = 0; i < size; i += 4)
					*(int*)(&dataBuff[i]) = *(int*)(&dataBuff[i]) ^ key;

				oFile.write(dataBuff.get(), size);
				oFile.close();

			}
			pFile.close();
			return true;
		}
		else
		{
			int offset = 0;
			pFile.read((char*)&offset, sizeof(int));
			pFile.seekg((offset - 1), pFile.beg);

			int files = 0;

			pFile.read((char*)&files, sizeof(int));

			if (files <= 0)
			{
				MessageBox(0, L"Failed to extract! This file might be encrypted", 0, MB_ICONWARNING);
				pFile.close();
				return false;
			}
			
			// read file entries stored as strings (name, size, offset)

			char temp = 0;
			char tempString[255];
			int strlen = 0;

			for (int i = 0; i < files * 3; i++)
			{
				while (temp != 0xD)
				{
					pFile.read((char*)&temp, sizeof(char));
					tempString[strlen] = temp;
					strlen++;
				}
				std::string entry(tempString, strlen);
				pakEntries.push_back(entry);
				strlen = 0;
				temp = 0;
				sprintf(tempString, "");
			}



			// update progress bar
			SendMessage(*progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, files));



		    if (!std::experimental::filesystem::exists(outPath))
					std::experimental::filesystem::create_directory(outPath);

				std::experimental::filesystem::current_path(
					std::experimental::filesystem::system_complete(std::experimental::filesystem::path(outPath)));
		

			// extract

			for (unsigned int i = 0; i < pakEntries.size(); i += 3)
			{
				int offset = 0, size = 0;
				std::string out(pakEntries[i].c_str(), pakEntries[i].length() - 1);
				std::wstring wstr(out.length(), L' ');
				std::copy(out.begin(), out.end(), wstr.begin());

				SetWindowText(*filename, wstr.c_str());
				SendMessage(*progressBar, PBM_STEPIT, 0, 0);
				sscanf(pakEntries[i + 1].c_str(), "%d", &offset);
				sscanf(pakEntries[i + 2].c_str(), "%d", &size);

				pFile.seekg(offset, pFile.beg);
				std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(size);
				pFile.read(dataBuff.get(), size);



				std::ofstream oFile(wstr, std::ofstream::binary);
				oFile.write(dataBuff.get(), size);

			}
			pFile.close();
			return true;
		}


	}
	return false;
}

void EPakFile::AttachProgressBar(HWND * bar)
{
	progressBar = bar;
	SendMessage(*progressBar, PBM_SETSTEP, 1, 0);
}

void EPakFile::AttachFilenameText(HWND * txt)
{
	filename = txt;

}


std::wstring SetPathFromButton(wchar_t* filter, wchar_t* ext, HWND hWnd)
{
	wchar_t szBuffer[MAX_PATH] = {};
	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = szBuffer;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = ext;
	std::wstring out;
	if (GetOpenFileName(&ofn))
		out = szBuffer;

	return out;
}


std::wstring  SetSavePathFromButton(wchar_t* filter, wchar_t* ext, HWND hWnd)
{
	wchar_t szBuffer[MAX_PATH] = {};
	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = szBuffer;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = ext;
	std::wstring out;
	if (GetSaveFileName(&ofn))
		out = szBuffer;

	return out;
}

std::wstring   SetFolderFromButton(HWND hWnd)
{
	wchar_t szBuffer[MAX_PATH];

	BROWSEINFO bi = {};
	bi.lpszTitle = L"Select Folder";
	bi.hwndOwner = hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	LPITEMIDLIST idl = SHBrowseForFolder(&bi);

	std::wstring out;

	if (idl)
	{
		SHGetPathFromIDList(idl, szBuffer);
		out = szBuffer;

	}

	return out;
}
