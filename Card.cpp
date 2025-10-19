// Card.cpp: implementation of the CCard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Card.h"
#include <rcommon/ROwnExc.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCard::CCard()
{
	m_bUsed = FALSE ; 
	m_nCardNr = 0 ;
}


CCard::CCard(T_SUIT a_enColor, T_RANK a_cv)
{
	ASSERT((a_cv >= E_CV_2) && (a_cv <= E_CV_A));
	m_bUsed = FALSE;
	m_nCardNr = (short(a_enColor) - 1) * 13 + short (a_cv);
}


CCard::~CCard()
{

}


// ---------------------------------------------------------
//	Czy karta jest wiêksza od podanej
//
BOOL	//WY TRUE - jest wiêksza
CCard::Bigger(
	const CCard* a_pCard	//WE porównywana karta
	)	const
{
	return Bigger(a_pCard->m_nCardNr) ;
}


// ---------------------------------------------------------
//	Czy karta jest wiêksza od podanej
//
BOOL	//WY TRUE - jest wiêksza
CCard::Bigger(
	short a_nCardNr		//WE numer kart
	)	const
{
	
	if (!EqualColor(a_nCardNr))
		return FALSE ;
	return (a_nCardNr < m_nCardNr) ;
}


// ---------------------------------------------------------
//	Czy karta jest mniejsza
//
BOOL	//WY TRUE - jest mniejsza
CCard::Less(
	const CCard* a_pCard	//WE sprawdzana karta
	)	const
{
	return Less(a_pCard->m_nCardNr) ;
}


// ---------------------------------------------------------
//	Czy karta jest mniejsza
//
BOOL	//WY TRUE - jest mniejsza
CCard::Less(
	short a_nCardNr		//WE numer karty
	)	const
{
	
	if (!EqualColor(a_nCardNr))
		return FALSE ;
	return (a_nCardNr > m_nCardNr) ;
}


// ---------------------------------------------------------
//	Czy karty s¹ w tym samym kolorze
//
BOOL	//WY TRUE - w tym samym kolorze
CCard::EqualColor(
	const CCard* a_pCard	//WE porównywana karta
	)	const
{
	return EqualColor(a_pCard->m_nCardNr) ;
}


// ---------------------------------------------------------
//	Czy karty s¹ w tym samym kolorze
//
BOOL	//WY TRUE - w tym samym kolorze
CCard::EqualColor(
	short a_nCardNr	//WE numer karty
	)	const
{
	if ((m_nCardNr >= 1) && (m_nCardNr <= 13))
		return (a_nCardNr >= 1) && (a_nCardNr <= 13) ;
	if ((m_nCardNr >= 14) && (m_nCardNr <= 26))
		return (a_nCardNr >= 14) && (a_nCardNr <= 26) ;
	if ((m_nCardNr >= 27) && (m_nCardNr <= 39))
		return (a_nCardNr >= 27) && (a_nCardNr <= 39) ;
	if ((m_nCardNr >= 40) && (m_nCardNr <= 52))
		return (a_nCardNr >= 40) && (a_nCardNr <= 52) ;
	ASSERT(FALSE) ; //jakiœ babol wielki
	return FALSE ;
}


// ---------------------------------------------------------
//	Sprawdzenie czy karta jest wiêksza od podanej
//
BOOL	//WY TRUE - wy¿sza
CCard::operator>(
	const CCard &a_Card		//WE porównywana karta
	)	const
{
	return Bigger(a_Card.m_nCardNr) ;
}


// ---------------------------------------------------------
//	Sprawdzenie czy karta jest mniejsza od podanej
//
BOOL	//WY TRUE - jest mniejsza
CCard::operator<(
	const CCard &a_Card	//WE porównywana karta
	)	const
{
	return Less(a_Card.m_nCardNr) ;
}


// ---------------------------------------------------------
// sets card nr
//
void CCard::SetCardNr(short a_nCardNr) 
{ 
	ASSERT((a_nCardNr >= 1) && (a_nCardNr <= 52)) ;	
	m_nCardNr = a_nCardNr; 
}


