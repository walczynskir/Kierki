// SortCards.h: interface for the CSortCards class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SORTCARDS_H__2494B841_17B6_11D4_A79A_347710C10000__INCLUDED_)
#define AFX_SORTCARDS_H__2494B841_17B6_11D4_A79A_347710C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

};

#endif // !defined(AFX_SORTCARDS_H__2494B841_17B6_11D4_A79A_347710C10000__INCLUDED_)
