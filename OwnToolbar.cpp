#include "stdafx.h"
#include "OwnToolbar.h"
#include <rcommon/drawutl.h>
#include <rcommon/RMemDC.h>




// ---------------------------------------------------------
// constructor
COwnToolbar::COwnToolbar() 
{
	m_theme.OpenData(NULL, L"TOOLBAR");

	COLORREF l_color;
	if (m_theme.IsValid())
	{
		m_theme.GetThemeColor(TP_BUTTON, 0, TMT_FILLCOLOR, &l_color);
		m_colorBackground = l_color;

		m_theme.GetThemeColor(TP_BUTTON, TS_HOT, TMT_FILLCOLORHINT, &l_color);
		m_colorHot = l_color;

		m_theme.GetThemeColor(TP_BUTTON, TS_PRESSED, TMT_FILLCOLORHINT, &l_color);
		m_colorPressed = l_color;

		m_colorFrame = ::GetSysColor(COLOR_3DSHADOW);
	}
	else
	{
		m_colorBackground = ::GetSysColor(COLOR_BTNFACE);
		m_colorHot = ::GetSysColor(COLOR_BTNFACE);
		m_colorPressed = ::GetSysColor(COLOR_BTNFACE);
		m_colorFrame = ::GetSysColor(COLOR_3DSHADOW);
	}

}


void COwnToolbar::OnMouseMove(HWND a_hWnd, int a_x, int a_y)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT l_tme = { sizeof(l_tme) };
		l_tme.dwFlags = TME_LEAVE;
		l_tme.hwndTrack = a_hWnd;
		TrackMouseEvent(&l_tme);
		m_bTracking = true;

	}

	POINT l_pt = { a_x, a_y };
	UINT l_iPrevHover = m_iHotItem;
	UINT l_iNewHover = HitTest(l_pt);

	// hover on the same item as before?
	if (l_iPrevHover == l_iNewHover)
	{
		return;
	}

	m_iHotItem = l_iNewHover;

	// invalidate toolbar
	InvalidateRect(a_hWnd, l_iPrevHover);
	InvalidateRect(a_hWnd, l_iNewHover);
	::SendMessage(a_hWnd, WM_TOOLBARHOVER, static_cast<WPARAM>(m_iHotItem), reinterpret_cast<LPARAM>(GetHoverName().c_str()));
}


void COwnToolbar::OnMouseLeave(HWND a_hWnd)
{
	m_bTracking = false;
	if (m_iHotItem != -1)
	{
		InvalidateRect(a_hWnd, m_iHotItem);
		m_iHotItem = -1;
		::SendMessage(a_hWnd, WM_TOOLBARLEAVE, static_cast<WPARAM>(m_iHotItem), 0);
	}
}


void COwnToolbar::OnLButtonDown(HWND a_hWnd, int a_x, int a_y)
{
	POINT l_pt = { a_x, a_y };
	// check if mouse is over toolbar item
	m_iHotItem = HitTest(l_pt);

	// invalidate toolbar
	if (IsHover())
	{
		m_iPressed = m_iHotItem;
	}
	InvalidateRect(a_hWnd, m_iHotItem);
}


void COwnToolbar::OnLButtonUp(HWND a_hWnd, int a_x, int a_y)
{
	POINT l_pt = { a_x, a_y };

	// check if mouse is over toolbar

	UINT l_iHover = HitTest(l_pt);

	m_iPressed = -1;
	InvalidateRect(a_hWnd, m_iHotItem);
	if (l_iHover != -1)
	{
		::SendMessage(a_hWnd, WM_COMMAND, MAKEWPARAM(GetHoverIdCmd(), 0), 0);
	}

}