// ---------------------------------------------------------
// sets card 
//
void 
CCard::SetCard(
	short a_nCardNr, 
	T_PLAYER a_enOwner, 
	BOOL a_bUsed /*= FALSE*/
	)
{
	ASSERT((a_nCardNr >= 1) && (a_nCardNr <= 52)) ;	// innej mo¿liwoœci nie ma
	m_nCardNr = a_nCardNr ; 
	m_enOwner = a_enOwner ;
	m_bUsed = a_bUsed ;
}


/*	---------------------------------------------------------------
	czy karta najwy¿sza w kolorze (to znaczy czy AS
*/
BOOL	//WY TRUE - najwy¿sza
CCard::IsBiggestInColor()	const
{
	if ((m_nCardNr == 13) || (m_nCardNr == 26) || 
		(m_nCardNr == 39) || (m_nCardNr == 52))
		return TRUE;
	return FALSE;
}


// ---------------------------------------------------------
//	Jaki jest numer karty najwy¿sza w moimm kolorze
//
short	//WY numer najwy¿szej karty
CCard::BiggestInColor()	const
{
	ASSERT((m_nCardNr >= 1) && (m_nCardNr <= 52));

	if ((m_nCardNr >= 1) && (m_nCardNr <= 13))
		return 13;
	if ((m_nCardNr >= 14) && (m_nCardNr <= 26))
		return 26;
	if ((m_nCardNr >= 27) && (m_nCardNr <= 39))
		return 39;
	return 52;
}


// ---------------------------------------------------------
//	Jaki jest numer karty najmniejszej w moim kolorze
//
short	//WY numer najni¿szej
CCard::LeastInColor()	const
{
	ASSERT((m_nCardNr >= 1) && (m_nCardNr <= 52));

	if ((m_nCardNr >= 1) && (m_nCardNr <= 13))
		return 1;
	if ((m_nCardNr >= 14) && (m_nCardNr <= 26))
		return 14;
	if ((m_nCardNr >= 27) && (m_nCardNr <= 39))
		return 27;
	return 40;
}






[[deprecated("Use CCard::GetRank() instead")]]
Rank CCard::CardValue() const
{
	return GetRank();
}


Rank CCard::GetRank() const
{
	if ((m_nCardNr < 1) || (m_nCardNr > 52))
	{
		ASSERT(FALSE); // co to za karta !!!
		return T_RANK(0);
	}

	return static_cast<Rank>((m_nCardNr - 1) % 13 + 1);
}

// ---------------------------------------------------------
// Zwraca kolor karty
//
Suit CCard::GetSuit() const
{
	if ((m_nCardNr < 1) || (m_nCardNr > 52))
	{
		ASSERT(FALSE); // Invalid card number
		return E_CC_NULL; // Fallback
	}

	static constexpr Suit suits[] = {
		E_CC_CLUB,     // 1–13
		E_CC_DIAMOND,  // 14–26
		E_CC_SPADE,    // 27–39
		E_CC_HEART     // 40–52
	};

	return suits[(m_nCardNr - 1) / 13];
}


[[deprecated("Use CCard::GetSuit() instead")]]
T_SUIT CCard::GetColor() const
{ 
	return GetSuit(); 
}


// ---------------------------------------------------------
// Zwraca numer karty
//
short		//WY kolor karty
CCard::GetNr() const
{
	return m_nCardNr;
}


// ---------------------------------------------------------
// Zwraca czy karta ju¿ u¿yta
//
BOOL	//WY TRUE - karta u¿yta
CCard::IsUsed() const
{
	return m_bUsed;
}


// ---------------------------------------------------------
// Ustawia karte na u¿yt¹
//
void 
CCard::SetUsed(
	BOOL a_bUsed	//WE TRUE - u¿yta
	)
{
	m_bUsed = a_bUsed;
}


// ---------------------------------------------------------
// Zwraca w³aœciciela karty.
// Atrybut u³atwia pobranie np. w³aœciciela lewy.
//
T_PLAYER	//WY w³aœciciel
CCard::GetOwner() const
{
	return m_enOwner;
}

