// PuzzleRows.cpp: implementation of the CPuzzleRows class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PuzzleRows.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPuzzleRows::CPuzzleRows()
{

}

CPuzzleRows::~CPuzzleRows()
{

}


// ---------------------------------------------------------
//	Wyczyszczenie wiersza
//
void 
CPuzzleRows::Clear()
{
	m_arPuzzleRow[E_CC_HEART - 1].Clear();
	m_arPuzzleRow[E_CC_SPADE - 1].Clear();
	m_arPuzzleRow[E_CC_DIAMOND - 1].Clear();
	m_arPuzzleRow[E_CC_CLUB - 1].Clear();
}


// ---------------------------------------------------------
//	Po�o�enie karty. Sprawdza czy mozna po�o�y�
//	W razie b��du wywa�ka (na razie - potem wyj�tki)
//
void 
CPuzzleRows::PutCard(
	const CCard* a_pCard	//WE k�adziona karta
	)
{
	T_RANK l_val = a_pCard->CardValue();
	if (l_val == E_CV_8)
	{
		ASSERT(m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardTop == NULL);
		ASSERT(m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardBottom == NULL);
		m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardTop = a_pCard;
		m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardBottom = a_pCard;
	}
	else if (l_val < E_CV_8)
	{
		ASSERT(m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardBottom != NULL);
		ASSERT(m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardBottom->CardValue() - 1 == l_val);
		m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardBottom = a_pCard;
	}
	else if (l_val > E_CV_8)
	{
		ASSERT(m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardTop != NULL);
		ASSERT(m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardTop->CardValue() + 1 == l_val);
		m_arPuzzleRow[a_pCard->GetColor() - 1].m_pCardTop = a_pCard;
	}
	else
		ASSERT(FALSE);	// co to?

}


// ---------------------------------------------------------
//	Wynulowanie wska�nik�w
//
void 
CPuzzleRows::CPuzzleRow::Clear()
{
	m_pCardTop = m_pCardBottom = NULL;
}


// ---------------------------------------------------------
//	Czy mozna po�o�y� dan� kart�
//
BOOL	//WY TRUE - mo�na
CPuzzleRows::CanPutCard(
	const CCard& a_card	//WE karta sprawdzana
	)	const
{
	return m_arPuzzleRow[a_card.GetColor() - 1].CanPutCard(a_card);
}


// ------------------------------------------------------------
//	Czy mo�na po�o�y� dan� kart�
//
BOOL	//WY TRUE - mo�na p[o�o�y�
CPuzzleRows::CPuzzleRow::CanPutCard(
	const CCard& a_card	//WE sprawdzana karta
	) const
{
	T_RANK l_val = a_card.CardValue();

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
CPuzzleRows::GetTopCard(
	T_SUIT a_enColor	//WE kolor
	) const
{
	return m_arPuzzleRow[a_enColor - 1].m_pCardTop;
}


// ------------------------------------------------------------
// Zwraca najni�sz� kart� w danym kolorze ju� po�o�on�
//
const CCard*	//WE najni�sza po�o�ona karta
CPuzzleRows::GetBottomCard(
	T_SUIT a_enColor	//WE kolor
	) const
{
	return m_arPuzzleRow[a_enColor - 1].m_pCardBottom;
}


// ------------------------------------------------------------
// Zwraca ilo�� rzuconych na st� kart
//
short	//WY ilo�� wyrzuconych kart
CPuzzleRows::GetCardsCnt() const
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
CPuzzleRows::CPuzzleRow::GetCardsCnt() const
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