void COwnToolbar::OnPaint(HWND a_hWnd)
{

	PAINTSTRUCT l_ps;
	HDC l_hdc = ::BeginPaint(a_hWnd, &l_ps);

	RECT l_rectWin;
	::GetClientRect(a_hWnd, &l_rectWin);
	{
		// prepare correct DC
#ifdef _DEBUG 
		HDC l_hPaintDC = l_hdc;
#else
		RMemDC l_memDC(l_hdc, &l_rectWin);
		HDC l_hPaintDC = l_memDC;
#endif
		RECT l_rectWin;
		::GetClientRect(a_hWnd, &l_rectWin);

		// TODO here to change if own toolbar can change position (horizontal / vertical)
		l_rectWin.bottom = m_iToolbarExtent;
		Draw(l_hPaintDC, l_rectWin);

	}
	::EndPaint(a_hWnd, &l_ps);


}

void COwnToolbar::Draw(HDC a_hDC, const RECT& a_rect)
{
	// background
	RDraw::FillSolidRect(a_hDC, a_rect, m_colorBackground);
	DrawItems(a_hDC, &m_itemsLR, m_iHotItem, m_iPressed);
	DrawItems(a_hDC, &m_itemsRL, m_iHotItem - m_itemsLR.size(), m_iPressed - m_itemsLR.size());
}


// 
// Draws both sides
//
// TODO vertical drawing
void COwnToolbar::DrawItems(HDC a_hDC, T_TOOLBARITEMS* a_pItems, int a_iHover, int a_iPressed)
{
	for (size_t l_iAt = 0; l_iAt < a_pItems->size(); l_iAt++)
	{
		if ((*a_pItems)[l_iAt].m_iType != OWNTOOLBAR_TYPE_BMP)
			continue; // only bitmaps for now

		if (m_theme.IsValid())
		{
			int l_iState = 0;

			if (static_cast<int>(l_iAt) == a_iHover)
				if (static_cast<int>(l_iAt) == a_iPressed)
					l_iState = TS_PRESSED;
				else 				
					l_iState = TS_HOT;
			m_theme.DrawBackground(a_hDC, TP_BUTTON, l_iState, &(*a_pItems)[l_iAt].m_rect, NULL);
		}
		else
		{
			if (l_iAt == a_iHover || l_iAt == a_iPressed)
			{
				HBRUSH l_hBrush = NULL;
				if (a_iHover != -1)
					if (m_iPressed != -1)
						l_hBrush = ::CreateSolidBrush(m_colorPressed);
					else
						l_hBrush = ::CreateSolidBrush(m_colorHot);
				else
					l_hBrush = ::CreateSolidBrush(m_colorBackground);

				HPEN l_hPen = ::CreatePen(PS_SOLID, 1, (m_iPressed == -1) ? m_colorFrame : m_colorBackground);
				HGDIOBJ l_hOldPen = SelectObject(a_hDC, l_hPen);
				HGDIOBJ l_hOldBrush = SelectObject(a_hDC, l_hBrush);
				// Narysuj prostok¹t wokó³ obrazka
				RoundRect(a_hDC, (*a_pItems)[l_iAt].m_rect.left - m_iSpacing, (*a_pItems)[l_iAt].m_rect.top - m_iSpacing,
					(*a_pItems)[l_iAt].m_rect.right + m_iSpacing, (*a_pItems)[l_iAt].m_rect.bottom + m_iSpacing, m_iRounding, m_iRounding);
				// restore objects
				::SelectObject(a_hDC, l_hOldBrush);
				::SelectObject(a_hDC, l_hOldPen);
				::DeleteObject(l_hPen);
				::DeleteObject(l_hBrush);
			}
		}
		// Narysuj obrazek	
		POINT l_pt = { (*a_pItems)[l_iAt].m_rect.left, (*a_pItems)[l_iAt].m_rect.top };
		RDraw::DrawBitmapTransparent(a_hDC, l_pt, (*a_pItems)[l_iAt].m_hBmp, RGB(255, 255, 255));
	}

}



