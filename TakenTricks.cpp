// TakenTricks.cpp: implementation of the CTakenTricks class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TakenTricks.h"
#include "UserCards.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//	-----------------------------------------------------------
//	Konstruktor
//
CTakenTricks::CTakenTricks()
{
	Clear();
}


//	-----------------------------------------------------------
//	Destruktor
//
CTakenTricks::~CTakenTricks()
{

}


//	-----------------------------------------------------------
//	Wyczyszczenie zawartoœci
//
void 
CTakenTricks::Clear()
{
	m_nTrickCnt = 0;
	int l_iAt;
	for (l_iAt = 0; l_iAt < ArraySize(m_tricks); ++l_iAt)
	{
		m_tricks[l_iAt].Clear();
	}
}


//	-----------------------------------------------------------
//	Zwraca kartê
//
const CTrick&	//WY lewa
CTakenTricks::operator[](
	short a_nTrick	//WE numer lewy
	) const
{
	return m_tricks[a_nTrick];
}


// ---------------------------------------------------------
//	Ile posz³o w kolore
//
short	//WY ile kart posz³o w kolorze
CTakenTricks::TakenInColor(
	T_SUIT a_enColor	//kolor sprawdzany
	) const
{
	short l_nTrick;
	short l_nTaken = 0;
	for (l_nTrick = 1; l_nTrick <= m_nTrickCnt; l_nTrick++)
	{
		l_nTaken = l_nTaken + m_tricks[l_nTrick - 1].CntInColor(a_enColor);
	}
	return l_nTaken;
}


// ---------------------------------------------------------
//	Ile zosta³o w kolore
//
short	//WY iloœæ w kolorze nieu¿ytych
CTakenTricks::LeftInColor(
	T_SUIT a_enColor	//WE sprawdzany kolor
	) const
{
	return 13 - TakenInColor(a_enColor);
}


// ---------------------------------------------------------
//	Czy s¹ jeszcze wy¿sze karty w tym kolorze od podanej i nie 
//	bêd¹ce w kartach CUserCards
//
BOOL	//WY TRUE - s¹ wy¿sze
CTakenTricks::AreBiggerInColor(
	const CCard* a_pCard,		//WE sprawdzana karta
	const CUserCards* a_pCards	//WE karty gracza
	) const
{
	T_SUIT l_enColor = a_pCard->GetColor();
	if (LeftInColor(l_enColor) == 0)
		return FALSE;
	
	// mo¿e to AS?
	if (a_pCard->IsBiggestInColor())
		return FALSE;

	short l_nTrick;
	short l_nCardNr;
	short l_nBiggestInColor = a_pCard->BiggestInColor();
	BOOL  l_bFound;
	// dla ka¿dego mo¿liwej wy¿szej sprawdzamy czy ju¿ posz³a
	for (l_nCardNr = a_pCard->GetNr(); l_nCardNr <= l_nBiggestInColor; l_nCardNr++)
	{
		l_bFound = FALSE;
		for (l_nTrick = 1; l_nTrick <= m_nTrickCnt; l_nTrick++)
		{
			if (m_tricks[l_nTrick - 1].IsCard(l_nCardNr))
			{
				l_bFound = TRUE;
				break;
			}
		}
		// i czy to my nie mamy takiej karty 
		if (a_pCards->IsCard(l_nCardNr))
			continue;
		if (!l_bFound)
			return TRUE;
	}
	
	return FALSE;
}


// ---------------------------------------------------------
//	Czy s¹ jeszcze ni¿sze karty w tym kolorze od podanej i nie 
//	bêd¹ce w kartach CUserCards
//
BOOL	//WY TRUE - s¹ jeszcze nizsze karty
CTakenTricks::AreLessInColor(
	const CCard* a_pCard,		//WE karta z któr¹ porównujemy
	const CUserCards* a_pCards	//WE karty usera
	) const
{
	T_SUIT l_enColor = a_pCard->GetColor();
	if (LeftInColor(l_enColor) == 0)
		return FALSE;
	
	// mo¿e to dwójka?
	if (a_pCard->CardValue() == E_CV_2)
		return FALSE;

	short l_nTrick;
	short l_nCardNr;
	short l_nLeastInColor = a_pCard->LeastInColor();
	BOOL  l_bFound;
	// dla ka¿dego mo¿liwej wy¿szej sprawdzamy czy ju¿ posz³a
	for (l_nCardNr = a_pCard->GetNr(); l_nCardNr >= l_nLeastInColor; l_nCardNr--)
	{
		l_bFound = FALSE;
		for (l_nTrick = 1; l_nTrick <= m_nTrickCnt; l_nTrick++)
		{
			if (m_tricks[l_nTrick - 1].IsCard(l_nCardNr))
			{
				l_bFound = TRUE;
				break;
			}
		}

		// i czy to my nie mamy takiej karty 
		if (a_pCards->IsCard(l_nCardNr))
			continue;
		if (!l_bFound)
			return TRUE;
	}
	
	return FALSE;
}


