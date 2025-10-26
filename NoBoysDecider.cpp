// NoBoysDecider.cpp: implementation of the CNoBoysDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NoBoysDecider.h"

// ---------------------------------------------------------
// Konstruktor
//
CNoBoysDecider::CNoBoysDecider(
	const CUserCards*   a_pUserCards, //WE karty gracza
	const CTakenTricks* a_pTricks	  //WE wzi�te lewy	
	)
{
	m_pCards = a_pUserCards;
	m_pTricks = a_pTricks;
}


// ---------------------------------------------------------
// Destruktor
//
CNoBoysDecider::~CNoBoysDecider()
{

}


// ---------------------------------------------------------
//	Daj kart�
//
short	//WY numer karty
CNoBoysDecider::GetCardNr(
	short a_iTrick	//WE kt�ra lewa
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;

	short l_iCardNr ;
	if (l_trickCurrent.GetCardsCnt() == 0)
	{
		l_iCardNr = FirstCard(a_iTrick) ;
	}
	else
	{
		l_iCardNr = NextCard(a_iTrick) ;
	}
	ASSERT((l_iCardNr >= 0) && (l_iCardNr < 13)) ;
	return l_iCardNr;
}


// ---------------------------------------------------------
//	Daj pierwsz� kart� (zawistuj)
//
short	//WY numer karty
CNoBoysDecider::FirstCard(
	short a_iTrick	//WE kt�ra lewa
	) const
{
	short l_iCardNr;
	T_COLOR l_enColor = DecideColor(a_iTrick);
	l_iCardNr = m_pCards->GetBiggestBelow(l_enColor, E_CV_J);
	if (l_iCardNr < 0)
	{
		l_iCardNr = m_pCards->FirstInColor(l_enColor);
	}
	ASSERT((l_iCardNr >= 0) && (l_iCardNr < 13));
	return l_iCardNr;
}


// ---------------------------------------------------------
//	Daj kart� do zawistowanego koloru
//
short	//WY numer karty
CNoBoysDecider::NextCard(
	short a_iTrick	//WE kt�ra lewa
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;
	T_COLOR l_enColor = l_trickCurrent.GetCardColor(0);;
	const CCard* l_pCard = l_trickCurrent.Biggest();
	if (m_pCards->HasColor(l_enColor))
		return CardForBoys(l_pCard);
	else
		return BoyOrBiggestInShortest();
}


