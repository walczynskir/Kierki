// NoKingOfHeartDecider.cpp: implementation of the CNoKingOfHeartDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NoKingOfHeartDecider.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// Konstruktor
//
CNoKingOfHeartDecider::CNoKingOfHeartDecider(
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
CNoKingOfHeartDecider::~CNoKingOfHeartDecider()
{

}


// ---------------------------------------------------------
//	Daj kart�
//
short	//WY numer karty
CNoKingOfHeartDecider::GetCardNr(
	short a_iTrick	//WE numer lewy
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
//	Pierwsza karta (wist)
//
short	//WY numer karty
CNoKingOfHeartDecider::FirstCard(
	short a_iTrick	//WE numer lewy
	) const
{
	T_COLOR l_enColor = DecideColor(a_iTrick);
	if (l_enColor == Suit::Heart)
		return m_pCards->FirstInColor(l_enColor);
	else
	{
		if ((m_pTricks->TricksInColor(l_enColor) < 2) && (m_pCards->ColorAllCnt(l_enColor) < 7))
			return m_pCards->Biggest(l_enColor);
		else
			return m_pCards->FirstInColor(l_enColor);
	}

	return a_iTrick;
}


// ---------------------------------------------------------
//	W jakim kolorze zagramy
//
T_COLOR		//WY wybrany kolor
CNoKingOfHeartDecider::DecideColor(
	short a_iTrick	//WE numer lewy
	) const
{
	short l_nRank;
	T_COLOR l_enColor;
	// mo�e nie ma innego koloru opr�cz kier�w
	if (!m_pCards->HasNoHeartsColor())
		return Suit::Heart;
	// pierwsza pr�ba - najkr�tszy kolor - najmniej lew
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank);
		if (l_enColor == Suit::Heart)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (m_pTricks->TricksInColor(l_enColor) < 2)
		{
			if (m_pCards->AllCardsInColor(l_enColor) < 7)
				return l_enColor;
		}
	}

	// znajd� sw�j najkr�tszy nie sercowy kolor 
	// je�li nie masz dw�jki to nie graj
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == Suit::Heart)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (LeftInColorOthers(l_enColor) < 2)
			continue;
		if (m_pCards->FindCard(l_enColor, E_CV_2) > 0)
			return l_enColor;
		if (m_pCards->FindCard(l_enColor, E_CV_3) > 0)
			return l_enColor;
	}

	// znajd� kolor w kt�rym reszta ma najwi�cej kart
	// je�li nie masz i graj w niego
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = LeftInColorOthersRank(l_nRank);
		if (l_enColor == Suit::Heart)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		return l_enColor;
	}

	// tu doj�� nie powinien
	ASSERT(FALSE);
	return l_enColor;
}


// ---------------------------------------------------------
//	Karta kolejna (do ju� dorzuconych)
//
short	//WY numer karty 
CNoKingOfHeartDecider::NextCard(
	short a_iTrick	//WE numer lewy
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;
	const CCard* l_cardFirst = l_trickCurrent.GetCard(0) ;
	T_COLOR l_enColor = l_trickCurrent.GetCardColor(0);
	short l_nCardNr;

	if (l_trickCurrent.IsKingOfHeart())
		if (m_pCards->HasColor(l_enColor))
		{
			l_nCardNr = m_pCards->FirstInColor(l_enColor);
			ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
			return l_nCardNr;
		}
		else
		{
			l_nCardNr = m_pCards->FirstInShortest();
			ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
			return l_nCardNr;
		}

	if (!m_pCards->HasColor(l_enColor))
	{
		l_nCardNr = FindCardOther();
		ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
		return l_nCardNr;
	}

	if (m_pTricks->TricksInColor(l_enColor) > 2)
	{
		l_nCardNr =  m_pCards->LessOrFirstBiggerInColor(l_trickCurrent.Biggest());
		ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
		return l_nCardNr;
	}
	else
	{
		l_nCardNr = m_pCards->Biggest(l_enColor);
		ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
		return l_nCardNr;
	}
}


// ---------------------------------------------------------
//	Wyb�r karty nie do koloru, kt�r� chcemy zrzuci� 
//
short	//WY numer karty
CNoKingOfHeartDecider::FindCardOther() const
{
	T_COLOR l_enColor;
	short l_nRank;
	short l_nCardNr;

	// jesli mamy kr�la to go zrzucamy
	if ((l_nCardNr = m_pCards->FindCard(Suit::Heart, E_CV_K)) > 0)
		return l_nCardNr;

	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		// kiery na ko�cu, chyba �e masz jednego
		if ((l_enColor == Suit::Heart) && (m_pCards->ColorAllCnt(l_enColor) > 1))
			continue;
		if (LeftInColorOthers(l_enColor) == 0)
			continue;
		// dw�jki nie ma sensu wyrzuca� je�li j� mamy
		if ((m_pCards->ColorCnt(l_enColor) == 1) && (m_pCards->FindCard(l_enColor, E_CV_2) > 0))
			continue;
		
		return m_pCards->Biggest(l_enColor);
	}

	// spr�bujmy wyrzuci� kiera
	if (m_pCards->HasColor(Suit::Heart))
		return m_pCards->Biggest(Suit::Heart);

	// teraz jest ju� mi wszystko jedno byle co� znale��
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		
		return m_pCards->Biggest(l_enColor);
	}
	ASSERT(FALSE);
	return l_nCardNr;
}