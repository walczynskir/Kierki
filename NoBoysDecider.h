// NoBoysDecider.h: interface for the CNoBoysDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOBOYSDECIDER_H__B09A12C1_A90E_11D3_A79A_9C7610C10000__INCLUDED_)
#define AFX_NOBOYSDECIDER_H__B09A12C1_A90E_11D3_A79A_9C7610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Decider.h"

class CNoBoysDecider : public CDecider  
{
public:
	CNoBoysDecider(const CUserCards *a_pUserCards, const CTakenTricks* a_pTricks);
	virtual ~CNoBoysDecider();
	virtual short GetCardNr(short a_iTrick) const;
private:
	short BoyOrBiggestInShortest() const;
	short CardForBoys(const CCard* a_pCard) const;
	short FirstCard(short a_iTrick) const;
	short NextCard(short a_iTrick) const;
	T_SUIT DecideColor(short a_iTrick) const;
	BOOL CanPlayColor(short a_iTrick, T_SUIT a_enColor) const;
};

#endif // !defined(AFX_NOBOYSDECIDER_H__B09A12C1_A90E_11D3_A79A_9C7610C10000__INCLUDED_)
