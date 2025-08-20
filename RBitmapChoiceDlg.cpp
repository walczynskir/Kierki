// RBitmapChoiceDlg.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "RBitmapChoiceDlg.h"
#include "layout.h"
#include <Commdlg.h>
#include <dlgs.h>
#include "RChooseBmpWnd.h"

static inline HINSTANCE LoadTemplate();
static inline void UnloadTemplate(HGLOBAL a_hRes);

static UINT_PTR CALLBACK ChooseBitmapHookProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
static inline void OnInitDone(HWND a_hDlg, LPOFNOTIFY a_pNotify);
static inline void OnSelChange(HWND a_hDlg, LPOFNOTIFY a_pNotify);
static inline void OnFolderChange(HWND a_hDlg);
static inline void OnOK(HWND a_hDlg, LPOFNOTIFY a_pOfn);


BOOL ChooseBitmap_DoModal(HWND a_hWndParent, LPRCOVERRES a_pRes)
{
	OPENFILENAME l_ofn;
	ZeroMemory(&l_ofn, sizeof(l_ofn));
	l_ofn.lStructSize = sizeof(l_ofn);
	l_ofn.hwndOwner = a_hWndParent;

	TCHAR l_sFilter[2048];
	ZeroMemory(l_sFilter, sizeof(l_sFilter));
	::LoadString(::GetModuleHandle(NULL), IDS_CHOOSEBMP_FILTER, l_sFilter, ArraySize(l_sFilter));

	l_ofn.lpstrFilter = l_sFilter;

	l_ofn.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_FILEMUSTEXIST | 
		OFN_ENABLETEMPLATEHANDLE | OFN_HIDEREADONLY | OFN_ENABLESIZING;
	l_ofn.hInstance = LoadTemplate();
	l_ofn.lpfnHook = ChooseBitmapHookProc;
	TCHAR l_sFile[1024];
	_tcsncpy_s(l_sFile, ArraySize(l_sFile), a_pRes->psPath, a_pRes->iPathSize);
	l_ofn.lpstrFile = l_sFile;
	l_ofn.nMaxFile = ArraySize(l_sFile);

	TCHAR l_sTitle[128];
	ZeroMemory(l_sTitle, sizeof(l_sTitle));
	::LoadString(::GetModuleHandle(NULL), IDS_CHOOSEBMP_TITLE, l_sTitle, ArraySize(l_sTitle));
	l_ofn.lpstrTitle = l_sTitle;

	RChooseBmpWnd_RegisterClass();
	l_ofn.lCustData = reinterpret_cast<LPARAM>(a_pRes);
	BOOL l_bOk = ::GetOpenFileName(&l_ofn);

	UnloadTemplate(reinterpret_cast<HGLOBAL>(l_ofn.hInstance));
	return l_bOk;
}


// ---------------------------------------------------------
// Za³adowanie template'a dialogu
//
HINSTANCE LoadTemplate()
{
	HRSRC l_hRes = ::FindResource(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DLLBITMAP), RT_DIALOG);
	HGLOBAL l_hDlgTempl = ::LoadResource(::GetModuleHandle(NULL), l_hRes);
	LPCDLGTEMPLATE l_pDlgTempl = NULL;
	if (l_hDlgTempl != NULL)
	{
		l_pDlgTempl = static_cast<LPCDLGTEMPLATE>(::LockResource(l_hDlgTempl));
	}
	return (HINSTANCE)l_pDlgTempl;
}


void UnloadTemplate(HGLOBAL a_hRes)
{
	UnlockResource(a_hRes);
}


UINT_PTR CALLBACK ChooseBitmapHookProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_NOTIFY:
		{
			OFNOTIFY* l_pNotify = (OFNOTIFY*)a_lParam;
			switch (l_pNotify->hdr.code)
			{
			case CDN_INITDONE:
				{
					OnInitDone(a_hDlg, l_pNotify);
					return TRUE;
				}
			case CDN_SELCHANGE:
				{
					OnSelChange(a_hDlg, l_pNotify);
					return TRUE;
				}
			case CDN_FOLDERCHANGE:
				{
					OnFolderChange(a_hDlg);
					return TRUE;
				}
			case CDN_FILEOK:
				{
					OnOK(a_hDlg, l_pNotify);
					return TRUE;
				}
				break;
			}
		}
	}
	return FALSE;
}


