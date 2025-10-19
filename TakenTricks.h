// TakenTricks.h: interface for the CTakenTricks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAKENTRICKS_H__B18A06C1_AB2C_11D3_A79A_9C7610C10000__INCLUDED_)
#define AFX_TAKENTRICKS_H__B18A06C1_AB2C_11D3_A79A_9C7610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Trick.h"

class CUserCards;

class CTakenTricks  
{
public:
	void SetCard(const CCard& a_card);
	void NextTrick();
	const CTrick& GetCurrentTrick() const;
	CTakenTricks();
	virtual ~CTakenTricks();

	void Clear();

	short GetTricksCnt() const;
	BOOL TrumpsInLastTrickInColor(T_SUIT a_enColor, T_SUIT a_enTrumps) const;
	BOOL FullTricksInColor(T_SUIT a_enColor) const;

	short TakenHearts() const;
	short TakenBoys() const;
	short TakenLadies() const;
	short TakenInColor(T_SUIT a_enColor) const;
	short TricksInColor(T_SUIT a_enColor) const;
	T_SUIT LeftInColorRank(short a_nRank) const;
	short BiggerInColorCnt(const CCard* a_pCard) const;
	BOOL AreBiggerInColor(const CCard* a_pCard, const CUserCards* a_pCards) const;
	BOOL AreLessInColor(const CCard* a_pCard, const CUserCards* a_pCards) const;
	short LeftInColor(T_SUIT a_enColor) const;
	const CTrick& operator[](short a_nTrick) const;

	T_PLAYER SetLastTrickOwner(T_SUIT a_colorTrumps);

private:
	CTrick m_tricks[13]{};
	short m_nTrickCnt = 0;
};

#endif // !defined(AFX_TAKENTRICKS_H__B18A06C1_AB2C_11D3_A79A_9C7610C10000__INCLUDED_)
