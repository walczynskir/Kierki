// Players.h: interface for the CPlayers class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "Player.h"
#include "SortCards.h"
#include <rcommon/RString.h>
#include <array>

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
	CPlayer& operator[](Player a_player) { return m_arPlayers[a_player]; };

private:
	T_PLAYER m_enFirstDealer;
	PlayerArray<CPlayer> m_arPlayers;
};

