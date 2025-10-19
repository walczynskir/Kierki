// PuzzleDecider.h: interface for the CPuzzleDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUZZLEDECIDER_H__863025E9_E0D3_11D3_A79A_A07610C10000__INCLUDED_)
#define AFX_PUZZLEDECIDER_H__863025E9_E0D3_11D3_A79A_A07610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Decider.h"
#include "PuzzleRows.h"
#include "AllowedCards.h"


class CPuzzleDecider : public CDecider
{
public:
	CPuzzleDecider(const CUserCards* a_pUserCards, const CPuzzleRows* a_pPuzzleRow, T_PLAYER a_enPlayer);
	virtual ~CPuzzleDecider();
	virtual short GetCardNr(short a_nTrick) const;

private:
	short FindBestRest(CAllowedCards* a_pAllowedCards) const;
	short FindForNoOne(CAllowedCards* a_pAllowedCards) const;
	void DecidePointsFurthest(CAllowedCards* a_pAllowedCards, short a_nCard) const;
	short FindFurthest(CAllowedCards* a_pAllowedCards) const;
	short DecideCard(CAllowedCards* a_pAllowedCards) const;
	short GetCardInColor(T_SUIT a_enColor, BOOL a_bTop) const;
	const CPuzzleRows* m_pPuzzleRows;
	void FillAllowedCards(CAllowedCards* a_pAllowedCards) const;
};

#endif // !defined(AFX_PUZZLEDECIDER_H__863025E9_E0D3_11D3_A79A_A07610C10000__INCLUDED_)