// ---------------------------------------------------------
//	Zdecyduj w jakim kolorze wychodzisz
//
T_COLOR		//WY kolor
CNoBoysDecider::DecideColor(
	short a_iTrick	//WE kt�ra lewa
	) const
{
	T_COLOR l_enColor;

	l_enColor = Suit::Club;
	if (CanPlayColor(a_iTrick, l_enColor))
		return l_enColor;

	l_enColor = Suit::Diamond;
	if (CanPlayColor(a_iTrick, l_enColor))
		return l_enColor;

	l_enColor = Suit::Spade;
	if (CanPlayColor(a_iTrick, l_enColor))
		return l_enColor;

	l_enColor = Suit::Heart;
	if (CanPlayColor(a_iTrick, l_enColor))
		return l_enColor;
	
	// nie znaleziono koloru, kt�ry mo�na swobodnie zagra�. Szukamy najkr�tszego, 
	// kt�rym s� jeszcze wy�sze
	short l_nRank; 
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pTricks->LeftInColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;

		short l_iCard = m_pCards->FirstInColor(l_enColor);
		const CCard& l_card = m_pCards->GetCard(l_iCard);
		if (m_pTricks->AreBiggerInColor(&l_card, m_pCards))
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
//	Czy mozna bez obaw zagra� kolor
//	1. S� karty wy�sze w tym kolorze
//	2. Nie ma karty wy�szej od 10
//
BOOL	//WY TRUE - mo�na zagra� kolor
CNoBoysDecider::CanPlayColor(
	short a_iTrick,		//WE kt�ra lewa
	T_COLOR a_enColor	//WE sprawdzany kolor
	) const
{
	short l_nCard;
	if (!m_pCards->HasColor(a_enColor))
		return FALSE;
	for (l_nCard = 1; l_nCard <= 13; l_nCard++)	
	{
		if (m_pCards->GetCard(l_nCard - 1).GetSuit() != a_enColor)
			continue;
		if (m_pCards->GetCard(l_nCard - 1).IsUsed())
			continue;
		
		if (m_pCards->GetCard(l_nCard - 1).CardValue() >= E_CV_J)
			continue;

		if (m_pTricks->AreBiggerInColor(&(m_pCards->GetCard(l_nCard - 1)), m_pCards))
			return TRUE;
	}
	return FALSE;	
}


// ---------------------------------------------------------
//	Najlepsza karta w danym kolorze, gdy po�o�ona podana karta
//	Algorytm:
//	Je�li najwy�sza wy�sza to D to po�o� J lub ni�sz� je�li masz
//	Je�li najwy�sza wy�sza to K to po�o� J lub D lub ni�sz� je�li masz
//	Je�li najwy�sza wy�sza to A to po�o� K, J lub D lub ni�sz� je�li masz
//	Je�li inne to po�� najwy�sz� kart� do 10 lub najni�sz� powy�ej 10
//	a_pCard - najwy�sza karta w granym kolorze
//
short	//WY numer najlepszej karty
CNoBoysDecider::CardForBoys(
	const CCard* a_pCard	// karta 
	) const
{
	short l_nCardNr;
	switch (a_pCard->CardValue())
	{
	case E_CV_D:
		l_nCardNr = m_pCards->FindCard(a_pCard->GetSuit(), E_CV_J); 
		if (l_nCardNr >= 0)
		{
			return l_nCardNr;
		}
		l_nCardNr = m_pCards->GetBiggestBelow(a_pCard->GetSuit(), E_CV_J);
		if (l_nCardNr >= 0)
		{
			return l_nCardNr;
		}
		return m_pCards->FirstInColor(a_pCard->GetSuit());

	case E_CV_K:
		l_nCardNr = m_pCards->FindCard(a_pCard->GetSuit(), E_CV_J); 
		if (l_nCardNr >= 0)
		{
			return l_nCardNr;
		}
		l_nCardNr = m_pCards->FindCard(a_pCard->GetSuit(), E_CV_D); 
		if (l_nCardNr >= 0)
		{
			return l_nCardNr;
		}
		break;

	case E_CV_A:
		l_nCardNr = m_pCards->FindCard(a_pCard->GetSuit(), E_CV_K); 
		if (l_nCardNr >= 0)
		{
			return l_nCardNr;
		}
		l_nCardNr = m_pCards->FindCard(a_pCard->GetSuit(), E_CV_J); 
		if (l_nCardNr >= 0)
		{
			return l_nCardNr;
		}
		l_nCardNr = m_pCards->FindCard(a_pCard->GetSuit(), E_CV_D); 
		if (l_nCardNr >= 0)
		{
			return l_nCardNr;
		}
		break;

	}
	l_nCardNr = m_pCards->GetBiggestBelow(a_pCard->GetSuit(), E_CV_J);
	if (l_nCardNr >= 0)
	{
		return l_nCardNr;
	}

	return m_pCards->FirstInColor(a_pCard->GetSuit());
}


// ---------------------------------------------------------
//	Znajd� kr�la lub waleta w kolorze najkr�tszym
//	Je�li nie to najwy�sz� kart� w najkr�tszym
//
short	//WY numer karty
CNoBoysDecider::BoyOrBiggestInShortest() const
{
	short l_nCardNr;
	
	short l_nRank;
	T_COLOR l_enColor;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank);
		l_nCardNr = m_pCards->FindCard(l_enColor, E_CV_K);
		if (l_nCardNr > 0)
			return l_nCardNr;
		l_nCardNr = m_pCards->FindCard(l_enColor, E_CV_J);
		if (l_nCardNr > 0)
			return l_nCardNr;
	}


	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank);
		if (m_pCards->HasColor(l_enColor))
			return m_pCards->Biggest(l_enColor);
	}

	ASSERT(FALSE);	// tu nie ma prawa wroci�
	return l_nCardNr;
}
