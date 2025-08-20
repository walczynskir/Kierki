// NoHeartsDecider.cpp: implementation of the CNoHeartsDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NoHeartsDecider.h"


// ---------------------------------------------------------
// Constructor
//
CNoHeartsDecider::CNoHeartsDecider(
	const CUserCards*   a_pUserCards, //WE karty gracza 
	const CTakenTricks* a_pTricks	  //WE wziête lewy
	)
{
	m_pCards = a_pUserCards ;	
	m_pTricks = a_pTricks ;

}


// ---------------------------------------------------------
// Destructor
//
CNoHeartsDecider::~CNoHeartsDecider()
{

}


// ---------------------------------------------------------
// Zwraca numer karty do zagrania
//
short	//WE numer karty
CNoHeartsDecider::GetCardNr(
	short a_iTrick	//WE która lewa
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;

	short l_nCardNr ;
	if (l_trickCurrent.GetCardsCnt() == 0)
		l_nCardNr = FirstCard(a_iTrick) ;
	else
		l_nCardNr = NextCard(a_iTrick) ;
	ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
	ASSERT(m_pCards->GetCard(l_nCardNr).IsUsed() == FALSE);
	return l_nCardNr;
}


// ---------------------------------------------------------
// Zwraca jak¹ kartê zagrac przy wiœcie
//
short	//WY numer karty przy wiœcie
CNoHeartsDecider::FirstCard(
	short a_iTrick	//WE numer lewy
	) const
{
	short l_nCardNr;

	if (m_pCards->AreNoHearts())
	{
		l_nCardNr = DecideFirstCardNoHearts(a_iTrick);
		ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
		ASSERT(!m_pCards->GetCard(l_nCardNr).IsUsed());
		ASSERT(m_pCards->GetCard(l_nCardNr).GetColor() != E_CC_HEART);
		return l_nCardNr;
	}

	l_nCardNr = m_pCards->FirstInColor(E_CC_HEART);
	ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
	ASSERT(m_pCards->GetCard(l_nCardNr).IsUsed() == FALSE);
	return l_nCardNr;
}


// ---------------------------------------------------------
// Zwraca numer karty do do³o¿enia
//
short	//WY numer karty
CNoHeartsDecider::NextCard(
	short a_iTrick	//WE która lewa
	) const
{
	// znajdŸ kartê w tym kolorze najwiêksz¹ jeœli nie gramy 
	// serc i pierwsz¹ mniejsz¹ od danej
	// lub najwiêksz¹
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;
	const CCard* l_pCardFirst = l_trickCurrent.GetCard(0);
	T_COLOR l_enColor = l_trickCurrent.GetCardColor(0);

	short l_iRetAt = 0;
	short l_nCardNr ;
	BOOL l_bFound = FALSE ;
	if (l_enColor != E_CC_HEART)
	{
		if (m_pCards->HasColor(l_enColor))
		{
			l_nCardNr = m_pCards->LastInColor(l_enColor) ;
		}
		else
		{
			l_nCardNr = DecideOtherColor(a_iTrick);
/*			if (m_pCards->HasColor(E_CC_HEART))
			{
				l_nCardNr = m_pCards->Biggest(E_CC_HEART) ;
			}
			else
			{
				l_nCardNr = m_pCards->LastInColor(m_pCards->ShortestColor()) ;
			}
*/
		}
	}
	else
	{
		l_nCardNr = m_pCards->LessOrBiggestInColor(l_trickCurrent) ;
	}
	ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
	ASSERT(m_pCards->GetCard(l_nCardNr).IsUsed() == FALSE);
	return l_nCardNr;
}


// ---------------------------------------------------------
// Decyzja karty wistowanej gdy s¹ karty inne ni¿ kier
//
short	//WY numer karty
CNoHeartsDecider::DecideFirstCardNoHearts(
	short a_iTrick	//WE lewa
	) const
{

	// first we play worst color (without 2 or 3)
	short l_nCardNr = GetWorstColorCard(a_iTrick);
	if (l_nCardNr >= 0)
	{
		return l_nCardNr;
	}

	// here we try to not take next trick
	l_nCardNr = GetNotTakingCard(a_iTrick);
	if (l_nCardNr >= 0)
	{
		return l_nCardNr;
	}

	// potem kolor nasz najkrótszy w którym inni maj¹ 
	// wy¿sze karty
	l_nCardNr = GetShortestAreBigger(a_iTrick);
	if (l_nCardNr >= 0)
	{
		return l_nCardNr;
	}

	// potem co mamy
	l_nCardNr = GetBestOtherCard(a_iTrick);
	if (l_nCardNr >= 0)
	{
		return l_nCardNr;
	}

	return -1;
}


// ---------------------------------------------------------
// Decyzja karty wistowanej - wœród kart, których mamy mniej 
// ni¿ 3.
//
short	//WY numer karty
CNoHeartsDecider::GetWorstColorCard(
	short a_iTrick	//WE lewa
	) const
{
	short l_nCardNr = -1;
	for (short l_nRank = 1; l_nRank <=4; l_nRank++)
	{
		T_COLOR l_enColor = 
			m_pCards->GetBiggestFirstCardColorRank(l_nRank);
		if (l_enColor == E_CC_HEART)
		{
			continue;
		}
		if  (
			(LeftInColorOthers(l_enColor) >= 3) &&
			(m_pTricks->TricksInColor(l_enColor) < 3)
			)
		{
			l_nCardNr = m_pCards->Biggest(l_enColor);
			return l_nCardNr;
		}
	}

	return l_nCardNr;
}


