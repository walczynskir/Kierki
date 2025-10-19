// HeartsGame.cpp : implementation of the HeartsGame class
//

#include "stdafx.h"
#include "resource.h"
#include "HeartsGame.h"
#include <fstream>



#pragma todo("players name should be external to CHEartsGameLogic")


/////////////////////////////////////////////////////////////////////////////
// HeartsGame construction/destruction




// ---------------------------------------------------------
// Konstruktor
//
HeartsGame::HeartsGame(const tstring& a_sName1, const tstring& a_sName2, const tstring& a_sName3, const tstring& a_sName4)
{
#pragma todo("get rid of names in CPlayer")
	m_pPlayers = new CPlayers();
	m_pPlayers->SetName(E_DL_1, &a_sName1);
	m_pPlayers->SetName(E_DL_2, &a_sName2);
	m_pPlayers->SetName(E_DL_3, &a_sName3);
	m_pPlayers->SetName(E_DL_4, &a_sName4);

	Clear();
}


// ---------------------------------------------------------
// Destruktor
//
HeartsGame::~HeartsGame()
{
	delete m_pPlayers;
}



// main game play method - recursive
// usually it is called only once by UI before finishing entire game, all other control is done via the events
// to pass the card played by human player, call PlayCard(short a_iCardNr) method
void HeartsGame::Play()
{
	// where we are - entire game, serie, game, trick. This function can be called to continue the game from any point, and recursively 
	if ((m_enSerie == Serie::E_SR_4) && (m_enGame == GameType::E_GM_PUZZLE) && !AreCardsRemaining())
	{
		// nothing to do - game over 
		EndEntireGame();
		return;
	}

	/*  
	if (m_enGame == GameType::E_GM_PUZZLE && !AreCardsRemaining())
		EndSerie();
	else if (m_tricks.GetTricksCnt() == 13)
		EndGame();
	else if (m_tricks.GetCurrentTrick().GetCardsCnt() == 4)
		EndTrick();*/
	if (m_enGame == GameType::E_GM_NULL)
	{
		StartGame();	// StartGame will call Play again
		return;
	}

	if (m_tricks.GetCurrentTrick().GetCardsCnt() == 4)
	{
		EndTrick();		// EndTrick will call Play again (or user will need to call Play())
		return;

	}
	// continue
	if (GetThrowerPlayer().GetPlayerType() == CPlayer::PlayerType::Human)
	{
		if (OnWaitForUser)
			OnWaitForUser(GetThrower());
		else
			ASSERT(FALSE);	// should not happen - UI must set this callback
	}
	else
	{
		short l_iCardNr = DecideCardNr();

		Play(l_iCardNr);
	}

}


void HeartsGame::Play(short a_iCardNr)
{
	ASSERT(a_iCardNr >= 0);
	ASSERT(a_iCardNr < 13);

	Player l_playerThrower = GetThrower();

	SetPlayerCard2Trick2(l_playerThrower, a_iCardNr);

	// notify listeners
	CardPlayedEvent(l_playerThrower, a_iCardNr);

	SetNextThrower();
	Play();
}





void HeartsGame::StartEntireGame(T_GAMES a_gameStart)
{
	m_enGame = a_gameStart;
	EntireGameStartedEvent();
	StartSerie();
}


void HeartsGame::EndEntireGame()
{
	EntireGameEndedEvent();
	Clear();
	m_pPlayers->ClearAllScores();
}

void HeartsGame::StartSerie()
{
	SerieStartedEvent();
	StartGame();
	EndSerie();
}


void HeartsGame::EndSerie()
{
	SerieEndedEvent();

	if (m_enSerie == Serie::E_SR_4)
	{
		EndEntireGame();
		return;
	}
	else
	{
		m_enSerie = SerieCycle::Next(m_enSerie);
		StartSerie();
	}
}

void HeartsGame::StartGame()
{
	if (OnPreStartGame)
		OnPreStartGame();	// here UI can set up game parameters like starting game. 

	if (m_enGame == GameType::E_GM_NULL)
		m_enGame = GameCycle::Next(m_enGame);

	m_sortcards.ShuffleDeck();
	m_pPlayers->DistributeCards(m_sortcards);
	m_pPlayers->SetThrowerFromDealer();

#pragma todo("Not implemented choosing trump - should be before sort players cards")
	m_pPlayers->SortAll();
	m_bDealed = TRUE;
	m_pPlayers->CreateDeciders(m_enGame, &m_tricks, m_colorTrumps);

	GameStartedEvent();

	StartTrick();

}


