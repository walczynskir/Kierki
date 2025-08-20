// Scores.cpp: implementation of the CScores class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Kierki.h"
#include "Scores.h"


// ---------------------------------------------------------
//	Konstruktor
//
CScores::CScores()
{
	Clear();
}


// ---------------------------------------------------------
//	Destruktor
//
CScores::~CScores()
{

}


// ---------------------------------------------------------
// Wyczyszczenie wyników
//
void CScores::Clear()
{
	short l_nAt;
	for (l_nAt = 0; l_nAt < 4; l_nAt++)
	{
		m_arScore[l_nAt].Clear();
	}
}