void OnInitDone(HWND a_hDlg, LPOFNOTIFY a_pNotify)
{
	HWND l_hWndParent = ::GetParent(a_hDlg);
	HWND l_hWndList = ::GetDlgItem(l_hWndParent, lst1);
	RECT l_rectList;
	::GetWindowRect(l_hWndList, &l_rectList);
	POINT l_ptList = { l_rectList.left, l_rectList.top };
	::ScreenToClient(a_hDlg, &l_ptList);

	HWND l_hWndImage = ::GetDlgItem(a_hDlg, IDC_IMAGE);
	RChooseBmpWnd_SetPictureSize(l_hWndImage, c_dxCard, c_dyCard);
	SIZE l_size;
	RChooseBmpWnd_GetMinSize(l_hWndImage, &l_size);
	RECT l_rectImage;
	::GetWindowRect(l_hWndImage, &l_rectImage);
	POINT l_ptImage = { l_rectImage.left, l_rectImage.top };
	::ScreenToClient(a_hDlg, &l_ptImage);
	::SetWindowPos(l_hWndImage, NULL, l_ptImage.x, l_ptList.y, l_size.cx, l_size.cy, SWP_NOZORDER);

	TCHAR l_sOkText[128];
	::LoadString(::GetModuleHandle(NULL), IDS_CHOOSEBMP_SELBTN, l_sOkText, ArraySize(l_sOkText));
	::SetWindowText(::GetDlgItem(l_hWndParent, IDOK), l_sOkText);

	RChooseBmpWnd_SetChosen(l_hWndImage, reinterpret_cast<LPRCOVERRES>(a_pNotify->lpOFN->lCustData));
}


void OnSelChange(HWND a_hDlg, LPOFNOTIFY a_pNotify)
{
	HWND l_hWndImage = ::GetDlgItem(a_hDlg, IDC_IMAGE);
	TCHAR l_sFile[1024];
	CommDlg_OpenSave_GetSpec(::GetParent(a_hDlg), l_sFile, ArraySize(l_sFile));
	if (_tcslen(l_sFile) > 0)
	{
		RCOVERRES l_resCover;
		RCoverRes_Init(&l_resCover);
		RChooseBmpWnd_GetChosen(l_hWndImage, &l_resCover);

		TCHAR l_sChosenPath[MAX_PATH];
		CommDlg_OpenSave_GetFilePath(::GetParent(a_hDlg), l_sChosenPath, ArraySize(l_sChosenPath));
		if ((l_resCover.psPath == NULL) || (_tcsnicmp(l_resCover.psPath, l_sChosenPath, ArraySize(l_sChosenPath)) != 0))
		{
			RChooseBmpWnd_SetFile(l_hWndImage, l_sChosenPath);
		}
		RCoverRes_Done(&l_resCover);
	}
}


void OnFolderChange(HWND a_hDlg)
{
	TCHAR l_sFolder[MAX_PATH];
	CommDlg_OpenSave_GetFolderPath(::GetParent(a_hDlg), l_sFolder, ArraySize(l_sFolder));
	RCOVERRES l_resCover;
	RCoverRes_Init(&l_resCover);

	HWND l_hWndImage = ::GetDlgItem(a_hDlg, IDC_IMAGE);
	RChooseBmpWnd_GetChosen(l_hWndImage, &l_resCover);


	LPCTSTR l_psPathEnd = NULL;
	if (l_resCover.psPath != NULL)
	{
		l_psPathEnd = _tcsrchr(l_resCover.psPath, _T('\\'));
	}

	if ((l_psPathEnd == NULL) || (_tcsnicmp(l_sFolder, l_resCover.psPath, l_psPathEnd - l_resCover.psPath) != 0))
	{
		::SendMessage(::GetParent(a_hDlg), CDM_SETCONTROLTEXT, 
			edt1, reinterpret_cast<LPARAM>(_T("")));
		RChooseBmpWnd_SetFile(l_hWndImage, _T(""));
	}
	RCoverRes_Done(&l_resCover);
}


void OnOK(HWND a_hDlg, LPOFNOTIFY a_pOfn)
{
	HWND l_hWndImage = ::GetDlgItem(a_hDlg, IDC_IMAGE);
	RChooseBmpWnd_GetChosen(l_hWndImage, 
		reinterpret_cast<LPRCOVERRES>(a_pOfn->lpOFN->lCustData));
}