void HeartsGame::EndGame()
{
	GameEndedEvent();

	m_tricks.Clear();
	m_PuzzleRows.Clear();
	m_pPlayers->RemoveDeciders();
	m_pPlayers->SetNextDealer();

	m_colorTrumps = Suit::E_CC_NULL;
	m_bDealed = false;
	m_bTrumpsChoice = false;
	m_bPass = false;

	if (m_enGame == GameType::E_GM_PUZZLE)
	{
		m_enGame = GameCycle::Next(m_enGame);
		StartSerie();
	}
	else
	{
		m_enGame = GameCycle::Next(m_enGame);
		StartGame();
	}
}


void HeartsGame::StartTrick()
{
	ASSERT(IsDealed());
	TrickStartedEvent();

	NextTrick();
	Play();
}

void HeartsGame::EndTrick()
{
	bool l_bPlayed = false;
	TrickEndedEvent(&l_bPlayed);
	if (!l_bPlayed)
		Play();
}







void HeartsGame::Clear()
{
	m_tricks.Clear();
	m_pPlayers->RemoveDeciders();
	m_pPlayers->ClearAllScores();
	m_PuzzleRows.Clear();
	m_enGame = E_GM_NULL;
	m_enSerie = E_SR_1;
	m_colorTrumps = E_CC_NULL;

	m_bDealed = false;
	m_bTrumpsChoice = false;
	m_bPass = false;
#pragma todo("Remove filePath from here - should be in some GameSession or so")
	m_filePath = std::filesystem::path();
}


// ---------------------------------------------------------
//	Przejœcie do nastêpnej gry
//  Ustawia nastêpn¹ grê, tworzy Deciderów u graczy
//	rozdaje karty
//
void	
HeartsGame::NextGame(
	BOOL a_bNext	//IN change game
	)
{
	m_tricks.Clear();
	m_pPlayers->RemoveDeciders();
	m_PuzzleRows.Clear();

	if (a_bNext)
	{
		if (m_enGame == E_GM_PUZZLE)
		{
			ASSERT(m_enSerie != E_SR_4);
			m_enGame = E_GM_NOTRICKS;
			m_enSerie = static_cast<T_SERIE>(m_enSerie + 1);
		}
		else
		{
			m_enGame = static_cast<T_GAMES>(m_enGame + 1);
		}
	}
	m_colorTrumps = E_CC_NULL;

	// w przypadku odgrywek trzeba w miêdzyczasie wybraæ atu.
	switch (m_enGame)
	{
	case E_GM_RECOVER1:
	case E_GM_RECOVER2:
	case E_GM_RECOVER3:
	case E_GM_RECOVER4:
		if (!DealStart(TRUE))
			return;
		break;

	default:
		DealStart();
	}
	DealEnd();
	GameStartedEvent();
}


// ---------------------------------------------------------
//	Rozpoczêcie procesu rozdawania kart
//	Zwraca czy nale¿y wybieraæ atu rêcznie
//
BOOL	//WY czy zwróciæ sterowanie do View w celu wybrania atu
HeartsGame::DealStart(
	BOOL a_bTrumpsChoice	//WE TRUE: wybór atu
	)
{
	BOOL l_bCont = TRUE;;

	// potasowanie kart i rozdanie graczom
	m_sortcards.ShuffleDeck();
	m_pPlayers->DistributeCards(m_sortcards);
	
	// jeœli mamy wybór atu - wiêksza zabawa
	if (a_bTrumpsChoice)
		l_bCont = ChooseTrumps();
	return l_bCont;
}


// ---------------------------------------------------------
//	Zakoñczenie procesu rozdawania kart
//
void
HeartsGame::DealEnd()
{
	// posortuj karty w ka¿dej z 4 paczek
	m_pPlayers->SortAll();

	// wyzerujmy lewy
	m_tricks.Clear();
	m_bDealed = TRUE;	

	// deciders must be created after cards were dealt
	if (m_enGame == E_GM_PUZZLE)
	{
		m_pPlayers->CreateDeciders(&m_PuzzleRows);
		m_nPuzzleScore = 10;
	}
	else
	{
		m_pPlayers->CreateDeciders(m_enGame, &m_tricks, m_colorTrumps);
	}

	// now we can set the next dealer
	m_pPlayers->SetNextDealer();
	// first thrower is the same as next dealer
	m_pPlayers->SetThrower(m_pPlayers->GetDealer());

	if (m_enGame != E_GM_PUZZLE)
	{
		NextTrick();
	}
}