// ---------------------------------------------------------
// We try to not take trick
//
short	//WY numer karty
CNoHeartsDecider::GetNotTakingCard(
	short a_iTrick	//WE lewa
	) const
{
	
	for (short l_nRank = 1; l_nRank <=4; l_nRank++)
	{
		T_COLOR l_enColor = 
			m_pCards->GetLowestCardColorRank(l_nRank);
		if (l_enColor == E_CC_HEART)
		{
			continue;
		}
		short l_nCardNr = m_pCards->FirstInColor(l_enColor);;
		const CCard& l_card = (*m_pCards)[l_nCardNr];
		if (m_pTricks->AreBiggerInColor(&l_card, m_pCards))
		{
			return l_nCardNr;
		}
	}
	return -1;
}


// ---------------------------------------------------------
// Zwraca kartê w kolorze najkrótszym w którym inni maj¹ 
// wy¿sze karty
//
short	//WY numer karty
CNoHeartsDecider::GetShortestAreBigger(
	short a_iTrick	//WE lewa
	) const
{
	short l_nCardNr = -1;
	short l_nRank;
	T_COLOR l_enColor = E_CC_NOTHING;

	for (l_nRank = 1; l_nRank < 5; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
		{
			continue;
		}
		if (m_pCards->CardsInColor(l_enColor) <= 0)
		{
			continue;
		}
		const CCard* l_pCard = m_pCards->FirstInColorCard(l_enColor);
		if (m_pTricks->AreBiggerInColor(l_pCard, m_pCards))
		{
			if (m_pTricks->TricksInColor(l_enColor) > 2)
			{
				l_nCardNr = m_pCards->FirstInColor(l_enColor);
			}
			else
			{
				l_nCardNr = m_pCards->Biggest(l_enColor);
			}
			break;
		}
	}
	return l_nCardNr;
}


// ---------------------------------------------------------
// Zwraca jak¹œ kartê, Stara siê by by³a to najlepsza
//
short	//WY numer karty
CNoHeartsDecider::GetBestOtherCard(
	short a_iTrick	//WE lewa
	) const
{
	// przeszukajmy od najkrótszego koloru i znajdŸmy najpierw
	// taki w którym s¹ wy¿sze
	short l_nCardNr = -1;
	short l_nRank;
	T_COLOR l_enColor = E_CC_NOTHING;

	for (l_nRank = 1; l_nRank < 5; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
		{
			continue;
		}
		if (m_pCards->CardsInColor(l_enColor) <= 0)
		{
			continue;
		}

		l_nCardNr = m_pCards->FirstInColor(l_enColor);		
	}

	ASSERT(l_nCardNr >= 0);
	return l_nCardNr;
}


// ---------------------------------------------------------
// returns best card to throw from all cards. We are sure we
// can't get this trick
//
short	//WY card nr
CNoHeartsDecider::DecideOtherColor(
	short a_iTrick	//IN trick nr
	) const
{
	// first - color with biggest lowest card
	for (short l_nRank = 4; l_nRank >= 1; l_nRank--)
	{
		T_COLOR l_enColor = 
			m_pCards->GetLowestCardColorRank(l_nRank);
		// hearts only if we have only 2 hearts
		// or in other cards we have only lowest cards
		if (l_enColor == E_CC_HEART)
		{
			if (ShouldThrowHearts(a_iTrick))
			{
				return m_pCards->Biggest(l_enColor);
			}
		}
		
		// if it is other color - check if others have this
		// color, or have less cards
		if (ShouldThrowOtherColor(a_iTrick, l_enColor))
		{
			return m_pCards->Biggest(l_enColor);
		}

	}

	// still didn't throw - lets throw biggest HEART if 
	// we have

	if (m_pCards->CardsInColor(E_CC_HEART) > 0)
	{
		return m_pCards->Biggest(E_CC_HEART);
	}
	// throw biggest card
	T_COLOR l_enColor = 
		m_pCards->GetBiggestFirstCardColorRank(1);
	return m_pCards->Biggest(l_enColor);
}


// ---------------------------------------------------------
// should we throw HEART color
//
BOOL //OUT TRUE - we should
CNoHeartsDecider::ShouldThrowHearts(
	short a_nTrick
	) const
{
	// we should if
	// we have only hearts in hand
	if (!m_pCards->AreNoHearts())
	{
		return TRUE;
	}

	// or we have two or less hearts in hand
	if (m_pCards->ColorAllCnt(E_CC_HEART) <= 2)
	{
		return TRUE;
	}

	// or we have only lowest cards in other colors (or don't
	// have a color)
	return	(
		(m_pCards->HasLeastInColor(E_CC_DIAMOND, m_pTricks)) &&
		(m_pCards->HasLeastInColor(E_CC_SPADE, m_pTricks)) &&
		(m_pCards->HasLeastInColor(E_CC_CLUB, m_pTricks))
			);
	
}


// ---------------------------------------------------------
// should we throw other than HEARTS color
//
BOOL	//OUT TRUE - we should
CNoHeartsDecider::ShouldThrowOtherColor(
	short a_nTrick,		//IN trick nr 
	T_COLOR a_enColor	//IN color to check
	) const
{
	if (LeftInColorOthers(a_enColor) <= 0)
	{
		return FALSE;
	}

	const CCard* l_pCard = m_pCards->BiggestInColorCard(a_enColor);
	return (m_pTricks->AreLessInColor(l_pCard, m_pCards));
}
