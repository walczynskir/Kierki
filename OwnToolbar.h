#pragma once
#include <rcommon/rstring.h>
#include <vector>
#include <rcommon/RTheme.h>




// TODO - replace by enum class
#define OWNTOOLBAR_TYPE_BMP 1
#define OWNTOOLBAR_TYPE_COLORPICKER 2

// TODO - first version of toolbar, without the options setting size, position etc
#define OWNTOOLBAR_DEFAULT_EXTENT 60
#define OWNTOOLBAR_DEFAULT_COLOR RGB(0, 0, 0)
#define OWNTOOLBAR_DEFAULT_SPACING 6
#define OWNTOOLBAR_ROUNDING 12
#define OWNTOOLBAR_ITEM_COLORPICKER_SIZE 80

// WPARAM =  (int)a_idItem, LPARAM = (LPCTSTR) a_sName
#define WM_TOOLBARHOVER (WM_APP + 101)
// WPARAM =  0, LPARAM = 0
#define WM_TOOLBARLEAVE (WM_APP + 102)


class CToolbarItem {
public:
	int m_iType;
	tstring m_sName{};
	HBITMAP m_hBmp{};
	UINT m_idCommand; 
	RECT m_rect;
	TOOLINFO m_ti{};
};

typedef std::vector<CToolbarItem> T_TOOLBARITEMS;

class COwnToolbar
{
public:
	COwnToolbar(); 

	void OnPaint(HWND a_hWnd);
	void OnMouseMove(HWND a_hWnd, int a_x, int a_y);
	void OnLButtonDown(HWND a_hWnd, int a_x, int a_y);
	void OnLButtonUp(HWND a_hWnd, int a_x, int a_y);
	void OnMouseLeave(HWND a_hWnd);

	void InvalidateRect(HWND a_hWnd, int a_iItem); // invalidates toolbar rect in given window

	void AddItem(HWND a_hWnd, const tstring& a_sName, UINT a_idCommand, HBITMAP a_hBmp, UINT a_iType, bool a_bLR);

	void DeleteBitmaps(); // deletes all bitmaps in items vector


	LONG GetExtent() const { return m_iToolbarExtent; };
	void SetExtent(UINT a_iToolbarExtent) { m_iToolbarExtent = a_iToolbarExtent; };

	UINT GetSpacing() const { return m_iSpacing; };
	void SetSpacing(UINT a_iSpacing) { m_iSpacing = a_iSpacing; };

	UINT GetRounding() const { return m_iRounding; };
	void SetRounding(UINT a_iRounding) { m_iRounding = a_iRounding; };

	int HitTest(const POINT& a_pt); // returns index of item under point or -1 if none, for RL returns index from right to left, so for example the first icon to the right has index that equals last index of LR + 1
	bool IsHover() const { return (m_iHotItem >= 0); };

	UINT GetHoverIdCmd() const;
	tstring GetHoverName() const;


	void Resize(int a_dxWidth, int a_dyHeight);	// has to be used after window size changed. Recalculates rects for RL items. a_iWinExtent - width (horizontal) or height (vertical) of window
	bool IsOverToolbar(int a_x, int a_y);


private:
	void Draw(HDC a_hDC, const RECT& a_rect);
	void DrawItems(HDC a_hDC, T_TOOLBARITEMS* a_pItems, int a_iHover, int a_iPressed);

	SIZE m_size{};
	T_TOOLBARITEMS m_itemsLR; // left to right (or top to bottom) items
	T_TOOLBARITEMS m_itemsRL; // right to left (or bottom to top) items

	RTheme m_theme;

	COLORREF m_colorBackground; // background color of toolbar	
	COLORREF m_colorHot; // color of selected item 
	COLORREF m_colorPressed; // color of selected item 
	COLORREF m_colorFrame; // color of frame of border

	LONG m_iToolbarExtent = OWNTOOLBAR_DEFAULT_EXTENT;	// height (horizontal) or width (vertical) of toolbar
	UINT m_iSpacing = OWNTOOLBAR_DEFAULT_SPACING; // spacing between items and border
	UINT m_iRounding = OWNTOOLBAR_ROUNDING; // rounding of corners
	
	bool m_bTracking = false;
	int  m_iHotItem = -1;
	int m_iPressed = -1;
	HWND m_hTooltip = nullptr;
};

