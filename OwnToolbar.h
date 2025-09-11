#pragma once
#include <rcommon/rstring.h>
#include <vector>



// TODO - replace by enum class
#define OWNTOOLBAR_TYPE_BMP 1
#define OWNTOOLBAR_TYPE_COLORPICKER 2


class CToolbarItem {
public:
	int m_iType;
	tstring m_sName;
	HBITMAP m_hBmp;
	UINT m_idCommand; // id for command, to be implemented
	RECT m_rect;

};

typedef std::vector<CToolbarItem> T_TOOLBARITEMS;

class COwnToolbar
{
public:
	COwnToolbar(); 
	void Draw(HDC a_hDC, const RECT& a_rect);

	void InvalidateRect(HWND a_hWnd, int a_iItem); // invalidates toolbar rect in given window

	void AddItem(const tstring& a_sName, UINT a_idCommand, HBITMAP a_hBmp, UINT a_iType, bool a_bLR);
	void SetColorBackground(COLORREF a_color) { m_colorBackground = a_color; };
	void SetColorSelected(COLORREF a_color) { m_colorSelected = a_color; };
	void SetColorFrame(COLORREF a_color) { m_colorFrame = a_color; }; // 

	COLORREF GetColorBackground() const { return m_colorBackground; };
	COLORREF GetColorSelected() const { return m_colorSelected; };
	COLORREF GetColorFrame() const { return m_colorFrame; };

	void DeleteBitmaps(); // deletes all bitmaps in items vector


	LONG GetExtent() const { return m_iToolbarExtent; };
	void SetExtent(UINT a_iToolbarExtent) { m_iToolbarExtent = a_iToolbarExtent; };

	UINT GetSpacing() const { return m_iSpacing; };
	void SetSpacing(UINT a_iSpacing) { m_iSpacing = a_iSpacing; };

	UINT GetRounding() const { return m_iRounding; };
	void SetRounding(UINT a_iRounding) { m_iRounding = a_iRounding; };

	int HitTest(const POINT& a_pt); // returns index of item under point or -1 if none, for RL returns index from right to left, so for example the first icon to the right has index that equals last index of LR + 1
	bool IsHover() const { return (m_iHover >= 0); };
	void SetHover(int a_iHover) { m_iHover = a_iHover;};
	int GetHover() const { return m_iHover; };
	void SetPressed(int a_iPressed) { m_iPressed = a_iPressed; };

	UINT GetHoverIdCmd() const;

	void Resize(UINT a_iWinExtent);	// has to be used after window size changed. Recalculates rects for RL items. a_iWinExtent - width (horizontal) or height (vertical) of window


private:

	void DrawItems(HDC a_hDC, T_TOOLBARITEMS* a_pItems, int a_iHover);

	T_TOOLBARITEMS m_itemsLR; // left to right (or top to bottom) items
	T_TOOLBARITEMS m_itemsRL; // right to left (or bottom to top) items

	COLORREF m_colorBackground; // background color of toolbar	
	COLORREF m_colorSelected; // color of selected item 
	COLORREF m_colorFrame; // color of frame of border

	LONG m_iToolbarExtent;	// height (horizontal) or width (vertical) of toolbar
	UINT m_iSpacing; // spacing between items and border
	UINT m_iRounding; // rounding of corners
	
	int m_iHover; // hovering item index or -1 if none (for RL returns index from right to left, adding to number of LR items)
	int m_iPressed; 
};

