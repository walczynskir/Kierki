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
		m_hRgnTableCards = NULL;
		m_hBmpCover = NULL;
		m_hBrush = NULL;
		m_nLastHighlighted = -1;
		m_bPassTime = false;
		m_bConfirmTrick = false;
		m_iTileHorz = 0;
		m_iTileVert = 0;
		m_ptPassPos = { 0,0 };
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
	RECT	  m_rectRes[4];
	HRGN	  m_hRgnTableCards;

	CHorzPos m_arrHorzPos[13] {};
	CVertPos m_arrVertPos[13] {};

	SIZE     m_sizeBmpPass;
	bool     m_bPassTime;
	POINT    m_ptPassPos;

	bool     m_bConfirmTrick;

	short m_iTileVert;
	short m_iTileHorz;

	short m_nLastHighlighted;
private:
	HBITMAP  m_hBmpCover;
	HBRUSH   m_hBrush;
	HBITMAP  m_hBmpPass;
};
