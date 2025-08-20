// NoTricksDecider.h: interface for the CNoTricksDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTRICKSDECIDER_H__CE1667E4_5EDB_11D3_B413_A87510C10000__INCLUDED_)
#define AFX_NOTRICKSDECIDER_H__CE1667E4_5EDB_11D3_B413_A87510C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Decider.h"

class CNoTricksDecider : public CDecider  
{
public:
	CNoTricksDecider(const CUserCards* a_pUserCards, const CTakenTricks* a_pTricks);
	virtual ~CNoTricksDecider();
	virtual short GetCardNr(short a_nTrick)	const;
private:
	short FirstCard(short a_nTrick) const;
	short NextCard(short a_nTrick) const;
	short DecideOtherColor(short a_nTrick) const;
};

#endif // !defined(AFX_NOTRICKSDECIDER_H__CE1667E4_5EDB_11D3_B413_A87510C10000__INCLUDED_)
