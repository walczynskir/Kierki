// Players.h: interface for the CPlayers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYERS_H__8BB517A1_11DE_11D4_A79A_347710C10000__INCLUDED_)
#define AFX_PLAYERS_H__8BB517A1_11DE_11D4_A79A_347710C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Player.h"
#include "SortCards.h"
#include <rcommon/RString.h>

class CPlayers
{

public:
	short SumPlayerAllScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie) const;
	short SumPlayerScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie) const;
	void ClearAllScores();
	void SetName(T_PLAYER a_enPlayer, const tstring* a_pName);
	void ClearScores(T_SERIE a_enSerie);
	void SetPuzzleScore(T_PLAYER a_enPlayer, short a_nScore, T_SERIE a_enSerie);
	CPlayers(T_PLAYER a_enFirstDealer);
	virtual ~CPlayers();

	void CreateDeciders(T_GAMES	a_enGame, const CTakenTricks* a_pTricks, T_COLOR a_colorTrumps);
	void CreateDeciders(const CPuzzleRows* a_pPuzzleRows);

	T_COLOR ChooseTrumps(T_PLAYER a_player) const;
	void DistributeCards(const CSortCards& a_sortcards);
	void SortAll(short a_nStart = 0, short a_nEnd = 12);
	void Sort(T_PLAYER a_player, short a_nStart = 0, short a_nEnd = 12);
	void RemoveDeciders();
	static T_PLAYER NextPlayer(T_PLAYER a_enCurrentPlayer);
	void SetNextFirstDealer();

	const CPlayer& GetPlayer(T_PLAYER a_enPlayer) const;

	const CUserCards& GetPlayerCards(T_PLAYER a_enPlayer) const;
	short DecideCardNr(T_PLAYER a_player, short a_iTrick);

	void SetUsedCard(T_PLAYER a_enPlayer, short a_nCard, BOOL a_bUsed);

	void AddScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie, T_GAMES a_enGame, short a_nScore);
	void SetScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie, T_GAMES a_enGame, short a_nScore);

	// saving and restoring
	void SaveState(LPSAVERESTORE a_pSaveRestore) const;
	void RestoreState(const LPSAVERESTORE a_pSaveRestore);

private:

	T_PLAYER m_enFirstDealer;
	CPlayer m_arPlayers[4];
};

#endif // !defined(AFX_PLAYERS_H__8BB517A1_11DE_11D4_A79A_347710C10000__INCLUDED_)
