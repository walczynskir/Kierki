// Players.cpp: implementation of the CPlayers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Players.h"


// ---------------------------------------------------------
//	Konstruktor obiektu
//
CPlayers::CPlayers(
	T_PLAYER a_enFirstDealer	//WE pierwszy rozdawaj¹cy
	)
{
	m_enFirstDealer = a_enFirstDealer;
	m_arPlayers[Player::South].SetMe(Player::South);
	m_arPlayers[Player::West].SetMe(Player::West);
	m_arPlayers[Player::North].SetMe(Player::North);
	m_arPlayers[Player::East].SetMe(Player::East);
}


// ---------------------------------------------------------
//	Destruktor obiektu
//
CPlayers::~CPlayers()
{

}


// ---------------------------------------------------------
//	Zwraca dane gracza
//
const CPlayer&	//WY gracz
CPlayers::GetPlayer(
	T_PLAYER a_enPlayer	//WE który gracz
	) const
{
	return m_arPlayers[a_enPlayer];
}


// ---------------------------------------------------------
//	Zwraca nastêpnego gracza po podanym
//
T_PLAYER	//WY nastêpny gracz
CPlayers::NextPlayer(
	T_PLAYER a_enCurrentPlayer	//WE obecny gracz
	)
{
	T_PLAYER l_enNextPlayer ;
	if (a_enCurrentPlayer == Player::East)
		 l_enNextPlayer = Player::South ;
	else
	{
		short l_iPlayer = (short)a_enCurrentPlayer ;
		l_enNextPlayer = (T_PLAYER)(l_iPlayer + 1);
	}
	return l_enNextPlayer ;
}


// ---------------------------------------------------------
//	Przestawia pierwszego rozdawacza na nastêpnego
//
void 
CPlayers::SetNextFirstDealer()
{
// TODO first dealer w dwu miejscach - przenieœæ do RegData, a tu tylko wskazanie
	m_enFirstDealer = NextPlayer(m_enFirstDealer);
}


// ---------------------------------------------------------
//	Usuniêcie decydentów
//
void 
CPlayers::RemoveDeciders()
{
	for (Player l_player : constAllPlayers) 
	{
		m_arPlayers[l_player].RemoveDecider();
	}
}


// ---------------------------------------------------------
//	Posortowanie kart dla konkretnego gracza
void 
CPlayers::Sort(
	T_PLAYER a_player,	//WE gracz sortowany
	short	 a_nStart,	//WE od której zacz¹æ sortowanie
	short	 a_nEnd		//WE na której skoñczyæ
	)
{
	m_arPlayers[a_player].Sort(a_nStart, a_nEnd);
}


// ---------------------------------------------------------
//	Posortowanie kart
//
void 
CPlayers::SortAll(
	short a_nStart,	// od której zacz¹æ sortowanie
	short a_nEnd	// na której skoñczyæ
	)
{
	short l_nPlayer;
	for	(l_nPlayer = 0; l_nPlayer <= 3; l_nPlayer++)
	{
		Sort((T_PLAYER)l_nPlayer, a_nStart, a_nEnd);
	}
}


// ---------------------------------------------------------
//	Rozdzielenie kart na wszystkich graczy z kart 
//	potasowanych
//
void 
CPlayers::DistributeCards(
	const CSortCards& a_sortcards	//WE karty potasowane
	)
{
	short l_nAt;
		// przypisanie 52 numerów kart do m_cards*
	for (l_nAt = 0; l_nAt < 13; l_nAt++)
	{
		m_arPlayers[Player::South].SetCard(l_nAt, a_sortcards.GetCardNr(l_nAt * 4));
		m_arPlayers[Player::West].SetCard(l_nAt, a_sortcards.GetCardNr(l_nAt * 4 + 1));
		m_arPlayers[Player::North].SetCard(l_nAt, a_sortcards.GetCardNr(l_nAt * 4 + 2));
		m_arPlayers[Player::East].SetCard(l_nAt, a_sortcards.GetCardNr(l_nAt * 4 + 3)) ;
	}

}


// ---------------------------------------------------------
//	Wybór atu
//
T_COLOR		//WY wybrany kolor
CPlayers::ChooseTrumps(
	T_PLAYER a_player	//WE gracz wybieraj¹cy
	) const
{
	return m_arPlayers[a_player].ChooseTrumps();
}


// ---------------------------------------------------------
// utworzenie decydentów
//
void 
CPlayers::CreateDeciders(
	T_GAMES				a_enGame,		//WE gra
	const CTakenTricks* a_pTricks,		//WE lewe
	T_COLOR				a_colorTrumps	//WE ew. kolor atutowy
	)
{
	m_arPlayers[Player::West].CreateDecider(a_enGame, a_pTricks, a_colorTrumps);
	m_arPlayers[Player::North].CreateDecider(a_enGame, a_pTricks, a_colorTrumps);
	m_arPlayers[Player::East].CreateDecider(a_enGame, a_pTricks, a_colorTrumps);
}


// ---------------------------------------------------------
//	Utworzenie decydentów - puzzle
//
void 
CPlayers::CreateDeciders(
	const CPuzzleRows* a_pPuzzleRow	//WE wskazanie na polo¿one karty
	)
{
	m_arPlayers[Player::West].CreateDecider(a_pPuzzleRow);
	m_arPlayers[Player::North].CreateDecider(a_pPuzzleRow);
	m_arPlayers[Player::East].CreateDecider(a_pPuzzleRow);
}

