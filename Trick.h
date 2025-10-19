// Trick.h: interface for the CTrick class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

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
	short CntInColor(T_SUIT a_enColor)	const;
	const CCard* Biggest(T_SUIT a_enTrumps = E_CC_NULL)	const;

	T_PLAYER SetTrickOwner(T_SUIT a_enTrumps);
	T_PLAYER GetTrickOwner() const;

	short GetHeartsCnt() const;
	short GetBoysCnt() const;
	short GetLadiesCnt() const;

	BOOL IsKingOfHeart() const;
	BOOL IsCard(short a_nCardNr) const;
	BOOL IsColor(T_SUIT a_enColor) const;
	BOOL InOneColor() const;

	T_PLAYER GetCardOwner(short a_nCard) const;
	short GetCardNr(short a_nCard) const;
	T_SUIT GetCardColor(short a_nCard) const;
	void Clear();

private:
	short m_nCards;
	const CCard* (m_tcCard[4]) ;
	T_PLAYER m_enOwner;
};
