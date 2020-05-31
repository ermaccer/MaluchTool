#include "polcarfile.h"
#include <string>
#include "depak.h"
#include <Windows.h>
#include <filesystem>
#include <vector>
#include "IniReader.h"

bool EPolCarFile::ProcessToINI()
{
	std::wstring inPath = SetPathFromButton(L"Car Information (*.car)\0*.car\0All Files (*.*)\0*.*\0", L"car", 0);
	std::ifstream pFile(inPath, std::ifstream::binary);

	if (!pFile && inPath.length() > 0)
	{
		MessageBox(0, L"Failed to open input file!", 0, MB_ICONWARNING);
		return false;
	}

	if (pFile)
	{
		polengine_car car;
		pFile.read((char*)&car, sizeof(polengine_car));

		if (!(strcmp(car.header, "MCAR00") == 0))
		{
			MessageBox(0, L"This is not a valid car file!", 0, MB_ICONWARNING);
			return false;
		}

		std::wstring out = SetSavePathFromButton(L"INI (*.ini)\0*.ini\0All Files (*.*)\0*.*\0", L"ini", 0);
	
		CIniReader ini(out.c_str());


		ini.WriteFloat(L"Car", L"fPowerFactor",   car.fPowerFactor);
		ini.WriteFloat(L"Car", L"fFrontTireMAX",  car.fFrontTireMAX);
		ini.WriteFloat(L"Car", L"fRearTireMAX",   car.fRearTireMAX);
		ini.WriteFloat(L"Car", L"fBreakingForce", car.fBreakingForce);
		ini.WriteFloat(L"Car", L"fYCarPosition",  car.fYCarPosition);
		ini.WriteFloat(L"Car", L"fSuspensionKU",  car.fSuspensionKU);
		ini.WriteFloat(L"Car", L"fSuspensionKV",  car.fSuspensionKV);

		std::string body(car.szBodyMesh, strlen(car.szBodyMesh));
		std::wstring wstr_body(body.length(), L' ');
		std::copy(body.begin(), body.end(), wstr_body.begin());

		ini.WriteString(L"Car", L"szBodyMesh", wstr_body.c_str());


		std::string wheel(car.szWheelMesh, strlen(car.szWheelMesh));
		std::wstring wstr_wheel(wheel.length(), L' ');
		std::copy(wheel.begin(), wheel.end(), wstr_wheel.begin());

		ini.WriteString(L"Car", L"szWheelMesh", wstr_wheel.c_str());
		ini.WriteFloat(L"Car", L"fWheelRadius", car.fWheelRadius);
		ini.WriteFloat(L"Car", L"fFrontWheelPos", car.fFrontWheelPos);
		ini.WriteFloat(L"Car", L"fRearWheelPos", car.fRearWheelPos);
		ini.WriteFloat(L"Car", L"fBodyWidth", car.fBodyWidth);

		ini.WriteFloat(L"Car",   L"fFrontBumper",     car.fFrontBumper);
		ini.WriteFloat(L"Car",   L"fRearBumper",      car.fRearBumper);
		ini.WriteInteger(L"Car", L"bFrontWheelDrive", car.bFrontWheelDrive);



		ini.WriteFloat(L"Car", L"fFrontLightX",   car.fFrontLight[0]);
		ini.WriteFloat(L"Car", L"fFrontLightY",   car.fFrontLight[1]);
		ini.WriteFloat(L"Car", L"fFrontLightZ",   car.fFrontLight[2]);


		ini.WriteFloat(L"Car", L"fRearLightX",    car.fRearLight[0]);
		ini.WriteFloat(L"Car", L"fRearLightY",    car.fRearLight[1]);
		ini.WriteFloat(L"Car", L"fRearLightZ",    car.fRearLight[2]);

		ini.WriteFloat(L"Car", L"fSideLightX",    car.fSideLight[0]);
		ini.WriteFloat(L"Car", L"fSideLightY",    car.fSideLight[1]);
		ini.WriteFloat(L"Car", L"fSideLightZ",    car.fSideLight[2]);

		ini.WriteFloat(L"Car", L"fReverseLightX", car.fReverseLight[0]);
		ini.WriteFloat(L"Car", L"fReverseLightY", car.fReverseLight[1]);
		ini.WriteFloat(L"Car", L"fReverseLightZ", car.fReverseLight[2]);

		ini.WriteFloat(L"Car", L"fStopLightX",    car.fStopLight[0]);
		ini.WriteFloat(L"Car", L"fStopLightY",    car.fStopLight[1]);
		ini.WriteFloat(L"Car", L"fStopLightZ",    car.fStopLight[2]);


		ini.WriteInteger(L"Car", L"unk1", car.unk_1);
		ini.WriteInteger(L"Car", L"unk2", car.unk_2);
		ini.WriteInteger(L"Car", L"unk3", car.unk_3);
		ini.WriteInteger(L"Car", L"unk4", car.unk_4);

		return true;
	
	}
}

