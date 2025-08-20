#pragma once
// RChooseBmpWnd.h: interface for the RChooseBmpWnd window.
//
//////////////////////////////////////////////////////////////////////
#include "coverres.h"

#define RChooseBmpWnd_ClassName _T("RCHOOSEBMPWND")


ATOM RChooseBmpWnd_RegisterClass();
HWND RChooseBmpWnd_CreateEx(DWORD a_iStyle, DWORD a_iStyleEx, int a_x, int a_y, int a_iWidth,
	int a_iHeight, HWND	a_hWndParent, LPVOID a_lpParam);
LRESULT CALLBACK RChooseBmpWnd_WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);

// sets file WPARAM = unused, LPARAM = file (LPCTSTR), LRESULT = success (BOOL)
#define RCBM_SETFILE (WM_USER + 1)
#define RChooseBmpWnd_SetFile(a_hWnd, a_sFile) (::SendMessage(a_hWnd, RCBM_SETFILE, 0, reinterpret_cast<LPARAM>(a_sFile)))

// sets picture size WPARAM = dx, LPARAM = dy (LPCTSTR), LRESULT = unused
#define RCBM_SETPICTURESIZE (WM_USER + 2)
#define RChooseBmpWnd_SetPictureSize(a_hWnd, a_dx, a_dy) (::SendMessage(a_hWnd, RCBM_SETPICTURESIZE, static_cast<WPARAM>(a_dx), static_cast<LPARAM>(a_dy)))

// gets min windows size WPARAM = unused, LPARAM = a_pSize (LPSIZE), LRESULT = unused
#define RCBM_GETMINSIZE (WM_USER + 3)
#define RChooseBmpWnd_GetMinSize(a_hWnd, a_pSize) (::SendMessage(a_hWnd, RCBM_GETMINSIZE, 0, reinterpret_cast<LPARAM>(a_pSize)))

// gets chosen file and index WPARAM = unused, LPARAM = a_pData (LPRCOVERRES), LRESULT = chosen (BOOL)
#define RCBM_GETCHOSEN (WM_USER + 4)
#define RChooseBmpWnd_GetChosen(a_hWnd, a_pData) (::SendMessage(a_hWnd, RCBM_GETCHOSEN, 0, reinterpret_cast<LPARAM>(a_pData)))

// sets chosen file and index WPARAM = unused, LPARAM = a_pData (LPRCOVERRES), LRESULT = chosen (BOOL)
#define RCBM_SETCHOSEN (WM_USER + 5)
#define RChooseBmpWnd_SetChosen(a_hWnd, a_pData) (::SendMessage(a_hWnd, RCBM_SETCHOSEN, 0, reinterpret_cast<LPARAM>(a_pData)))

#define RCBS_PICTUREONLY (0x01)