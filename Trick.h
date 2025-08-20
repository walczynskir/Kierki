// Trick.h: interface for the CTrick class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRICK_H__CE1667E5_5EDB_11D3_B413_A87510C10000__INCLUDED_)
#define AFX_TRICK_H__CE1667E5_5EDB_11D3_B413_A87510C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Card.h"

// klasa jednej wziêtej lewy
class CTrick  
{
public:
	const CCard* GetCard(short a_nCard) const;
	CTrick() ;
	virtual ~CTrick();

	void SetCard(const CCard* a_pCard);
	short GetCardsCnt() const;

	short AllPointsCnt() const;
	short CntInColor(T_COLOR a_enColor)	const;
	const CCard* Biggest(T_COLOR a_enTrumps = E_CC_NOTHING)	const;

	T_PLAYER SetTrickOwner(T_COLOR a_enTrumps);
	T_PLAYER GetTrickOwner() const;

	short GetHeartsCnt() const;
	short GetBoysCnt() const;
	short GetLadiesCnt() const;

	BOOL IsKingOfHeart() const;
	BOOL IsCard(short a_nCardNr) const;
	BOOL IsColor(T_COLOR a_enColor) const;
	BOOL InOneColor() const;

	T_PLAYER GetCardOwner(short a_nCard) const;
	short GetCardNr(short a_nCard) const;
	T_COLOR GetCardColor(short a_nCard) const;
	void Clear();

private:
	short m_nCards;
	const CCard* (m_tcCard[4]) ;
	T_PLAYER m_enOwner;
};

#endif // !defined(AFX_TRICK_H__CE1667E5_5EDB_11D3_B413_A87510C10000__INCLUDED_)