// invalidates toolbar rect in given item
void COwnToolbar::InvalidateRect(HWND a_hWnd, int a_iItem)
{
	if (a_iItem < 0)
		return; // not item to invalidate
	RECT l_rect;

	if (a_iItem < static_cast<int>(m_itemsLR.size()))
	{
		l_rect = m_itemsLR[a_iItem].m_rect;
	}
	else if (a_iItem - static_cast<int>(m_itemsLR.size()) < static_cast<int>(m_itemsRL.size()))
	{
		l_rect = m_itemsRL[a_iItem - m_itemsLR.size()].m_rect;
	}
	else
	{
		return; // invalid item index
	}

	l_rect.left -= m_iSpacing;
	l_rect.top -= m_iSpacing;
	l_rect.right += m_iSpacing;
	l_rect.bottom += m_iSpacing;

	::InvalidateRect(a_hWnd, &l_rect, FALSE);
}



void COwnToolbar::AddItem(HWND a_hWnd, const tstring& a_sName, UINT a_idCommand, HBITMAP a_hBmp, UINT a_iType, bool a_bLR)
{
	CToolbarItem l_item;
	l_item.m_iType = a_iType;
	l_item.m_idCommand = a_idCommand; 
	l_item.m_sName = a_sName;
	l_item.m_hBmp = a_hBmp;

	if (m_hTooltip == nullptr)
	{
		// create tooltip
		m_hTooltip = CreateWindowEx(0, TOOLTIPS_CLASS, nullptr,
			WS_POPUP | TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			a_hWnd, nullptr, ::GetModuleHandle(nullptr), nullptr);
		if (m_hTooltip == nullptr)
			return;
	}

	l_item.m_ti = { sizeof(TOOLINFO) };
	l_item.m_ti.uFlags = TTF_SUBCLASS;
	l_item.m_ti.hwnd = a_hWnd;
	l_item.m_ti.hinst = ::GetModuleHandle(nullptr);
	l_item.m_ti.uId = a_idCommand; // Unique ID per button
	tstring l_sTooltip = a_sName; // Make a copy
	l_item.m_ti.lpszText = l_sTooltip.data();
	l_item.m_ti.rect = RECT(10, 10, 50, 50); // RECT in client coordinates (later updated)
	a_bLR ? m_itemsLR.push_back(l_item) : m_itemsRL.push_back(l_item);

	SendMessage(m_hTooltip, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&l_item.m_ti));
}


// 
// has to be used after window size changed. Recalculates rects for items
//
void COwnToolbar::Resize(int a_dxWidth, int a_dyHeight)
{
	m_size = { a_dxWidth, a_dyHeight };

	// calculates rect for items positioned from left to right (or top to bottom)
	// calculates rect for items positioned from right to left (or bottom to top)
	for (size_t l_iAt = 0; l_iAt < m_itemsLR.size(); l_iAt++)
	{

		UINT l_iItemExtent;
		switch (m_itemsLR[l_iAt].m_iType)
		{
		case OWNTOOLBAR_TYPE_BMP:
			l_iItemExtent = m_iToolbarExtent - 2 * m_iSpacing;
			break;
		case OWNTOOLBAR_TYPE_COLORPICKER:
			// to be implemented
			l_iItemExtent = OWNTOOLBAR_ITEM_COLORPICKER_SIZE;
			break;
		default:
			ASSERT(FALSE); // unknown type
		}

		UINT l_iStartExtent = (l_iAt == 0) ? 0 : m_itemsLR[l_iAt - 1].m_rect.right + m_iSpacing;
		m_itemsLR[l_iAt].m_rect.left = l_iStartExtent + m_iSpacing;
		m_itemsLR[l_iAt].m_rect.top = m_iSpacing;
		m_itemsLR[l_iAt].m_rect.right = m_itemsLR[l_iAt].m_rect.left + l_iItemExtent;
		m_itemsLR[l_iAt].m_rect.bottom = m_itemsLR[l_iAt].m_rect.top + l_iItemExtent;
		m_itemsLR[l_iAt].m_ti.rect = m_itemsLR[l_iAt].m_rect; // update tooltip rect
		SendMessage(m_hTooltip, TTM_NEWTOOLRECT, 0, reinterpret_cast<LPARAM>(& m_itemsLR[l_iAt].m_ti));
	}

	// calculates rect for items positioned from right to left (or bottom to top)
	for (size_t l_iAt = 0; l_iAt < m_itemsRL.size(); l_iAt++)
	{

		UINT l_iItemExtent;
		switch (m_itemsRL[l_iAt].m_iType)
		{
		case OWNTOOLBAR_TYPE_BMP:
			l_iItemExtent = m_iToolbarExtent - 2 * m_iSpacing;
			break;
		case OWNTOOLBAR_TYPE_COLORPICKER:
			// to be implemented
			l_iItemExtent = OWNTOOLBAR_ITEM_COLORPICKER_SIZE;
			break;
		default:
			ASSERT(FALSE); // unknown type
		}
	
		UINT l_iStartExtent = (l_iAt == 0) ? m_size.cx : m_itemsRL[l_iAt - 1].m_rect.left - m_iSpacing;
		m_itemsRL[l_iAt].m_rect.left = l_iStartExtent - l_iItemExtent - m_iSpacing;
		m_itemsRL[l_iAt].m_rect.top = m_iSpacing;
		m_itemsRL[l_iAt].m_rect.right = m_itemsRL[l_iAt].m_rect.left + l_iItemExtent;
		m_itemsRL[l_iAt].m_rect.bottom = m_itemsRL[l_iAt].m_rect.top + l_iItemExtent;
		m_itemsRL[l_iAt].m_ti.rect = m_itemsRL[l_iAt].m_rect; // update tooltip rect
		::SendMessage(m_hTooltip, TTM_NEWTOOLRECT, 0, reinterpret_cast<LPARAM>(&m_itemsRL[l_iAt].m_ti));

	}


}


