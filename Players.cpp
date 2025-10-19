// Players.cpp: implementation of the CPlayers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Players.h"
#include <rcommon/ROwnExc.h>
#include <random>


// ---------------------------------------------------------
//	Konstruktor obiektu
//
CPlayers::CPlayers()
{
	m_enDealer = DrawRandomDealer();
	m_arPlayers[E_DL_1].SetMe(E_DL_1, CPlayer::PlayerType::Human);
	m_arPlayers[E_DL_2].SetMe(E_DL_2, CPlayer::PlayerType::Computer);
	m_arPlayers[E_DL_3].SetMe(E_DL_3, CPlayer::PlayerType::Computer);
	m_arPlayers[E_DL_4].SetMe(E_DL_4, CPlayer::PlayerType::Computer);
}


T_PLAYER CPlayers::DrawRandomDealer()
{
	static std::uniform_int_distribution<> l_dist(E_DL_1, E_DL_4);
	return static_cast<T_PLAYER>(l_dist(m_gen));
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



//	returns next player (circular)
T_PLAYER CPlayers::NextPlayer(T_PLAYER a_enCurrent)
{
	return PlayerCycle::Next(a_enCurrent);
}


// ---------------------------------------------------------
//	Usuniêcie decydentów
//
void 
CPlayers::RemoveDeciders()
{
	short l_nPlayer;
	for	(l_nPlayer = 0; l_nPlayer <= 3; l_nPlayer++)
	{
		m_arPlayers[l_nPlayer].RemoveDecider();
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
	for	(l_nPlayer = E_DL_1; l_nPlayer <= E_DL_4; ++l_nPlayer)
	{
		Sort(static_cast<T_PLAYER>(l_nPlayer), a_nStart, a_nEnd);
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
	for (short l_iHand = 0; l_iHand < 13; ++l_iHand)
	{
		for (short l_iPlayer = E_DL_1; l_iPlayer <= E_DL_4; ++l_iPlayer)
		{
			m_arPlayers[static_cast<T_PLAYER>(l_iPlayer)].SetCard(l_iHand, a_sortcards.GetCardNr(l_iHand * 4 + l_iPlayer));
		}
	}

}


// ---------------------------------------------------------
//	Wybór atu
//
T_SUIT		//WY wybrany kolor
CPlayers::ChooseTrumps() const
{
	return m_arPlayers[m_enDealer].ChooseTrumps();
}


// ---------------------------------------------------------
// utworzenie decydentów
//
void 
CPlayers::CreateDeciders(
	T_GAMES				a_enGame,		//WE gra
	const CTakenTricks* a_pTricks,		//WE lewe
	T_SUIT				a_colorTrumps	//WE ew. kolor atutowy
	)
{
	m_arPlayers[E_DL_1].CreateDecider(a_enGame, a_pTricks, a_colorTrumps);
	m_arPlayers[E_DL_2].CreateDecider(a_enGame, a_pTricks, a_colorTrumps);
	m_arPlayers[E_DL_3].CreateDecider(a_enGame, a_pTricks, a_colorTrumps);	
	m_arPlayers[E_DL_4].CreateDecider(a_enGame, a_pTricks, a_colorTrumps);	
}


// ---------------------------------------------------------
//	Utworzenie decydentów - puzzle
//
void 
CPlayers::CreateDeciders(
	const CPuzzleRows* a_pPuzzleRow	//WE wskazanie na polo¿one karty
	)
{
	m_arPlayers[E_DL_2].CreateDecider(a_pPuzzleRow);
	m_arPlayers[E_DL_3].CreateDecider(a_pPuzzleRow);
	m_arPlayers[E_DL_4].CreateDecider(a_pPuzzleRow);
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
	short l_nAt;
	for (l_nAt = 0; l_nAt < 4; l_nAt++)
	{
		m_arPlayers[l_nAt].ClearScore(a_enSerie);
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
	short l_nAt;
	for (l_nAt = 0; l_nAt < 4; l_nAt++)
	{
		m_arPlayers[l_nAt].ClearAllScores();
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


bool CPlayers::HaveCardsToPlay() const
{
	for (short l_iPlayer = E_DL_1; l_iPlayer <= E_DL_4; ++l_iPlayer)
	{
		if (GetPlayerCards(static_cast<Player>(l_iPlayer)).CardsLeft() > 0)
			return true;
	}
	return false;
}



//	---------------------------------------------------------
//	Players data to structure (serialization)
//
void CPlayers::FillScore(SAVERESTORE& a_save)
{
	for (int l_iPlayer = 0; l_iPlayer < ArraySize(m_arPlayers); l_iPlayer++)
	{
		m_arPlayers[l_iPlayer].FillScore(a_save.m_playerscore[l_iPlayer]);
	}

}


//	---------------------------------------------------------
//	Loads players data from structure (serialization) const 
//
void CPlayers::RestoreScore(const SAVERESTORE& a_restore)
{
	for (UINT l_iAt = 0; l_iAt < ArraySize(m_arPlayers); l_iAt++)
	{
		m_arPlayers[l_iAt].RestoreScore(a_restore.m_playerscore[l_iAt]);
	}
}



