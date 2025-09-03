3// OptionsViewDlg.cpp : view option dialog handlers
//
#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"
#include "OptionsViewDlg.h"
#include "layout.h"
#include <rcommon/RColorWnd.h>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <RCards/RCards.h>
#include <RCards/resource.h>


#pragma todo ("put static for selecting covers in rcommon library")
#pragma todo("restore possibility of taking bitmap from other resources")
#pragma todo ("should be a part of RCards module, to be refactored") 



// animation 
enum EAnimDirection
{
	AnimDirection_Null,
	AnimDirection_Right,
	AnimDirection_Left
};


// animation
#define TIMER_ANIM (1)
static const int cc_iAnimStep = 10;
static const int cc_iAnimInterval = 20;



#pragma todo ("add to rcommon library") 
// nice template for indexed map + vector
template<typename Key, typename Value>
class IndexedMapVector {
public:
	using Pair = std::pair<Key, Value>;

	// Add new element (no duplicates allowed)
	void add(const Key& key, const Value& value) {
		if (m_indexByKey.find(key) != m_indexByKey.end()) {
			throw std::runtime_error("Key already exists");
		}
		m_indexByKey[key] = m_data.size();
		m_data.emplace_back(key, value);
	}

	// Access by index
	const Pair& atIndex(size_t index) const {
		return m_data.at(index);
	}

	// Access by key (throws if not found)
	Value& atKey(const Key& key) {
		return m_data.at(m_indexByKey.at(key)).second;
	}

	const Value& atKey(const Key& key) const {
		return m_data.at(m_indexByKey.at(key)).second;
	}

	// Try to find (returns nullptr if not found)
	Value* find(const Key& key) {
		auto it = m_indexByKey.find(key);
		if (it == m_indexByKey.end()) return nullptr;
		return &m_data[it->second].second;
	}

	// Find index by key, returns -1 if not found
	int indexOf(const Key& key) const {
		auto it = m_indexByKey.find(key);
		if (it != m_indexByKey.end()) {
			return static_cast<int>(it->second);
		}
		return -1; // not found
	}

	// check if Key exists
	bool contains(const Key& key) const {
		return m_indexByKey.find(key) != m_indexByKey.end();
	}

	// Number of stored elements
	size_t size() const { return m_data.size(); }

	// Iteration support (like vector)
	auto begin() { return m_data.begin(); }
	auto end() { return m_data.end(); }
	auto begin() const { return m_data.begin(); }
	auto end() const { return m_data.end(); }
	// Number of stored elements

	// Remove all elements
	void clear() {
		m_data.clear();
		m_indexByKey.clear();
	}

	// Access by index (no bounds checking, like std::vector::operator[])
	Pair& operator[](size_t index) {
		return m_data[index];
	}

	const Pair& operator[](size_t index) const {
		return m_data[index];
	}
private:
	std::vector<Pair> m_data;
	std::unordered_map<Key, size_t> m_indexByKey;
};

class ROptionsViewData
{
public:
	ROptionsViewData(RRegData::RBaseRegData* a_pRegData) : m_pRegViewData(reinterpret_cast<RRegData::RViewRegData*>(a_pRegData)), m_idxCurrent(0), m_iAnimOffset(0), m_enumAnimDirection(AnimDirection_Null), m_idxTarget(0)
	{
		for (int l_iAt = IDB_COVER_START; l_iAt <= IDB_COVER_END; l_iAt++)
		{
			HBITMAP l_hBmpCover = ::LoadBitmap(RCards_GetInstance(), MAKEINTRESOURCE(l_iAt));
			if (l_hBmpCover != NULL) 
				m_mapCovers.add(l_iAt, l_hBmpCover);
		}
	}

	~ROptionsViewData()
	{
		for (const auto& [l_id, l_hBmp] : m_mapCovers) 
		{
			::DeleteObject(l_hBmp);
		}
		m_mapCovers.clear(); // clear vector
	}

	RRegData::RViewRegData* GetRegData() const { return m_pRegViewData; };

	int m_idxCurrent;
	int m_iAnimOffset;
	EAnimDirection m_enumAnimDirection;
	int m_idxTarget;
	IndexedMapVector<UINT, HBITMAP> m_mapCovers;

private:
	RRegData::RViewRegData* m_pRegViewData;
};



inline static void OnInitDialog(HWND a_hDlg, RRegData::RViewRegData* a_pData);
inline static void OnCommand(HWND a_hDlg, UINT a_idCtrl, UINT a_idCmd);
inline static void OnTimer(HWND a_hDlg, UINT a_idTimer);
inline static void OnDrawItem(HWND a_hDlg, UINT a_idCtrl, LPDRAWITEMSTRUCT a_pDrawItem);

inline static void OnNcDestroy(HWND a_hDlg);

static INT_PTR CALLBACK OptionsViewDlgProc(HWND a_hDlg, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam);
inline static void SetCtrlValues(HWND a_hDlg);


