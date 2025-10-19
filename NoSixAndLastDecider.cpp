// NoSixAndLastDecider.cpp: implementation of the CNoSixAndLastDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NoSixAndLastDecider.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// Konstruktor
//
CNoSixAndLastDecider::CNoSixAndLastDecider(
	const CUserCards*   a_pUserCards, //WE karty gracza 
	const CTakenTricks* a_pTricks	  //WE wziête lewy
	)
{
	m_pCards = a_pUserCards;
	m_pTricks = a_pTricks;
}


// ---------------------------------------------------------
// Destruktor
//
CNoSixAndLastDecider::~CNoSixAndLastDecider()
{

}


// ---------------------------------------------------------
//	Daj kartê
//
short	//WY numer karty
CNoSixAndLastDecider::GetCardNr(
	short a_iTrick	//WE numer lewy
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;

	short l_iCardNr ;
	if (l_trickCurrent.GetCardsCnt() == 0)
		l_iCardNr = FirstCard(a_iTrick) ;
	else
		l_iCardNr = NextCard(a_iTrick) ;
	ASSERT((l_iCardNr >= 0) && (l_iCardNr < 13)) ;
	return l_iCardNr;
}


// ---------------------------------------------------------
//	Zagraj pierwsz¹ kartê
//
short	//WY numer karty
CNoSixAndLastDecider::FirstCard(
	short a_iTrick	//WE numer lewy
	) const
{
	T_SUIT l_enColor = DecideColor(a_iTrick);
	if (a_iTrick == 6)
		return m_pCards->FirstInColor(l_enColor);

	return m_pCards->Biggest(l_enColor);
}


// ---------------------------------------------------------
//	Zagraj w kolorze
//
short	//WY numer karty
CNoSixAndLastDecider::NextCard(
	short a_iTrick	//WE numer lewy
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;
	T_SUIT l_enColor = l_trickCurrent.GetCardColor(0);

	// tu nie ma wyboru
	if (a_iTrick == 13)
		m_pCards->FirstInShortest();

	// tu trzeba zagraæ pierwsz¹ mniejsz¹ od najwy¿szej, 
	// lub najnizsz¹
	if (a_iTrick == 6)
		return Next6Card(a_iTrick);

	// ani ostatnia ani 6 - zagraj najwy¿sz¹ w kolorze
	// lub najwy¿sz¹ w najkrótszym kolorze, który jeszcze 
	// inni maj¹
	if (!m_pCards->HasColor(l_enColor))
		return BestCard(a_iTrick);

	return m_pCards->Biggest(l_enColor);
}


// ---------------------------------------------------------
//	Zdecyduj w jakim kolorze wyjœæ
//
T_SUIT	//WY wybrany kolor
CNoSixAndLastDecider::DecideColor(
	short a_iTrick	//WE numer lewy
	) const
{
	T_SUIT l_enColor;
	// przy ostatniej lewie i tak nie ma wyboru
	if (a_iTrick == 13)
		return m_pCards->ShortestColor();

	// jeœli to lewa szósta to wybierz kartê najmniejsz¹
	// z wszystkich które masz (i w których inni maja jeszcze wy¿sze karty)
	if (a_iTrick == 6)
		return Decide6Color(a_iTrick);

	if (a_iTrick == 5)
		return Decide5Color(a_iTrick);

	// graj kolor w którym masz najwy¿sz¹ pierwsz¹ kartê,
	// i inni te¿ maj¹ ten kolor
	short l_nRank;
	for (l_nRank = 1; l_nRank < 4; l_nRank++)
	{
		l_enColor = m_pCards->GetBiggestFirstCardColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		// je¿eli inni nie maj¹ kart w tym kolorze
		// to nie graj 
		if ((m_pTricks->LeftInColor(l_enColor)) <= (m_pCards->CardsInColor(l_enColor)))
			continue;
		// wszystkie waruny spe³nione - graj w tym kolorze
		return l_enColor;
	}

	//	graj pierwszy lepszy kolor
	for (l_nRank = 1; l_nRank < 4; l_nRank++)
	{
		l_enColor = m_pCards->GetBiggestFirstCardColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;

		// wszystkie waruny spe³nione - graj w tym kolorze
		return l_enColor;
	}

	// czyzby coœ nie gra³o?
	ASSERT(FALSE);
	return l_enColor;
}


