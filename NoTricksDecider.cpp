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
	const CTakenTricks* a_pTricks	  //WE wziête lewy
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
// Zwraca numer karty w rêce który winien byæ rzucony
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
// Zwraca numer karty któr¹ nale¿y zawistowaæ	
//
short	//WY numer karty
CNoTricksDecider::FirstCard(
	short a_iTrick	//WE która lewa
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
// Zwraca numer karty w która nale¿y zagraæ	
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

	// staramy siê zagraæ najwy¿sz¹ mniejsz¹ kartê
	// jesli mamy mniejsz¹ to j¹ k³adziemy
	if (l_pCard->CardValue() > E_CV_2)
	{
		if (m_pCards->CardsBetween(l_cardLowest, l_cardBiggest) > 0)
		{
			return m_pCards->LessOrBiggestInColor(l_trickCurrent);
		}
	}

	// trzeba zdecydowaæ czy k³adziemy najwy¿sz¹, czy jest 
	// szansa, ¿e ktoœ jeszcze weŸmie t¹ lewê
	// jesli jesteœmy ostatnim graczem lub
	// jeœli karta najwiêksza to conajmniej król
	// to gramy i tak najwy¿sz¹
	short l_nCards = l_trickCurrent.GetCardsCnt();
	if (
		(l_nCards == (c_app_btPlayersCnt - 1)) ||
		(l_pCard->CardValue() >= E_CV_K)
		)
	{
		return m_pCards->Biggest(l_enColor);		
	}

	// próbujemy znaleŸæ kartê o iloœæ graczy przed nami wy¿sz¹
	// od najwy¿szej w lewie
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

