#pragma once
#include "TakenTricks.h"
#include "PuzzleRows.h"	
#include "Players.h"	
#include "SortCards.h"	
#include "Scores.h"	
#include "RegData.h"	
#include <rcommon/RString.h>


class GameData
{

// Operations
public:

	GameData();
	virtual ~GameData();

// Implementation
public:
	bool IsFile() const { return (m_sFile.length() != 0); };
	const tstring& GetFile() const { return m_sFile; };
	short GetPlayerTricksCnt(T_PLAYER a_enPlayer) const;
	void SetPlayerCard2Trick(T_PLAYER a_enPlayer, short a_nInHand);
	BOOL NextSerie();
	short SumPlayerScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie) const;
	short SumPlayerAllScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie) const;
	T_SERIE GetSerie() const;
	void DecreasePuzzleScore();
	T_COLOR GetTrumps() const;
	BOOL GetPlayedTrick();
	void SetNextPlayer();
	short SumPlayerAll(T_PLAYER a_enPlayer, T_SERIE a_enSeria) const;
	short SumPlayer(T_PLAYER a_enPlayer, T_SERIE a_enSeria) const;
	short SumPlayerMinuses(T_PLAYER a_enPlayer, T_SERIE a_enSeria, BOOL a_bRobber = FALSE) const;
	short GetPlayerScore(T_PLAYER a_enPlayer, T_SERIE a_enSeria, T_GAMES a_enGame) const;
	T_GAMES GetGame() const;
	const CPuzzleRows& GetPuzzleRows() const;
	void SetPlayerCard2Puzzle(T_PLAYER a_enPlayer, short a_nInHand);
	BOOL HasCardForPuzzle(T_PLAYER a_enPlayer) const;
	const CTakenTricks& GetTricks() const;
	void SetScores();
	void SetPuzzleScore(T_PLAYER a_enPlayer);
	T_PLAYER SetLastTrickOwner(T_COLOR a_colorTrumps);
	const CTrick& GetTrick(short a_nTrick) const;
	const CTrick& GetLastTrick() const;
	short GetPlayerCardNr(T_PLAYER a_enPlayer, short a_nInHand) const;
	const CCard& GetPlayerCard(T_PLAYER a_enPlayer, short a_nInHand) const;
	void SetUsedPlayerCard(T_PLAYER a_enPlayer, short a_nCard, BOOL a_bUsed);

	void SetTrickCard(const CCard& a_card);
	void NextTrick();
	short DecideCardNr();
	BOOL IsDealed() const;
	short GetTricksCnt() const;
	const CUserCards& GetPlayerCards(T_PLAYER a_enPlayer) const;
	T_PLAYER GetDealer() const;
	T_PLAYER GetThrower() const;
	short CardsOnTable() const;
	void EndTrumpsChoice(T_COLOR a_enColor);
	BOOL IsTrumpsChoice() const;
	void NewGame();
	void NextGame(BOOL a_bNext);

	void DealEnd();

	tstring CreateTitle() const;

	void SetPass(bool a_bPass) { m_bPass = a_bPass; };
	bool IsPass() const { return m_bPass; };
	bool Save(LPCTSTR a_psFile);
	bool Restore(LPCTSTR a_psFile);
	void GameOver(void);

	CRegData	 m_regData;

private:
	BOOL ChooseTrumps();
	void CreatePlayers();
	BOOL DealStart(BOOL a_bTrumpsChoice = FALSE);
	tstring DecodeTrumps() const;
	short PointsInTrick(short a_iTrick) const;
	BOOL Continue();

	T_PLAYER GetFirstDealerAndSetNext();

	CTakenTricks m_tricks;
	CPuzzleRows  m_PuzzleRows;
	T_GAMES		 m_enGame;
	T_COLOR		 m_colorTrumps;
	BOOL		 m_bTrumpsChoice;
	CSortCards	 m_sortcards;
	BOOL		 m_bDealed;
	T_PLAYER	 m_enDealer;
	T_PLAYER	 m_enThrower;
	CPlayers*    m_pPlayers;	// bêdzie utworzony w konstruktorze
	short		 m_nPuzzleScore;
	T_SERIE		 m_enSerie;
	bool		 m_bPass;
	tstring		 m_sFile;
};