bool EPolCarFile::ProcessToCAR()
{

	std::wstring inPath = SetPathFromButton(L"INI (*.ini)\0*.ini\0All Files (*.*)\0*.*\0", L"ini", 0);
	std::ifstream pFile(inPath, std::ifstream::binary);

	if (!pFile && inPath.length() > 0)
	{
		MessageBox(0, L"Failed to open input file!", 0, MB_ICONWARNING);
		return false;
	}

	if (pFile)
	{
		std::wstring out = SetSavePathFromButton(L"Car Information (*.car)\0*.car\0All Files (*.*)\0*.*\0", L"car", 0);

		CIniReader ini(inPath.c_str());


		std::ofstream oFile(out, std::ofstream::binary);
		polengine_car car;
		sprintf(car.header, "MCAR00");
		car.fPowerFactor = ini.ReadFloat(L"Car", L"fPowerFactor", 0);
		car.fFrontTireMAX = ini.ReadFloat(L"Car", L"fFrontTireMAX", 0);
		car.fRearTireMAX = ini.ReadFloat(L"Car", L"fRearTireMAX", 0);
		car.fBreakingForce = ini.ReadFloat(L"Car", L"fBreakingForce", 0);
		car.fYCarPosition = ini.ReadFloat(L"Car", L"fYCarPosition", 0);
		car.fSuspensionKU = ini.ReadFloat(L"Car", L"fSuspensionKU", 0);
		car.fSuspensionKV = ini.ReadFloat(L"Car", L"fSuspensionKV", 0);

		std::wstring wstr = ini.ReadString(L"Car", L"szBodyMesh", 0);
		std::string body(wstr.length(), ' ');
		std::copy(wstr.begin(), wstr.end(), body.begin());

		sprintf(car.szBodyMesh, body.c_str());

		wstr = ini.ReadString(L"Car", L"szWheelMesh", 0);
		std::string wheel(wstr.length(), ' ');
		std::copy(wstr.begin(), wstr.end(), wheel.begin());

		sprintf(car.szWheelMesh, wheel.c_str());


		car.fWheelRadius = ini.ReadFloat(L"Car", L"fWheelRadius", 0);
		car.fFrontWheelPos = ini.ReadFloat(L"Car", L"fFrontWheelPos", 0);
		car.fRearWheelPos = ini.ReadFloat(L"Car", L"fRearWheelPos", 0);
		car.fBodyWidth = ini.ReadFloat(L"Car", L"fBodyWidth", 0);




		car.fFrontBumper = ini.ReadFloat(L"Car", L"fFrontBumper", 0);
		car.fRearBumper = ini.ReadFloat(L"Car", L"fRearBumper", 0);
		car.bFrontWheelDrive = ini.ReadInteger(L"Car", L"bFrontWheelDrive", 0);

		car.fFrontLight[0] = ini.ReadFloat(L"Car", L"fFrontLightX", 0);
		car.fFrontLight[1] = ini.ReadFloat(L"Car", L"fFrontLightY", 0);
		car.fFrontLight[2] = ini.ReadFloat(L"Car", L"fFrontLightZ", 0);


		car.fRearLight[0] = ini.ReadFloat(L"Car", L"fRearLightX", 0);
		car.fRearLight[1] = ini.ReadFloat(L"Car", L"fRearLightY", 0);
		car.fRearLight[2] = ini.ReadFloat(L"Car", L"fRearLightZ", 0);

		car.fSideLight[0] = ini.ReadFloat(L"Car", L"fSideLightX", 0);
		car.fSideLight[1] = ini.ReadFloat(L"Car", L"fSideLightY", 0);
		car.fSideLight[2] = ini.ReadFloat(L"Car", L"fSideLightZ", 0);

		car.fReverseLight[0] = ini.ReadFloat(L"Car", L"fReverseLightX", 0);
		car.fReverseLight[1] = ini.ReadFloat(L"Car", L"fReverseLightY", 0);
		car.fReverseLight[2] = ini.ReadFloat(L"Car", L"fReverseLightZ", 0);

		car.fStopLight[0] = ini.ReadFloat(L"Car", L"fStopLightX", 0);
		car.fStopLight[1] = ini.ReadFloat(L"Car", L"fStopLightY", 0);
		car.fStopLight[2] = ini.ReadFloat(L"Car", L"fStopLightZ", 0);

		car.unk_1 = ini.ReadInteger(L"Car", L"unk1", 0);
		car.unk_2 = ini.ReadInteger(L"Car", L"unk2", 0);
		car.unk_3 = ini.ReadInteger(L"Car", L"unk3", 0);
		car.unk_4 = ini.ReadInteger(L"Car", L"unk4", 0);

		oFile.write((char*)&car, sizeof(polengine_car));

		return true;
	}

	return false;
}
