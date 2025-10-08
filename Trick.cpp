// Trick.cpp: implementation of the CTrick class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Trick.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrick::CTrick()
{
	Clear();
}

CTrick::~CTrick()
{

}

// ---------------------------------------------------------
// Ustawienie karty
//
void 
CTrick::SetCard(
	const CCard* a_pCard //WE karta
	) 
{
	ASSERT(m_nCards >= 0);
	ASSERT(m_nCards < 4);

	m_tcCard[m_nCards] = a_pCard ;
	m_nCards++;
}


// ---------------------------------------------------------
// Ustalenie w�a�ciciela lewy
//
T_PLAYER 
CTrick::SetTrickOwner(
	T_COLOR a_enTrumps	//WE kolor atu
	)
{
	ASSERT(m_nCards == 4) ;

	const CCard* l_ptcFirst = m_tcCard[0] ;
	const CCard* l_ptcBiggest = l_ptcFirst;

	short l_iAt ;
	for (l_iAt = 2; l_iAt <= 4; l_iAt++)
	{
		if ((*(m_tcCard[l_iAt - 1])) > (*l_ptcBiggest))
			l_ptcBiggest = m_tcCard[l_iAt - 1] ;
		if (a_enTrumps == E_CC_NOTHING)
			continue;
		if (l_ptcBiggest->GetColor() == a_enTrumps)
			continue;
		if (m_tcCard[l_iAt - 1]->GetColor() == a_enTrumps)
			l_ptcBiggest = m_tcCard[l_iAt - 1] ;
	}

	m_enOwner = l_ptcBiggest->GetOwner();
	return m_enOwner ;
}


// ---------------------------------------------------------
// Zwraca w�a�ciciela lewy
//
T_PLAYER	//WY w�a�ciciel lewy
CTrick::GetTrickOwner() const
{
	return m_enOwner;
}


// ---------------------------------------------------------
//	Najwi�ksza karta w lewie w kolorze pierwszej karty
//	Je�li a_enTrumps != E_CC_NOTHING uwzgl�dnia te� atu
//
const CCard*	//WY szukana karta
CTrick::Biggest(
	T_COLOR a_enTrumps	//WE ewent. kolor atutuowy
	)	const
{
// TODO Czy faktycznie musz� by� przechowywane wska�niki a nie referencje?
	const CCard* l_pCard ;
	short l_nCard ;

	l_pCard = m_tcCard[0]; // pierwsza najwy�sza
	for (l_nCard = 2; l_nCard <= m_nCards; l_nCard++)
	{
		// je�li jest wy�sza od poprzedniej wy�szej to nie ma si� co zastanawia�
		if ((*(m_tcCard[l_nCard - 1])) > *l_pCard)
			l_pCard = m_tcCard[l_nCard - 1];
		
		if (a_enTrumps == E_CC_NOTHING)
			continue;
		// je�eli karta jest atutem, a nast�pna nie jest wy�sza no to ok
		if (l_pCard->GetColor() == a_enTrumps)
			continue;
		// ale je�li poprzednia nie jest atutem, a nasza jest
		// to nasza jest wy�sza
		if (m_tcCard[l_nCard - 1]->GetColor() == a_enTrumps)
			l_pCard = m_tcCard[l_nCard - 1];
	}

	return l_pCard ;
}


// ---------------------------------------------------------
//	Ilo�� kier�w w lewie
//
short	//WY ilo�� kier�w w lewie
CTrick::GetHeartsCnt() const
{
	short l_iAt ;
	const CCard* l_pCard ;
	short l_nCnt = 0 ;
	for (l_iAt = 1; l_iAt <= m_nCards; l_iAt++)
	{
		l_pCard = m_tcCard[l_iAt - 1] ;
		if (l_pCard->GetColor() == E_CC_HEART)
			l_nCnt++ ;

	}
	return l_nCnt ;
}


// ---------------------------------------------------------
//	Ile w lewie kart o podanym kolorze
//
short	//WY ilo�c kart w podanym kolorze w lewie
CTrick::CntInColor(
	T_COLOR a_enColor	//WE kolor
	)	const
{
	short l_nCard;
	short l_nCntInColor = 0;
	for(l_nCard = 1; l_nCard <= m_nCards; l_nCard++)
	{
		if (m_tcCard[l_nCard - 1]->GetColor() == a_enColor)
			l_nCntInColor++;
	}
	return l_nCntInColor;
}


