// NoHeartsDecider.h: interface for the CNoHeartsDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOHEARTSDECIDER_H__DF2796C1_8583_11D3_B413_E87410C10000__INCLUDED_)
#define AFX_NOHEARTSDECIDER_H__DF2796C1_8583_11D3_B413_E87410C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Decider.h"

class CNoHeartsDecider : public CDecider  
{
public:
	CNoHeartsDecider(const CUserCards* a_pUserCards, const CTakenTricks* a_pTricks);
	virtual ~CNoHeartsDecider();
	virtual short GetCardNr(short a_iTrick) const;
private:
	short FirstCard(short a_iTrick) const;
	short NextCard(short a_iTrick) const;
	short DecideFirstCardNoHearts(short a_iTrick) const;
	short GetWorstColorCard(short a_iTrick) const;
	short GetNotTakingCard(short a_iTrick) const;
	short GetShortestAreBigger(short a_iTrick) const;
	short GetBestOtherCard(short a_iTrick) const;
	short DecideOtherColor(short a_iTrick) const;
	BOOL ShouldThrowHearts(short a_nTrick) const;
	BOOL ShouldThrowOtherColor(short a_nTrick, T_COLOR a_enColor) const;
};

#endif // !defined(AFX_NOHEARTSDECIDER_H__DF2796C1_8583_11D3_B413_E87410C10000__INCLUDED_)
