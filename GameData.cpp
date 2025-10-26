// GameData.cpp : implementation of the GameData class
//

#include "stdafx.h"
#include "resource.h"
#include "GameData.h"



/////////////////////////////////////////////////////////////////////////////
// GameData

/////////////////////////////////////////////////////////////////////////////
// GameData construction/destruction

// ---------------------------------------------------------
// Konstruktor
//
GameData::GameData()
{
	m_tricks.Clear();
	CreatePlayers();
	m_enGame = E_GM_NOTHING;
	m_enSerie = E_SR_NULL;
	m_bDealed = FALSE;
	m_bTrumpsChoice = FALSE;
	m_bPass = false;
}


// ---------------------------------------------------------
// Destruktor
//
GameData::~GameData()
{
	delete m_pPlayers;
}


// ---------------------------------------------------------
//	Utworzenie graczy, nadanie im imion
//
void 
GameData::CreatePlayers()
{
	m_pPlayers = new CPlayers(m_regData.m_regAuto.m_enFirstDealer);
	m_pPlayers->SetName(Player::South, &(m_regData.GetPlayerName(Player::South)));
	m_pPlayers->SetName(Player::West, &(m_regData.GetPlayerName(Player::West)));
	m_pPlayers->SetName(Player::North, &(m_regData.GetPlayerName(Player::North)));
	m_pPlayers->SetName(Player::East, &(m_regData.GetPlayerName(Player::East)));
}


// ---------------------------------------------------------
//	Przejœcie do nastêpnej gry
//  Ustawia nastêpn¹ grê, tworzy Deciderów u graczy
//	rozdaje karty
//
void	
GameData::NextGame(
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
			m_enSerie = T_SERIE(m_enSerie + 1);
		}
		else
		{
			m_enGame = T_GAMES(m_enGame + 1);
		}
	}
	m_colorTrumps = Suit::Nothing;

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
}


