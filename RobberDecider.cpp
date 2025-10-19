// RobberDecider.cpp: implementation of the CRobberDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RobberDecider.h"


// ---------------------------------------------------------
//
//
CRobberDecider::CRobberDecider(
	const CUserCards* a_pUserCards, //WE karty
	const CTakenTricks*	a_pTricks	//WE lewy						//
	)
{
	m_pCards = a_pUserCards;	
	m_pTricks = a_pTricks;
}


// ---------------------------------------------------------
// Destruktor
//
CRobberDecider::~CRobberDecider()
{

}


// ---------------------------------------------------------
//	Numer karty z puli w r�ku do zagrania
//
short	//WY numer karty
CRobberDecider::GetCardNr(
	short a_iTrick	//WE numer lewy
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;

	short l_nCardNr ;
	if (l_trickCurrent.GetCardsCnt() == 0)
	{
		l_nCardNr =  FirstCard(a_iTrick);
#ifdef _DEBUG
		if ((*m_pCards)[l_nCardNr].GetColor() == E_CC_HEART)
		{
			ASSERT(!m_pCards->AreNoHearts());
		}
#endif
	}
	else
	{
		l_nCardNr = NextCard(a_iTrick);
	}

	ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
	ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
	return l_nCardNr;
}


// ---------------------------------------------------------
//	Jak� kart� zawistowa�
//
short	//WY numer karty
CRobberDecider::FirstCard(
	short a_iTrick	//WE numer lewy
	) const
{
	// ostatnia lewa - nie ma si� co zastanawia�
	if (a_iTrick == 13)
		return m_pCards->FirstInShortest();

	// do pi�tej lewy gramy zawsze w najkr�tszym kolorze
	if (a_iTrick <= 5)
		return FirstCardBefore6(a_iTrick);
	else if (a_iTrick == 6)
		return FirstCard6(a_iTrick);
	else
		return FirstCardAfter6(a_iTrick);

	return a_iTrick;
}


// ---------------------------------------------------------
//	Jak� kart� do�o�y� do wistu
//
short	//WY numer karty
CRobberDecider::NextCard(
	short a_iTrick	//WE numer lewy
	) const
{
	// ostatnia lewa - nie ma si� co zastanawia�
	if (a_iTrick == 13)
		return m_pCards->FirstInShortest();

	// rozr�nienie w kt�rej fazie gry jeste�my
	if (a_iTrick <= 5)
		return NextCardBefore6(a_iTrick);
	else if (a_iTrick == 6)
		return NextCard6(a_iTrick);
	else
		return NextCardAfter6(a_iTrick);

	return a_iTrick;
}


// ---------------------------------------------------------
//	Jak� taktyk� gry podejmujemy - 
//	1. Nie bierzemy nic (TC_GETNOTHING)
//	2. Bierzemy na pocz�tku by co� uratowa� (TC_GETATBEG)
//	3. ... jeszcze nie ma
//
void 
CRobberDecider::DecideTactic()
{
	short l_nHearts = m_pCards->ColorAllCnt(E_CC_HEART);
	short l_nSpades = m_pCards->ColorAllCnt(E_CC_SPADE);
	short l_nDiamonds = m_pCards->ColorAllCnt(E_CC_DIAMOND);
	short l_nClubs = m_pCards->ColorAllCnt(E_CC_CLUB);
	// je�li mniej kier�w ni� 4 to sytuacja mo�e by� ci�ka
	if (l_nHearts < 4)
	{
		DecideTacticFewHearts();
	}
}


// ---------------------------------------------------------
//	Jak� taktyk� gry podejmujemy gdy kier�w mniej ni� 4.
//
void 
CRobberDecider::DecideTacticFewHearts()
{
	short l_nSpades = m_pCards->ColorAllCnt(E_CC_SPADE);
	short l_nDiamonds = m_pCards->ColorAllCnt(E_CC_DIAMOND);
	short l_nClubs = m_pCards->ColorAllCnt(E_CC_CLUB);
	BOOL l_bSituationOK = TRUE;	// sprawdzamy sytuacj�

	// sprawd�my SPADE
	m_pCards->CardsBetween(CCard(E_CC_SPADE, E_CV_3), CCard(E_CC_SPADE, E_CV_4));
}


// ---------------------------------------------------------
//	Wist karty przed sz�st�
//
short	//WY numer karty
CRobberDecider::FirstCardBefore6(
	short a_iTrick	//WE numer lewy
	) const
{
	BYTE l_nRank;
	short l_nCardNr;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (this->LeftInColorOthers(l_enColor) == 0)
			continue;
		// mamy najkr�tszy kolor z posiadanych
		l_nCardNr = m_pCards->GetBiggestBelow(l_enColor, E_CV_J);
		if (l_nCardNr >= 0)
			return l_nCardNr;
		else
			return m_pCards->FirstInColor(l_enColor);
	}

	
	// ma�o to prawdopodobne, ale jesli tu dotrzemy 
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		// mamy najkr�tszy kolor z posiadanych, ale 
		// inni nie powinni mie� kart w tym kolorze
		ASSERT(this->LeftInColorOthers(l_enColor) == 0);
		return m_pCards->FirstInColor(l_enColor);
	}
	// skoro tu jeste�my, to znaczy �e ma same kiery (ale si� upewnijmy,
	// �e nie ma babola
	ASSERT(!m_pCards->HasNoHeartsColor());
	return m_pCards->FirstInShortest();
}


