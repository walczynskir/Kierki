// RecoverDecider.h: interface for the CRecoverDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECOVERDECIDER_H__8A41AE41_DA77_11D3_A79A_407610C10000__INCLUDED_)
#define AFX_RECOVERDECIDER_H__8A41AE41_DA77_11D3_A79A_407610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Decider.h"

class CRecoverDecider : public  CDecider  
{
public:
	CRecoverDecider(const CUserCards* a_pUserCards, const CTakenTricks* a_pTricks, T_PLAYER a_enPlayer, T_SUIT a_enTrumps);
	virtual ~CRecoverDecider();
	virtual short GetCardNr(short a_nTrick) const;

private:
	short DecideNotNeededCard() const;
	short FirstCard(short a_nTrick) const;
	short NextCard(short a_nTrick) const;
	short NextCardInColor(short a_nTrick) const;
	short NextCardInColor3(short a_nTrick) const;
	short NextCardInColor2(short a_nTrick) const;
	short NextCardInColor1(short a_nTrick) const;
	short NextCardNotInColor(short a_nTrick) const;
	short NextCardNotInColor3(short a_nTrick) const;
	short NextCardNotInColor2(short a_nTrick) const;
	short NextCardNotInColor1(short a_nTrick) const;
	short FirstCardNoTrumps(short a_nTrick) const;
	short NextCardNoTrumps(short a_nTrick) const;
	short NextCardInColorNoTrumps(short a_nTrick) const;
	short NextCardNotInColorNoTrumps(short a_nTrick) const;
	T_SUIT m_enTrumps;
};

#endif // !defined(AFX_RECOVERDECIDER_H__8A41AE41_DA77_11D3_A79A_407610C10000__INCLUDED_)