// ---------------------------------------------------------
//	Ile jest wy¿szych w kolorze od podanej
//
short	//WY iloœæ kart wy¿szych
CTakenTricks::BiggerInColorCnt(
	const CCard* a_pCard
	) const
{
	T_SUIT l_enColor = a_pCard->GetColor();
	short l_nCard = a_pCard->GetNr();
	if (LeftInColor(l_enColor) == 0)
		return 0;
	
	// mo¿e to AS?
	if (a_pCard->IsBiggestInColor())
		return 0;

	short l_nTrick;
	short l_nCardNr;
	short l_nBiggestInColor = a_pCard->BiggestInColor();
	short l_nCnt = 0;
	BOOL  l_bFound;

	// dla ka¿dego mo¿liwej wy¿szej sprawdzamy czy ju¿ posz³a
	for (l_nCardNr = a_pCard->GetNr(); l_nCardNr <= l_nBiggestInColor; l_nCardNr++)
	{
		l_bFound = FALSE;
		for (l_nTrick = 1; l_nTrick <= m_nTrickCnt; l_nTrick++)
		{
			if (m_tricks[l_nTrick - 1].IsCard(l_nCardNr))
			{
				l_bFound = TRUE;
				break;
			}
		}
		if (!l_bFound)
			l_nCnt++;
	}
	
	return l_nCnt;
}



// ---------------------------------------------------------
//	Zwraca kolor w którym zosta³o najwiêcej wed³ug rankingu
//	Ranking 1 oznacza zwróæ kolor w którym zosta³o najwiêcej kart
//	Ranking 2 oznacza 2 z kolei kolor w którym zosta³o 
//	najwiêcej kart, itd.
//
T_SUIT		//WY wybrany kolor
CTakenTricks::LeftInColorRank(
	short a_nRank	//WE ranking
	) const
{
	ASSERT((a_nRank >= 1) && (a_nRank <= 4));
	struct S_RANK
	{
		T_SUIT enColor;
		short	nLeft;
	} l_aColorCnt[4], l_aColorSort[4];

	short l_iAt = 0;
	l_aColorCnt[l_iAt].enColor = E_CC_CLUB;
	l_aColorCnt[l_iAt].nLeft = LeftInColor(l_aColorCnt[l_iAt].enColor);
	l_iAt++;

	l_aColorCnt[l_iAt].enColor = E_CC_DIAMOND;
	l_aColorCnt[l_iAt].nLeft = LeftInColor(l_aColorCnt[l_iAt].enColor);
	l_iAt++;

	l_aColorCnt[l_iAt].enColor = E_CC_SPADE;
	l_aColorCnt[l_iAt].nLeft = LeftInColor(l_aColorCnt[l_iAt].enColor);
	l_iAt++;

	l_aColorCnt[l_iAt].enColor = E_CC_HEART;
	l_aColorCnt[l_iAt].nLeft = LeftInColor(l_aColorCnt[l_iAt].enColor);

	// po wszystkich elementach - najd³u¿szy przepisuj
	short l_nSort = 1;
	short l_nMax = -1;
	short l_nMaxPos = -1;
	while (l_nSort <= 4)
	{
		l_nMax = -1;
		for (l_iAt = 1; l_iAt <= 4; l_iAt++)
		{
			if (l_nMax < l_aColorCnt[l_iAt - 1].nLeft)
			{
				l_nMax = l_aColorCnt[l_iAt - 1].nLeft;
				l_nMaxPos = l_iAt;
			}
		}
		l_aColorSort[l_nSort - 1].enColor = l_aColorCnt[l_nMaxPos - 1].enColor;
		l_aColorSort[l_nSort - 1].nLeft = l_aColorCnt[l_nMaxPos - 1].nLeft;
		l_aColorCnt[l_nMaxPos - 1].nLeft = -1;
		l_nSort++;
	}

	return l_aColorSort[a_nRank - 1].enColor;
}


// ---------------------------------------------------------
//	Ile lew wziêto w podanym kolorze
//
short	//WY iloœæ wziêtych lew
CTakenTricks::TricksInColor(
	T_SUIT a_enColor	//WE sprawdzany kolor
	) const
{
	short l_nTrick;
	short l_nCnt = 0;
	for (l_nTrick = 1; l_nTrick <= m_nTrickCnt; l_nTrick++)
	{
		if (m_tricks[l_nTrick - 1].GetCardsCnt() == 0)
			break;
		if (m_tricks[l_nTrick - 1].GetCardColor(0) != a_enColor)
			continue;
		l_nCnt++;
	}
	return l_nCnt;
}


