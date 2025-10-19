#pragma once
#include "TakenTricks.h"
#include "PuzzleRows.h"	
#include "Players.h"	
#include "SortCards.h"	
#include "Scores.h"	
#include "RegData.h"	
#include <rcommon/RString.h>
#include <filesystem>
#include <functional>

class HeartsGame
{

// Operations
public:
	HeartsGame(const tstring& a_sName1, const tstring& a_sName2, const tstring& a_sName3, const tstring& a_sName4);
	virtual ~HeartsGame();

// Implementation
public:

	// new game logic
	void Play();
	void Play(short a_nCard);
	bool CanPlayCard(T_PLAYER a_enPlayer, short a_nCard) const;


	// as for now - all below should be private
	void Clear();
	//void PlayCard(short a_iCardNr);	// human played card (card number in player's hand) - for the sake of invalidation in GUI

	void StartEntireGame(T_GAMES a_gameStart = E_GM_NOTRICKS);		// starts 4 series
	short GetPlayerTricksCnt(T_PLAYER a_enPlayer) const;
	void SetPlayerCard2Trick(T_PLAYER a_enPlayer, short a_nInHand);
	void SetPlayerCard2Trick2(T_PLAYER a_enPlayer, short a_nInHand);

	BOOL NextSerie();
	short SumPlayerScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie) const;
	short SumPlayerAllScore(T_PLAYER a_enPlayer, T_SERIE a_enSerie) const;
	T_SERIE GetSerie() const;
	void DecreasePuzzleScore();
	T_SUIT GetTrumps() const;
	BOOL GetPlayedTrick();

	void SetNextThrower() { m_pPlayers->SetNextThrower(); }
	void SetNextDealer() { m_pPlayers->SetNextDealer(); }
	const CPlayer& GetThrowerPlayer() { return m_pPlayers->GetThrowerPlayer(); };

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
	void SetLastTrickOwner(T_SUIT a_colorTrumps);
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
	T_PLAYER GetThrower() const;
	short CardsOnTable() const;
	void EndTrumpsChoice(T_SUIT a_enColor);
	BOOL IsTrumpsChoice() const;
	void NewGame();
	void NextGame(BOOL a_bNext);
	void StartNextGame();

	bool AreCardsRemaining() const;


	void DealEnd();

	tstring CreateTitle() const;

	void SetPass(bool a_bPass) { m_bPass = a_bPass; };
	bool IsPass() const { return m_bPass; };

	bool IsFile() const { return std::filesystem::exists(m_filePath); };
	const std::filesystem::path& GetFile() const { return m_filePath; };
	bool Save(const std::filesystem::path& a_filePath);
	bool Restore(const std::filesystem::path& a_filePath);
	void Fill(SAVERESTORE& a_save) const;

	void GameOver(void);
	bool IsGameSaved() const;

	// callbacks
	std::function<void()> OnPreStartGame;					// first game can be set
	std::function<void(T_PLAYER, short)> OnCardPlayed;		// window should be redrawn
	std::function<void(T_PLAYER)> OnWaitForUser;			// card should be selected and void Play(short a_nCard) with selected Card should be called. 
															// Card must be valid - check earlier with bool CanPlayCard(T_PLAYER a_enPlayer, short a_nCard) const;
															// If card is not valid - game will ask again for card

	std::function<void(bool*)> OnTrickTaken;				// either Window should wait for confirmation of trick by a user (e.g. click) or just redraw the window
															// user must return, who continues the game in the parameter - if return is true it means that user continued the game
															// if false - game will continue to play by itself

/*	
	std::function<void(T_PLAYER, CTrick)> OnTrickWon;
	std::function<void(T_PLAYER)> OnPlayerTurn;
	std::function<void(T_GAMES)> OnGameStarted;
	std::function<void(T_GAMES)> OnGameEnded;
*/


private:
	BOOL ChooseTrumps();
	BOOL DealStart(BOOL a_bTrumpsChoice = FALSE);
	tstring DecodeTrumps() const;
	short PointsInTrick(short a_iTrick) const;
	BOOL Continue();
	static bool ReadFromFile(const std::filesystem::path& a_sFilePath, SAVERESTORE& a_outRestore);

#ifdef _DEBUG
	void CheckDiff(const SAVERESTORE& a_save, const SAVERESTORE& a_restore) const;
#endif

	void StartSerie();												// starts 12 games
	void StartGame();												// start one game (12 different games of GameType
	void EndGame();
	void EndSerie();
	void EndEntireGame();

	void StartTrick();
	void EndTrick();



	// events
	void TrickStartedEvent();
	void GameStartedEvent();
	void SerieStartedEvent();
	void EntireGameStartedEvent();
	void TrickEndedEvent(bool* a_pbPlayed);
	void GameEndedEvent();
	void SerieEndedEvent();
	void EntireGameEndedEvent();

	void CardPlayedEvent(Player a_playerThrower, short a_nCard);



	CTakenTricks m_tricks{};
	CPuzzleRows  m_PuzzleRows;
	T_GAMES		 m_enGame;
	T_SUIT		 m_colorTrumps;
	bool		 m_bTrumpsChoice;
	CSortCards	 m_sortcards;
	bool		 m_bDealed;
	CPlayers*    m_pPlayers;	// old approach to keep pointer instead of member - as I need to pass data to constructors
	short		 m_nPuzzleScore;
	T_SERIE		 m_enSerie;
	bool		 m_bPass;
	std::filesystem::path		 m_filePath;

};
