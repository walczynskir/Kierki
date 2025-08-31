// OptionsViewDlg.cpp : view option dialog handlers
//
#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"
#include "OptionsViewDlg.h"
#include "RBitmapChoiceDlg.h"
#include "layout.h"
#include "RChooseBmpWnd.h"
#include <rcommon/RColorWnd.h>

inline static void OnInitDialog(HWND a_hDlg, RRegData::RViewRegData* a_pData);
inline static void OnCommand(HWND a_hDlg, UINT a_idCtrl, UINT a_idCmd);
static INT_PTR CALLBACK OptionsViewDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void SetCtrlValues(HWND a_hDlg);
static RRegData::RViewRegData* GetRData(HWND a_hDlg);
inline static void GetCtrlValues(HWND a_hDlg);
static void MoveDlgItem(HWND a_hDlg, UINT a_idItem, LONG a_x, LONG a_y);
static void ResizeDlgItem(HWND a_hDlg, UINT a_idItem, LONG a_dx, LONG a_dy);



HWND CreateOptViewDlg(HWND a_hParent, RRegData::RBaseRegData* a_pData, LPVOID a_pObj)
{
	RColorWnd_RegisterClass();
	RChooseBmpWnd_RegisterClass();
	return ::CreateDialogParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OPT_VIEW), 
		a_hParent, OptionsViewDlgProc, reinterpret_cast<LPARAM>(a_pData));
}


static void OnInitDialog(HWND a_hDlg, RRegData::RViewRegData* a_pData)
{
	::SetWindowLongPtr(a_hDlg, GWL_USERDATA, reinterpret_cast<LONG_PTR>(a_pData));
	HWND l_hWndImage = ::GetDlgItem(a_hDlg, IDC_VIEW_COVER);
	RChooseBmpWnd_SetPictureSize(l_hWndImage, c_dxCard, c_dyCard);
	SIZE l_size;
	RChooseBmpWnd_GetMinSize(l_hWndImage, &l_size);
	::SetWindowPos(l_hWndImage, NULL, 0, 0, l_size.cx, l_size.cy, 
		SWP_NOZORDER | SWP_NOMOVE);

	RECT l_rectImage;
	::GetWindowRect(l_hWndImage, &l_rectImage);
	POINT l_ptImage = { l_rectImage.left, l_rectImage.top };
	::ScreenToClient(a_hDlg, &l_ptImage);

	LONG l_xButtons = l_ptImage.x + l_size.cx + 7;
	MoveDlgItem(a_hDlg, IDC_CHANGECOVER, l_xButtons, l_ptImage.y);
/*	::SetWindowPos(::GetDlgItem(a_hDlg, IDC_CHANGECOVER), NULL,
		l_xButtons, l_ptImage.y, 
		0, 0, 
		SWP_NOZORDER | SWP_NOSIZE);
*/	
	MoveDlgItem(a_hDlg, IDC_VIEW_DEFUALTTABLE, l_xButtons, 0);
	MoveDlgItem(a_hDlg, IDC_VIEW_DEFUALTRESULT, l_xButtons, 0);

	ResizeDlgItem(a_hDlg, IDC_VIEW_TABLECOLOR, l_size.cx, 0);
	ResizeDlgItem(a_hDlg, IDC_VIEW_RESULTCOLOR, l_size.cx, 0);

	SetCtrlValues(a_hDlg);
}


static INT_PTR CALLBACK OptionsViewDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(a_hDlg, reinterpret_cast<RRegData::RViewRegData*>(a_lParam));
		return TRUE;

	case WM_GETVALUES:
		GetCtrlValues(a_hDlg);
		::SetWindowLong(a_hDlg, DWL_MSGRESULT, TRUE);
		return TRUE; // must be TRUE too return proper LRESULT

	case WM_COMMAND:
		OnCommand(a_hDlg, LOWORD(a_wParam), HIWORD(a_wParam));
		break;
	}
	return FALSE;
}