// ---------------------------------------------------------
//	Wist karty sz�stej
//
short	//WY numer karty
CRobberDecider::FirstCard6(
	short a_iTrick	//WE numer lewy
	) const
{
	short l_nRank;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (LeftInColorOthers(l_enColor) == 0)
			continue;
		// mamy najkr�tszy kolor z posiadanych
		// sprawd�my czy s� wy�sze w kolorze jeszcze
		const CCard& l_card = (*m_pCards)[m_pCards->FirstInColor(l_enColor)];
		if (!m_pTricks->AreBiggerInColor(&l_card, m_pCards))
			continue;
		// dobrze by by�a to karta poni�ej 5
		if (m_pCards->CardsBetween(CCard(l_enColor, E_CV_2), CCard(l_enColor, E_CV_4)) == 0)
			continue;
		// wszystkie warunki spe�nione - zwr�cmy najni�sz� w kolorze
		return m_pCards->FirstInColor(l_enColor);
	}

	// rezygnujemy z warunku, �e to karta poni�ej pi�tki
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (LeftInColorOthers(l_enColor) == 0)
			continue;
		// mamy najkr�tszy kolor z posiadanych
		// sprawd�my czy s� wy�sze w kolorze jeszcze
		const CCard& l_card = (*m_pCards)[m_pCards->FirstInColor(l_enColor)];
		if (!m_pTricks->AreBiggerInColor(&l_card, m_pCards))
			continue;
		// wszystkie warunki spe�nione - zwr�cmy najni�sz� w kolorze
		return m_pCards->FirstInColor(l_enColor);
	}

	// rezygnujemy z warunku, �e s� wy�sze w kolorze
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (LeftInColorOthers(l_enColor) == 0)
			continue;

		// wszystkie warunki spe�nione - zwr�cmy najni�sz� w kolorze
		return m_pCards->FirstInColor(l_enColor);
	}

	// rezygnujemy z warunku, �e s� w kolorze
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;

		// wszystkie warunki spe�nione - zwr�cmy najni�sz� w kolorze
		return m_pCards->FirstInColor(l_enColor);
	}
	
	// skoro tu jeste�my, to znaczy �e ma same kiery (ale si� upewnijmy,
	// �e nie ma babola
	ASSERT(!m_pCards->HasNoHeartsColor());

	return m_pCards->FirstInShortest();
}


// ---------------------------------------------------------
//	Wist karty po sz�stej
//
short	//WY numer karty
CRobberDecider::FirstCardAfter6(
	short a_iTrick	//WE numer lewy
	) const
{
	short l_nRank;
	short l_nCardNr;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (LeftInColorOthers(l_enColor) == 0)
			continue;
		// mamy najkr�tszy kolor z posiadanych
		switch (m_pTricks->TakenInColor(l_enColor))
		{
		case 0:
		case 1:
		case 2:
			l_nCardNr = m_pCards->GetBiggestBelow(l_enColor, E_CV_J);
			if (l_nCardNr >= 0)
				return l_nCardNr;
			else
				return m_pCards->FirstInColor(l_enColor);
		default:
			{
				const CCard& l_card = (*m_pCards)[m_pCards->FirstInColor(l_enColor)];
				if (!m_pTricks->AreBiggerInColor(&l_card, m_pCards))
					continue;
				return m_pCards->FirstInColor(l_enColor);
			}
		}
	}

	// je�li tu dotrzemy to sytuacja jest zapewne kiepska
	// ale znajd�my jeszcze co�
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (LeftInColorOthers(l_enColor) == 0)
			continue;
		const CCard* l_pCard = m_pCards->FirstInColorCard(l_enColor);

		if (!m_pTricks->AreBiggerInColor(l_pCard, m_pCards))
			continue;
		// zagrajmy t� kart�
		return m_pCards->FirstInColor(l_enColor);
	}

	// je�li tu dotrzemy to sytuacja jest zapewne bardzo kiepska
	// ale znajd�my jeszcze co�
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (LeftInColorOthers(l_enColor) == 0)
			continue;

		// zagrajmy t� kart�
		return m_pCards->FirstInColor(l_enColor);
	}

	// je�li tu dotrzemy to sytuacja jest beznadziejna
	// zagrajmy co mamy
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (l_enColor == E_CC_HEART)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;

		// zagrajmy t� kart�
		return m_pCards->FirstInColor(l_enColor);
	}
	// skoro tu jeste�my, to znaczy �e ma same kiery (ale si� upewnijmy,
	// �e nie ma babola
	ASSERT(!m_pCards->HasNoHeartsColor());
	return m_pCards->FirstInShortest();
}