void COwnToolbar::DeleteBitmaps()
{
	for (size_t l_iAt = 0; l_iAt < m_itemsLR.size(); l_iAt++)
	{
		if (m_itemsLR[l_iAt].m_iType == OWNTOOLBAR_TYPE_BMP && m_itemsLR[l_iAt].m_hBmp != NULL)
		{
			::DeleteObject(m_itemsLR[l_iAt].m_hBmp);
			m_itemsLR[l_iAt].m_hBmp = NULL;
		}
	}
	for (size_t l_iAt = 0; l_iAt < m_itemsRL.size(); l_iAt++)
	{
		if (m_itemsRL[l_iAt].m_iType == OWNTOOLBAR_TYPE_BMP && m_itemsRL[l_iAt].m_hBmp != NULL)
		{
			::DeleteObject(m_itemsRL[l_iAt].m_hBmp);
			m_itemsRL[l_iAt].m_hBmp = NULL;
		}
	}
};


int COwnToolbar::HitTest(const POINT& a_pt)
{
	//  LR items
	for (size_t l_iAt = 0; l_iAt < m_itemsLR.size(); l_iAt++)
	{
		if (PtInRect(&m_itemsLR[l_iAt].m_rect, a_pt))
			return static_cast<int>(l_iAt);
	}

	for (size_t l_iAt = 0; l_iAt < m_itemsRL.size(); l_iAt++)
	{
		if (PtInRect(&m_itemsRL[l_iAt].m_rect, a_pt))
			return static_cast<int>(l_iAt) + m_itemsLR.size();
	}

	return -1;
}


UINT COwnToolbar::GetHoverIdCmd() const
{
	if (m_iHotItem == -1)
		return 0;

	if (static_cast<UINT>(m_iHotItem) < m_itemsLR.size())
		return m_itemsLR[m_iHotItem].m_idCommand;
	else
		return m_itemsRL[m_iHotItem - m_itemsLR.size()].m_idCommand;
}


tstring COwnToolbar::GetHoverName() const
{
	if (m_iHotItem == -1)
		return _T("");

	if (static_cast<UINT>(m_iHotItem) < m_itemsLR.size())
		return m_itemsLR[m_iHotItem].m_sName;
	else
		return m_itemsRL[m_iHotItem - m_itemsLR.size()].m_sName;
}


bool COwnToolbar::IsOverToolbar(int a_x, int a_y)
{
	if (a_x < 0 || a_y < 0 || a_x > m_size.cx || a_y > m_size.cy)
		return false; // out of toolbar rect
	return true;
}
