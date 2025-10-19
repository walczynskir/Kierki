// Decider.h: interface for the CDecider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DECIDER_H__CE1667E3_5EDB_11D3_B413_A87510C10000__INCLUDED_)
#define AFX_DECIDER_H__CE1667E3_5EDB_11D3_B413_A87510C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "Trick.h"
#include "UserCards.h"
#include "TakenTricks.h"

class CDecider  
{
public:
	CDecider(const CUserCards* a_pCards = NULL);
	virtual ~CDecider();
	virtual short GetCardNr(short a_iTrick) const = 0 ;

	T_SUIT LeftInColorOthersRank(short a_nRank)	const;
	short LeftInColorOthers(T_SUIT a_enColor)	const;

protected:
	T_PLAYER m_enPlayer;
	const CUserCards* m_pCards;
	const CTakenTricks* m_pTricks;
};

#endif // !defined(AFX_DECIDER_H__CE1667E3_5EDB_11D3_B413_A87510C10000__INCLUDED_)