// ---------------------------------------------------------
//	Do�o�enie karty w kolorze przed 6
//
short	//WY numer karty
CRobberDecider::NextCardBefore6(
	short a_iTrick	//WE numer lewy
	) const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_iTrick - 1] ;
	T_SUIT l_enColor = l_trickCurrent.GetCardColor(0);
	const CCard* l_pCardBiggest = l_trickCurrent.Biggest();
	short l_nCardNr;
	
	if (!m_pCards->HasColor(l_enColor))
	{
		l_nCardNr = DecideOtherCard();
		ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
		return l_nCardNr;
	}

	if (l_trickCurrent.GetCardsCnt() < 3)
	{
		l_nCardNr =  m_pCards->LessOrFirstBiggerInColor(l_pCardBiggest);
		ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
		return l_nCardNr;
	}
	else
	// ostatnia karta - je�li nie mamy ni�szej, to zagrajmy
	// najwy�sz� dobr� w kolejno�ci A, 10, 9, .., K, J, D
	{
		l_nCardNr = m_pCards->GetBiggestBelow(l_enColor, l_pCardBiggest->CardValue());
		if (l_nCardNr >= 0)
		{
			ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
			return l_nCardNr;
		}

		l_nCardNr = m_pCards->FindCard(l_enColor, E_CV_A);
		if (l_nCardNr >= 0)
		{
			ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
			return l_nCardNr;
		}
		l_nCardNr = m_pCards->GetBiggestBelow(l_enColor, E_CV_J);
		if (l_nCardNr >= 0)
		{
			ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
			return l_nCardNr;
		}
		l_nCardNr = m_pCards->FindCard(l_enColor, E_CV_K);
		if (l_nCardNr >= 0)
		{
			ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
			return l_nCardNr;
		}
		l_nCardNr = m_pCards->FindCard(l_enColor, E_CV_J);
		if (l_nCardNr >= 0)
		{
			ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
			return l_nCardNr;
		}
		l_nCardNr = m_pCards->FindCard(l_enColor, E_CV_D);
		if (l_nCardNr >= 0)
		{
			ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
			return l_nCardNr;
		}

	}
	// nigdy tutaj
	ASSERT(FALSE);
	return -1;
}


// ---------------------------------------------------------
//	Do�o�enie karty w kolorze lewa 6
//
short	//WY kt�ra karta
CRobberDecider::NextCard6(
	short a_iTrick	//WE kt�ra lewa
	) const
{
	// na razie na skr�ty
	return NextCardBefore6(a_iTrick);
}


// ---------------------------------------------------------
//	Do�o�enie karty w kolorze lewa po sz�stej
//
short	//WY numer karty
CRobberDecider::NextCardAfter6(
	short a_iTrick	//WE numer lewy
	) const
{
	// na razie na skr�ty
	return NextCardBefore6(a_iTrick);
}


// ---------------------------------------------------------
//	Jak� kart� wyrzuci�
//
short	//WY numer karty
CRobberDecider::DecideOtherCard() const
{
	
	// je�li mamy kr�la kier lub asa to si� go pozb�d�my
	short l_nCardNr;
	l_nCardNr = m_pCards->FindCard(E_CC_HEART, E_CV_K);
	if (l_nCardNr >= 0)
		return l_nCardNr;

	l_nCardNr = m_pCards->FindCard(E_CC_HEART, E_CV_A);
	if (l_nCardNr >= 0)
		return l_nCardNr;

	// teraz wyrzu�my karte w najkr�tszych kolorach
	// chyba, �e to dw�jka
	short l_nRank;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		// je�li inni nie maj� tego koloru to po co wyrzuca�
		if (LeftInColorOthers(l_enColor) == 0)
			continue;
		// nie ma sensu wyrzuca� je�li s� tylko wy�sze w kolorze
		const CCard* l_pCard = m_pCards->BiggestInColorCard(l_enColor);
		if (!m_pTricks->AreLessInColor(l_pCard, m_pCards))
			continue;
		// szkoda te� wyrzuci� dw�jki
		if (l_pCard->CardValue() == E_CV_2)
			continue;
		// teraz ju� OK
		return m_pCards->Biggest(l_enColor);

	}
	
	// spr�bujmy wyrzuci� najwy�szego kiera
	if (m_pCards->HasColor(E_CC_HEART))
		return m_pCards->Biggest(E_CC_HEART);
	
	// wyrzu� w najkr�tszym posiadanym
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		T_SUIT l_enColor = m_pCards->GetShortestColorAllRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		return m_pCards->Biggest(l_enColor);
	}
	// tu nie powinni�my si� pojawi�
	ASSERT(FALSE);
	return -1;
}