static ROptionsViewData* GetRData(HWND a_hDlg);
inline static void GetCtrlValues(HWND a_hDlg);
static void MoveDlgItem(HWND a_hDlg, UINT a_idItem, LONG a_x, LONG a_y);
static void ResizeDlgItem(HWND a_hDlg, UINT a_idItem, LONG a_dx, LONG a_dy);

void DrawBitmap(HDC a_hDC, HBITMAP a_hBmp, int a_xStart, int a_xEnd, const RECT& a_rectDest);
void DrawBitmap(HDC a_hDC, HBITMAP a_hBmp, int a_xStart, int a_yStart);

static void StartAnimation(HWND a_hDlg, EAnimDirection a_enumDirection);




HWND CreateOptViewDlg(HWND a_hParent, RRegData::RBaseRegData* a_pData, LPVOID a_pObj)
{
	RColorWnd_RegisterClass();
	ROptionsViewData* l_pViewData = new ROptionsViewData(a_pData);
	return ::CreateDialogParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OPT_VIEW), 
		a_hParent, OptionsViewDlgProc, reinterpret_cast<LPARAM>(l_pViewData));
}


static void OnInitDialog(HWND a_hDlg, RRegData::RViewRegData* a_pData)
{
	::SetWindowLongPtr(a_hDlg, GWL_USERDATA, reinterpret_cast<LONG_PTR>(a_pData));

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

	case WM_TIMER:
		OnTimer(a_hDlg, a_wParam);
		break;

	case WM_DRAWITEM:
		OnDrawItem(a_hDlg, a_wParam, reinterpret_cast<LPDRAWITEMSTRUCT>(a_lParam));
		break;

	case WM_NCDESTROY:
		OnNcDestroy(a_hDlg);
		return TRUE;
	}
	return FALSE;
}