#pragma todo ("needs refactoring - and bug fix for not displaying sometimes chosen trump")
// ---------------------------------------------------------
// returns window title
//
tstring	HeartsGame::CreateTitle() const 
{
#pragma todo ("move logic to GamWend";)
	tstring l_sTitle(_T(""));
	tstring l_sAdd(_T(""));
	tstring l_sGame(_T(""));
	UINT	l_idStr = -1;

	switch (m_enGame)
	{
	case E_GM_NOTRICKS:
		l_idStr = IDS_TITLE_NOTRICKS;
		break ;
	case E_GM_NOHEARTS:
		l_idStr = IDS_TITLE_NOHEARTS;
		break ;
	case E_GM_NOBOYS:
		l_idStr = IDS_TITLE_NOBOYS;
		break ;
	case E_GM_NOLADIES:
		l_idStr = IDS_TITLE_NOLADIES;
		break ;
	case E_GM_NOKINGOFHEART:
		l_idStr = IDS_TITLE_NOKINGOFHEART;
		break ;
	case E_GM_NOSIXANDLAST:
		l_idStr = IDS_TITLE_NOSIXANDLAST;
		break ;
	case E_GM_ROBBER:
		l_idStr = IDS_TITLE_ROBBER;
		break ;
	case E_GM_RECOVER1:
	case E_GM_RECOVER2:
	case E_GM_RECOVER3:
	case E_GM_RECOVER4:
		l_idStr = IDS_TITLE_RECOVER;
		if (!IsTrumpsChoice())
		{
			l_sAdd = _T(" ") + DecodeTrumps();
		}
		break ;
	case E_GM_PUZZLE:
		l_idStr = IDS_TITLE_PUZZLE;
		break ;
	default:
		ASSERT(FALSE);
	}

	ASSERT(l_idStr > 0);
	TCHAR l_sStr[128];
	::LoadString(::GetModuleHandle(NULL), IDS_APP_TITLE, l_sStr, ArraySize(l_sStr));
	l_sGame = l_sStr;
	::LoadString(::GetModuleHandle(NULL), l_idStr, l_sStr, ArraySize(l_sStr));
	l_sTitle = l_sStr;

	_sntprintf_s(l_sStr, ArraySize(l_sStr), _TRUNCATE, _T("%s - %s%s"), l_sGame.c_str(), l_sTitle.c_str(), l_sAdd.c_str());
	return l_sStr;
}


bool HeartsGame::AreCardsRemaining() const
{
	return m_pPlayers->HaveCardsToPlay();
}

// ---------------------------------------------------------
//	Rozpoczêcie nowej gry.
//
void HeartsGame::NewGame()
{
	// TODO put into "hidden" regisry variable
#ifdef _DEBUG
	m_enGame = E_GM_NULL;
/*	m_enGame = E_GM_NOTRICKS;
	m_enGame = E_GM_NOHEARTS;
	m_enGame = E_GM_NOLADIES;
	m_enGame = E_GM_ROBBER;
	m_enGame = E_GM_RECOVER1;
	m_enGame = E_GM_RECOVER2;
	m_enGame = E_GM_RECOVER3;
	m_enGame = E_GM_RECOVER4;
	m_enGame = E_GM_NOTHING;
	m_enGame = E_GM_RECOVER4;
*/#else
	m_enGame = E_GM_NOTHING;
#endif
	m_enSerie = E_SR_1;
	m_pPlayers->ClearAllScores();
	m_PuzzleRows.Clear();
	NextGame(TRUE);
}


// ---------------------------------------------------------
//	Wybór atu (rêczny lub automatyczny)
//
BOOL	//WY czy przyst¹piæ do rozdania wszystkich kart
HeartsGame::ChooseTrumps()
{
	switch (m_pPlayers->GetDealer())
	{
	case E_DL_4:	// player chooses trumps, if dealer is E_DL_4
		m_pPlayers->Sort(E_DL_1, 0, 6);
		m_bTrumpsChoice = TRUE;
		return FALSE;

	default:
		m_colorTrumps = m_pPlayers->ChooseTrumps();
		return TRUE;
	}
	return TRUE;
}


// ---------------------------------------------------------
BOOL	//WY zwraca czy wybieramy atu
HeartsGame::IsTrumpsChoice() const
{
	return m_bTrumpsChoice;
}


// ---------------------------------------------------------
//	Zakoñczenie procesu wyboru atu
//
void 
HeartsGame::EndTrumpsChoice(
	T_SUIT a_enColor	//WE jaki kolor zosta³ wybrany
	)
{
	m_colorTrumps = a_enColor;
	m_bTrumpsChoice = FALSE;
}


// ---------------------------------------------------------
//	Zakoñczenie procesu wyboru atu
//
short	//WY ilosæ kart na stole
HeartsGame::CardsOnTable() const
{
	if (m_enGame == E_GM_PUZZLE)
	{
		return m_PuzzleRows.GetCardsCnt();
	}
	else
	{
		return m_tricks.GetCurrentTrick().GetCardsCnt();
	}
}


// ---------------------------------------------------------
// Aktualnie rzucajacy kartê na stó³
//
T_PLAYER	//WY gracz rzucaj¹cy kartê na stó³
HeartsGame::GetThrower() const
{
	return m_pPlayers->GetThrower();
}



// ---------------------------------------------------------
// Zwraca karty gracza
//
const CUserCards&  //WY karty gracza
HeartsGame::GetPlayerCards(
	T_PLAYER a_enPlayer		//WE gracz
	) const
{
	return m_pPlayers->GetPlayerCards(a_enPlayer);
}


