#include "stdafx.h"
#include "OwnToolbar.h"
#include <rcommon/drawutl.h>
#include <rcommon/RTheme.h>


// TODO - first version of toolbar, without the options setting size, position etc
#define OWNTOOLBAR_DEFAULT_EXTENT 60
#define OWNTOOLBAR_DEFAULT_COLOR RGB(0, 0, 0)
#define OWNTOOLBAR_DEFAULT_SPACING 6
#define OWNTOOLBAR_ROUNDING 12
#define OWNTOOLBAR_ITEM_COLORPICKER_SIZE 80


// ---------------------------------------------------------
// constructor
COwnToolbar::COwnToolbar() : m_iToolbarExtent(OWNTOOLBAR_DEFAULT_EXTENT), m_iHover(-1), m_iPressed(-1), m_iSpacing(OWNTOOLBAR_DEFAULT_SPACING), m_iRounding(OWNTOOLBAR_ROUNDING)
{
	RTheme l_theme;
	l_theme.OpenData(NULL, L"TOOLBAR");

	COLORREF l_color;
	l_theme.GetThemeColor(TP_BUTTON, 0, TMT_FILLCOLOR, &l_color);
	m_colorBackground = l_color;

	l_theme.GetThemeColor(TP_BUTTON, TS_HOT, TMT_FILLCOLORHINT, &l_color);
	m_colorSelected = l_color;

	l_theme.GetThemeColor(TP_BUTTON, TS_HOT, TMT_BORDERCOLOR, &l_color);
	m_colorFrame = l_color;

}

void COwnToolbar::Draw(HDC a_hDC, const RECT& a_rectWin)
{
	// background
	RDraw::FillSolidRect(a_hDC, a_rectWin, m_colorBackground);
	DrawItems(a_hDC, &m_itemsLR, m_iHover);
	DrawItems(a_hDC, &m_itemsRL, m_iHover - m_itemsLR.size()) ;

}

// 
// Draws both sides
//
// TODO vertical drawing
void COwnToolbar::DrawItems(HDC a_hDC, T_TOOLBARITEMS* a_pItems, int a_iHover)
{

	// Ustaw kolor ramki
	// TODO - take colors from theme or different place
	HBRUSH hBrush = ::CreateSolidBrush(m_colorSelected); 
	HPEN hPen = ::CreatePen(PS_SOLID, 1, (m_iPressed == -1) ? m_colorFrame : m_colorBackground); 

	for (size_t l_iAt = 0; l_iAt < a_pItems->size(); l_iAt++)
	{
		if ((*a_pItems)[l_iAt].m_iType != OWNTOOLBAR_TYPE_BMP)
			continue; // only bitmaps for now

		if (l_iAt == a_iHover)
		{
			HGDIOBJ hOldPen = SelectObject(a_hDC, hPen);
			HGDIOBJ oldBrush = SelectObject(a_hDC, hBrush);
			// Narysuj prostok¹t wokó³ obrazka
			RoundRect(a_hDC, (*a_pItems)[l_iAt].m_rect.left - m_iSpacing, (*a_pItems)[l_iAt].m_rect.top - m_iSpacing,
				(*a_pItems)[l_iAt].m_rect.right + m_iSpacing, (*a_pItems)[l_iAt].m_rect.bottom + m_iSpacing, m_iRounding, m_iRounding);
			// restore objects
			SelectObject(a_hDC, oldBrush);
			SelectObject(a_hDC, hOldPen);
		}
		// Narysuj obrazek	
		POINT l_pt = { (*a_pItems)[l_iAt].m_rect.left, (*a_pItems)[l_iAt].m_rect.top };
		RDraw::DrawBitmapTransparent(a_hDC, l_pt, (*a_pItems)[l_iAt].m_hBmp, RGB(255, 255, 255));
	}

	DeleteObject(hPen);
	DeleteObject(hBrush);
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



void COwnToolbar::AddItem(const tstring& a_sName, UINT a_idCommand, HBITMAP a_hBmp, UINT a_iType, bool a_bLR)
{
	CToolbarItem l_item;
	l_item.m_iType = a_iType;
	l_item.m_idCommand = a_idCommand; 
	l_item.m_sName = a_sName;
	l_item.m_hBmp = a_hBmp;
	a_bLR ? m_itemsLR.push_back(l_item) : m_itemsRL.push_back(l_item);
}


// 
// has to be used after window size changed. Recalculates rects for items
//
void COwnToolbar::Resize(UINT a_iWinExtent)
{
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
	
		UINT l_iStartExtent = (l_iAt == 0) ? a_iWinExtent : m_itemsRL[l_iAt - 1].m_rect.left - m_iSpacing;
		m_itemsRL[l_iAt].m_rect.left = l_iStartExtent - l_iItemExtent - m_iSpacing;
		m_itemsRL[l_iAt].m_rect.top = m_iSpacing;
		m_itemsRL[l_iAt].m_rect.right = m_itemsRL[l_iAt].m_rect.left + l_iItemExtent;
		m_itemsRL[l_iAt].m_rect.bottom = m_itemsRL[l_iAt].m_rect.top + l_iItemExtent;

//		TRACE4("%d - %d - %d - %d\n", m_itemsRL[l_iAt].m_rect.left, m_itemsRL[l_iAt].m_rect.top, m_itemsRL[l_iAt].m_rect.right, m_itemsRL[l_iAt].m_rect.bottom);	
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
	if (m_iHover == -1)
		return 0;

	if (static_cast<UINT>(m_iHover) < m_itemsLR.size())
		return m_itemsLR[m_iHover].m_idCommand;
	else
		return m_itemsRL[m_iHover - m_itemsLR.size()].m_idCommand;
}
