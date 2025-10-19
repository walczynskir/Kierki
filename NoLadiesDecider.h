// NoLadiesDecider.h: interface for the CNoLadiesDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOLADIESDECIDER_H__69EFE701_AB11_11D3_A79A_9C7610C10000__INCLUDED_)
#define AFX_NOLADIESDECIDER_H__69EFE701_AB11_11D3_A79A_9C7610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Decider.h"

class CNoLadiesDecider : public CDecider  
{
public:
	CNoLadiesDecider(const CUserCards* a_pUserCards, const CTakenTricks* a_pTricks);
	virtual ~CNoLadiesDecider();
	virtual short GetCardNr(short a_iTrick) const;
private:
	short LadyOrBiggestInShortest() const;
	short CardForLadies(const CCard* a_pCard) const;
	short FirstCard(short a_iTrick) const;
	short NextCard(short a_iTrick) const;
	T_SUIT DecideColor(short a_iTrick) const;
	BOOL CanPlayColor(short a_iTrick, T_SUIT a_enColor) const;
};

#endif // !defined(AFX_NOLADIESDECIDER_H__69EFE701_AB11_11D3_A79A_9C7610C10000__INCLUDED_)