// ---------------------------------------------------------
//	Czy w lewie jest karta o podanym numerze
//
BOOL	//WY TRUE - jest
CTrick::IsCard(
	short a_nCardNr		//WE szukany numer
	)	const
{
	short l_iCard;
	for (l_iCard = 1; l_iCard <= m_nCards; l_iCard++)
	{
		if (m_tcCard[l_iCard - 1]->GetNr() == a_nCardNr)
			return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------
//	Ile wzi�tych ch�opc�w
//
short	//WY ilo�� ch�opc�w
CTrick::GetBoysCnt() const
{
	short l_iAt ;
	const CCard* l_pCard ;
	short l_nCnt = 0 ;
	for (l_iAt = 1; l_iAt <= m_nCards; l_iAt++)
	{
		l_pCard = m_tcCard[l_iAt - 1] ;
		if ((l_pCard->CardValue() == E_CV_J) || (l_pCard->CardValue() == E_CV_K))
			l_nCnt++ ;

	}
	return l_nCnt ;
}


// ---------------------------------------------------------
//	Ile wzi�tych dam
//
short	//WY ilo�c dam
CTrick::GetLadiesCnt() const 
{
	short l_iAt ;
	const CCard* l_pCard ;
	short l_nCnt = 0 ;
	for (l_iAt = 1; l_iAt <= m_nCards; l_iAt++)
	{
		l_pCard = m_tcCard[l_iAt - 1] ;
		if (l_pCard->CardValue() == E_CV_D)
			l_nCnt++ ;

	}
	return l_nCnt ;
}


// ---------------------------------------------------------
//	Czy jest kr�l kier w lewie
//
BOOL	//WY TRUE - jest
CTrick::IsKingOfHeart() const
{
	short l_iAt ;
	const CCard* l_pCard ;
	for (l_iAt = 1; l_iAt <= m_nCards; l_iAt++)
	{
		l_pCard = m_tcCard[l_iAt - 1] ;
		if ((l_pCard->CardValue() == E_CV_K) && (l_pCard->GetColor() == E_CC_HEART))
			return TRUE;

	}
	return FALSE;
}


// ---------------------------------------------------------
//	Ile jest punkt�w w lewie (liczenie dla rozb�jnika)
//	- bez sz�stej i ostatniej
//
short	//WY ilo�� punkt�w
CTrick::AllPointsCnt() const
{
	short l_nPoints = 2; // za lew� na pocz�tek
	l_nPoints += GetHeartsCnt() * 2;
	l_nPoints += GetBoysCnt() * 3;
	l_nPoints += GetLadiesCnt() * 5;
	if (IsKingOfHeart())
		l_nPoints += 16;
	return l_nPoints;
}


// ---------------------------------------------------------
//	Czy lewa w jednym kolorze
//
BOOL	//WY TRUE - lewa w jednym kolorze
CTrick::InOneColor()	const
{
	short l_nCard;

	T_COLOR l_enColor = m_tcCard[0]->GetColor();
	for (l_nCard = 2; l_nCard <= m_nCards; l_nCard++)
	{
		if (m_tcCard[l_nCard - 1]->GetColor() != l_enColor)
			return FALSE;
	}
	return TRUE;
}


// ---------------------------------------------------------
//	Czy jest karta w podanym kolorze
//
BOOL	//WY TRUE - jest karta 
CTrick::IsColor(
	T_COLOR a_enColor	//WE sprawdzany kolor
	) const
{
	short l_nCard;

	for (l_nCard = 1; l_nCard <= m_nCards; l_nCard++)
	{
		if (m_tcCard[l_nCard - 1]->GetColor() == a_enColor)
			return TRUE;
	}
	return FALSE;
	
}


// ---------------------------------------------------------
// Zwraca ilo�� kart w lewie
//
short	//WY ilos� kart w lewie
CTrick::GetCardsCnt() const
{
	return m_nCards;
}


// ---------------------------------------------------------
// Zwraca kolor podanej karty 
//
T_COLOR		//WY kolor karty
CTrick::GetCardColor(
	short a_nCard	//WE kt�ra karta
	) const
{
	ASSERT(a_nCard >= 0);
	ASSERT(a_nCard < m_nCards);
	return m_tcCard[a_nCard]->GetColor();
}


// ---------------------------------------------------------
// Zwraca w�a�ciciela podanej karty 
//
T_PLAYER	//WY w�a�ciciel
CTrick::GetCardOwner(
	short a_nCard	//WE kt�ra karta
	) const
{
	return m_tcCard[a_nCard]->GetOwner();
}


// ---------------------------------------------------------
// Zwraca w�a�ciciela podanej karty 
//
short	//WY w�a�ciciel
CTrick::GetCardNr(
	short a_nCard	//WE kt�ra karta
	) const
{
	return m_tcCard[a_nCard]->GetNr();
}


// ---------------------------------------------------------
// Zwraca kart�
//
const CCard*	//WY karta
CTrick::GetCard(
	short a_nCard	//WE kt�ra karta
	) const
{
	return m_tcCard[a_nCard];
}


//  ---------------------------------------------------------
//	Clears trick
//
void
CTrick::Clear()
{
	m_nCards = 0 ;
	m_enOwner = E_DL_NULL;
	memset(m_tcCard, 0, ArraySize(m_tcCard));
}
