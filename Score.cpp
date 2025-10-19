// Score.cpp: implementation of the CScore class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Kierki.h"
#include "Score.h"
#include <algorithm> // for std::fill
#include <ranges>    // optional, for std::ranges::fill

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
//	Konstruktor
//
CScore::CScore()
{
	Clear();
}


// ---------------------------------------------------------
//	Destruktor
//
CScore::~CScore()
{

}


// ---------------------------------------------------------
//	Wyczyszczenie wyników
//
void 
CScore::Clear()
{
	std::ranges::fill(m_arScore, 0); // if you prefer ranges
}


// ---------------------------------------------------------
//	Wyliczenie wyniku na ujemnych grach
//	(od nie braæ lew do rozbójnika)
//
short	//WY wynik
CScore::SumMinuses(
	BOOL a_bRobber	//WE TRUE: uwzglêdniæ rozbójnik
	)	const
{
	short l_iAt;
	short l_nScore = 0;
	short l_nCnt = 5;
	if (a_bRobber)
		l_nCnt++;
	for (l_iAt = 0; l_iAt <= l_nCnt; l_iAt++)
		l_nScore += m_arScore[l_iAt];
	return l_nScore;
}


// ---------------------------------------------------------
//	Wyliczenie wyniku na dodatnich grach
//	(odgrywka)
//
short	//WY wynik
CScore::SumPluses() const
{
	short l_iAt;
	short l_nScore = 0;
	for (l_iAt = 7; l_iAt <= 10; l_iAt++)
		l_nScore += m_arScore[l_iAt];	
	return l_nScore;
}


// ---------------------------------------------------------
//	Wyliczenie wyniku
//
short	//WY wynik
CScore::Sum()	const
{
	return SumPluses() - SumMinuses(TRUE);
}


// ---------------------------------------------------------
//	Dodanie wartoœci do wyniku
//
void 
CScore::AddScore(
	T_GAMES a_enGame,	//WE gra
	short a_nScore		//WE wynik
	)
{
	m_arScore[a_enGame] += a_nScore;
}


// ---------------------------------------------------------
//	Ustawienie wyniku
//
void 
CScore::SetScore(
	T_GAMES a_enGame,	//WE gra
	short a_nScore		//WE wynik
	)
{
	m_arScore[a_enGame] = a_nScore;
}


// ---------------------------------------------------------
//	Pobranie wyniku
//
short	//WY wynik
CScore::GetScore(
	T_GAMES a_enGame	//WE gra
	)	const
{
	return m_arScore[a_enGame];
}


// ---------------------------------------------------------
//	Pobranie wyniku
//
short	//WY wynik
CScore::GetScore(
	short a_iGame	//WE gra
	) const
{
	return m_arScore[a_iGame];
}


// ---------------------------------------------------------
//	Wyliczenie wyniku po wszystkich grach
//
short	//WY wynik
CScore::SumAll() const
{
	return Sum() + m_arScore[E_GM_PUZZLE];
}


//	---------------------------------------------------------
//	Saves object to archive
//
void CScore::FillScore(SCORE& a_score) const
{
	for (int l_iGame = 0; l_iGame < ArraySize(m_arScore); l_iGame++)
		a_score.m_arScore[l_iGame] = m_arScore[l_iGame];
}


//	---------------------------------------------------------
//	Loads object from archive
//
void CScore::RestoreScore(const SCORE& a_score)
{
	for (int l_iGame = 0; l_iGame < ArraySize(m_arScore); l_iGame++)
		m_arScore[l_iGame] = a_score.m_arScore[l_iGame];
}
