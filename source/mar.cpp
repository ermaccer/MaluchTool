#include "mar.h"
#include "depak.h"
#include <fstream>
#include "framework.h"
#include "filef.h"
#include <filesystem>
#include <memory>
#include <CommCtrl.h>
#include <shlobj.h>




char* removeAccented(char* str) {
	char *p = str;
	while ((*p) != 0) {
		const char* tr = "AAAAAAECEEEEIIIIDNOOOOOx0UUUUYPsaaaaaaeceeeeiiiiOnooooo/0uuuuypy";
		unsigned char ch = (*p);
		if (ch >= 192) {
			(*p) = tr[ch - 192];
		}
		++p;
	}
	return str;
}



EMarFile::EMarFile(std::wstring in, std::wstring out, int m)
{
	mode = m;
	inPath = in;
	outPath = out;
}

bool EMarFile::Process()
{
	if (mode == MODE_EXTRACT)
	{
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


			mar_header mar;
			pFile.read((char*)&mar, sizeof(mar_header));
			if (mar.header != '\0RAM')
			{
				MessageBox(0, L"This file is not a valid MAR archive!", 0, MB_ICONWARNING);
				pFile.close();
				return false;
			}



			std::unique_ptr<mar_entry[]> files = std::make_unique<mar_entry[]>(mar.files);


			for (int i = 0; i < mar.files; i++)
				pFile.read((char*)&files[i], sizeof(mar_entry));

			if (!std::experimental::filesystem::exists(outPath))
				std::experimental::filesystem::create_directory(outPath);

			std::experimental::filesystem::current_path(
				std::experimental::filesystem::system_complete(std::experimental::filesystem::path(outPath)));


			// update progress bar
			SendMessage(*progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, mar.files));


			for (int i = 0; i < mar.files; i++)
			{
				char* line = files[i].name;
				wsprintf(progressBuffer, L"%d/%d", i + 1, mar.files);
				SetWindowText(*filename, progressBuffer);
				SendMessage(*progressBar, PBM_STEPIT, 0, 0);

				pFile.seekg(files[i].offset, pFile.beg);
				std::ofstream oFile(line, std::ofstream::binary);

				std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(files[i].size);
				pFile.read(dataBuff.get(), files[i].size);
				oFile.write(dataBuff.get(), files[i].size);
			}
			pFile.close();
			return true;
		}
	}
	if (mode == MODE_CREATE)
	{
		SendMessage(*progressBar, PBM_SETPOS, 0, 0);
		SendMessage(*progressBar, PBM_SETSTATE, PBST_NORMAL, 0);

		if (!std::experimental::filesystem::exists(inPath))
		{
			MessageBox(0, L"Folder does not exist!", 0, 0);
			return false;
		}

		int filesFound = 0;
		// get files number
		for (const auto & file : std::experimental::filesystem::recursive_directory_iterator(inPath))
		{
			if (file.path().has_extension())
				filesFound++;

		}
		// update progress bar
		SendMessage(*progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, filesFound));

		// stuffs
		std::unique_ptr<std::string[]> filePaths = std::make_unique<std::string[]>(filesFound); // full path
		std::unique_ptr<std::string[]> fileNames = std::make_unique<std::string[]>(filesFound); // file name
		std::unique_ptr<int[]> sizes = std::make_unique<int[]>(filesFound); // raw size


		int i = 0;
		for (const auto & file : std::experimental::filesystem::recursive_directory_iterator(inPath))
		{
			if (file.path().has_extension())
			{
				filePaths[i] = file.path().string();
				fileNames[i] = file.path().filename().string();
				std::ifstream tFile(filePaths[i], std::ifstream::binary);
				if (tFile)
				{
					sizes[i] = (int)getSizeToEnd(tFile);
					tFile.close();
				}
				i++;

			}
		}

		std::ofstream oFile(outPath, std::ofstream::binary);
		// write heder
		mar_header header;
		header.header = '\0RAM';
		header.files = filesFound;
		oFile.write((char*)&header, sizeof(mar_header));
		int baseOffset = (sizeof(mar_entry) * filesFound) + sizeof(mar_header);

		for (int z = 0; z < filesFound; z++)
		{
			mar_entry ent;
			sprintf(ent.name, "%s", fileNames[z].c_str());
			ent.offset = baseOffset;
			ent.size = sizes[z];
			ent.pad = 0;
			oFile.write((char*)&ent, sizeof(mar_entry));
			baseOffset += sizes[z];
		}

		for (int z = 0; z < filesFound; z++)
		{
			wsprintf(progressBuffer, L"%d/%d", i + 1, filesFound);
			SetWindowText(*filename, progressBuffer);
			SendMessage(*progressBar, PBM_STEPIT, 0, 0);



			std::ifstream pFile(filePaths[z], std::ifstream::binary);
			std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(sizes[z]);
			pFile.read(dataBuff.get(), sizes[z]);
			oFile.write(dataBuff.get(), sizes[z]);
		}

	}
	
	return false;
}

void EMarFile::AttachProgressBar(HWND * bar)
{
	progressBar = bar;
	SendMessage(*progressBar, PBM_SETSTEP, 1, 0);
}

void EMarFile::AttachFilenameText(HWND * txt)
{
	filename = txt;
}

