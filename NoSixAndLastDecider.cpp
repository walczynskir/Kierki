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
	const CTakenTricks* a_pTricks	  //WE wzi�te lewy
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
//	Daj kart�
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
//	Zagraj pierwsz� kart�
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

	// tu trzeba zagra� pierwsz� mniejsz� od najwy�szej, 
	// lub najnizsz�
	if (a_iTrick == 6)
		return Next6Card(a_iTrick);

	// ani ostatnia ani 6 - zagraj najwy�sz� w kolorze
	// lub najwy�sz� w najkr�tszym kolorze, kt�ry jeszcze 
	// inni maj�
	if (!m_pCards->HasColor(l_enColor))
		return BestCard(a_iTrick);

	return m_pCards->Biggest(l_enColor);
}


// ---------------------------------------------------------
//	Zdecyduj w jakim kolorze wyj��
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

	// je�li to lewa sz�sta to wybierz kart� najmniejsz�
	// z wszystkich kt�re masz (i w kt�rych inni maja jeszcze wy�sze karty)
	if (a_iTrick == 6)
		return Decide6Color(a_iTrick);

	if (a_iTrick == 5)
		return Decide5Color(a_iTrick);

	// graj kolor w kt�rym masz najwy�sz� pierwsz� kart�,
	// i inni te� maj� ten kolor
	short l_nRank;
	for (l_nRank = 1; l_nRank < 4; l_nRank++)
	{
		l_enColor = m_pCards->GetBiggestFirstCardColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		// je�eli inni nie maj� kart w tym kolorze
		// to nie graj 
		if ((m_pTricks->LeftInColor(l_enColor)) <= (m_pCards->CardsInColor(l_enColor)))
			continue;
		// wszystkie waruny spe�nione - graj w tym kolorze
		return l_enColor;
	}

	//	graj pierwszy lepszy kolor
	for (l_nRank = 1; l_nRank < 4; l_nRank++)
	{
		l_enColor = m_pCards->GetBiggestFirstCardColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;

		// wszystkie waruny spe�nione - graj w tym kolorze
		return l_enColor;
	}

	// czyzby co� nie gra�o?
	ASSERT(FALSE);
	return l_enColor;
}


// ---------------------------------------------------------
// Decyduj w jakim kolorze wyj�� przy 6 lewie
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

	// nie ma mo�liwo�ci zagrania dobrej karty - zagraj najkr�tszy kolor
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetLowestCardColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		return l_enColor; 
	}

	// co� nie gra - nie ma �adnych kart?
	ASSERT(FALSE); 
	return l_enColor;
}


// ---------------------------------------------------------
//	Zdecyduj w jakim kolorze wyj�� przy 5 lewie
//	(Wychodz� w kart� w kt�rej na pewno wezm� lew� lub
//	w kart� kt�r� mam tylko jed� w kolorze, inni maj� 
//	jeszcze ten kolor i jest wy�sza od 3)
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

	// nie ma mo�liwo�ci zagrania karty kt�ra na pewno we�mie lew�
	// zagraj w kolorze najkr�tszym, najwy�sz� kart�
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		return l_enColor; 
	}

	// co� nie gra - nie ma �adnych kart?
	ASSERT(FALSE); 
	return l_enColor;
}


// ---------------------------------------------------------
//	Zagraj w kolorze w sz�stej lewie
//	tu trzeba zagra� pierwsz� mniejsz� od najwy�szej, 
//	lub najnizsz�
//
short		//WY karta wybrana
CNoSixAndLastDecider::Next6Card(
	short a_iTrick	//WE numer lewy
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;
	T_SUIT l_enColor = l_trickCurrent.GetCardColor(0);

	// je�li nie ma karty w podanym kolorze
	// to zagraj kart� w kolorze w kt�rym masz 
	// najwy�sz� pierwsz� kart� (je�li inni maja ten kolor)
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
//	Najlepsza karta jak� mog� zagra�
//
short	//WY karta
CNoSixAndLastDecider::BestCard(
	short a_iTrick	//WE kt�ra lewa
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

	// wygl�da, �e mam tylko karty kt�rych inni nie posiadaj�
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