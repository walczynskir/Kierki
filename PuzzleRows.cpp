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
//	Po³o¿enie karty. Sprawdza czy mozna po³o¿yæ
//	W razie b³êdu wywa³ka (na razie - potem wyj¹tki)
//
void 
CPuzzleRows::PutCard(
	const CCard* a_pCard	//WE k³adziona karta
	)
{
	T_CARDVAL l_val = a_pCard->CardValue();
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
//	Wynulowanie wskaŸników
//
void 
CPuzzleRows::CPuzzleRow::Clear()
{
	m_pCardTop = m_pCardBottom = NULL;
}


// ---------------------------------------------------------
//	Czy mozna po³o¿yæ dan¹ kartê
//
BOOL	//WY TRUE - mo¿na
CPuzzleRows::CanPutCard(
	const CCard& a_card	//WE karta sprawdzana
	)	const
{
	return m_arPuzzleRow[a_card.GetColor() - 1].CanPutCard(a_card);
}


// ------------------------------------------------------------
//	Czy mo¿na po³o¿yæ dan¹ kartê
//
BOOL	//WY TRUE - mo¿na p[o³o¿yæ
CPuzzleRows::CPuzzleRow::CanPutCard(
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
// Zwraca najwy¿sz¹ kartê w danym kolorze ju¿ po³o¿on¹
//
const CCard*	//WE najwy¿sza po³o¿ona karta
CPuzzleRows::GetTopCard(
	T_COLOR a_enColor	//WE kolor
	) const
{
	return m_arPuzzleRow[a_enColor - 1].m_pCardTop;
}


// ------------------------------------------------------------
// Zwraca najni¿sz¹ kartê w danym kolorze ju¿ po³o¿on¹
//
const CCard*	//WE najni¿sza po³o¿ona karta
CPuzzleRows::GetBottomCard(
	T_COLOR a_enColor	//WE kolor
	) const
{
	return m_arPuzzleRow[a_enColor - 1].m_pCardBottom;
}


// ------------------------------------------------------------
// Zwraca iloœæ rzuconych na stó³ kart
//
short	//WY iloœæ wyrzuconych kart
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
// Zwraca iloœæ rzuconych na stó³ kart
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
