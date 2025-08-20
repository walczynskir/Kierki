// SortCards.cpp: implementation of the CSortCards class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SortCards.h"


#include <random>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
//	Konstruktor
//
CSortCards::CSortCards()
{
	short l_iAt ;
	for (l_iAt = 0; l_iAt < 52; l_iAt++)
		m_arrSortCards[l_iAt] = l_iAt + 1;

}


// ---------------------------------------------------------
//	Destruktor
//
CSortCards::~CSortCards()
{

}


// ---------------------------------------------------------
//	Deck shuffling
//
void 
CSortCards::ShuffleDeck()
{
	int l_iAt;

	// initialization of random number generator
	std::random_device l_rd;
	std::mt19937 l_gen(l_rd());

	// Fisher-Yates shuffle algorithm
	for (l_iAt = 51; l_iAt > 0; l_iAt--)
	{
		// Utworzenie rozk³adu do losowania indeksu j z zakresu [0, i]
		std::uniform_int_distribution<int> l_dist(0, l_iAt);
		int l_iAtNew = l_dist(l_gen);
		ASSERT((l_iAtNew >= 0) && (l_iAtNew <= 51));

		// Zamiana elementu na pozycji i z elementem na losowej pozycji j
		std::swap(m_arrSortCards[l_iAt], m_arrSortCards[l_iAtNew]);
	}

}


// ---------------------------------------------------------
//	Zwraca numer karty na wybranej pozycji
//
short	//WY numer karty na wybranej pozycji
CSortCards::GetCardNr(
	short a_iPos	//WE wybrana pozycja
	) const
{
	ASSERT(a_iPos >= 0);
	ASSERT(a_iPos < 52);

	return m_arrSortCards[a_iPos];
}


// ---------------------------------------------------------
//	Zwraca numer karty na wybranej pozycji
//
short	//WY numer karty na wybranej pozycji
CSortCards::operator[](
	short a_iPos	//WE wybrana pozycja
	) const
{
	return GetCardNr(a_iPos);
}