// ---------------------------------------------------------
// Iloœæ lew które ju¿ posz³y w grze
//
short	//WY iloœæ lew
HeartsGame::GetTricksCnt() const
{
	return m_tricks.GetTricksCnt();
}


// ---------------------------------------------------------
// Czy karty s¹ rozdane
//
BOOL	//WY TRUE - karty rozdane
HeartsGame::IsDealed() const
{
	return m_bDealed;
}


// ---------------------------------------------------------
// returns number of the card in hand to be played
//
short HeartsGame::DecideCardNr()
{
	return m_pPlayers->DecideCardNr(GetThrower(), GetTricksCnt());
}



// ---------------------------------------------------------
// Ustalenie nastêpnej lewy
//
void 
HeartsGame::NextTrick()
{
	m_tricks.NextTrick();
	TrickStartedEvent();
}


// ---------------------------------------------------------
// Przydzielenie lewie karty kolejnej
//
void 
HeartsGame::SetTrickCard(
	const CCard& a_card		//WE karta
	)
{
	m_tricks.SetCard(a_card);
}


// ---------------------------------------------------------
//	Zwraca nazwê koloru w zale¿noœci od wybranego koloru atu
//
tstring		//WY nazwa koloru
HeartsGame::DecodeTrumps() const
{
	UINT l_idStr;

	switch (m_colorTrumps)
	{
	case E_CC_CLUB:
		l_idStr = IDS_COLOR_CLUB;
		break;
	case E_CC_DIAMOND:
		l_idStr = IDS_COLOR_DIAMOND;
		break;
	case E_CC_SPADE:
		l_idStr = IDS_COLOR_SPADE;
		break;
	case E_CC_HEART:
		l_idStr = IDS_COLOR_HEART;
		break;
	case E_CC_NOTRUMPS:
		l_idStr = IDS_COLOR_NOTRUMP;
		break;
	default:
		ASSERT(FALSE);
	}

	TCHAR l_sColor[128];
	::LoadString(::GetModuleHandle(NULL), l_idStr, l_sColor, ArraySize(l_sColor));
	return l_sColor;
}


// ---------------------------------------------------------
//	Ustawia kartê na u¿yt¹ dla gracza
//
void 
HeartsGame::SetUsedPlayerCard(
	T_PLAYER a_enPlayer, //WE gracz
	short a_nCard,		 //WE karta
	BOOL a_bUsed		 //WE czy u¿yta
	)
{
	m_pPlayers->SetUsedCard(a_enPlayer, a_nCard, a_bUsed);
}


// ---------------------------------------------------------
//	Zwraca numer karty na podstawie numeru w rêce
//
short	//WY numer karty
HeartsGame::GetPlayerCardNr(
	T_PLAYER a_enPlayer, //WE gracz
	short a_nInHand		 //WE która w rêce
	) const
{
	const CUserCards& l_usercards = m_pPlayers->GetPlayerCards(a_enPlayer);	
	return l_usercards.CardNr(a_nInHand);
}


// ---------------------------------------------------------
//	Zwraca kartê na podstawie numeru w rêce
//
const CCard&	//WY karta
HeartsGame::GetPlayerCard(
	T_PLAYER a_enPlayer, //WE gracz
	short a_nInHand		 //WE która w rêce
	) const
{
	const CUserCards& l_usercards = m_pPlayers->GetPlayerCards(a_enPlayer);	
	return l_usercards[a_nInHand];
}


// ---------------------------------------------------------
// Zwraca ostatni¹ lewê
//
const CTrick&	//WY ostatnia lewa
HeartsGame::GetLastTrick() const
{
	return m_tricks[GetTricksCnt() - 1];
}


// ---------------------------------------------------------
// Zwraca ostatni¹ lewê
//
const CTrick&	//WY lewa
HeartsGame::GetTrick(
	short a_nTrick		//WE numer lewy
	) const
{
	return m_tricks[a_nTrick];
}


// ---------------------------------------------------------
// Ustawia w³aœciciela ostatniej lewy i zwraca go
//
void HeartsGame::SetLastTrickOwner(
	T_SUIT a_colorTrumps	//WE kolor atu lub E_CC_NOTHING
	)
{
	m_pPlayers->SetThrower(m_tricks.SetLastTrickOwner(a_colorTrumps));
}