// ---------------------------------------------------------
//	Rozpoczêcie procesu rozdawania kart
//	Zwraca czy nale¿y wybieraæ atu rêcznie
//
BOOL	//WY czy zwróciæ sterowanie do View w celu wybrania atu
GameData::DealStart(
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
GameData::DealEnd()
{
	// posortuj karty w ka¿dej z 4 paczek
	m_pPlayers->SortAll();

	// wyzerujmy lewy
	m_tricks.Clear();
	m_bDealed = TRUE;	

	// po rozdaniu kart dopiero mozna utworzyæ decydentów!
	if (m_enGame == E_GM_PUZZLE)
	{
		m_pPlayers->CreateDeciders(&m_PuzzleRows);
		m_nPuzzleScore = 10;
	}
	else
	{
		m_pPlayers->CreateDeciders(m_enGame, &m_tricks, m_colorTrumps);
	}

	m_enThrower = m_enDealer;
	m_enDealer = CPlayers::NextPlayer(m_enDealer);
	if (m_enGame != E_GM_PUZZLE)
	{
		NextTrick();
	}
}


// ---------------------------------------------------------
//	Nadanie tytu³u oknu 
//
tstring		//WY nazwa okna
GameData::CreateTitle() const 
{
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
	TCHAR l_sStr[1024];
	::LoadString(::GetModuleHandle(NULL), IDS_APP_TITLE, l_sStr, ArraySize(l_sStr));
	l_sGame = l_sStr;
	::LoadString(::GetModuleHandle(NULL), l_idStr, l_sStr, ArraySize(l_sStr));
	l_sTitle = l_sStr;

	_sntprintf_s(l_sStr, ArraySize(l_sStr), _TRUNCATE, _T("%s - %s%s"), l_sGame.c_str(), l_sTitle.c_str(), l_sAdd.c_str());
	return l_sStr;
}


// ---------------------------------------------------------
//	Rozpoczêcie nowej gry.
//
void
GameData::NewGame()
{
	m_enDealer = GetFirstDealerAndSetNext();
#ifdef _DEBUG
	m_enGame = E_GM_NOTHING;
/*	m_enGame = E_GM_NOTRICKS;
	m_enGame = E_GM_NOHEARTS;
	m_enGame = E_GM_NOLADIES;
	m_enGame = E_GM_ROBBER;
	m_enGame = E_GM_RECOVER1;
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
GameData::ChooseTrumps()
{
	switch (m_enDealer)
	{
	case Player::South:
		m_pPlayers->Sort(Player::South, 0, 6);
		m_bTrumpsChoice = TRUE;
		return FALSE;

	default:
		m_colorTrumps = m_pPlayers->ChooseTrumps(m_enDealer);
		return TRUE;
	}
	return TRUE;
}


// ---------------------------------------------------------
BOOL	//WY zwraca czy wybieramy atu
GameData::IsTrumpsChoice() const
{
	return m_bTrumpsChoice;
}


// ---------------------------------------------------------
//	Zakoñczenie procesu wyboru atu
//
void 
GameData::EndTrumpsChoice(
	T_COLOR a_enColor	//WE jaki kolor zosta³ wybrany
	)
{
	m_colorTrumps = a_enColor;
	m_bTrumpsChoice = FALSE;
}


// ---------------------------------------------------------
//	Zakoñczenie procesu wyboru atu
//
short	//WY ilosæ kart na stole
GameData::CardsOnTable() const
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
GameData::GetThrower() const
{
	return m_enThrower;
}


// ---------------------------------------------------------
// Gracz który rozdawa³ karty
//
T_PLAYER	//WY rozdaj¹cy karty
GameData::GetDealer() const
{
	return m_enDealer;
}


// ---------------------------------------------------------
// Zwraca karty gracza
//
const CUserCards&  //WY karty gracza
GameData::GetPlayerCards(
	T_PLAYER a_enPlayer		//WE gracz
	) const
{
	return m_pPlayers->GetPlayerCards(a_enPlayer);
}


// ---------------------------------------------------------
// Iloœæ lew które ju¿ posz³y w grze
//
short	//WY iloœæ lew
GameData::GetTricksCnt() const
{
	return m_tricks.GetTricksCnt();
}


// ---------------------------------------------------------
// Czy karty s¹ rozdane
//
BOOL	//WY TRUE - karty rozdane
GameData::IsDealed() const
{
	return m_bDealed;
}


// ---------------------------------------------------------
// Czy karty s¹ rozdane
//
short	//WY numer karty do rzucenia
GameData::DecideCardNr()
{
	return m_pPlayers->DecideCardNr(GetThrower(), GetTricksCnt());
}


// ---------------------------------------------------------
// Ustalenie nastêpnej lewy
//
void 
GameData::NextTrick()
{
	m_tricks.NextTrick();
}


// ---------------------------------------------------------
// Przydzielenie lewie karty kolejnej
//
void 
GameData::SetTrickCard(
	const CCard& a_card		//WE karta
	)
{
	m_tricks.SetCard(a_card);
}


// ---------------------------------------------------------
//	Zwraca nazwê koloru w zale¿noœci od wybranego koloru atu
//
tstring		//WY nazwa koloru
GameData::DecodeTrumps() const
{
	UINT l_idStr;

	switch (m_colorTrumps)
	{
	case Suit::Club:
		l_idStr = IDS_COLOR_CLUB;
		break;
	case Suit::Diamond:
		l_idStr = IDS_COLOR_DIAMOND;
		break;
	case Suit::Spade:
		l_idStr = IDS_COLOR_SPADE;
		break;
	case Suit::Heart:
		l_idStr = IDS_COLOR_HEART;
		break;
	case Suit::NoTrumps:
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
GameData::SetUsedPlayerCard(
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
GameData::GetPlayerCardNr(
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
GameData::GetPlayerCard(
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
GameData::GetLastTrick() const
{
	return m_tricks[GetTricksCnt() - 1];
}


// ---------------------------------------------------------
// Zwraca ostatni¹ lewê
//
const CTrick&	//WY lewa
GameData::GetTrick(
	short a_nTrick		//WE numer lewy
	) const
{
	return m_tricks[a_nTrick];
}


// ---------------------------------------------------------
// Ustawia w³aœciciela ostatniej lewy i zwraca go
//
T_PLAYER	//WY w³aœciciel ostatniej lewy
GameData::SetLastTrickOwner(
	T_COLOR a_colorTrumps	//WE kolor atu lub Suit::Nothing
	)
{
	m_enThrower = m_tricks.SetLastTrickOwner(a_colorTrumps);
	return m_enThrower;
}


// ---------------------------------------------------------
//	Zapisanie wyników gry
//
void GameData::SetScores()
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
GameData::PointsInTrick(
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
GameData::GetTricks() const
{
	return m_tricks;
}


// ---------------------------------------------------------
// Czy ma karty do gry w puzzle
//
BOOL	//WY TRUE - ma
GameData::HasCardForPuzzle(
	T_PLAYER a_enPlayer		//WE gracz
	) const
{
	return GetPlayerCards(a_enPlayer).HasCardForPuzzle(&m_PuzzleRows);
}


// ---------------------------------------------------------
// Do³o¿enie karty do puzzli
//
void 
GameData::SetPlayerCard2Puzzle(
	T_PLAYER a_enPlayer,	//WE gracz
	short a_nInHand			//WE która karta na rêce
	)
{
	const CCard& l_card = GetPlayerCard(a_enPlayer, a_nInHand);
	ASSERT(!l_card.IsUsed());

	// decyzja o tym komu przyznamy kartê
	ASSERT(CardsOnTable() <= 52);

	m_PuzzleRows.PutCard(&l_card);
	SetNextPlayer();

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
GameData::SetPuzzleScore(
	T_PLAYER a_enPlayer	//WE gracz
	)
{
	m_pPlayers->SetPuzzleScore(a_enPlayer, m_nPuzzleScore, m_enSerie);
	DecreasePuzzleScore();
}

// ---------------------------------------------------------
// Zwraca wierze uk³adanki
//
const CPuzzleRowSet&	//WY wiersze uk³adanki
GameData::GetPuzzleRows() const
{
	return m_PuzzleRows;
}


// ---------------------------------------------------------
// Zwraca aktualn¹ grê
//
T_GAMES		//WY aktualna gra
GameData::GetGame() const
{
	return m_enGame;
}


// ---------------------------------------------------------
// Zwraca wynik dla gracza w serii i 
//
short	//WY wynik
GameData::GetPlayerScore(
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
GameData::SumPlayerMinuses(
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
GameData::SumPlayer(
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
GameData::SumPlayerAll(
	T_PLAYER a_enPlayer,	//WE gracz
	T_SERIE a_enSeria		//WE seria
	) const
{
	return m_pPlayers->GetPlayer(a_enPlayer).SumAll(a_enSeria);
}


// ---------------------------------------------------------
// Nastêpny gracz rzuca
// 
void 
GameData::SetNextPlayer()
{
	m_enThrower = CPlayers::NextPlayer(m_enThrower);
}


// ---------------------------------------------------------
// Wziêcie lewy. Zwraca, czy kontynuowaæ grê
//
BOOL	//WY TRUE: kontynuowaæ grê 
GameData::GetPlayedTrick()
{
	SetLastTrickOwner(m_colorTrumps);
	BOOL l_bContinue = Continue();

	if (l_bContinue)
	{
		NextTrick();
	}
	else
	{
		m_bDealed = FALSE ;
		SetScores();
		NextGame(TRUE);		
	}

	return l_bContinue;
}


// ---------------------------------------------------------
//	Czy kontyuowaæ grê (nie ma sensu np. w przypadku 
//	gry o krola kier gdy wziêty w lewie)
///
BOOL	//WY TRUE kontynuowaæ
GameData::Continue()
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
T_COLOR		//WY kolor atutowy
GameData::GetTrumps() const
{
	return m_colorTrumps;
}


// ---------------------------------------------------------
//	Zmniejsza punktacjê w puzzlach
//
void 
GameData::DecreasePuzzleScore()
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
GameData::GetSerie() const
{
	return m_enSerie;
}


// ---------------------------------------------------------
// Zwraca sumê punktów gracza z poprzednich przed a_enSerie
// grami
//
short	//WY suma punktów
GameData::SumPlayerScore(
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
GameData::SumPlayerAllScore(
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
GameData::NextSerie()
{
	if (m_enSerie == E_SR_4)
		return FALSE;
	else
	{
		m_enDealer = GetFirstDealerAndSetNext();
		NextGame(TRUE);
		return TRUE;
	}
}


// ---------------------------------------------------------
// Wstawienie zagranej przez gracza karty do lewy
//
void 
GameData::SetPlayerCard2Trick(
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
		SetNextPlayer();
	}
}


// ---------------------------------------------------------
// Wstawienie zagranej przez gracza karty do lewy
//
short	//WY ilosæ lew podanego gracza
GameData::GetPlayerTricksCnt(
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


// ---------------------------------------------------------
// Zapis dokumentu
//
bool GameData::Save(LPCTSTR a_psFile) 
{
	HANDLE l_hFile = ::CreateFile(a_psFile, GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (l_hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	SAVERESTORE l_sr;
	l_sr.m_enDealer = m_enDealer;
	l_sr.m_enGame = m_enGame;
	l_sr.m_enSerie = m_enSerie;

	m_pPlayers->SaveState(&l_sr);
	DWORD l_dwWritten;
	BOOL l_bOk = ::WriteFile(l_hFile, &l_sr, sizeof(l_sr), &l_dwWritten, NULL);
	ASSERT(l_dwWritten == sizeof(l_sr));

	::CloseHandle(l_hFile);
	if (l_bOk)
	{
		m_sFile = a_psFile;
	}
	return (l_bOk != FALSE);
}


// ---------------------------------------------------------
// opens document
//
bool GameData::Restore(LPCTSTR a_psFile) 
{
	HANDLE l_hFile = ::CreateFile(a_psFile, GENERIC_READ, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (l_hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	SAVERESTORE l_sr;

	DWORD l_dwRead;
	BOOL l_bOk = ::ReadFile(l_hFile, &l_sr, sizeof(l_sr), &l_dwRead, NULL);
	ASSERT(l_dwRead == sizeof(l_sr));

	::CloseHandle(l_hFile);
	if (!l_bOk)
	{
		return FALSE;
	}

	m_enDealer = l_sr.m_enDealer;
	m_enGame = l_sr.m_enGame;
	m_enSerie = l_sr.m_enSerie;

	m_pPlayers->RestoreState(&l_sr);
	m_sFile = a_psFile;

	return TRUE;
}


// ---------------------------------------------------------
//	Pobranie pierwszego rozdaj¹cego i ustawienie nastêpnego
//	pierwszym
//
Player GameData::GetFirstDealerAndSetNext()
{
	Player l_playerLast = m_regData.m_regAuto.m_enFirstDealer;
	Player l_playerNew = CPlayers::NextPlayer(l_playerLast);
	m_regData.m_regAuto.m_enFirstDealer = l_playerNew;
	return l_playerLast;
}


void GameData::GameOver(void)
{
	m_tricks.Clear();
	m_enGame = E_GM_NOTHING;
	m_enSerie = E_SR_NULL;
	m_bDealed = FALSE;
	m_bTrumpsChoice = FALSE;
	m_bPass = false;
}
