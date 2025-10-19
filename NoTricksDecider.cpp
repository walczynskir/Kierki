// NoTricksDecider.cpp: implementation of the CNoTricksDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NoTricksDecider.h"
#include <algorithm>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// Konstruktor
//
CNoTricksDecider::CNoTricksDecider(
	const CUserCards*   a_pUserCards, //WE karty gracza 
	const CTakenTricks* a_pTricks	  //WE wzi�te lewy
	)
{
	m_pCards = a_pUserCards ;	
	m_pTricks = a_pTricks ;
}


// ---------------------------------------------------------
// Destruktor
//
CNoTricksDecider::~CNoTricksDecider()
{

}


// ---------------------------------------------------------
// Zwraca numer karty w r�ce kt�ry winien by� rzucony
//
short	//WY numer karty
CNoTricksDecider::GetCardNr(
	short a_nTrick	//WE numer lewy
	)	const
{
	short l_nCardNr;
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1];
	if (l_trickCurrent.GetCardsCnt() == 0)
	{
		l_nCardNr = FirstCard(a_nTrick) ;
	}
	else
	{
		l_nCardNr = NextCard(a_nTrick) ;
	}

	ASSERT(m_pCards->GetCard(l_nCardNr).IsUsed() == FALSE);
	return l_nCardNr;
}


// ---------------------------------------------------------
// Zwraca numer karty kt�r� nale�y zawistowa�	
//
short	//WY numer karty
CNoTricksDecider::FirstCard(
	short a_iTrick	//WE kt�ra lewa
	)	const
{
	short l_nCardNr;
	BOOL l_bFound = FALSE;
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1];


	// check all colors
	short l_nRank;
	for (l_nRank = 1; l_nRank <= c_app_btPlayersCnt; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (m_pCards->ColorCnt(l_enColor) == 0)
		{
			continue;
		}
		const CCard* l_pCard = m_pCards->FirstInColorCard(l_enColor);
		if (m_pTricks->AreBiggerInColor(l_pCard, m_pCards))
		{
			l_nCardNr = m_pCards->FirstInColor(l_enColor);
			l_bFound = TRUE;
			break;
		}

	}

	if (!l_bFound)
	{
		l_nCardNr = m_pCards->FirstInShortest();
	}

	ASSERT(m_pCards->GetCard(l_nCardNr).IsUsed() == FALSE);
	return l_nCardNr;
}


// ---------------------------------------------------------
// Zwraca numer karty w kt�ra nale�y zagra�	
//
short 
CNoTricksDecider::NextCard(
	short a_nTrick	//WE numer lewy
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1] ;
	Suit l_enColor = l_trickCurrent.GetCardColor(0);

	if (!m_pCards->HasColor(l_enColor))
	{
		return DecideOtherColor(a_nTrick);
	}

	const CCard* l_pCard = l_trickCurrent.Biggest();
	CCard l_cardLowest(l_enColor, E_CV_2);
	CCard l_cardBiggest(l_enColor, l_pCard->CardValue());

	// staramy si� zagra� najwy�sz� mniejsz� kart�
	// jesli mamy mniejsz� to j� k�adziemy
	if (l_pCard->CardValue() > E_CV_2)
	{
		if (m_pCards->CardsBetween(l_cardLowest, l_cardBiggest) > 0)
		{
			return m_pCards->LessOrBiggestInColor(l_trickCurrent);
		}
	}

	// trzeba zdecydowa� czy k�adziemy najwy�sz�, czy jest 
	// szansa, �e kto� jeszcze we�mie t� lew�
	// jesli jeste�my ostatnim graczem lub
	// je�li karta najwi�ksza to conajmniej kr�l
	// to gramy i tak najwy�sz�
	short l_nCards = l_trickCurrent.GetCardsCnt();
	if (
		(l_nCards == (c_app_btPlayersCnt - 1)) ||
		(l_pCard->CardValue() >= E_CV_K)
		)
	{
		return m_pCards->Biggest(l_enColor);		
	}

	// pr�bujemy znale�� kart� o ilo�� graczy przed nami wy�sz�
	// od najwy�szej w lewie
	l_cardLowest = CCard(l_enColor, std::min((T_RANK)(l_pCard->CardValue() + 1), E_CV_A));
	l_cardBiggest = 
		CCard(l_enColor, 
			std::min((T_RANK)(l_pCard->CardValue() + c_app_btPlayersCnt - l_nCards), E_CV_A));
	if (m_pCards->CardsBetween(l_cardLowest, l_cardBiggest) > 0)
	{
		return m_pCards->LessOrFirstBiggerInColor(l_pCard);
	}

	return m_pCards->Biggest(l_enColor);		
}


// ---------------------------------------------------------
// Returns card in other color than whisted
//
short 
CNoTricksDecider::DecideOtherColor(
	short a_nTrick
	) const
{
	// first if we have color with no more than 2 cards
	// lets play this color
	short l_nRank;
	for (l_nRank = 1; l_nRank <= c_app_btPlayersCnt; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
		{
			continue;
		}
		if (m_pCards->AllCardsInColor(l_enColor) > 2)
		{
			break;
		}
		return m_pCards->Biggest(l_enColor);
	}

	// second
	for (l_nRank = 1; l_nRank <= c_app_btPlayersCnt; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetBiggestFirstCardColorRank(1);
		if (!m_pCards->HasColor(l_enColor))
		{
			continue;
		}
		return m_pCards->Biggest(l_enColor);
	}

	ASSERT(FALSE);
	return -1;
}

