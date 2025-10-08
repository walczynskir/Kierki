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
// Ustalenie w³aœciciela lewy
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
// Zwraca w³aœciciela lewy
//
T_PLAYER	//WY w³aœciciel lewy
CTrick::GetTrickOwner() const
{
	return m_enOwner;
}


// ---------------------------------------------------------
//	Najwiêksza karta w lewie w kolorze pierwszej karty
//	Jeœli a_enTrumps != E_CC_NOTHING uwzglêdnia te¿ atu
//
const CCard*	//WY szukana karta
CTrick::Biggest(
	T_COLOR a_enTrumps	//WE ewent. kolor atutuowy
	)	const
{
// TODO Czy faktycznie musz¹ byæ przechowywane wskaŸniki a nie referencje?
	const CCard* l_pCard ;
	short l_nCard ;

	l_pCard = m_tcCard[0]; // pierwsza najwy¿sza
	for (l_nCard = 2; l_nCard <= m_nCards; l_nCard++)
	{
		// jeœli jest wy¿sza od poprzedniej wy¿szej to nie ma siê co zastanawiaæ
		if ((*(m_tcCard[l_nCard - 1])) > *l_pCard)
			l_pCard = m_tcCard[l_nCard - 1];
		
		if (a_enTrumps == E_CC_NOTHING)
			continue;
		// je¿eli karta jest atutem, a nastêpna nie jest wy¿sza no to ok
		if (l_pCard->GetColor() == a_enTrumps)
			continue;
		// ale jeœli poprzednia nie jest atutem, a nasza jest
		// to nasza jest wy¿sza
		if (m_tcCard[l_nCard - 1]->GetColor() == a_enTrumps)
			l_pCard = m_tcCard[l_nCard - 1];
	}

	return l_pCard ;
}


// ---------------------------------------------------------
//	Iloœæ kierów w lewie
//
short	//WY iloœæ kierów w lewie
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
short	//WY iloœc kart w podanym kolorze w lewie
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
//	Ile wziêtych ch³opców
//
short	//WY iloœæ ch³opców
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
//	Ile wziêtych dam
//
short	//WY iloœc dam
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
//	Czy jest król kier w lewie
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
//	Ile jest punktów w lewie (liczenie dla rozbójnika)
//	- bez szóstej i ostatniej
//
short	//WY iloœæ punktów
CTrick::AllPointsCnt() const
{
	short l_nPoints = 2; // za lewê na pocz¹tek
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
// Zwraca iloœæ kart w lewie
//
short	//WY ilosæ kart w lewie
CTrick::GetCardsCnt() const
{
	return m_nCards;
}


// ---------------------------------------------------------
// Zwraca kolor podanej karty 
//
T_COLOR		//WY kolor karty
CTrick::GetCardColor(
	short a_nCard	//WE która karta
	) const
{
	ASSERT(a_nCard >= 0);
	ASSERT(a_nCard < m_nCards);
	return m_tcCard[a_nCard]->GetColor();
}


// ---------------------------------------------------------
// Zwraca w³aœciciela podanej karty 
//
T_PLAYER	//WY w³aœciciel
CTrick::GetCardOwner(
	short a_nCard	//WE która karta
	) const
{
	return m_tcCard[a_nCard]->GetOwner();
}


// ---------------------------------------------------------
// Zwraca w³aœciciela podanej karty 
//
short	//WY w³aœciciel
CTrick::GetCardNr(
	short a_nCard	//WE która karta
	) const
{
	return m_tcCard[a_nCard]->GetNr();
}


// ---------------------------------------------------------
// Zwraca kartê
//
const CCard*	//WY karta
CTrick::GetCard(
	short a_nCard	//WE która karta
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
