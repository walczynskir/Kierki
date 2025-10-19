// NoLadiesDecider.cpp: implementation of the CNoLadiesDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NoLadiesDecider.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// Konstruktor
//
CNoLadiesDecider::CNoLadiesDecider(	
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
CNoLadiesDecider::~CNoLadiesDecider()
{

}


// ---------------------------------------------------------
// Zwraca numer karty do zagrania
//
short	//WY numer karty
CNoLadiesDecider::GetCardNr(
	short a_iTrick	//WE która lewa
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;

	short l_iCardNr ;
	if (l_trickCurrent.GetCardsCnt() == 0)
		l_iCardNr = FirstCard(a_iTrick) ;
	else
		l_iCardNr = NextCard(a_iTrick) ;
	ASSERT((l_iCardNr >= 0) && (l_iCardNr < 13)) ;
	return l_iCardNr;
}


// ---------------------------------------------------------
//	Daj pierwsz¹ kartê (zawistuj)
//
short	//WY numer karty
CNoLadiesDecider::FirstCard(
	short a_iTrick	//WE która lewa
	) const
{
	short l_iCardNr;
	T_SUIT l_enColor = DecideColor(a_iTrick);
	l_iCardNr = m_pCards->GetBiggestBelow(l_enColor, E_CV_D);
	if (l_iCardNr <= 0)
	{
		l_iCardNr = m_pCards->FirstInColor(l_enColor);
	}
	ASSERT((l_iCardNr >= 0) && (l_iCardNr < 13));
	return l_iCardNr;
}


// ---------------------------------------------------------
//	Daj kartê do zawistowanego koloru
//
short	//WY numer karty
CNoLadiesDecider::NextCard(
	short a_iTrick	//WE numer lewy
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;
	T_SUIT l_enColor = l_trickCurrent.GetCardColor(0);
	const CCard* l_pCard = l_trickCurrent.Biggest();
	if (m_pCards->HasColor(l_enColor))
		return CardForLadies(l_pCard);
	else
		return LadyOrBiggestInShortest();
}


// ---------------------------------------------------------
//	Zdecyduj w jakim kolorze wychodzisz
//
T_SUIT	//WY wybrany kolor
CNoLadiesDecider::DecideColor(
	short a_iTrick	//WE numer lewy
	) const
{
	T_SUIT l_enColor;

	l_enColor = E_CC_CLUB;
	if (CanPlayColor(a_iTrick, l_enColor))
		return l_enColor;

	l_enColor = E_CC_DIAMOND;
	if (CanPlayColor(a_iTrick, l_enColor))
		return l_enColor;

	l_enColor = E_CC_SPADE;
	if (CanPlayColor(a_iTrick, l_enColor))
		return l_enColor;

	l_enColor = E_CC_HEART;
	if (CanPlayColor(a_iTrick, l_enColor))
		return l_enColor;
	
	// nie znaleziono koloru, który mo¿na swobodnie zagraæ. Szukamy najkrótszego, 
	// którym s¹ jeszcze wy¿sze
	short l_nRank; 
	const CCard* l_pCard;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pTricks->LeftInColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;

		short l_iCard = m_pCards->FirstInColor(l_enColor);
		l_pCard = &(m_pCards->GetCard(l_iCard));
		if (m_pTricks->AreBiggerInColor(l_pCard, m_pCards))
			return l_enColor;
	}
	
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pTricks->LeftInColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		return l_enColor;
	}

	ASSERT(FALSE);
	return l_enColor;
}


// ---------------------------------------------------------
//	Czy mozna bez obaw zagraæ kolor
//	1. S¹ karty wy¿sze w tym kolorze
//	2. Nie ma karty wy¿szej od J
//
BOOL	//WY TRUE - mo¿na zagraæ
CNoLadiesDecider::CanPlayColor(
	short a_iTrick,		//WE numer lewy
	T_SUIT a_enColor	//WE sprawdzany kolor
	) const
{
	short l_nCard;
	if (!m_pCards->HasColor(a_enColor))
		return FALSE;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)	
	{
		if (m_pCards->GetCard(l_nCard).GetColor() != a_enColor)
			continue;
		if (m_pCards->GetCard(l_nCard).IsUsed())
			continue;
		
		if (m_pCards->GetCard(l_nCard).CardValue() >= E_CV_D)
			continue;

		if (m_pTricks->AreBiggerInColor(&(m_pCards->GetCard(l_nCard)), m_pCards))
			return TRUE;
	}
	return FALSE;	
}


// ---------------------------------------------------------
//	Najlepsza karta w danym kolorze, gdy po³o¿ona podana karta
//	Algorytm:
//	Jeœli najwy¿sza wy¿sza to D to po³o¿ J lub ni¿sz¹ jeœli masz
//	Jeœli najwy¿sza wy¿sza to K to po³o¿ J lub D lub ni¿sz¹ jeœli masz
//	Jeœli najwy¿sza wy¿sza to A to po³o¿ K, J lub D lub ni¿sz¹ jeœli masz
//	Jeœli inne to po³ó¿ najwy¿sz¹ kartê do 10 lub najni¿sz¹ powy¿ej 10
//	a_pCard - najwy¿sza karta w granym kolorze
//
short	//WY numer karty
CNoLadiesDecider::CardForLadies(
	const CCard* a_pCard	//WE sprawdzana karta
	) const
{
	short l_nCardNr;
	switch (a_pCard->CardValue())
	{
	case E_CV_K:
		l_nCardNr = m_pCards->FindCard(a_pCard->GetColor(), E_CV_D); 
		if (l_nCardNr >= 0)
			return l_nCardNr;
		l_nCardNr = m_pCards->GetBiggestBelow(a_pCard->GetColor(), E_CV_D);
		if (l_nCardNr >= 0)
			return l_nCardNr;
		return m_pCards->FirstInColor(a_pCard->GetColor());

	case E_CV_A:
		l_nCardNr = m_pCards->FindCard(a_pCard->GetColor(), E_CV_D); 
		if (l_nCardNr >= 0)
			return l_nCardNr;
		l_nCardNr = m_pCards->FindCard(a_pCard->GetColor(), E_CV_K); 
		if (l_nCardNr >= 0)
			return l_nCardNr;
		break;
	}
	l_nCardNr = m_pCards->GetBiggestBelow(a_pCard->GetColor(), E_CV_D);
	if (l_nCardNr >= 0)
		return l_nCardNr;

	return m_pCards->FirstInColor(a_pCard->GetColor());
}


// ---------------------------------------------------------
//	ZnajdŸ damê w kolorze najkrótszym
//	Jeœli nie to najwy¿sz¹ kartê w najkrótszym
//
short	//WY numer karty
CNoLadiesDecider::LadyOrBiggestInShortest() const
{
	short l_nCardNr;
	
	short l_nRank;
	T_SUIT l_enColor;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank);
		l_nCardNr = m_pCards->FindCard(l_enColor, E_CV_D);
		if (l_nCardNr >= 0)
			return l_nCardNr;
	}


	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank);
		if (m_pCards->HasColor(l_enColor))
			return m_pCards->Biggest(l_enColor);
	}

	ASSERT(FALSE);	// tu nie ma prawa wrociæ
	return l_nCardNr;
}
