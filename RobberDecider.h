// RobberDecider.h: interface for the CRobberDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROBBERDECIDER_H__DEE2C101_D360_11D3_A79A_A47610C10000__INCLUDED_)
#define AFX_ROBBERDECIDER_H__DEE2C101_D360_11D3_A79A_A47610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Decider.h"

class CRobberDecider : public CDecider  
{
public:
	CRobberDecider(const CUserCards* a_pUserCards, const CTakenTricks* a_pTricks);
	virtual ~CRobberDecider();
	virtual short GetCardNr(short a_iTrick) const;
private:
	short DecideOtherCard() const;
	short FirstCardAfter6(short a_iTrick) const;
	short FirstCardBefore6(short a_iTrick) const;
	short FirstCard6(short a_iTrick) const;
	short FirstCard(short a_iTrick) const;
	short NextCard(short a_iTrick) const;
	short NextCardBefore6(short a_iTrick) const;
	short NextCard6(short a_iTrick) const;
	short NextCardAfter6(short a_iTrick) const;
	void DecideTactic();
	void DecideTacticFewHearts();
	enum {
		TC_GETNOTHING,
		TC_GETATBEG
	} m_enTactic ;
};

#endif // !defined(AFX_ROBBERDECIDER_H__DEE2C101_D360_11D3_A79A_A47610C10000__INCLUDED_)
