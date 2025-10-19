// PuzzleRows.h: interface for the CPuzzleRows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUZZLEROWS_H__863025EA_E0D3_11D3_A79A_A07610C10000__INCLUDED_)
#define AFX_PUZZLEROWS_H__863025EA_E0D3_11D3_A79A_A07610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Card.h"

class CPuzzleRows
{
public:
	short GetCardsCnt() const;
	CPuzzleRows();
	virtual ~CPuzzleRows();

	BOOL CanPutCard(const CCard& a_card)	const;
	void PutCard(const CCard* a_pCard);
	void Clear();
	const CCard* GetTopCard(T_SUIT a_enColor) const;
	const CCard* GetBottomCard(T_SUIT a_enColor) const;

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
	CPuzzleRow m_arPuzzleRow[4];
};

#endif // !defined(AFX_PUZZLEROWS_H__863025EA_E0D3_11D3_A79A_A07610C10000__INCLUDED_)
