#pragma once
#include "GameData.h"
#include <array>


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
		m_nLastHighlighted = -1;
		m_enPassPlayer = Player::E_DL_NULL;
		m_bConfirmTrick = false;
	}

	~GameWndData(void)
	{
		SetBmpCover(NULL);
		SetBmpPass(NULL);
		SetBmpFelt(NULL);
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
	
	void SetBmpFelt(HBITMAP a_hBmp)
	{
		if (m_hBmpFelt != NULL)
		{
			::DeleteObject(m_hBmpFelt);
			m_hBmpFelt = NULL;
		}
		m_hBmpFelt = a_hBmp;
	}

	HBITMAP GetBmpFelt() const { return m_hBmpFelt; };


	GameData* m_pGameData;
	HWND	  m_hWndApp;

	HWND      m_hWndNoTrump;

	RECT m_rectLaidCards{};	// rect of laid cards (for invalidation)
	PlayerArray<RECT> m_rectsNames{};		// rect of names
	PlayerArray<POINT> m_ptsPass{};
	PlayerArray <POINT> m_ptsLaidCards{};	// left corners points of laid cards


	CHorzPos m_arrHorzPos[13] {};
	CVertPos m_arrVertPos[13] {};

	SIZE		m_sizeBmpPass;
	Player      m_enPassPlayer;

	bool     m_bConfirmTrick;
	short m_nLastHighlighted;

private:
	HBITMAP  m_hBmpFelt{};
	HBITMAP  m_hBmpCover{};
	HBITMAP  m_hBmpPass{};
};
