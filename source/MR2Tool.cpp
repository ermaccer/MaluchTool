// MR2Tool.cpp : Defines the entry point for the application.
//


#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "framework.h"
#include "MR2Tool.h"
#include "depak.h"
#include "carfile.h"
#include "mar.h"
#include "pthfile.h"
#include "polcarfile.h"

HINSTANCE hInst;                                
      

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    mr2main(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AboutBox(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	DialogBox(hInst, MAKEINTRESOURCE(MR2TOOL_MAIN), 0, mr2main);
}

INT_PTR CALLBACK mr2main(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND FileName = GetDlgItem(hDlg, MR2_ANAME);
	HWND InputPath = GetDlgItem(hDlg, MR2_INPUT);
	HWND OutputPath = GetDlgItem(hDlg, MR2_INPUT2);
	HWND ProgressBar = GetDlgItem(hDlg, MR2_BAR);
	HWND GameDropList = GetDlgItem(hDlg, MR2_CLIST);
	HANDLE hicon = 0;
	LRESULT GameResult;
	int iWorkMode = 0, iGameIdentifier = 0;
	const wchar_t* gameNames[SUPPORTED_GAMES] = { L"Pol Engine (.mar)",L"Blitz3D (.dat)" };
	switch (message)
	{
	case WM_INITDIALOG:
		 for (int i = 0; i < SUPPORTED_GAMES;i++)
			SendMessage(GameDropList, CB_ADDSTRING, 0, (LPARAM)gameNames[i]);
		 SendMessage(GameDropList, CB_SETCURSEL, 0, 0);
		 SendMessage(GetDlgItem(hDlg, MR2_M_E), BM_SETCHECK, 1, 0);
		 EnableWindow(GetDlgItem(hDlg, MR2_M_B), 0);
		 hicon = LoadImage(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
		 SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hicon);
		return (INT_PTR)TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, LOWORD(wParam));
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		GameResult = SendMessage(GameDropList, CB_GETCURSEL, 0, 0);
		iGameIdentifier = GameResult;
		if (IsDlgButtonChecked(hDlg, MR2_M_E))
			iWorkMode = MODE_EXTRACT;

		if (IsDlgButtonChecked(hDlg, MR2_M_B))
			iWorkMode = MODE_CREATE;

		if (iGameIdentifier == GAME_B3D)
		{
			SendMessage(GetDlgItem(hDlg, MR2_M_B), BM_SETCHECK, 0, 0);
			EnableWindow(GetDlgItem(hDlg, MR2_M_B), 0);
			SendMessage(GetDlgItem(hDlg, MR2_M_E), BM_SETCHECK, 1, 0);
		}
		else
			EnableWindow(GetDlgItem(hDlg, MR2_M_B), 1);

		if (iGameIdentifier == GAME_POL)
		{
			EnableWindow(GetDlgItem(hDlg, MR2_ENC),0);
		}
		else
			EnableWindow(GetDlgItem(hDlg, MR2_ENC),1);

		if (iWorkMode == MODE_CREATE)
		{
			SetWindowText(GetDlgItem(hDlg, text1), L"Input folder");
			SetWindowText(GetDlgItem(hDlg, text2), L"Output file");
		}
		if (iWorkMode == MODE_EXTRACT)
		{
			SetWindowText(GetDlgItem(hDlg, text1), L"Input file");
			SetWindowText(GetDlgItem(hDlg, text2), L"Output folder");
		}

		if (wParam == IDM_ABOUT)
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, AboutBox);
		if (wParam == MR2_LOAD)
		{
			if (iWorkMode == MODE_EXTRACT)
			{
				if (iGameIdentifier == GAME_B3D)
				SetWindowText(InputPath, SetPathFromButton(L"Archive (*.dat)\0*.dat\0All Files (*.*)\0*.*\0", L"dat", hDlg).c_str());
				if (iGameIdentifier == GAME_POL)
				SetWindowText(InputPath, SetPathFromButton(L"Archive (*.mar)\0*.mar\0All Files (*.*)\0*.*\0", L"mar", hDlg).c_str());
			}
			if (iWorkMode == MODE_CREATE)
			{
				if (iGameIdentifier == GAME_POL)
					SetWindowText(InputPath, SetFolderFromButton(hDlg).c_str());
			}

		}

		if (wParam == MR2_FOLDER)
		{
			if (iWorkMode == MODE_EXTRACT)
				SetWindowText(OutputPath, SetFolderFromButton(hDlg).c_str());
			if (iWorkMode == MODE_CREATE)
			{
				if (iGameIdentifier == GAME_POL)
					SetWindowText(OutputPath, SetSavePathFromButton(L"Archive (*.mar)\0*.mar\0All Files (*.*)\0*.*\0", L"mar", hDlg).c_str());
			}
		}

		if (wParam == MR2_EXTRACT)
		{
			if (GetWindowTextLength(InputPath) == 0)
			{
				MessageBox(0, L"No input file specified!", 0, 0);
				break;
			}

			if (GetWindowTextLength(OutputPath) == 0)
			{
				MessageBox(0, L"No output path specified!", 0, 0);
				break;
			}

			wchar_t szInPath[MAX_PATH] = {}, szOutPath[MAX_PATH] = {};

			// get input
			GetWindowText(InputPath, szInPath, GetWindowTextLength(InputPath) + 1);
			// get output
			GetWindowText(OutputPath, szOutPath, GetWindowTextLength(OutputPath) + 1);

			if (iGameIdentifier == GAME_B3D)
			{
				EPakFile* pak = new EPakFile(szInPath, szOutPath, IsDlgButtonChecked(hDlg, MR2_ENC));
				pak->AttachFilenameText(&FileName);
				pak->AttachProgressBar(&ProgressBar);
				if (pak->Process())
					MessageBox(0, L"Finished!", L"Information", MB_ICONINFORMATION);
			}
			if (iGameIdentifier == GAME_POL)
			{
				EMarFile* mar = new EMarFile(szInPath, szOutPath, iWorkMode);
				mar->AttachFilenameText(&FileName);
				mar->AttachProgressBar(&ProgressBar);
				if (mar->Process())
					MessageBox(0, L"Finished!", L"Information", MB_ICONINFORMATION);
			}


	       
			SetWindowText(GetDlgItem(hDlg, MR2_ANAME), L"Idle");

		}

		if (wParam == ID_FILE_EXIT)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (wParam == ID_FILE_DECRYPTCARINFO)
		{
			ECarFile* car = new ECarFile();
			car->Process();
		}

		if (wParam == ID_PTH2TXT)
		{
			EPathFile* pth = new EPathFile();
			if (pth->ProcessToText())
				MessageBox(0, L"Finished!", L"Information", MB_ICONINFORMATION);
		}

		if (wParam == ID_TXT2PTH)
		{
			EPathFile* pth = new EPathFile();
			if (pth->ProcessToPath())
				MessageBox(0, L"Finished!", L"Information", MB_ICONINFORMATION);
		}

		if (wParam == ID_CAR2INI)
		{
			EPolCarFile* car = new EPolCarFile();
			if (car->ProcessToINI())
				MessageBox(0, L"Finished!", L"Information", MB_ICONINFORMATION);
		}

		if (wParam == ID_INI2CAR)
		{
			EPolCarFile* car = new EPolCarFile();
			if (car->ProcessToCAR())
				MessageBox(0, L"Finished!", L"Information", MB_ICONINFORMATION);
		}


		break;
	}

	return (INT_PTR)FALSE;
}



INT_PTR CALLBACK AboutBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