void OnCommand(HWND a_hDlg, UINT a_idCtrl, UINT a_idCmd)
{
	switch (a_idCtrl)
	{
	case IDC_VIEW_DEFUALTTABLE:
		// instant redraw with a new color
		if (a_idCmd == BN_CLICKED)
		{
			RRegData::RViewRegData* l_pData = GetRData(a_hDlg)->GetRegData();
			HWND l_hWndTable = ::GetDlgItem(a_hDlg, IDC_VIEW_TABLECOLOR);
			RColorWnd_SetColor(l_hWndTable, l_pData->GetDefaultTableColor(), FALSE);
			::RedrawWindow(l_hWndTable, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}		
		break;

	case IDC_VIEW_DEFUALTRESULT:
		// instant redraw with a new color
		if (a_idCmd == BN_CLICKED)
		{
			RRegData::RViewRegData* l_pData = GetRData(a_hDlg)->GetRegData();
			HWND l_hWndResult = ::GetDlgItem(a_hDlg, IDC_VIEW_RESULTCOLOR);
			RColorWnd_SetColor(l_hWndResult, l_pData->GetDefaultResultColor(), FALSE);
			::RedrawWindow(l_hWndResult, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}		
		break;

	case IDC_VIEW_LEFT:
		StartAnimation(a_hDlg, AnimDirection_Left);
		break;

	case IDC_VIEW_RIGHT:
		StartAnimation(a_hDlg, AnimDirection_Right);
		break;
	}
}


void OnTimer(HWND a_hDlg, UINT a_idTimer)
{
	if (a_idTimer != TIMER_ANIM)
		return;
	
	ROptionsViewData* l_pData = GetRData(a_hDlg);

	if (l_pData->m_enumAnimDirection == AnimDirection_Null)
		return;

	RECT l_rect;
	::GetClientRect(::GetDlgItem(a_hDlg, IDC_VIEW_COVER), &l_rect);
	int l_dx = l_rect.right - l_rect.left;

	l_pData->m_iAnimOffset += cc_iAnimStep;

	if (l_pData->m_iAnimOffset >= l_dx) 
	{
		l_pData->m_idxCurrent = l_pData->m_idxTarget;
		l_pData->m_enumAnimDirection = AnimDirection_Null;
		::KillTimer(a_hDlg, TIMER_ANIM);
	}
	::RedrawWindow(GetDlgItem(a_hDlg, IDC_VIEW_COVER), NULL,NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

void OnDrawItem(HWND a_hDlg, UINT a_idCtrl, LPDRAWITEMSTRUCT a_pDrawItem)
{
	if (a_idCtrl != IDC_VIEW_COVER)
		return;
	ROptionsViewData* l_pData = GetRData(a_hDlg);

	HDC l_hdc = a_pDrawItem->hDC;
	RECT l_rect = a_pDrawItem->rcItem;
	int l_dx = l_rect.right - l_rect.left;

	if (l_pData->m_enumAnimDirection == AnimDirection_Null)
	{
		DrawBitmap(l_hdc, l_pData->m_mapCovers[l_pData->m_idxCurrent].second, 0, RectWidth(l_rect), l_rect);
	}
	else
	{
		int l_iOffset = l_pData->m_iAnimOffset * (l_pData->m_enumAnimDirection == AnimDirection_Left ? 1 : -1);
		DrawBitmap(l_hdc, l_pData->m_mapCovers[l_pData->m_idxTarget].second, 0, l_iOffset, l_rect);
		DrawBitmap(l_hdc, l_pData->m_mapCovers[l_pData->m_idxCurrent].second, l_iOffset, RectWidth(l_rect), l_rect);
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
	RColorWnd_SetColor(::GetDlgItem(a_hDlg, IDC_VIEW_TABLECOLOR), l_pRegData->m_colorTable, TRUE);
	RColorWnd_SetColor(::GetDlgItem(a_hDlg, IDC_VIEW_RESULTCOLOR), l_pRegData->m_colorResult, TRUE);

	if (l_pRegData->m_idCover == 0)
		l_pRegData->m_idCover = IDB_COVER_START; // default
	if (l_pData->m_mapCovers.contains(l_pRegData->m_idCover))
		l_pData->m_idxCurrent =  l_pData->m_mapCovers.indexOf(l_pRegData->m_idCover);
}


static ROptionsViewData* GetRData(HWND a_hDlg)
{
	return reinterpret_cast<ROptionsViewData*>(::GetWindowLong(a_hDlg, GWL_USERDATA));
}


static void GetCtrlValues(HWND a_hDlg)
{
	ROptionsViewData* l_pData = GetRData(a_hDlg);
	RRegData::RViewRegData* l_pRegData = l_pData->GetRegData();
	l_pRegData->m_colorTable = RColorWnd_GetColor(::GetDlgItem(a_hDlg, IDC_VIEW_TABLECOLOR));
	l_pRegData->m_colorResult = RColorWnd_GetColor(::GetDlgItem(a_hDlg, IDC_VIEW_RESULTCOLOR));

	l_pRegData->m_idCover = l_pData->m_mapCovers[l_pData->m_idxCurrent].first;
}

void StartAnimation(HWND a_hDlg, EAnimDirection a_enumDirection)
{
	ROptionsViewData* l_pData = GetRData(a_hDlg);

	if (l_pData->m_enumAnimDirection != AnimDirection_Null)
		return; // already started

	l_pData->m_enumAnimDirection = a_enumDirection;
	l_pData->m_idxTarget = (l_pData->m_idxCurrent + (l_pData->m_enumAnimDirection == AnimDirection_Right ? 1 : -1) + l_pData->m_mapCovers.size()) % l_pData->m_mapCovers.size();
	l_pData->m_iAnimOffset = 0;
	SetTimer(a_hDlg, TIMER_ANIM, cc_iAnimInterval, NULL);
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


// with keeping aspect and fitting into the static 
void DrawBitmap(HDC a_hDC, HBITMAP a_hBmp, int a_x, int a_y, const RECT& a_rectDest)
{
	if (!a_hBmp) 
		return;

	HDC l_hDCMem = ::CreateCompatibleDC(a_hDC);
	HBITMAP l_hBmpOld = (HBITMAP)::SelectObject(l_hDCMem, a_hBmp);

	BITMAP l_bitmap;
	::GetObject(a_hBmp, sizeof(l_bitmap), &l_bitmap);

	// destination size
	int l_dxDest = a_rectDest.right - a_rectDest.left;
	int l_dyDest = a_rectDest.bottom - a_rectDest.top;

	// calculate scales
	double l_dScaleX = (double)l_dxDest / l_bitmap.bmWidth;
	double l_dScaleY = (double)l_dyDest / l_bitmap.bmHeight;
	double l_dScale = min(l_dScaleX, l_dScaleY);  // dopasuj tak, by ca³a bitmapa siê zmieœci³a

	int l_dxNew = (int)(l_bitmap.bmWidth * l_dScale);
	int l_dyNew = (int)(l_bitmap.bmHeight * l_dScale);

	// Wyœrodkuj obraz w polu STATIC
	int l_iOffsetX = a_rectDest.left + (l_dxDest - l_dxNew) / 2;
	int l_iOffsetY = a_rectDest.top + (l_dyDest - l_dyNew) / 2;


	::StretchBlt(a_hDC, l_iOffsetX, l_iOffsetY, l_dxNew, l_dyNew, l_hDCMem, a_x, 0, l_bitmap.bmWidth, l_bitmap.bmHeight, SRCCOPY);

	::SelectObject(l_hDCMem, l_hBmpOld);
	::DeleteDC(l_hDCMem);
}

// 
// 
//
void DrawBitmap(HDC a_hDC, HBITMAP a_hBmp, int a_xStart, int a_yStart)
{
	if (!a_hBmp) 
		return;

	HDC l_hDCMem = CreateCompatibleDC(a_hDC);
	HBITMAP l_hBmpOld = (HBITMAP)SelectObject(l_hDCMem, a_hBmp);

	BITMAP l_bitmap;
	GetObject(a_hBmp, sizeof(l_bitmap), &l_bitmap);

	BitBlt(a_hDC, a_xStart, a_yStart, l_bitmap.bmWidth, l_bitmap.bmHeight, l_hDCMem, 0, 0, SRCCOPY);

	SelectObject(l_hDCMem, l_hBmpOld);
	DeleteDC(l_hDCMem);
}