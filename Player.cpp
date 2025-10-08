// Player.cpp: implementation of the CPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Player.h"

#include "NoTricksDecider.h"
#include "NoHeartsDecider.h"
#include "NoBoysDecider.h"
#include "NoLadiesDecider.h"
#include "NoKingOfHeartDecider.h"
#include "NoSixAndLastDecider.h"
#include "RobberDecider.h"
#include "RecoverDecider.h"
#include "PuzzleDecider.h"


// ---------------------------------------------------------
//	konstruktor
//
CPlayer::CPlayer()
{
	m_pDecider = NULL;
	m_playerMe = E_DL_NULL;
// TODO nie zaimplementowane zapamietanie wskaŸnika na imiê z m_pRegData
}


// ---------------------------------------------------------
//	Destruktor
//
CPlayer::~CPlayer()
{
	RemoveDecider();
}


// ---------------------------------------------------------
//	Wyczyszczenie wyniku
//
void 
CPlayer::ClearScore(
	T_SERIE a_enSerie	//WE seria
	)
{
	m_score[a_enSerie].Clear();
}


// ---------------------------------------------------------
//	Ustawienie jeœli jeszcze nie ustawiony wyniku Puzzli
//
BOOL	// WY czy ustawiono
CPlayer::SetPuzzleScore(
	short a_nScore,		//WE wynik
	T_SERIE a_enSerie	//WE seria
	)
{
	if (m_score[a_enSerie].GetScore(E_GM_PUZZLE) == 0)
	{
		m_score[a_enSerie].SetScore(E_GM_PUZZLE, a_nScore);
		return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------
//	Pobranie nazwy gracza
//
const tstring& 
CPlayer::GetName() const
{
	return *m_pName;
}


// ---------------------------------------------------------
//	Ustawienie nazwy gracza
//
void  
CPlayer::SetName(
	const tstring* a_pName	//WE nazwa gracza
	)
{
	m_pName = a_pName;
}


// ---------------------------------------------------------
//	Dodanie wartoœci do wyniku
//
void 
CPlayer::AddScore(
	T_SERIE a_enSeria,	//WE seria grania
	T_GAMES a_enGame,	//WE gra
	short a_nScore		//WE dodawany wynik
	)
{
	ASSERT(a_enSeria != E_SR_NULL);
	m_score[a_enSeria].AddScore(a_enGame, a_nScore);
}


// ---------------------------------------------------------
//	Ustawienie wyniku
//
void 
CPlayer::SetScore(
	T_SERIE a_enSeria,	//WE seria grania
	T_GAMES a_enGame,	//WE gra
	short a_nScore		//WE dodawany wynik
	)
{
	ASSERT(a_enSeria != E_SR_NULL);

	m_score[a_enSeria].SetScore(a_enGame, a_nScore);
}


// ---------------------------------------------------------
//	Pobranie wyniku
//
short	//WY wynik
CPlayer::GetScore(
	T_SERIE a_enSerie,	//WE seria grania
	T_GAMES a_enGame	//WE gra
	) const
{
	ASSERT(a_enSerie != E_SR_NULL);
	return m_score[a_enSerie].GetScore(a_enGame);
}


// ---------------------------------------------------------
//	Pobranie wyniku
//
short	//WY wynik
CPlayer::GetScore(
	T_SERIE a_enSerie,	//WE seria grania
	short a_nGame		//WE gra
	) const
{
	ASSERT(a_enSerie != E_SR_NULL);
	return m_score[a_enSerie].GetScore(a_nGame);
}


// ---------------------------------------------------------
//	Podsumowanie minusów gracza
//
short	// minusy podsumowane
CPlayer::SumMinuses(
	T_SERIE a_enSerie,	//WE seria
	BOOL a_bRobber	//WE czy z rozbójnikiem 
	) const
{
	ASSERT(a_enSerie != E_SR_NULL);
	return m_score[a_enSerie].SumMinuses(a_bRobber); 
}


// ---------------------------------------------------------
//	Podsumowanie gracza
//
short CPlayer::Sum(
	T_SERIE a_enSerie	//WE seria
	) const
{
	ASSERT(a_enSerie != E_SR_NULL);
	return m_score[a_enSerie].Sum(); 
}


// ---------------------------------------------------------
//	Podsumowanie po wszystkich grach
//
short 
CPlayer::SumAll(
	T_SERIE a_enSerie	//WE seria
	) const
{
	ASSERT(a_enSerie != E_SR_NULL);
	return m_score[a_enSerie].SumAll(); 
}


// ---------------------------------------------------------
//	Usuniêcie decydenta
//
void CPlayer::RemoveDecider()
{
	if (m_pDecider != NULL)
	{
		delete m_pDecider;
		m_pDecider = NULL;
	}
}


// ---------------------------------------------------------
//	Posortowanie kart
//
void 
CPlayer::Sort(
	short a_nStart,	// od której zacz¹æ sortowanie
	short a_nEnd	// na której skoñczyæ
	)
{
	m_usercards.Sort(a_nStart, a_nEnd);
}


// ---------------------------------------------------------
//	Ustawienie karty
//
void 
CPlayer::SetCard(
	short a_nCard,	//WE która karta
	short a_nCardNr	//WE numer karty
	)
{
	ASSERT(a_nCard >= 0);
	ASSERT(a_nCard < 13);
	ASSERT(a_nCardNr >= 1);
	ASSERT(a_nCard < 53);
	ASSERT(m_playerMe != E_DL_NULL);

	m_usercards.SetCard(a_nCard, a_nCardNr, m_playerMe);
}


// ---------------------------------------------------------
//	Ustawienie kim jest
//
void 
CPlayer::SetMe(
	T_PLAYER a_playerMe	//WE ktróym graczem jestem
	)
{
	m_playerMe = a_playerMe;
}


// ---------------------------------------------------------
//	Wybór atu
//
T_COLOR		//WY wybrany kolor
CPlayer::ChooseTrumps() const
{
	struct S_TRUMPS
	{
		short   nCnt;
		short   nPoints;
	}	l_arColorsCnt[4];

	memset(l_arColorsCnt, 0, sizeof(l_arColorsCnt)); 

	short l_nCard;
	for (l_nCard = 0; l_nCard < 7; l_nCard++)
	{
		const CCard& a_card = m_usercards[l_nCard];
		l_arColorsCnt[a_card.GetColor() - 1].nCnt += 1;
		l_arColorsCnt[a_card.GetColor() - 1].nPoints += a_card.CardValue();
	}

	short l_iAt;
	short l_nCntBest = 0;;
	short l_nPointsBest = 0;
	short l_nPosBest = 0;
	for (l_iAt = 0; l_iAt < 4; l_iAt++)
	{
		if ((l_arColorsCnt[l_iAt].nCnt > l_nCntBest) ||
			((l_arColorsCnt[l_iAt].nCnt == l_nCntBest) && ((l_arColorsCnt[l_iAt].nPoints > l_nPointsBest))))
		{
			l_nCntBest = l_arColorsCnt[l_iAt].nCnt;
			l_nPointsBest = l_arColorsCnt[l_iAt].nPoints;
			l_nPosBest = l_iAt;
		}
	}
	return (T_COLOR)(l_nPosBest + 1);	
}


// ---------------------------------------------------------
//	Utworzenie decydentów
//
void 
CPlayer::CreateDecider(
	T_GAMES			    a_enGame,		//WE gra
	const CTakenTricks* a_pTricks,		//WE wskaŸnik na wziête lewy
	T_COLOR				a_colorTrumps	//WE atu kolor
	)
{
	RemoveDecider();
	switch (a_enGame)
	{
	case E_GM_NOTRICKS:
		m_pDecider = new CNoTricksDecider(&m_usercards, a_pTricks);
		break;

	case E_GM_NOHEARTS:
		m_pDecider = new CNoHeartsDecider(&m_usercards, a_pTricks);
		break;

	case E_GM_NOBOYS:
		m_pDecider = new CNoBoysDecider(&m_usercards, a_pTricks);
		break;

	case E_GM_NOLADIES:
		m_pDecider = new CNoLadiesDecider(&m_usercards, a_pTricks);
		break ;

	case E_GM_NOKINGOFHEART:
		m_pDecider = new CNoKingOfHeartDecider(&m_usercards, a_pTricks);
		break;

	case E_GM_NOSIXANDLAST:
		m_pDecider = new CNoSixAndLastDecider(&m_usercards, a_pTricks);
		break;

	case E_GM_ROBBER:
		m_pDecider = new CRobberDecider(&m_usercards, a_pTricks);
		break;

	case E_GM_RECOVER1:
	case E_GM_RECOVER2:
	case E_GM_RECOVER3:
	case E_GM_RECOVER4:
		m_pDecider = new CRecoverDecider(&m_usercards, a_pTricks, m_playerMe, a_colorTrumps);
		break ;

	}

}


// ---------------------------------------------------------
//	Utworzenie decydentów - puzzle
//
void 
CPlayer::CreateDecider(
	const CPuzzleRows* a_pPuzzleRow	//WE wskazanie na polo¿one karty
	)
{
	RemoveDecider();
	m_pDecider = new CPuzzleDecider(&m_usercards, a_pPuzzleRow, m_playerMe);
}


// ---------------------------------------------------------
// Zwraca karty gracza
//
const CUserCards&		//WY karty gracza
CPlayer::GetPlayerCards() const
{
	return m_usercards;
}


// ---------------------------------------------------------
// Zwraca kartê do zagrania
//
short	//WY	karta do zagrania
CPlayer::DecideCardNr(
	short a_iTrick	//WE która lewa
	)
{
	return m_pDecider->GetCardNr(a_iTrick);
}


// ---------------------------------------------------------
//	Ustawia kartê na u¿yt¹
//
void 
CPlayer::SetUsedCard(
	short a_nCard,		 //WE karta
	BOOL a_bUsed		 //WE czy u¿yta
	)
{
	m_usercards.SetUsed(a_nCard, a_bUsed);
}


// ---------------------------------------------------------
// Wyczyszczenie wszystkich wyników
//
void 
CPlayer::ClearAllScores()
{
	short l_nAt;
	for (l_nAt = 0; l_nAt < 4; l_nAt++)
	{
		m_score[l_nAt].Clear();
	}
}


// ---------------------------------------------------------
// Zwraca sumê punktów gracza z poprzednich przed a_enSerie
// grami
//
short	//WY suma punktów
CPlayer::SumScore(
	T_SERIE a_enSerie		//WE seria
	) const
{
	ASSERT(a_enSerie != E_SR_NULL);
	short l_nSerie;
	short l_nScore = 0;

	for (l_nSerie = 0; l_nSerie < a_enSerie; l_nSerie++)
	{
		l_nScore = l_nScore + SumAll((T_SERIE)l_nSerie);
	}
	return l_nScore;
}


// ---------------------------------------------------------
// Zwraca sumê punktów gracza z poprzednich przed a_enSerie
// grami
//
short	//WY suma punktów
CPlayer::SumAllScore(
	T_SERIE a_enSerie		//WE seria
	) const
{
	ASSERT(a_enSerie != E_SR_NULL);
	return SumScore(a_enSerie) + SumAll(a_enSerie);
}


//	---------------------------------------------------------
//	Saves player data to archive
//
void 
CPlayer::SaveState(LPPLAYERSCORE a_pPlayerScore) const
{
	for (UINT l_iAt = 0; l_iAt < ArraySize(m_score); l_iAt++)
	{
		const CScore& l_score = m_score[l_iAt];
		l_score.SaveState(&(a_pPlayerScore->m_score[l_iAt]));
	}
}


//	---------------------------------------------------------
//	Loads player data from archive
//
void CPlayer::RestoreState(const LPPLAYERSCORE a_pPlayerScore)
{
	for (UINT l_iAt = 0; l_iAt < ArraySize(m_score); l_iAt++)
	{
		CScore& l_score = m_score[l_iAt];
		l_score.RestoreState(&(a_pPlayerScore->m_score[l_iAt]));
	}
}


//	---------------------------------------------------------
//	Saves player score to file (appends data)
//
BOOL 
CPlayer::SaveScore(
	tfile* a_pFile
	) const
{
	return TRUE;
}
