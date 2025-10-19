// Players.h: interface for the CPlayers class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Player.h"
#include "SortCards.h"
#include <rcommon/RString.h>
#include <random>




class CPlayers
{

public:
	CPlayers();
	virtual ~CPlayers();

	short SumPlayerAllScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie) const;
	short SumPlayerScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie) const;
	void ClearAllScores();
	void SetName(T_PLAYER a_enPlayer, const tstring* a_pName);
	void ClearScores(T_SERIE a_enSerie);
	void SetPuzzleScore(T_PLAYER a_enPlayer, short a_nScore, T_SERIE a_enSerie);

	void CreateDeciders(T_GAMES	a_enGame, const CTakenTricks* a_pTricks, T_SUIT a_colorTrumps);
	void CreateDeciders(const CPuzzleRows* a_pPuzzleRows);

	T_SUIT ChooseTrumps() const;
	void DistributeCards(const CSortCards& a_sortcards);
	void SortAll(short a_nStart = 0, short a_nEnd = 12);
	void Sort(T_PLAYER a_player, short a_nStart = 0, short a_nEnd = 12);
	void RemoveDeciders();
	static T_PLAYER NextPlayer(T_PLAYER a_enCurrentPlayer);

	const CPlayer& GetPlayer(T_PLAYER a_enPlayer) const;

	const CUserCards& GetPlayerCards(T_PLAYER a_enPlayer) const;
	short DecideCardNr(T_PLAYER a_player, short a_iTrick);

	void SetUsedCard(T_PLAYER a_enPlayer, short a_nCard, BOOL a_bUsed);

	void AddScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie, T_GAMES a_enGame, short a_nScore);
	void SetScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie, T_GAMES a_enGame, short a_nScore);

	bool HaveCardsToPlay() const;

	// setters / getters
	T_PLAYER GetThrower() const { return m_enThrower; };
	const CPlayer& GetThrowerPlayer() const { ASSERT(m_enThrower != E_DL_NULL); return m_arPlayers[m_enThrower]; };

	T_PLAYER GetDealer() const { return m_enDealer; };
	void SetThrower(T_PLAYER a_enThrower) { m_enThrower = a_enThrower; };
	void SetDealer(T_PLAYER a_enDealer) { m_enDealer = a_enDealer; };
	void SetNextThrower() { m_enThrower = NextPlayer(m_enThrower); };
	void SetNextDealer() { m_enDealer = NextPlayer(m_enDealer); };
	void SetThrowerFromDealer() { m_enThrower = NextPlayer(m_enDealer); };

	// saving and restoring state
	void RestoreScore(const SAVERESTORE& a_restore);
	void FillScore(SAVERESTORE& a_save);
	bool StateChanged() const;

private:
	T_PLAYER DrawRandomDealer();
	T_PLAYER m_enThrower = E_DL_NULL;
	T_PLAYER m_enDealer = E_DL_NULL;

	CPlayer m_arPlayers[4];

	std::mt19937 m_gen;
};

