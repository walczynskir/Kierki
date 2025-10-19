// Decider.cpp: implementation of the CDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Decider.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// Konstruktor
//
CDecider::CDecider(
	const CUserCards* a_pCards	//WE karty gracza
	)
{
	m_pCards = a_pCards ;
	m_pTricks = NULL ;
}


// ---------------------------------------------------------
//	Destruktor
//
CDecider::~CDecider()
{

}


// ---------------------------------------------------------
//	Ile kart zosta³o w kolorze u innych graczy 
//	(dla danego gracza)
//
short	//WY iloœæ kart w kolorze u innych
CDecider::LeftInColorOthers(
	T_SUIT a_enColor	//WE sprawdzany kolor
	)	const
{
	short l_nWent;
	short l_nPlayerCards;
	l_nWent = m_pTricks->TakenInColor(a_enColor);
	l_nPlayerCards = m_pCards->ColorCnt(a_enColor);

	return (13 - (l_nWent + l_nPlayerCards));
	
}

// ---------------------------------------------------------
//	Kolory w których zosta³o najmniej kart wed³ug rankingu
//
T_SUIT		//WY kolor z najmniejsz¹ iloœci¹ kart
CDecider::LeftInColorOthersRank(
	short a_nRank	//WE ranking
	)	const
{
	ASSERT((a_nRank >= 1) && (a_nRank <= 4));
	struct S_RANK
	{
		T_SUIT enColor;
		short	nLeft;
	} l_aColorCnt[4], l_aColorSort[4];

	short l_iAt = 0;
	l_aColorCnt[l_iAt].enColor = E_CC_CLUB;
	l_aColorCnt[l_iAt].nLeft = LeftInColorOthers(l_aColorCnt[l_iAt].enColor);
	l_iAt++;

	l_aColorCnt[l_iAt].enColor = E_CC_DIAMOND;
	l_aColorCnt[l_iAt].nLeft = LeftInColorOthers(l_aColorCnt[l_iAt].enColor);
	l_iAt++;

	l_aColorCnt[l_iAt].enColor = E_CC_SPADE;
	l_aColorCnt[l_iAt].nLeft = LeftInColorOthers(l_aColorCnt[l_iAt].enColor);
	l_iAt++;

	l_aColorCnt[l_iAt].enColor = E_CC_HEART;
	l_aColorCnt[l_iAt].nLeft = LeftInColorOthers(l_aColorCnt[l_iAt].enColor);

	// po wszystkich elementach - najd³u¿szy przepisuj
	short l_nSort = 1;
	short l_nMax = -1;
	short l_nMaxPos = -1;
	while (l_nSort <= 4)
	{
		l_nMax = -1;
		for (l_iAt = 1; l_iAt <= 4; l_iAt++)
		{
			if (l_nMax < l_aColorCnt[l_iAt - 1].nLeft)
			{
				l_nMax = l_aColorCnt[l_iAt - 1].nLeft;
				l_nMaxPos = l_iAt;
			}
		}
		l_aColorSort[l_nSort - 1].enColor = l_aColorCnt[l_nMaxPos - 1].enColor;
		l_aColorSort[l_nSort - 1].nLeft = l_aColorCnt[l_nMaxPos - 1].nLeft;
		l_aColorCnt[l_nMaxPos - 1].nLeft = -1;
		l_nSort++;
	}

	return l_aColorSort[4 - a_nRank].enColor;
}
