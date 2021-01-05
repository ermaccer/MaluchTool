#pragma once
#include <string>
#include <Windows.h>
#include <fstream>

struct tm_header {
	char markerType[6] = {};
};
#pragma pack (push,1)

struct tm_model_marker {
	char pad[261] = {};
	int  meshes;

};
#pragma (pop)

#pragma pack (push,1)
struct tm_mesh_entry {
	char modelName[554] = {}; // what are these sizes even
	char textureName[255] = {};
	//int     unkAmount; 
	//float unk[16 * unkAmount];
	//int     faceAmount;
	//int   faces[3 * faceAmount/3];
//	int     trisAmount;

};
#pragma (pop)

class ETMFile {
private:
	std::wstring   inPath, outPath;
	int           mode;
	std::ifstream  pFile;
	HWND* progressBar;
	HWND* filename;
	wchar_t  progressBuffer[256];
public:
	ETMFile(std::wstring in, std::wstring out, int m);
	bool Process();
	void AttachProgressBar(HWND* bar);
	void AttachFilenameText(HWND* txt);
};