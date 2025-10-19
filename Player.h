// Player.h: interface for the CPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYER_H__6DCA56C2_80FB_11D3_B413_E87410C10000__INCLUDED_)
#define AFX_PLAYER_H__6DCA56C2_80FB_11D3_B413_E87410C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "Score.h"
#include "UserCards.h"	
#include "Decider.h"	
#include <rcommon/RString.h>
#include "RFile.h"	

class CPlayer
{
public:
	enum class PlayerType {
		Human,
		Computer
	};

public:
	short SumAllScore(T_SERIE a_enSerie) const;
	short SumScore(T_SERIE a_enSerie) const;
	void ClearAllScores();
	CPlayer();
	virtual ~CPlayer();

	void CreateDecider(T_GAMES a_enGame, const CTakenTricks* a_pTricks, T_SUIT a_colorTrumps = E_CC_NULL);
	void CreateDecider(const CPuzzleRows* a_pPuzzleRow);

	T_SUIT ChooseTrumps() const;

	void SetMe(T_PLAYER a_playerMe, PlayerType a_typePlayer);
	void SetCard(short a_nCard, short a_nCardNr);
	void Sort(short a_nStart = 0, short a_nEnd = 12);
	void RemoveDecider();

	short SumAll(T_SERIE a_enSerie) const;
	short Sum(T_SERIE a_enSerie) const;
	short SumMinuses(T_SERIE a_enSerie, BOOL a_bRobber = FALSE) const;

	void AddScore(T_SERIE a_enSeria, T_GAMES a_enGame, short a_nScore);
	void SetScore(T_SERIE a_enSeria, T_GAMES a_enGame, short a_nScore);
	short GetScore(T_SERIE a_enSeria, T_GAMES a_enGame) const;
	short GetScore(T_SERIE a_enSeria, short a_nGame) const;

	BOOL SetPuzzleScore(short a_nScore, T_SERIE a_enSerie);
	void ClearScore(T_SERIE a_enSerie);

	const tstring& GetName() const;
	void  SetName(const tstring* a_pName);

	const CUserCards& GetPlayerCards() const;
	short DecideCardNr(short a_iTrick);

	void SetUsedCard(short a_nCard, BOOL a_bUsed);
	// serialization
	BOOL SaveScore(tfile* m_pFile) const;

	void FillScore(PLAYERSCORE& a_scorePlayer) const;
	void RestoreScore(const PLAYERSCORE& a_scorePlayer);

	PlayerType GetPlayerType() const { return m_typePlayer; }


private:

	T_PLAYER		m_playerMe = E_DL_NULL;
	const tstring*	m_pName = nullptr;	// wskazanie na nazwê z m_pRegData

	CUserCards		m_usercards;
	CScore			m_score[4];	// na cztery gry !
	CDecider*		m_pDecider = nullptr;
	PlayerType		m_typePlayer = PlayerType::Computer;
};

#endif // !defined(AFX_PLAYER_H__6DCA56C2_80FB_11D3_B413_E87410C10000__INCLUDED_)