// ---------------------------------------------------------
// Decyduj w jakim kolorze wyjœæ przy 6 lewie
//
T_SUIT	//WY wybrany kolor
CNoSixAndLastDecider::Decide6Color(
	short a_iTrick	//WE numer lewy
	) const
{
	T_SUIT l_enColor;

	short l_nRank;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetLowestCardColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
		{
			continue;
		}
		const CCard& l_card = m_pCards->GetCard(m_pCards->FirstInColor(l_enColor));
		if (m_pTricks->AreBiggerInColor(&l_card, m_pCards))
		{
			return l_enColor;
		}
	}

	// nie ma mo¿liwoœci zagrania dobrej karty - zagraj najkrótszy kolor
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetLowestCardColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		return l_enColor; 
	}

	// coœ nie gra - nie ma ¿adnych kart?
	ASSERT(FALSE); 
	return l_enColor;
}


// ---------------------------------------------------------
//	Zdecyduj w jakim kolorze wyjœæ przy 5 lewie
//	(Wychodzê w kartê w której na pewno wezmê lewê lub
//	w kartê któr¹ mam tylko jed¹ w kolorze, inni maj¹ 
//	jeszcze ten kolor i jest wy¿sza od 3)
//
T_SUIT		//WY wybrany kolor
CNoSixAndLastDecider::Decide5Color(
	short a_iTrick	//WE numer lewy
	) const
{
	T_SUIT l_enColor;

	short l_nRank;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetBiggestFirstCardColorRank(l_nRank);
		const CCard& l_card = m_pCards->GetCard(m_pCards->Biggest(l_enColor));
		if (m_pTricks->AreBiggerInColor(&l_card, m_pCards))
			continue;
		return l_enColor; 
	}

	// nie ma mo¿liwoœci zagrania karty która na pewno weŸmie lewê
	// zagraj w kolorze najkrótszym, najwy¿sz¹ kartê
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		return l_enColor; 
	}

	// coœ nie gra - nie ma ¿adnych kart?
	ASSERT(FALSE); 
	return l_enColor;
}


// ---------------------------------------------------------
//	Zagraj w kolorze w szóstej lewie
//	tu trzeba zagraæ pierwsz¹ mniejsz¹ od najwy¿szej, 
//	lub najnizsz¹
//
short		//WY karta wybrana
CNoSixAndLastDecider::Next6Card(
	short a_iTrick	//WE numer lewy
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;
	T_SUIT l_enColor = l_trickCurrent.GetCardColor(0);

	// jeœli nie ma karty w podanym kolorze
	// to zagraj kartê w kolorze w którym masz 
	// najwy¿sz¹ pierwsz¹ kartê (jeœli inni maja ten kolor)
	if (!m_pCards->HasColor(l_enColor))
		return BestCard(a_iTrick);

	T_RANK l_cvBiggest;

	l_cvBiggest = l_trickCurrent.Biggest()->CardValue();

	short l_iCard;
	l_iCard = m_pCards->GetBiggestBelow(l_enColor, l_cvBiggest);
	if (l_iCard > 0)
		return l_iCard;
	return m_pCards->FirstInColor(l_enColor);
}


// ---------------------------------------------------------
//	Najlepsza karta jak¹ mogê zagraæ
//
short	//WY karta
CNoSixAndLastDecider::BestCard(
	short a_iTrick	//WE która lewa
	) const
{
	short l_nRank;
	T_SUIT l_enColor;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetBiggestFirstCardColorRank(l_nRank);

		if (!m_pCards->HasColor(l_enColor))
				continue;
		if (LeftInColorOthers(l_enColor) == 0)
			continue;

		return m_pCards->Biggest(l_enColor);
	}

	// wygl¹da, ¿e mam tylko karty których inni nie posiadaj¹
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetBiggestFirstCardColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		return m_pCards->Biggest(l_enColor);
	}

	// babol!
	ASSERT(FALSE);
	return -1;
}