// ---------------------------------------------------------
// Zwraca wskaŸnik na karty gracza
//
const CUserCards& 
CPlayers::GetPlayerCards(
	T_PLAYER a_enPlayer		//WE gracz
	) const
{
	return m_arPlayers[a_enPlayer].GetPlayerCards();
}


// ---------------------------------------------------------
// Zwraca referencjê na karty gracza
//
short	//WY zagrana karta
CPlayers::DecideCardNr(
	T_PLAYER a_player,	//WE gracz
	short    a_iTrick	//WE która lewa
	)
{
	return m_arPlayers[a_player].DecideCardNr(a_iTrick);
}


// ---------------------------------------------------------
//	Ustawia kartê na u¿yt¹ dla gracza
//
void 
CPlayers::SetUsedCard(
	T_PLAYER a_enPlayer, //WE gracz
	short a_nCard,		 //WE karta
	BOOL a_bUsed		 //WE czy u¿yta
	)
{
	m_arPlayers[a_enPlayer].SetUsedCard(a_nCard, a_bUsed);
}


// ---------------------------------------------------------
// Dodaje punkty dla gracza
//
void 
CPlayers::AddScore(
	T_PLAYER a_enPlayer,	//WE gracz
	T_SERIE a_enSerie,		//WE seria
	T_GAMES a_enGame,		//WE gra
	short a_nScore			//WE punkty
	)
{
	m_arPlayers[a_enPlayer].AddScore(a_enSerie, a_enGame, a_nScore);
}


// ---------------------------------------------------------
// Ustawia punkty dla gracza
//
void 
CPlayers::SetScore(
	T_PLAYER a_enPlayer,	//WE gracz
	T_SERIE a_enSerie,			//WE seria
	T_GAMES a_enGame,		//WE gra
	short a_nScore			//WE punkty
	)
{
	m_arPlayers[a_enPlayer].SetScore(a_enSerie, a_enGame, a_nScore);
}


// ---------------------------------------------------------
// Ustawienie punktów w uk³adance	
//
void 
CPlayers::SetPuzzleScore(
	T_PLAYER a_enPlayer,	//WE gracz
	short a_nScore,			//WE wynik
	T_SERIE a_enSerie		//WE seria
	)
{
	m_arPlayers[a_enPlayer].SetPuzzleScore(a_nScore, a_enSerie);
}


// ---------------------------------------------------------
// Wyczyszczenie wyników
//
void 
CPlayers::ClearScores(
	T_SERIE a_enSerie	//WE seria
	)
{
	for (Player l_player : constAllPlayers)
	{
		m_arPlayers[l_player].ClearScore(a_enSerie);
	}
}


// ---------------------------------------------------------
// Zapamiêtanie wskaŸnika na imiê gracza
//
void 
CPlayers::SetName(
	T_PLAYER a_enPlayer,	//WE gracz
	const tstring* a_pName	//WE wskaŸnik na imiê gracza
	)
{
	m_arPlayers[a_enPlayer].SetName(a_pName);
}


// ---------------------------------------------------------
// Wyczyszczenie wszystkich wyników
//
void 
CPlayers::ClearAllScores()
{
	for (Player l_player : constAllPlayers)
	{
		m_arPlayers[l_player].ClearAllScores();
	}	
}


// ---------------------------------------------------------
// Zwraca sumê punktów gracza z poprzednich przed a_enSerie
// grami
//
short	//WY suma punktów
CPlayers::SumPlayerScore(
	T_PLAYER a_enPlayer,	//WE gracz
	T_SERIE a_enSerie		//WE seria
	) const
{
	return m_arPlayers[a_enPlayer].SumScore(a_enSerie);
}


// ---------------------------------------------------------
// Zwraca sumê punktów gracza z poprzednich przed a_enSerie
// grami
//
short	//WY suma punktów
CPlayers::SumPlayerAllScore(
	T_PLAYER a_enPlayer,	//WE gracz
	T_SERIE a_enSerie		//WE seria
	) const
{
	return m_arPlayers[a_enPlayer].SumAllScore(a_enSerie);
}


//	---------------------------------------------------------
//	Saves players data to file (serialization)
//
void CPlayers::SaveState(LPSAVERESTORE a_pSaveRestore) const
{
	a_pSaveRestore->m_enFirstDealer = m_enFirstDealer;
	for (Player l_player : constAllPlayers)
	{
		m_arPlayers[l_player].SaveState(&(a_pSaveRestore->m_playerscore[static_cast<int>(l_player)]));
	}

}


//	---------------------------------------------------------
//	Loads players data from file (serialization)
//
void CPlayers::RestoreState(const LPSAVERESTORE a_pSaveRestore)
{
	m_enFirstDealer = a_pSaveRestore->m_enFirstDealer;
	for (Player l_player : constAllPlayers)
	{
		m_arPlayers[l_player].RestoreState(&(a_pSaveRestore->m_playerscore[static_cast<int>(l_player)]));	// I need to use plain-old static_Cast, as it is plain-c struct
	}
}