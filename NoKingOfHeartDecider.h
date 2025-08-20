// NoKingOfHeartDecider.h: interface for the CNoKingOfHeartDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOKINGOFHEARTDECIDER_H__F96957A1_B1AF_11D3_A79A_9C7610C10000__INCLUDED_)
#define AFX_NOKINGOFHEARTDECIDER_H__F96957A1_B1AF_11D3_A79A_9C7610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Decider.h"

class CNoKingOfHeartDecider : public CDecider  
{
public:
	CNoKingOfHeartDecider(const CUserCards* a_pUserCards, const CTakenTricks* a_pTricks);
	virtual ~CNoKingOfHeartDecider();
	virtual short GetCardNr(short a_iTrick) const;

private:
	short FirstCard(short a_iTrick) const;
	short NextCard(short a_iTrick) const;
	T_COLOR DecideColor(short a_iTrick) const;
	short FindCardOther() const;
};

#endif // !defined(AFX_NOKINGOFHEARTDECIDER_H__F96957A1_B1AF_11D3_A79A_9C7610C10000__INCLUDED_)
