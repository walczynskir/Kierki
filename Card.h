// Card.h: interface for the CCard class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "TypeDefs.h"

class CCard  
{
public:
	CCard();
	CCard(T_SUIT a_enColor, T_RANK a_cv);
	virtual ~CCard();

	BOOL operator>(const CCard& a_Card)	const;
	BOOL operator<(const CCard& a_Card)	const;
	CCard& operator=(const CCard& a_Card) = default;

	Suit GetColor() const;
	Suit GetSuit() const;

	Rank CardValue() const;
	Rank GetRank() const;

	T_PLAYER GetOwner() const;
	void SetUsed(BOOL a_bUsed);
	BOOL IsUsed() const;
	short BiggestInColor()	const;
	short LeastInColor()	const;
	BOOL IsBiggestInColor()	const;
	BOOL Bigger(const CCard* a_pCard)	const;	// czy karta wy¿sza
	BOOL Bigger(short a_nCardNr)	const;	// czy karta wy¿sza
	BOOL Less(const CCard* a_pCard)	const;		// czy karta ni¿sza
	BOOL Less(short a_nCardNr)	const;
	BOOL EqualColor(const CCard* a_pCard) const;
	BOOL EqualColor(short a_nCardNr) const;

	void SetCardNr(short a_nCardNr) ;
	void SetCard(short a_nCardNr, T_PLAYER a_enOwner, BOOL a_bUsed = FALSE) ;
	short GetNr() const;


private:
	BOOL	m_bUsed ;	// czy u¿yta
	short	m_nCardNr;	// numer karty
	T_PLAYER m_enOwner;

};

