#pragma once
#include "GameData.h"


struct CVertPos  
{
	int m_iTop ;
	int m_iBottom ;
	CVertPos() : m_iTop(0), m_iBottom(0) {}
};

struct CHorzPos  
{
	int m_iLeft ;
	int m_iRight ;
	CHorzPos() : m_iLeft(0), m_iRight(0) {}
};

class GameWndData
{
public:

	GameWndData(HWND a_hWndApp, GameData* a_pGameData) :
		m_hWndApp(a_hWndApp), m_pGameData(a_pGameData)
	{
		m_hWndNoTrump = NULL;
		m_hBmpPass = NULL;
		m_hBmpCover = NULL;
		m_hBrush = NULL;
		m_nLastHighlighted = -1;
		m_enPassPlayer = E_DL_NULL;
		m_bConfirmTrick = false;
		m_iTileHorz = 0;
		m_iTileVert = 0;
	}

	~GameWndData(void)
	{
		SetBmpCover(NULL);
		SetBrush(NULL);
		SetBmpPass(NULL);
	}

	void SetBmpCover(HBITMAP a_hBmp) 
	{ 
		if (m_hBmpCover != NULL)
		{
			::DeleteObject(m_hBmpCover);
			m_hBmpCover = NULL;
		}
		m_hBmpCover = a_hBmp; 
	}

	HBITMAP GetBmpCover() const { return m_hBmpCover; };
	
	void SetBrush(HBRUSH a_hBrush) 
	{ 
		if (m_hBrush != NULL)
		{
			::DeleteObject(m_hBrush);
			m_hBrush = NULL;
		}
		m_hBrush = a_hBrush; 
	}

	HBRUSH GetBrush() const { return m_hBrush; };

	void SetBmpPass(HBITMAP a_hBmp) 
	{ 
		if (m_hBmpPass != NULL)
		{
			::DeleteObject(m_hBmpPass);
			m_hBmpPass = NULL;
		}
		m_hBmpPass = a_hBmp; 
	}

	HBITMAP GetBmpPass() const { return m_hBmpPass; };
	
	GameData* m_pGameData;
	HWND	  m_hWndApp;

	HWND      m_hWndNoTrump;

	POINT	  m_ptsLaidCards[4] = {};	// left corners points of laid cards
	RECT	  m_rectLaidCards = {};		// rect of laid cards (for invalidation)
	RECT	  m_rectsNames[4] = {};		// rect of names
	POINT	  m_ptsPass[4] = {};		   // points (left top) of passes

	CHorzPos m_arrHorzPos[13] {};
	CVertPos m_arrVertPos[13] {};

	SIZE		m_sizeBmpPass;
	T_PLAYER    m_enPassPlayer;

	bool     m_bConfirmTrick;

	short m_iTileVert;
	short m_iTileHorz;

	short m_nLastHighlighted;
private:
	HBITMAP  m_hBmpCover;
	HBRUSH   m_hBrush;
	HBITMAP  m_hBmpPass;
};