// ---------------------------------------------------------
//	Zapisanie wyników gry
//
void HeartsGame::SetScores()
{
	short l_iTrick ; 
	short l_nTrickCnt = GetTricksCnt();

	switch (m_enGame)
	{
	case E_GM_NOTRICKS:
		
		for (l_iTrick = 1; l_iTrick <= l_nTrickCnt; l_iTrick++) 
		{
			T_PLAYER l_enPlayer = m_tricks[l_iTrick - 1].GetTrickOwner();
			m_pPlayers->AddScore(l_enPlayer, m_enSerie, m_enGame, 2);
		}
		break;

	case E_GM_NOHEARTS:
		for (l_iTrick = 1; l_iTrick <= l_nTrickCnt; l_iTrick++) 
		{
			short l_nHearts = m_tricks[l_iTrick - 1].GetHeartsCnt() ;
			T_PLAYER l_enPlayer = m_tricks[l_iTrick - 1].GetTrickOwner();
			m_pPlayers->AddScore(l_enPlayer, m_enSerie, m_enGame, 2 * l_nHearts);
		}
		break;

	case E_GM_NOBOYS:
		for (l_iTrick = 1; l_iTrick <= l_nTrickCnt; l_iTrick++) 
		{
			short l_nBoys = m_tricks[l_iTrick - 1].GetBoysCnt() ;
			T_PLAYER l_enPlayer = m_tricks[l_iTrick - 1].GetTrickOwner();
			m_pPlayers->AddScore(l_enPlayer, m_enSerie, m_enGame, 3 * l_nBoys);
		}
		break;

	case E_GM_NOLADIES:
		for (l_iTrick = 1; l_iTrick <= l_nTrickCnt; l_iTrick++) 
		{
			short l_nLadies = m_tricks[l_iTrick - 1].GetLadiesCnt();
			T_PLAYER l_enPlayer = m_tricks[l_iTrick - 1].GetTrickOwner();
			m_pPlayers->AddScore(l_enPlayer, m_enSerie, m_enGame, 5 * l_nLadies);
		}
		break;

	case E_GM_NOKINGOFHEART:
		for (l_iTrick = 1; l_iTrick <= l_nTrickCnt; l_iTrick++) 
		{
			short l_bKingOfHeart = m_tricks[l_iTrick - 1].IsKingOfHeart();
			T_PLAYER l_enPlayer = m_tricks[l_iTrick - 1].GetTrickOwner();
			if (l_bKingOfHeart)
			{
				m_pPlayers->SetScore(l_enPlayer, m_enSerie, m_enGame, 16);
				break;
			}
		}
		break;

	case E_GM_NOSIXANDLAST:
		{
			T_PLAYER l_enPlayer = m_tricks[6 - 1].GetTrickOwner();
			m_pPlayers->SetScore(l_enPlayer, m_enSerie, m_enGame, 9);
			l_enPlayer = m_tricks[13 - 1].GetTrickOwner();
			m_pPlayers->AddScore(l_enPlayer, m_enSerie, m_enGame, 9);
		}
		break;

	case E_GM_ROBBER:
		for (l_iTrick = 1; l_iTrick <= l_nTrickCnt; l_iTrick++) 
		{
			T_PLAYER l_enPlayer = m_tricks[l_iTrick - 1].GetTrickOwner();
			short l_nPoints = PointsInTrick(l_iTrick);
			m_pPlayers->AddScore(l_enPlayer, m_enSerie, m_enGame, l_nPoints);
		}
		break;
	case E_GM_RECOVER1:
	case E_GM_RECOVER2:
	case E_GM_RECOVER3:
	case E_GM_RECOVER4:
		
		for (l_iTrick = 1; l_iTrick <= l_nTrickCnt; l_iTrick++) 
		{
			T_PLAYER l_enPlayer = m_tricks[l_iTrick - 1].GetTrickOwner();
			m_pPlayers->AddScore(l_enPlayer, m_enSerie, m_enGame, 5);
		}
		break;
	}
}


// ---------------------------------------------------------
// Ile punktów w lewie
//
short	//WY iloœæ punktów
HeartsGame::PointsInTrick(
	short a_iTrick	//WE numer lewy
	) const
{
	const CTrick& l_trick = m_tricks[a_iTrick - 1]; 
	short l_nPoints = l_trick.AllPointsCnt();
	if (a_iTrick == 6)
	{
		l_nPoints += 9;
	}
	if (a_iTrick == 13)
	{
		l_nPoints += 9;
	}
	return l_nPoints;
}


// ---------------------------------------------------------
// Referencja na lewy
//
const CTakenTricks&		//WY lewy
HeartsGame::GetTricks() const
{
	return m_tricks;
}


// ---------------------------------------------------------
// Czy ma karty do gry w puzzle
//
BOOL	//WY TRUE - ma
HeartsGame::HasCardForPuzzle(
	T_PLAYER a_enPlayer		//WE gracz
	) const
{
	return GetPlayerCards(a_enPlayer).HasCardForPuzzle(&m_PuzzleRows);
}