// ---------------------------------------------------------
//	Ile jest kierów we wszystkich lewach
//
short	//WY iloœc kierów
CTakenTricks::TakenHearts() const
{
	short l_nTrick;
	short l_nCnt = 0;
	for (l_nTrick = 1; l_nTrick <= m_nTrickCnt; l_nTrick++)
	{
		if (m_tricks[l_nTrick - 1].GetCardsCnt() == 0)
			break;
		l_nCnt += m_tricks[l_nTrick - 1].GetHeartsCnt();
	}
	return l_nCnt;

}


// ---------------------------------------------------------
//	Ile jest ch³opców we wszystkich lewach
//
short	//WY iloœæ wziêtych ch³opaków
CTakenTricks::TakenBoys() const
{
	short l_nTrick;
	short l_nCnt = 0;
	for (l_nTrick = 1; l_nTrick <= m_nTrickCnt; l_nTrick++)
	{
		if (m_tricks[l_nTrick - 1].GetCardsCnt() == 0)
			break;
		l_nCnt += m_tricks[l_nTrick - 1].GetBoysCnt();
	}
	return l_nCnt;

}


// ---------------------------------------------------------
//	Ile jest dam we wszystkich lewach
//
short	//WY iloœæ wziêtych dam
CTakenTricks::TakenLadies() const 
{
	short l_nTrick;
	short l_nCnt = 0;
	for (l_nTrick = 1; l_nTrick <= m_nTrickCnt; l_nTrick++)
	{
		if (m_tricks[l_nTrick - 1].GetCardsCnt() == 0)
			break;
		l_nCnt += m_tricks[l_nTrick - 1].GetLadiesCnt();
	}
	return l_nCnt;

}


// ---------------------------------------------------------
//	Czy by³y grane tylko ca³e lewe, tzn. wszystkie karty 
//	we wszystki lewach w danym kolorze s¹ jednego koloru
//
BOOL	//WY TRUE - grane tylko ca³e lewy
CTakenTricks::FullTricksInColor(
	T_SUIT a_enColor	//WE kolor sprawdzany
	)	const
{
	short l_nTrick;
	for (l_nTrick = 1; l_nTrick <= m_nTrickCnt; l_nTrick++)
	{
		// ostatnia lewa
		if (m_tricks[l_nTrick - 1].GetCardsCnt() == 0)
			break;
		if (m_tricks[l_nTrick - 1].GetCardColor(0) != a_enColor)
			continue;
		if (!m_tricks[l_nTrick - 1].InOneColor())
			return FALSE;
	}
	return TRUE;

}


// ---------------------------------------------------------
//	Czy w ostatniej lewie w tym kolorze grano kolor atutowy
//	TRUE - oznacza, ¿e by³y
//
BOOL	//WY TRUE - by³y atu
CTakenTricks::TrumpsInLastTrickInColor(
	T_SUIT a_enColor,	//WE kolor sprawdzany
	T_SUIT a_enTrumps	//WE kolor atutowy
	) const
{
	short l_nTrick;
	for (l_nTrick = m_nTrickCnt; l_nTrick >= 1; l_nTrick--)
	{
		// szukamy pierwszej lewy w podanym kolorze od koñca
		if (m_tricks[l_nTrick - 1].GetCardColor(0) != a_enColor)
			continue;

		return m_tricks[l_nTrick - 1].IsColor(a_enTrumps);
	}

	// nie by³o ¿adnych lew w tym kolorze
	ASSERT(this->TricksInColor(a_enColor) == 0);
	return FALSE;
}


// ---------------------------------------------------------
// Zwraca iloœæ lew	
//
short	//WY iloœæ lew
CTakenTricks::GetTricksCnt() const
{
	return m_nTrickCnt;
}


// ---------------------------------------------------------
// Zwraca referencjê na aktualnie rozgrywan¹ lewê
//
const CTrick&	//WY rozgrywana lewa
CTakenTricks::GetCurrentTrick() const
{
	ASSERT(m_nTrickCnt > 0);
	ASSERT(m_nTrickCnt <=13);
	return m_tricks[m_nTrickCnt - 1];
}


// ---------------------------------------------------------
// Ustawia nastêpn¹ lewê
//
void 
CTakenTricks::NextTrick()
{
	m_nTrickCnt++;
}


// ---------------------------------------------------------
// Ustawia nastêpn¹ kartê w lewie
//
void 
CTakenTricks::SetCard(
	const CCard &a_card		//WE karta w lewie
	)
{
	m_tricks[m_nTrickCnt - 1].SetCard(&a_card);
}


// ---------------------------------------------------------
// Ustawia w³aœciciela ostatniej lewy i zwraca go
//
T_PLAYER	//WY w³aœciciel ostatniej lewy
CTakenTricks::SetLastTrickOwner(
	T_SUIT a_colorTrumps	//WE kolor atu lub E_CC_NOTHING
	)
{
	return m_tricks[GetTricksCnt() - 1].SetTrickOwner(a_colorTrumps);
}