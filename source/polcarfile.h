#pragma once

struct polengine_car {
	char header[8] = {}; // MCAR00
	float fPowerFactor;
	float fFrontTireMAX;
	float fRearTireMAX;
	float fBreakingForce;
	float fYCarPosition;
	float fSuspensionKU;
	float fSuspensionKV;
	char  szBodyMesh[64] = {};
	char  szWheelMesh[64] = {};
	float fWheelRadius;
	float fFrontWheelPos;
	float fRearWheelPos;
	float fBodyWidth;
	float fFrontBumper;
	float fRearBumper;
	int   bFrontWheelDrive;
	int   unk_1;
	int   unk_2;
	float fFrontLight[3];
	float fRearLight[3];
	float fSideLight[3];
	float fReverseLight[3];
	float fStopLight[3];
	int   unk_3;
	int   unk_4;
};


class EPolCarFile {
public:
	bool ProcessToINI();
	bool ProcessToCAR();
};