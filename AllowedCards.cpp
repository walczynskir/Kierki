// AllowedCards.cpp: implementation of the CAllowedCards class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AllowedCards.h"
#include <time.h>



// ---------------------------------------------------------
// Konstruktor
//
CAllowedCards::CAllowedCards() 
{ 
	m_nCards = 0;
}


// ---------------------------------------------------------
// Dodanie kart do kart mo¿liwych do po³o¿enia
// w puzzlach
//
void 
CAllowedCards::AddCard(
	short a_nInHand		//WE która w rêce
	)
{
	if (a_nInHand < 0)
	{
		return;
	}
	m_arrCardsPoints[m_nCards].SetCardPoints(a_nInHand);
	m_nCards++;

}


// ---------------------------------------------------------
// Posortowanie wybranych kart
//
void 
CAllowedCards::Sort()
{
	// nie ma co sortowaæ?
	if ((m_nCards <= 1))
	{
		return;
	}

    srand((unsigned)time(NULL)); // inicjalizacja generatora

	int l_iAt;
	// rozsortowanie tablicy 52 liczb
	for (l_iAt = 0; l_iAt < 10; l_iAt++)
	{
		int l_iFirst = rand() * m_nCards / RAND_MAX - 1;
		int l_iSecond = rand() * m_nCards / RAND_MAX - 1;
		if (l_iFirst < 0)
			l_iFirst = 0;
		if (l_iFirst > 7)
			l_iFirst = 7;
		if (l_iSecond < 0)
			l_iSecond = 0;
		if (l_iSecond > 7)
			l_iSecond = 7;
		short l_nCopyBuf = m_arrCardsPoints[l_iFirst].GetCard();
		m_arrCardsPoints[l_iFirst].SetCardPoints(m_arrCardsPoints[l_iSecond].GetCard());
		m_arrCardsPoints[l_iSecond].SetCardPoints(l_nCopyBuf);
	}

}


// ---------------------------------------------------------
// Zwraca iloœæ punktów dla pozycji
//
short	//WY iloœæ punktów
CAllowedCards::GetPoints(
	short a_nAt		//WE pozycja
	) const
{
	return m_arrCardsPoints[a_nAt].GetPoints();
}


// ---------------------------------------------------------
// Pobiera numer karty
//
short	//WE iloœæ dozwolonych kart
CAllowedCards::GetAllowedCount() const
{
	return m_nCards;
}


// ---------------------------------------------------------
// Zwraca dozwolona kartê
//
short	//WY numer w rêce dozwolonej karty
CAllowedCards::GetCard(
	short a_nPos	//WE która dozwolona karta
	) const
{
	ASSERT(a_nPos >= 0);
	ASSERT(a_nPos < m_nCards);
	return m_arrCardsPoints[a_nPos].GetCard();
}


// ---------------------------------------------------------
// Ustawia iloœæ punktów dla karty
//
void 
CAllowedCards::SetPoints(
	short a_nAt,		//WE karta
	short a_nPoints		//WE punktów
	)
{
	m_arrCardsPoints[a_nAt].SetPoints(a_nPoints);
}


// ---------------------------------------------------------
// Zwraca iloœæ punktów
//
short	//WY iloœæ punktów
CAllowedCards::CCardsPoints::GetPoints() const
{
	return m_nPoints;
}


// ---------------------------------------------------------
// Pobiera numer karty
//
short	//WY numer karty
CAllowedCards::CCardsPoints::GetCard() const
{
	return m_nInHand;
}


// ---------------------------------------------------------
// Ustawienie punktacji
//
void 
CAllowedCards::CCardsPoints::SetPoints(
	short a_nPoints		//WE iloœæ punktów
	)
{
	m_nPoints = a_nPoints;
}


// ---------------------------------------------------------
// Ustawienie dostêpnej karty
//
void 
CAllowedCards::CCardsPoints::SetCardPoints(
	short a_nInHand,	//WE karta w rêku
	short a_nPoints		//WE iloœæ punktów
	)
{
	m_nInHand = a_nInHand;
	m_nPoints = a_nPoints;
}