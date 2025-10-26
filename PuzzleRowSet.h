// PuzzleRows.h: interface for the CPuzzleRowSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUZZLEROWS_H__863025EA_E0D3_11D3_A79A_A07610C10000__INCLUDED_)
#define AFX_PUZZLEROWS_H__863025EA_E0D3_11D3_A79A_A07610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Card.h"

class CPuzzleRowSet
{
public:
	short GetCardsCnt() const;
	CPuzzleRowSet();
	virtual ~CPuzzleRowSet();

	BOOL CanPutCard(const CCard& a_card)	const;
	void PutCard(const CCard* a_pCard);
	void Clear();
	const CCard* GetTopCard(Suit a_suit) const;
	const CCard* GetBottomCard(Suit a_suit) const;

	class CPuzzleRow
	{
	public:
		short GetCardsCnt() const;
		BOOL CanPutCard(const CCard& a_card) const;
		void Clear();
		CPuzzleRow() { m_pCardTop = m_pCardBottom = NULL;};
		const CCard* m_pCardTop;
		const CCard* m_pCardBottom;
	};


	CPuzzleRow& operator[](Suit suit)
	{
		ASSERT(suit >= Suit::Club && suit <= Suit::Heart);
		return m_arPuzzleRow[SuitToIndex(suit)];
	}

	const CPuzzleRow& operator[](Suit suit) const
	{
		ASSERT(suit >= Suit::Club && suit <= Suit::Heart);
		return m_arPuzzleRow[SuitToIndex(suit)];
	}

private:
	CPuzzleRow m_arPuzzleRow[4];
};

#endif // !defined(AFX_PUZZLEROWS_H__863025EA_E0D3_11D3_A79A_A07610C10000__INCLUDED_)
