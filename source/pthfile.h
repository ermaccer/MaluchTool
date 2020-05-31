#pragma once
#include <string>

struct pth_header {
	char header[8] = {}; // PATH00
	int  points;
};

struct pth_point {
	float x, y, z;
	int unk1;
	int unk2;
};

class EPathFile {
public:
	bool ProcessToText();
	bool ProcessToPath();
};