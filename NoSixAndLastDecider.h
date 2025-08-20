// NoSixAndLastDecider.h: interface for the CNoSixAndLastDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOSIXANDLASTDECIDER_H__8C292621_B41B_11D3_A79A_9C7610C10000__INCLUDED_)
#define AFX_NOSIXANDLASTDECIDER_H__8C292621_B41B_11D3_A79A_9C7610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Decider.h"

class CNoSixAndLastDecider : public CDecider  
{
public:
	CNoSixAndLastDecider(const CUserCards* a_pUserCards, const CTakenTricks* a_pTricks);
	virtual ~CNoSixAndLastDecider();
	virtual short GetCardNr(short a_iTrick) const;
private:
	short BestCard(short a_iTrick) const;
	short FirstCard(short a_iTrick) const;
	short NextCard(short a_iTrick) const;
	short Next6Card(short a_iTrick) const;
	T_COLOR DecideColor(short a_iTrick) const;
	T_COLOR Decide6Color(short a_iTrick) const;
	T_COLOR Decide5Color(short a_iTrick) const;
};

#endif // !defined(AFX_NOSIXANDLASTDECIDER_H__8C292621_B41B_11D3_A79A_9C7610C10000__INCLUDED_)
