// SortCards.h: interface for the CSortCards class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <random>

class CSortCards  
{
public:
	CSortCards();
	virtual ~CSortCards();

	void ShuffleDeck();

	short operator[](short a_iPos) const;
	short GetCardNr(short a_iPos) const;

private:
	short m_arrSortCards[52];
	std::mt19937 m_gen;
};

