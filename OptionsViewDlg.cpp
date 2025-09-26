3// OptionsViewDlg.cpp : view option dialog handlers
//
#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"
#include "OptionsViewDlg.h"
#include "RSelectBitmap.h"
#include "layout.h"
#include <RCards/RCards.h>
#include <RCards/resource.h>
#include <rcommon/RSystemExc.h>
#include <rcommon/ROwnExc.h>



#pragma todo ("put static for selecting covers in rcommon library")
#pragma todo("restore possibility of taking bitmap from other resources")
#pragma todo ("should be a part of RCards module, to be refactored") 





class ROptionsViewData
{
public:
	ROptionsViewData(RRegData::RBaseRegData* a_pRegData) : m_pRegViewData(reinterpret_cast<RRegData::RViewRegData*>(a_pRegData))
	{

	}
	~ROptionsViewData() = default;

	RRegData::RViewRegData* GetRegData() const { return m_pRegViewData; }

	HWND  m_hFelt = nullptr;
	HWND  m_hCover = nullptr;

private:
	RRegData::RViewRegData* m_pRegViewData;

};



inline static void OnInitDialog(HWND a_hDlg, ROptionsViewData* a_pData);
inline static void OnCommand(HWND a_hDlg, UINT a_idCtrl, UINT a_idCmd);

inline static void OnNcDestroy(HWND a_hDlg);

static INT_PTR CALLBACK OptionsViewDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void SetCtrlValues(HWND a_hDlg);


static ROptionsViewData* GetRData(HWND a_hDlg);
inline static void GetCtrlValues(HWND a_hDlg);
static void MoveDlgItem(HWND a_hDlg, UINT a_idItem, LONG a_x, LONG a_y);
static void ResizeDlgItem(HWND a_hDlg, UINT a_idItem, LONG a_dx, LONG a_dy);


HWND CreateOptViewDlg(HWND a_hParent, RRegData::RBaseRegData* a_pData, LPVOID a_pObj)
{
	ROptionsViewData* l_pViewData = new ROptionsViewData(a_pData);
	RSelectBitmap_RegisterClass(::GetModuleHandle(NULL));

	return ::CreateDialogParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OPT_VIEW), 
		a_hParent, OptionsViewDlgProc, reinterpret_cast<LPARAM>(l_pViewData));
}


static INT_PTR CALLBACK OptionsViewDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	try
	{
		switch (a_iMsg)
		{
		case WM_INITDIALOG:
			OnInitDialog(a_hDlg, reinterpret_cast<ROptionsViewData*>(a_lParam));
			return TRUE;

		case WM_GETVALUES:
			GetCtrlValues(a_hDlg);
			::SetWindowLong(a_hDlg, DWL_MSGRESULT, TRUE);
			return TRUE; // must be TRUE too return proper LRESULT

		case WM_COMMAND:
			OnCommand(a_hDlg, LOWORD(a_wParam), HIWORD(a_wParam));
			break;

		case WM_NCDESTROY:
			OnNcDestroy(a_hDlg);
			return TRUE;
		}
	}
	catch (const RSystemExc& l_exc)
	{
		TRACE1("%s\n", l_exc.GetFormattedMsg().c_str());
		return FALSE; // abort safely
	}
	catch (const ROwnExc& l_exc)
	{
		TRACE1("%s\n", l_exc.GetFormattedMsg().c_str());
		return FALSE;
	}
	catch (...)
	{
		TRACE0("Unknown exception\n");
	}
	return FALSE;
}


static void OnInitDialog(HWND a_hDlg, ROptionsViewData* a_pData)
{
	::SetWindowLongPtr(a_hDlg, GWL_USERDATA, reinterpret_cast<LONG_PTR>(a_pData));

	a_pData->m_hFelt = ::GetDlgItem(a_hDlg, IDC_VIEW_FELT);
	a_pData->m_hCover = ::GetDlgItem(a_hDlg, IDC_VIEW_COVER);

	for (int l_iAt = IDB_FELT_START; l_iAt <= IDB_FELT_END; l_iAt++)
	{
		SelectBitmap_AddBitmap(a_pData->m_hFelt, RCards_GetInstance(), l_iAt);
	}

	for (int l_iAt = IDB_COVER_START; l_iAt <= IDB_COVER_END; l_iAt++)
	{
		SelectBitmap_AddBitmap(a_pData->m_hCover, RCards_GetInstance(), l_iAt);
	}

	SetCtrlValues(a_hDlg);
}


void OnCommand(HWND a_hDlg, UINT a_idCtrl, UINT a_idCmd)
{
	ROptionsViewData* l_pData = GetRData(a_hDlg);
	switch (a_idCtrl)
	{

	case IDC_VIEW_FELTLEFT:
		SelectBitmap_MoveLeft(l_pData->m_hFelt);
		break;

	case IDC_VIEW_FELTRIGHT:
		SelectBitmap_MoveRight(l_pData->m_hFelt);
		break;

	case IDC_VIEW_COVERLEFT:
		SelectBitmap_MoveLeft(l_pData->m_hCover);
		break;

	case IDC_VIEW_COVERRIGHT:
		SelectBitmap_MoveRight(l_pData->m_hCover);
		break;
	}
}



void OnNcDestroy(HWND a_hDlg)
{
	ROptionsViewData* l_pData = GetRData(a_hDlg);
	delete l_pData;
}


void SetCtrlValues(HWND a_hDlg)
{
	ROptionsViewData* l_pData = GetRData(a_hDlg);
	RRegData::RViewRegData* l_pRegData = l_pData->GetRegData();

	if (l_pRegData->m_idCover == 0)
		l_pRegData->m_idCover = IDB_COVER_DEFAULT; // default

	if (l_pRegData->m_idFelt == 0)
		l_pRegData->m_idFelt = IDB_FELT_DEFAULT; // default

	SelectBitmap_SetSelected(l_pData->m_hFelt, l_pRegData->m_idFelt);
	SelectBitmap_SetSelected(l_pData->m_hCover, l_pRegData->m_idCover);
}


static ROptionsViewData* GetRData(HWND a_hDlg)
{
	return reinterpret_cast<ROptionsViewData*>(::GetWindowLong(a_hDlg, GWL_USERDATA));
}


static void GetCtrlValues(HWND a_hDlg)
{
	ROptionsViewData* l_pData = GetRData(a_hDlg);
	RRegData::RViewRegData* l_pRegData = l_pData->GetRegData();

#pragma todo("Brakuje sprawdzenia b³êdu")
	l_pRegData->m_idFelt = SelectBitmap_GetSelected(l_pData->m_hFelt);
	l_pRegData->m_idCover = SelectBitmap_GetSelected(l_pData->m_hCover);
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
