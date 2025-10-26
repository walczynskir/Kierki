// PuzzleRows.cpp: implementation of the CPuzzleRowSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PuzzleRowSet.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPuzzleRowSet::CPuzzleRowSet()
{

}

CPuzzleRowSet::~CPuzzleRowSet()
{

}


// ---------------------------------------------------------
//	Wyczyszczenie wiersza
//
void 
CPuzzleRowSet::Clear()
{
	for (Suit l_suit : ConstStandardSuits)
	{
		(*this)[l_suit].Clear();
	}
}


// ---------------------------------------------------------
//	Po�o�enie karty. Sprawdza czy mozna po�o�y�
//	W razie b��du wywa�ka (na razie - potem wyj�tki)
//
void 
CPuzzleRowSet::PutCard(
	const CCard* a_pCard	//WE k�adziona karta
	)
{
	T_CARDVAL l_val = a_pCard->CardValue();
	if (l_val == E_CV_8)
	{
		ASSERT((*this)[a_pCard->GetSuit()].m_pCardTop == NULL);
		ASSERT((*this)[a_pCard->GetSuit()].m_pCardBottom == NULL);
		(*this)[a_pCard->GetSuit()].m_pCardTop = a_pCard;
		(*this)[a_pCard->GetSuit()].m_pCardBottom = a_pCard;
	}
	else if (l_val < E_CV_8)
	{
		ASSERT((*this)[a_pCard->GetSuit()].m_pCardBottom != NULL);
		ASSERT((*this)[a_pCard->GetSuit()].m_pCardBottom->CardValue() - 1 == l_val);
		(*this)[a_pCard->GetSuit()].m_pCardBottom = a_pCard;
	}
	else if (l_val > E_CV_8)
	{
		ASSERT((*this)[a_pCard->GetSuit()].m_pCardTop != NULL);
		ASSERT((*this)[a_pCard->GetSuit()].m_pCardTop->CardValue() + 1 == l_val);
		(*this)[a_pCard->GetSuit()].m_pCardTop = a_pCard;
	}
	else
		ASSERT(FALSE);	// co to?

}


// ---------------------------------------------------------
//	Wynulowanie wska�nik�w
//
void 
CPuzzleRowSet::CPuzzleRow::Clear()
{
	m_pCardTop = m_pCardBottom = NULL;
}


// ---------------------------------------------------------
//	Czy mozna po�o�y� dan� kart�
//
BOOL	//WY TRUE - mo�na
CPuzzleRowSet::CanPutCard(
	const CCard& a_card	//WE karta sprawdzana
	)	const
{
	return (*this)[a_card.GetSuit()].CanPutCard(a_card);
}


// ------------------------------------------------------------
//	Czy mo�na po�o�y� dan� kart�
//
BOOL	//WY TRUE - mo�na p[o�o�y�
CPuzzleRowSet::CPuzzleRow::CanPutCard(
	const CCard& a_card	//WE sprawdzana karta
	) const
{
	T_CARDVAL l_val = a_card.CardValue();

	if ((m_pCardTop == NULL) && (m_pCardBottom == NULL))
		return (l_val == E_CV_8);
	if (l_val == E_CV_8)
		return ((m_pCardTop == NULL) && (m_pCardBottom == NULL));
	else if (l_val > E_CV_8)
		return (m_pCardTop->CardValue() + 1 == l_val);
	else if (l_val < E_CV_8)
		return (m_pCardBottom->CardValue() - 1 == l_val);
	ASSERT(FALSE);
	return FALSE;
}

// ------------------------------------------------------------
// Zwraca najwy�sz� kart� w danym kolorze ju� po�o�on�
//
const CCard*	//WE najwy�sza po�o�ona karta
CPuzzleRowSet::GetTopCard(
	Suit a_suit	//WE kolor
	) const
{
	return (*this)[a_suit].m_pCardTop;
}


// ------------------------------------------------------------
// Zwraca najni�sz� kart� w danym kolorze ju� po�o�on�
//
const CCard*	//WE najni�sza po�o�ona karta
CPuzzleRowSet::GetBottomCard(Suit a_suit) const
{
	return (*this)[a_suit].m_pCardBottom;
}


// ------------------------------------------------------------
// Zwraca ilo�� rzuconych na st� kart
//
short	//WY ilo�� wyrzuconych kart
CPuzzleRowSet::GetCardsCnt() const
{
	short l_iAt;
	short l_nCnt = 0;
	for (l_iAt = 0; l_iAt < 4; l_iAt++)
	{
		l_nCnt = l_nCnt + m_arPuzzleRow[l_iAt].GetCardsCnt();
	}
	return l_nCnt;
}


// ------------------------------------------------------------
// Zwraca ilo�� rzuconych na st� kart
//
short 
CPuzzleRowSet::CPuzzleRow::GetCardsCnt() const
{
	short l_nCnt;
	if (m_pCardTop == NULL)
	{
		return 0;
	}
	if  (
		m_pCardTop->CardValue() == E_CV_8 &&
		m_pCardBottom->CardValue() == E_CV_8
		)
	{
		return 1;
	}

	l_nCnt = (E_CV_8 - m_pCardBottom->CardValue()) + 
		(m_pCardTop->CardValue() - E_CV_8) + 1;
	return l_nCnt;
}