void OnCommand(HWND a_hDlg, UINT a_idCtrl, UINT a_idCmd)
{
	switch (a_idCtrl)
	{
	case IDC_CHANGECOVER:
		if (a_idCmd == BN_CLICKED)
		{
			RRegData::RViewRegData* l_pData = GetRData(a_hDlg);

			if (ChooseBitmap_DoModal(a_hDlg, &(l_pData->m_resCover)))
			{
				RChooseBmpWnd_SetChosen(::GetDlgItem(a_hDlg, IDC_VIEW_COVER), &(l_pData->m_resCover));
			}
		}
		break;

	case IDC_VIEW_DEFUALTTABLE:
		if (a_idCmd == BN_CLICKED)
		{
			RRegData::RViewRegData* l_pData = GetRData(a_hDlg);
			HWND l_hWndTable = ::GetDlgItem(a_hDlg, IDC_VIEW_TABLECOLOR);
			RColorWnd_SetColor(l_hWndTable, l_pData->GetDefaultTableColor(), FALSE);
			::RedrawWindow(l_hWndTable, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}		
		break;

	case IDC_VIEW_DEFUALTRESULT:
		if (a_idCmd == BN_CLICKED)
		{
			RRegData::RViewRegData* l_pData = GetRData(a_hDlg);
			HWND l_hWndResult = ::GetDlgItem(a_hDlg, IDC_VIEW_RESULTCOLOR);
			RColorWnd_SetColor(l_hWndResult, l_pData->GetDefaultResultColor(), FALSE);
			::RedrawWindow(l_hWndResult, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}		
		break;
	}
}

void SetCtrlValues(HWND a_hDlg)
{
	RRegData::RViewRegData* l_pData = GetRData(a_hDlg);
	RColorWnd_SetColor(::GetDlgItem(a_hDlg, IDC_VIEW_TABLECOLOR), l_pData->m_colorTable, TRUE);
	RColorWnd_SetColor(::GetDlgItem(a_hDlg, IDC_VIEW_RESULTCOLOR), l_pData->m_colorResult, TRUE);
	RChooseBmpWnd_SetChosen(::GetDlgItem(a_hDlg, IDC_VIEW_COVER), &(l_pData->m_resCover));
}


static RRegData::RViewRegData* GetRData(HWND a_hDlg)
{
	return reinterpret_cast<RRegData::RViewRegData*>(::GetWindowLong(a_hDlg, GWL_USERDATA));
}


static void GetCtrlValues(HWND a_hDlg)
{
	RRegData::RViewRegData* l_pData = GetRData(a_hDlg);
	l_pData->m_colorTable = RColorWnd_GetColor(::GetDlgItem(a_hDlg, IDC_VIEW_TABLECOLOR));
	l_pData->m_colorResult = RColorWnd_GetColor(::GetDlgItem(a_hDlg, IDC_VIEW_RESULTCOLOR));

	RChooseBmpWnd_GetChosen(::GetDlgItem(a_hDlg, IDC_VIEW_COVER), &(l_pData->m_resCover));
}


static void MoveDlgItem(HWND a_hDlg, UINT a_idItem, LONG a_x, LONG a_y)
{
	
	HWND l_hWndItem = ::GetDlgItem(a_hDlg, a_idItem);
	RECT l_rcItem;
	::GetWindowRect(l_hWndItem, &l_rcItem);						// Get screen coordinates
	::MapWindowPoints(NULL, a_hDlg, (LPPOINT)&l_rcItem, 2);		// Convert to dialog-relative coordinates

	l_rcItem.left = (a_x > 0) ? a_x : l_rcItem.left;
	l_rcItem.top = (a_y > 0) ? a_y : l_rcItem.top;
	::SetWindowPos(l_hWndItem, NULL, l_rcItem.left, l_rcItem.top, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
}


static void ResizeDlgItem(HWND a_hDlg, UINT a_idItem, LONG a_dx, LONG a_dy)
{
	HWND l_hWndItem = ::GetDlgItem(a_hDlg, a_idItem);
	RECT l_rcItem;
	::GetWindowRect(l_hWndItem, &l_rcItem);						// Get screen coordinates
	::MapWindowPoints(NULL, a_hDlg, (LPPOINT)&l_rcItem, 2);		// Convert to dialog-relative coordinates

	l_rcItem.left = (a_dx > 0) ? a_dx : l_rcItem.left;
	l_rcItem.top = (a_dy > 0) ? a_dy : l_rcItem.top;

	LONG l_dx = (a_dx > 0) ? a_dx : l_rcItem.right - l_rcItem.left;
	LONG l_dy = (a_dy > 0) ? a_dy : l_rcItem.bottom - l_rcItem.top;

	::SetWindowPos(l_hWndItem, NULL, 0, 0, l_dx, l_dy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
}