// ---------------------------------------------------------
// Do³o¿enie karty do puzzli
//
void 
HeartsGame::SetPlayerCard2Puzzle(
	T_PLAYER a_enPlayer,	//WE gracz
	short a_nInHand			//WE która karta na rêce
	)
{
	const CCard& l_card = GetPlayerCard(a_enPlayer, a_nInHand);
	ASSERT(!l_card.IsUsed());

	// decyzja o tym komu przyznamy kartê
	ASSERT(CardsOnTable() <= 52);

	m_PuzzleRows.PutCard(&l_card);
	SetNextThrower();

	// ustawienie u¿ycia karty
	SetUsedPlayerCard(a_enPlayer, a_nInHand, TRUE);
	if (GetPlayerCards(a_enPlayer).CardsLeft() == 0)
	{
		SetPuzzleScore(a_enPlayer);
	}
}


// ---------------------------------------------------------
//	Zapisanie wyniku puzzli dla gracza
//
void	
HeartsGame::SetPuzzleScore(
	T_PLAYER a_enPlayer	//WE gracz
	)
{
	m_pPlayers->SetPuzzleScore(a_enPlayer, m_nPuzzleScore, m_enSerie);
	DecreasePuzzleScore();
}

// ---------------------------------------------------------
// Zwraca wierze uk³adanki
//
const CPuzzleRows&	//WY wiersze uk³adanki
HeartsGame::GetPuzzleRows() const
{
	return m_PuzzleRows;
}


// ---------------------------------------------------------
// Zwraca aktualn¹ grê
//
T_GAMES		//WY aktualna gra
HeartsGame::GetGame() const
{
	return m_enGame;
}


// ---------------------------------------------------------
// Zwraca wynik dla gracza w serii i 
//
short	//WY wynik
HeartsGame::GetPlayerScore(
	T_PLAYER	a_enPlayer,	//WE gracz
	T_SERIE		a_enSeria,	//WE seria  
	T_GAMES		a_enGame	//WE gra
	) const
{
	return m_pPlayers->GetPlayer(a_enPlayer).GetScore(a_enSeria, a_enGame);
}


// ---------------------------------------------------------
// Oblcza sumê minusów dla gracza
//
short	//WY suma minusów
HeartsGame::SumPlayerMinuses(
	T_PLAYER a_enPlayer, //WE gracz
	T_SERIE a_enSeria,		 //WE seria
	BOOL  a_bRobber		 //WE czy z rozbójnikiem
	) const
{
	return m_pPlayers->GetPlayer(a_enPlayer).SumMinuses(a_enSeria, a_bRobber);
}


// ---------------------------------------------------------
// Oblcza sumê dla gracza (bez uk³adanki)
//
short	//WY suma
HeartsGame::SumPlayer(
	T_PLAYER a_enPlayer,	//WE gracz
	T_SERIE a_enSeria		//WE seria
	) const
{
	return m_pPlayers->GetPlayer(a_enPlayer).Sum(a_enSeria);
}


// ---------------------------------------------------------
// Oblcza sumê dla gracza z uk³adank¹
//
short	//WY suma
HeartsGame::SumPlayerAll(
	T_PLAYER a_enPlayer,	//WE gracz
	T_SERIE a_enSeria		//WE seria
	) const
{
	return m_pPlayers->GetPlayer(a_enPlayer).SumAll(a_enSeria);
}


// ---------------------------------------------------------
// Wziêcie lewy. Zwraca, czy kontynuowaæ grê
//
BOOL	//WY TRUE: kontynuowaæ grê 
HeartsGame::GetPlayedTrick()
{
	SetLastTrickOwner(m_colorTrumps);
	return Continue();
}


void HeartsGame::StartNextGame()
{
	m_bDealed = FALSE;
	SetScores();
	NextGame(TRUE);
}




