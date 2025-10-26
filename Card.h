// Card.h: interface for the CCard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARD_H__996169E4_5EEB_11D3_B413_A87510C10000__INCLUDED_)
#define AFX_CARD_H__996169E4_5EEB_11D3_B413_A87510C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "TypeDefs.h"

class CCard  
{
public:
	Suit GetSuit() const;
	T_PLAYER GetOwner() const;
	void SetUsed(BOOL a_bUsed);
	BOOL IsUsed() const;
	T_CARDVAL CardValue() const;
	short BiggestInColor()	const;
	short LeastInColor()	const;
	BOOL IsBiggestInColor()	const;
	CCard();
	CCard(T_COLOR a_enColor, T_CARDVAL a_cv);
	virtual ~CCard();
	BOOL Bigger(const CCard* a_pCard)	const;	// czy karta wy¿sza
	BOOL Bigger(short a_nCardNr)	const;	// czy karta wy¿sza
	BOOL Less(const CCard* a_pCard)	const;		// czy karta ni¿sza
	BOOL Less(short a_nCardNr)	const;
	BOOL EqualColor(const CCard* a_pCard) const;
	BOOL EqualColor(short a_nCardNr) const;
	BOOL operator>(const CCard &a_Card)	const;
	BOOL operator<(const CCard& a_Card)	const;
	BOOL operator==(const CCard& a_Card) const;
	CCard& operator=(const CCard& a_Card);

	void SetCardNr(short a_nCardNr) ;
	void SetCard(short a_nCardNr, T_PLAYER a_enOwner, BOOL a_bUsed = FALSE) ;
	short GetNr() const;

private:
	void SetColor();

private:
	BOOL	m_bUsed ;	// czy u¿yta
	T_COLOR	m_enColor ;
	short	m_nCardNr;	// numer karty
	T_PLAYER m_enOwner;

};

#endif // !defined(AFX_CARD_H__996169E4_5EEB_11D3_B413_A87510C10000__INCLUDED_)