// ---------------------------------------------------------
//	Czy kontyuowaæ grê (nie ma sensu np. w przypadku 
//	gry o krola kier gdy wziêty w lewie)
///
BOOL	//WY TRUE kontynuowaæ
HeartsGame::Continue()
{
	switch (GetGame())
	{
	case E_GM_NOHEARTS:
		return (GetTricks().TakenHearts() < 13);
	case E_GM_NOBOYS:
		return (GetTricks().TakenBoys() < 8);
	case E_GM_NOLADIES:
		return (GetTricks().TakenLadies() < 4);
	case E_GM_NOKINGOFHEART:
		{
			const CTrick& l_trick = GetLastTrick();
			return (!l_trick.IsKingOfHeart());
		}
	default:
		if (GetTricksCnt() <= 12)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
		
}


// ---------------------------------------------------------
// Zwraca kolor atu
//
T_SUIT		//WY kolor atutowy
HeartsGame::GetTrumps() const
{
	return m_colorTrumps;
}


// ---------------------------------------------------------
//	Zmniejsza punktacjê w puzzlach
//
void 
HeartsGame::DecreasePuzzleScore()
{
	switch (m_nPuzzleScore)
	{
	case 10:
		m_nPuzzleScore = 5;
		break;
	case 5:
		m_nPuzzleScore = -5;
		break;
	case -5:
		m_nPuzzleScore = -10;
		break;
	case -10:
		break;

	default:
		ASSERT(FALSE);	
	}
}


// ---------------------------------------------------------
// Zwraca aktualnie grana seriê
//
T_SERIE		//WY grana seria
HeartsGame::GetSerie() const
{
	return m_enSerie;
}


// ---------------------------------------------------------
// Zwraca sumê punktów gracza z poprzednich przed a_enSerie
// grami
//
short	//WY suma punktów
HeartsGame::SumPlayerScore(
	T_PLAYER a_enPlayer,	//WE gracz
	T_SERIE a_enSerie		//WE seria
	) const
{
	return m_pPlayers->SumPlayerScore(a_enPlayer, a_enSerie);
}


// ---------------------------------------------------------
// Zwraca sumê punktów gracza wszystkich gier
//
short	//WY suma punktów
HeartsGame::SumPlayerAllScore(
	T_PLAYER a_enPlayer,	//WE gracz
	T_SERIE a_enSerie		//WE seria
	) const
{
	return m_pPlayers->SumPlayerAllScore(a_enPlayer, a_enSerie);
}


// ---------------------------------------------------------
// Rozpoczêcie nastêpnej serii, lub zakoñczenie gry.
// Zwraca czy skoñczy³a siê gra
//
BOOL	//WY TRUE - kontynuowaæ grê
HeartsGame::NextSerie()
{
	if (m_enSerie == E_SR_4)
		return FALSE;
	else
	{
		NextGame(TRUE);
		return TRUE;
	}
}

// ---------------------------------------------------------
// Wstawienie zagranej przez gracza karty do lewy
//
void
HeartsGame::SetPlayerCard2Trick2(
	T_PLAYER a_enPlayer,
	short a_nInHand
)
{
	ASSERT(GetPlayerCardNr(a_enPlayer, a_nInHand) > 0);

	const CCard& l_card = GetPlayerCard(a_enPlayer, a_nInHand);
	ASSERT(!l_card.IsUsed());

	// decyzja o tym komu przyznamy kartê
	SetTrickCard(l_card);
	ASSERT(CardsOnTable() <= 4);

	// ustawienie u¿ycia karty
	SetUsedPlayerCard(a_enPlayer, a_nInHand, TRUE);
}


// ---------------------------------------------------------
// Wstawienie zagranej przez gracza karty do lewy
//
void 
HeartsGame::SetPlayerCard2Trick(
	T_PLAYER a_enPlayer, 
	short a_nInHand
	)
{
	ASSERT(GetPlayerCardNr(a_enPlayer, a_nInHand) > 0) ;

	const CCard& l_card = GetPlayerCard(a_enPlayer, a_nInHand);
	ASSERT(!l_card.IsUsed());

	// decyzja o tym komu przyznamy kartê
	SetTrickCard(l_card);
	ASSERT(CardsOnTable() <= 4);

	// ustawienie u¿ycia karty
	SetUsedPlayerCard(a_enPlayer, a_nInHand, TRUE);
	if (CardsOnTable() < 4)
	{
		SetNextThrower();
	}
}


// ---------------------------------------------------------
// Wstawienie zagranej przez gracza karty do lewy
//
short	//WY ilosæ lew podanego gracza
HeartsGame::GetPlayerTricksCnt(
	T_PLAYER a_enPlayer		//WE gracz
	) const
{
	short l_iAt; 
	short l_iTricksNr = 0 ;
	for (l_iAt = 0; l_iAt < GetTricksCnt(); l_iAt++)
	{
		const CTrick& l_trick = GetTrick(l_iAt);
		if (l_trick.GetCardsCnt() < 4)
			break ;
		if (l_trick.GetTrickOwner() == a_enPlayer)
			l_iTricksNr++ ;
	}
	return l_iTricksNr;
}


bool HeartsGame::Save(const std::filesystem::path& a_filePath)
{
	SAVERESTORE l_save{};

	Fill(l_save);

	std::ofstream l_file(a_filePath, std::ios::binary | std::ios::trunc);
	if (!l_file)
		return false;

	l_file.write(reinterpret_cast<const char*>(&l_save), sizeof(l_save));
	if (!l_file || l_file.fail())
		return false;

	m_filePath = a_filePath; 

	return true;
}


void HeartsGame::Fill(SAVERESTORE& a_save) const
{
	a_save.m_enDealer = m_pPlayers->GetDealer();
	a_save.m_enGame = m_enGame;
	a_save.m_enSerie = m_enSerie;

	m_pPlayers->FillScore(a_save);
}

#pragma todo ("move saving and restoring outside of the HeartsGame class! - just return and set SAVERESTORE, file saving must be external.")
// ---------------------------------------------------------
// restore game from file
//
bool HeartsGame::Restore(const std::filesystem::path& a_filePath)
{
	SAVERESTORE l_restore{};
	if (!ReadFromFile(a_filePath, l_restore))
		return false;

	m_pPlayers->SetDealer(l_restore.m_enDealer);
	m_enGame = l_restore.m_enGame;
	m_enSerie = l_restore.m_enSerie;

	m_pPlayers->RestoreScore(l_restore);
	m_filePath = a_filePath;

	return true;
}



bool HeartsGame::ReadFromFile(const std::filesystem::path& a_sFilePath, SAVERESTORE& a_outRestore)
{
	if (!std::filesystem::exists(a_sFilePath))
		return false;

	std::ifstream l_file(a_sFilePath, std::ios::binary);
	if (!l_file)
		return false;

	SAVERESTORE l_restore{};
	l_file.read(reinterpret_cast<char*>(&l_restore), sizeof(l_restore));

	if (!l_file || l_file.gcount() != sizeof(l_restore))
		return false;

	a_outRestore = l_restore;
	return true;
}


void HeartsGame::GameOver(void)
{
	m_tricks.Clear();
	m_enGame = E_GM_NULL;
	m_enSerie = E_SR_NULL;
	m_bDealed = FALSE;
	m_bTrumpsChoice = FALSE;
	m_bPass = false;
}


bool HeartsGame::IsGameSaved() const
{
	if (!IsFile())
		return false;

	SAVERESTORE l_restoreFile{};
	if (!ReadFromFile(m_filePath, l_restoreFile))
		return false;

	SAVERESTORE l_restoreGame{};
	Fill(l_restoreGame);

	CheckDiff(l_restoreFile, l_restoreGame);
	return (std::memcmp(&l_restoreFile, &l_restoreGame, sizeof(SAVERESTORE)) == 0);

}


#ifdef _DEBUG
void HeartsGame::CheckDiff(const SAVERESTORE& a_restore1, const SAVERESTORE& a_restore2) const
{

	std::ofstream l_file1(_T("C:\\TEMP\\a1.txt"), std::ios::binary | std::ios::trunc);
	if (!l_file1)
		return;

	l_file1.write(reinterpret_cast<const char*>(&a_restore1), sizeof(a_restore1));
	if (!l_file1 || l_file1.fail())
		return;

	std::ofstream l_file2(_T("C:\\TEMP\\a2.txt"), std::ios::binary | std::ios::trunc);
	if (!l_file2)
		return;

	l_file2.write(reinterpret_cast<const char*>(&a_restore2), sizeof(a_restore2));
	if (!l_file2 || l_file2.fail())
		return;

	if (std::memcmp(&a_restore1, &a_restore2, sizeof(SAVERESTORE)) != 0)
	{
		const std::uint32_t* pa = reinterpret_cast<const std::uint32_t*>(&a_restore1);
		const std::uint32_t* pb = reinterpret_cast<const std::uint32_t*>(&a_restore2);

		for (size_t i = 0; i < sizeof(SAVERESTORE) / sizeof(int); ++i)
		{
			if (pa[i] != pb[i])
				TRACE3("CheckDiff: %d: %d - %d\n", i, pa[i], pb[i]);
		}
	}

}
#endif




// ---------------------------------- EVENTS -------------------------------------------

void HeartsGame::CardPlayedEvent(Player a_playerCurrent, short a_iCardNr)
{
	TRACE1("HeartsGame::OnCardPlayedEvent: %d\n", GetTricksCnt());
	if (OnCardPlayed)
		OnCardPlayed(a_playerCurrent, a_iCardNr);
}



void HeartsGame::TrickStartedEvent()
{
	TRACE1("HeartsGame::TrickStartedEvent: %d\n", GetTricksCnt());
}


void HeartsGame::GameStartedEvent()
{
	TRACE1("HeartsGame::GameStartedEvent: %d\n", GetGame());
}


void HeartsGame::SerieStartedEvent()
{
	TRACE1("HeartsGame::SerieStartedEvent: %d\n", GetSerie());
}


void HeartsGame::EntireGameStartedEvent()
{
	TRACE0("HeartsGame::EntireGameStartedEvent\n");
}


void HeartsGame::TrickEndedEvent(bool* a_pbPlayed)
{
	TRACE1("HeartsGame::TrickEndedEvent: %d\n", GetTricksCnt());
	if (OnTrickTaken)
		OnTrickTaken(a_pbPlayed);
}


void HeartsGame::GameEndedEvent()
{
	TRACE1("HeartsGame::GameEndedEvent: %d\n", GetGame());
}


void HeartsGame::SerieEndedEvent()
{
	TRACE1("HeartsGame::SerieEndedEvent: %d\n", GetSerie());
}


void HeartsGame::EntireGameEndedEvent()
{
	TRACE0("HeartsGame::EntireGameEndedEvent\n");
}


