// UserCards.cpp: implementation of the CUserCards class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserCards.h"
#include "TakenTricks.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserCards::CUserCards()
{

}

CUserCards::~CUserCards()
{

}

// ---------------------------------------------------------
//	Najm³odsza karta w najkrótszym kolorze
//
short	//WY numer karty w rêce
CUserCards::FirstInShortest()	const
{
	// znajdŸ najm³odsz¹ kartê w najkrótszym kolorze
	T_COLOR l_enShortest = ShortestColor() ;
	return FirstInColor(l_enShortest) ;
}


// ---------------------------------------------------------
//	Pierwsza karta w kolorze
//
short	//WY numer najm³odszej karty w kolorze
CUserCards::FirstInColor(
	T_COLOR a_enShortest	//WE kolor w którym szukamy
	)	const
{
	// znajdŸ najm³odsz¹ kartê w najkrótszym kolorze
	short l_nAt ;

	for (l_nAt = 0; l_nAt < 13; l_nAt++)
	{
		if (!m_cards[l_nAt].IsUsed())
		{
			if (m_cards[l_nAt].GetColor() == a_enShortest)
			{
				return l_nAt;
			}
		}
	}
	ASSERT(FALSE) ;
	return -1 ;
}


// ---------------------------------------------------------
//	Pierwsza karta w kolorze - wartoœæ
//
T_CARDVAL	//WY wartoœæ najmniejszej karty
CUserCards::FirstInColorVal(
	T_COLOR a_enColor	//WE kolor
	)	const
{
	// znajdŸ najm³odsz¹ kartê w najkrótszym kolorze
	const CCard* l_pCard = FirstInColorCard(a_enColor);
	if (l_pCard == NULL)
	{
		return E_CV_NULL;
	}
	else
	{
		return l_pCard->CardValue();
	}

}


// ---------------------------------------------------------
//	Pierwsza karta w kolorze - wartoœæ
//
const CCard* 
CUserCards::FirstInColorCard(
	T_COLOR a_enColor	//WE kolor
	)	const
{
	// znajdŸ najm³odsz¹ kartê w najkrótszym kolorze
	short l_iAt ;

	for (l_iAt = 0; l_iAt < 13; l_iAt++)
	{
		if (!m_cards[l_iAt].IsUsed())
		{
			if (m_cards[l_iAt].GetColor() == a_enColor)
			{
				return &(m_cards[l_iAt]);
			}
		}
	}
	return NULL;
}


// ---------------------------------------------------------
//	Posortowanie kart
//
void 
CUserCards::Sort(
 	short a_nStart,	// od której zacz¹æ sortowanie
	short a_nEnd	// na której skoñczyæ
	)
{
#pragma message(HERE "zmieñ algorytm sortowania")
	short l_nAt ;
	short l_nAtIn ;
	for (l_nAt = a_nStart; l_nAt <= a_nEnd; l_nAt++)
	{
		short l_nNumAt = (m_cards[l_nAt]).GetNr() ;
		short l_nMin = l_nNumAt ;
		short l_nMinPos = l_nAt ;

		for (l_nAtIn = l_nAt + 1; l_nAtIn <= a_nEnd; l_nAtIn++)
		{
			if (l_nMin > (m_cards[l_nAtIn]).GetNr())
			{
				l_nMin = (m_cards[l_nAtIn]).GetNr() ;
				l_nMinPos = l_nAtIn ;
			}
		}
		if (l_nAt != l_nMinPos)
		{
			m_cards[l_nMinPos].SetCardNr(l_nNumAt) ;
			m_cards[l_nAt].SetCardNr(l_nMin) ; 
		}
	}
}



// ---------------------------------------------------------
//	Pobranie numeru karty
//
short	//WY numer karty
CUserCards::CardNr(
	short a_nInHand		//WE numer karty
	) const
{
	return m_cards[a_nInHand].GetNr() ;
}


// ---------------------------------------------------------
//	Zwraca kartê
//
const CCard&	//WY referencja do karty
CUserCards::operator[](
	short a_nCard		//WE element
	) const
{
	return GetCard(a_nCard);
}


// ---------------------------------------------------------
//	Ustawienie danych karty
//
void 
CUserCards::SetCard(
	short a_nAt,		//WE która karta
	short a_nCardNr,	//WE numer karty
	T_PLAYER a_enOwner,	//WE w³aœciciel
	BOOL a_bUsed		//WE czy u¿yta
	)
{
	m_cards[a_nAt].SetCard(a_nCardNr, a_enOwner, a_bUsed) ;
}


// ---------------------------------------------------------
//	Najkrótszy kolor w kartach
//
T_COLOR		//WY kolor najkrótszy
CUserCards::ShortestColor()	const
{

	short l_iClubsCnt = ColorCnt(E_CC_CLUB);
	short l_iDiamondsCnt = ColorCnt(E_CC_DIAMOND);
	short l_iSpadesCnt = ColorCnt(E_CC_SPADE);
	short l_iHeartsCnt = ColorCnt(E_CC_HEART);

	short l_iShortest = l_iClubsCnt;
	T_COLOR l_enColor = E_CC_CLUB ;

	if (l_iShortest == 0)
	{
		l_iShortest = l_iDiamondsCnt ;
		l_enColor = E_CC_DIAMOND ;
	}
	if (l_iShortest == 0)
	{
		l_iShortest = l_iSpadesCnt ;
		l_enColor = E_CC_SPADE ;
	}
	if (l_iShortest == 0)
	{
		l_iShortest = l_iHeartsCnt ;
		l_enColor = E_CC_HEART ;
	}
	ASSERT(l_iShortest > 0) ;

	if ((l_iDiamondsCnt < l_iShortest) && (l_iDiamondsCnt > 0))
	{
		l_enColor = E_CC_DIAMOND ;
		l_iShortest = l_iDiamondsCnt ;
	}

	if ((l_iSpadesCnt < l_iShortest) && (l_iSpadesCnt > 0))
	{
		l_enColor = E_CC_SPADE ;
		l_iShortest = l_iSpadesCnt ;
	}

	if ((l_iHeartsCnt < l_iShortest) && (l_iHeartsCnt > 0))
	{
		l_enColor = E_CC_HEART ;
		l_iShortest = l_iHeartsCnt ;
	}

	return l_enColor ;

}


// ---------------------------------------------------------
//	Ile user ma w podanym kolorze kart
//
short	//WY ile kart w kartach u¿ytk. w podanym kolorze
CUserCards::ColorCnt(
	T_COLOR a_enColor,	//WE kolor 
	BOOL a_bUsed		//WE TRUE nie uwzglêdniaj u¿ytych
	)	const
{
	short l_nCnt = 0 ;
	short l_nAt ;
	for (l_nAt = 0; l_nAt < 13; l_nAt++)
	{	
		if (m_cards[l_nAt].IsUsed() && a_bUsed)
			continue;

		if (m_cards[l_nAt].GetColor() == a_enColor)
			l_nCnt++ ;
	}
	return l_nCnt ;
}


// ---------------------------------------------------------
//	Ile user ma w podanym kolorze kart uwzglêdniaj¹c zu¿yte
//
short	//WY iloœæ kart w kolorze z uzytymi
CUserCards::ColorAllCnt(
	T_COLOR a_enColor	//WE sprawdzany kolor
	)	const
{
	return ColorCnt(a_enColor, FALSE);
}


// ---------------------------------------------------------
//	Najkrótszy kolor niekierowy
//
T_COLOR		//WY najkrótszy niekierowy kolor
CUserCards::ShortestColorNoHearts() const
{

	short l_iClubsCnt = ColorCnt(E_CC_CLUB);
	short l_iDiamondsCnt = ColorCnt(E_CC_DIAMOND);
	short l_iSpadesCnt = ColorCnt(E_CC_SPADE);

	short l_iShortest = l_iClubsCnt;
	T_COLOR l_enColor = E_CC_CLUB ;

	if (l_iShortest == 0)
	{
		l_iShortest = l_iDiamondsCnt ;
		l_enColor = E_CC_DIAMOND ;
	}
	if (l_iShortest == 0)
	{
		l_iShortest = l_iSpadesCnt ;
		l_enColor = E_CC_SPADE ;
	}
	ASSERT(l_iShortest > 0) ;

	if ((l_iDiamondsCnt < l_iShortest) && (l_iDiamondsCnt > 0))
	{
		l_enColor = E_CC_DIAMOND ;
		l_iShortest = l_iDiamondsCnt ;
	}

	if ((l_iSpadesCnt < l_iShortest) && (l_iSpadesCnt > 0))
	{
		l_enColor = E_CC_SPADE ;
		l_iShortest = l_iSpadesCnt ;
	}

	return l_enColor ;
}


// ---------------------------------------------------------
//	Czy s¹ karty nie "kiery"
//
BOOL	//WY s¹ nie kiery
CUserCards::AreNoHearts() const
{
	short l_iAt ;
	for (l_iAt = 0; l_iAt <= 12; l_iAt++)
	{	
		if (!(m_cards[l_iAt].IsUsed()))
		{
			if (m_cards[l_iAt].GetColor() != E_CC_HEART)
			{
				return TRUE;
			}
		}
	}
	return FALSE ;

}


// ---------------------------------------------------------
//	Zwraca najwiêksz¹ kartê w kolorze
//
short	//WY numer karty najwiekszej w kolorze
CUserCards::LastInColor(
	T_COLOR a_enColor	//WE badany kolor
	) const
{
	// znajdŸ najstarsz¹ kartê w kolorze
	// karty ju¿ pouk³adane
	short l_iAt ;

	for (l_iAt = 13; l_iAt > 0; l_iAt--)
	{
		if (!m_cards[l_iAt - 1].IsUsed())
		{
			if (m_cards[l_iAt - 1].GetColor() == a_enColor)
			{
				return l_iAt - 1;
			}
		}
	}
	ASSERT(FALSE) ;
	return 1 ;
}


// ---------------------------------------------------------
//	Mniejsza w kolorze lub najwieksza
//
short	//WY numer karty w rêce
CUserCards::LessOrBiggestInColor(
	const CTrick& a_trick	//lewa sprawdzana
	)	const
{
	// znajdŸ pierwsz¹ mniejsz¹ lub najwiêksz¹ w kolorze
	// jeœli nie masz - wyrzuæ najwy¿sz¹ w najkrótszym kolorze
	// karty ju¿ pouk³adane
	T_COLOR l_enColor ;
	short l_iFoundAt = -1 ;
	l_enColor = a_trick.GetCardColor(0);
	const CCard* l_pCard = a_trick.Biggest() ;


	if (HasColor(l_enColor))
	{
		l_iFoundAt = GetBiggestBelow(l_enColor, l_pCard->CardValue());
		if (l_iFoundAt < 0)
		{
			l_iFoundAt = Biggest(l_enColor);
		}

	}
	else
	{
		l_iFoundAt = FirstInShortest() ;
	}

	ASSERT((l_iFoundAt >= 0) && (l_iFoundAt < 13)) ;
	return l_iFoundAt ;
}


// ---------------------------------------------------------
// Czy jest kolor podany. Uwzglêdniany stan u¿ycia - nie 
// liczy siê kart u¿ytych.
//
BOOL	//WY TRUE - kolor jest
CUserCards::HasColor(
	T_COLOR a_enColor	//WE sprawdzany kolor
	)	const
{
	short l_iCard ;
	for (l_iCard = 0; l_iCard < 13; l_iCard++)
	{
		if (m_cards[l_iCard].IsUsed())
		{
			continue ;
		}

		if (m_cards[l_iCard].GetColor() != a_enColor)
		{
			continue;
		}
		return TRUE ;
	}
	return FALSE ;
}


// ---------------------------------------------------------
//	Najwieksza w kolorze
//
short	//WE pozycja najwiêkszej karty w kolorze
CUserCards::Biggest(
	T_COLOR a_enColor	//WE sprawdzany kolor
	)	const
{

	short l_nCard;
	for (l_nCard = 13; l_nCard > 0; l_nCard--)
	{
		if (m_cards[l_nCard - 1].IsUsed())
		{
			continue;
		}

		if (m_cards[l_nCard - 1].GetColor() != a_enColor)
		{
			continue;
		}
		return l_nCard - 1;
	}

	// nie ma najwiêkszej w kolorze
	ASSERT(FALSE);
	return -1;
}


// ---------------------------------------------------------
//	Najwieksza w kolorze
//
const CCard*	//WY karta najwiêksza w kolorze
CUserCards::BiggestInColorCard(
	T_COLOR a_enColor	//WE sprawdzany kolor
	)	const
{
	return BiggestInColorCardRank(a_enColor, 1);
}


// ---------------------------------------------------------
//	Najwieksza w kolorze wed³ug rankingu
//
const CCard*	//WY karta najwy¿sza w kolorze
CUserCards::BiggestInColorCardRank(
	T_COLOR a_enColor,	//WE sprawdzany kolor
	short a_nRank		//WE która najwy¿sza
	)	const
{
	short l_iCnt = 0;
	short l_iCardNr;
	for (l_iCardNr = 13; l_iCardNr > 0; l_iCardNr--)
	{
		if (m_cards[l_iCardNr - 1].IsUsed())
			continue ;

		if (m_cards[l_iCardNr - 1].GetColor() != a_enColor)
			continue ;
		l_iCnt++;
		if (l_iCnt != a_nRank)
			continue;
		return &(m_cards[l_iCardNr - 1]);
	}

	// nie ma najwiêkszej w kolorze
	return NULL ;
}




// ---------------------------------------------------------
//	Najwieksza w kolorze poni¿ej podanej
//
short	//WY numer karty
CUserCards::GetBiggestBelow(
	T_COLOR a_enColor,	//WE kolor
	T_CARDVAL a_cv		//WE wartoœæ karty
	) const
{
	short l_iCard;
	for (l_iCard = 13; l_iCard > 0; l_iCard--)
	{
		if (m_cards[l_iCard - 1].IsUsed())
			continue;
		if (m_cards[l_iCard - 1].GetColor() != a_enColor)
			continue;
		if (m_cards[l_iCard - 1].CardValue() <	a_cv)
			return l_iCard - 1;
	}
	return -1;
}


// ---------------------------------------------------------
//	ZnajdŸ podan¹ kartê
//	a_bUsed, okreœla czy karty u¿yte nie s¹ brane pod uwagê
//
short	//WY numer karty w rêce
CUserCards::FindCard(
	T_COLOR a_enColor,	//WE kolor
	T_CARDVAL a_cv,		//WE wartoœæ karty
	BOOL a_bUsed		//WE TRUE - bez u¿ywanych
	) const
{
	short l_nCard;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed() == a_bUsed)
			continue;
		if (m_cards[l_nCard].GetColor() != a_enColor)
			continue;
		if (m_cards[l_nCard].CardValue() != a_cv)
			continue;
		return l_nCard;
	}
	return -1;
}


// ---------------------------------------------------------
//	ZnajdŸ podan¹ kartê
//	a_bUsed, okreœla czy karty u¿yte nie s¹ brane pod uwagê
//
short	//WY numer karty w rêce lub -1, gdy nie ma 
CUserCards::FindCard(
	const CCard& a_card,	//WE karta szukana
	BOOL a_bUsed	//WE TRUE - szukaæ tylko w nieu¿ytych
	)	const
{
	return FindCard(&a_card, a_bUsed);
}


// ---------------------------------------------------------
//	ZnajdŸ podan¹ kartê
//	a_bUsed, okreœla czy karty u¿yte nie s¹ brane pod uwagê
//
short	//WY numer karty w rêce lub -1, gdy nie ma 
CUserCards::FindCard(
	const CCard* a_pCard,	//WE karta szukana
	BOOL a_bUsed	//WE TRUE - szukaæ tylko w nieu¿ytych
	)	const
{
	short l_nCard;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed() == a_bUsed)
			continue;
		if (m_cards[l_nCard].GetColor() != a_pCard->GetColor())
			continue;
		if (m_cards[l_nCard].CardValue() != a_pCard->CardValue())
			continue;
		return l_nCard;
	}
	return -1;
}


// ---------------------------------------------------------
//	Zwraca kolor najkrótszy wed³ug rankingu
//	Ranking 1 oznacza zwróæ kolor w którym jest najmniej kart
//	Ranking 2 oznacza 2 z kolei kolor w którym jest najmniej 
//	kart, itd.
//
T_COLOR		//WY njkrótszy kolor wed³ug rankingu
CUserCards::GetShortestColorRank(
	short a_nRank,	//WE ranking
	BOOL a_bUsed	//WE TRUE - nie uwzglêdniaæ u¿ytych
	)	const
{
	ASSERT((a_nRank >= 1) && (a_nRank <= 4));
	struct S_RANK
	{
		T_COLOR enColor;
		short	nLeft;
	} l_aColorCnt[4], l_aColorSort[4];

	short l_nAt = 0;
	l_aColorCnt[l_nAt].enColor = E_CC_CLUB;
	l_aColorCnt[l_nAt].nLeft = ColorCnt(l_aColorCnt[l_nAt].enColor, a_bUsed);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = E_CC_DIAMOND;
	l_aColorCnt[l_nAt].nLeft = ColorCnt(l_aColorCnt[l_nAt].enColor, a_bUsed);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = E_CC_SPADE;
	l_aColorCnt[l_nAt].nLeft = ColorCnt(l_aColorCnt[l_nAt].enColor, a_bUsed);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = E_CC_HEART;
	l_aColorCnt[l_nAt].nLeft = ColorCnt(l_aColorCnt[l_nAt].enColor, a_bUsed);

	// po wszystkich elementach - najd³u¿szy przepisuj
	short l_nSort = 1;
	short l_nMax = -1;
	short l_nMaxPos = -1;
	while (l_nSort <= 4)
	{
		l_nMax = -1;
		for (l_nAt = 1; l_nAt <= 4; l_nAt++)
		{
			if (l_nMax < l_aColorCnt[l_nAt - 1].nLeft)
			{
				l_nMax = l_aColorCnt[l_nAt - 1].nLeft;
				l_nMaxPos = l_nAt;
			}
		}
		l_aColorSort[l_nSort - 1].enColor = l_aColorCnt[l_nMaxPos - 1].enColor;
		l_aColorSort[l_nSort - 1].nLeft = l_aColorCnt[l_nMaxPos - 1].nLeft;
		l_aColorCnt[l_nMaxPos - 1].nLeft = -1;
		l_nSort++;
	}

	return l_aColorSort[4 - a_nRank].enColor;
}


// ---------------------------------------------------------
//	Zwraca kolor najkrótszy wed³ug rankingu uwzglêdniaj¹c 
//	wziête karty.
//	Ranking 1 oznacza zwróæ kolor w którym jest najmniej kart
//	Ranking 2 oznacza 2 z kolei kolor w którym jest najmniej 
//	kart, itd.
//
T_COLOR		//WY znaleziony kolor
CUserCards::GetShortestColorAllRank(
	short a_nRank	//WE który najkrótszy
	)	const
{
	return GetShortestColorRank(a_nRank, FALSE);
}


// ---------------------------------------------------------
//	Zwraca kolor najkrótszy wed³ug rankingu
//	Ranking 1 oznacza zwróæ kolor w którym jest najmniej kart
//	Ranking 2 oznacza 2 z kolei kolor w którym jest najmniej 
//	kart, itd.
//
T_COLOR		//WY njkrótszy kolor wed³ug rankingu
CUserCards::GetLongestColorRank(
	short a_nRank,	//WE ranking
	BOOL a_bUsed	//WE TRUE - nie uwzglêdniaæ u¿ytych
	)	const
{
	return GetShortestColorRank(5 - a_nRank, a_bUsed);
}


// ---------------------------------------------------------
//	Czy posiada kolor ró¿ny od kierów
//
BOOL	//WY TRUE - posiada
CUserCards::HasNoHeartsColor() const
{
	if (HasColor(E_CC_CLUB))
	{
		return TRUE;
	}
	if (HasColor(E_CC_SPADE))
	{
		return TRUE;
	}
	if (HasColor(E_CC_DIAMOND))
	{
		return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------
//	Czy user ma jeszcze nie u¿yt¹ karte o podanym numerze
//
BOOL	//WY TRUE - jest karta
CUserCards::IsCard(
	short a_nCardNr		//WE numer karty
	)	const
{
	short l_nCard;
	for (l_nCard = 0; l_nCard < c_app_btPlayerCards; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed())
			continue;
		if (m_cards[l_nCard].GetNr() == a_nCardNr)
			return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------
//	Ile mam nieu¿ytych kart w kolorze
//
short	//WY ilosæ kart w kolorze
CUserCards::CardsInColor(
	T_COLOR a_enColor	//WE sprawdzany kolor
	) const
{
	short l_nCard;
	short l_nCnt = 0;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed())
			continue;
		if (m_cards[l_nCard].GetColor() != a_enColor)
			continue;
		l_nCnt++;
	}
	return l_nCnt;
}


// ---------------------------------------------------------
//	Ile mam wszystkich kart w kolorze
//
short	//WY iloœæ kart
CUserCards::AllCardsInColor(
	T_COLOR a_enColor	//WE sprawdzany kolor
	)	const
{
	short l_nCard;
	short l_nCnt = 0;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].GetColor() != a_enColor)
		{
			continue;
		}
		l_nCnt++;
	}
	return l_nCnt;
}


// ---------------------------------------------------------
//	Karta mniejsza od podanej ale najwiêksza z mo¿liwych 
//	lub pierwsza wiêksza
//
short	//WY numer karty
CUserCards::LessOrFirstBiggerInColor(
	const CCard* a_pCard	//WE porównywana karta
	) const
{
	short l_nCard;
	const CCard* l_pCardLast = NULL;
	short l_nCardLast = 0;
	BOOL l_bHasColor = FALSE;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed())
			continue;
		if (m_cards[l_nCard].GetColor() != a_pCard->GetColor())
			continue;
		l_bHasColor = TRUE;
		if (m_cards[l_nCard] > *a_pCard)
		{
			if (l_pCardLast == NULL)
			{
				ASSERT(!m_cards[l_nCard].IsUsed());
				return l_nCard;
			}
			else
			{
				ASSERT(!m_cards[l_nCardLast].IsUsed());
				return l_nCardLast;
			}
		}
		l_pCardLast = &(m_cards[l_nCard]);
		l_nCardLast = l_nCard;

	}
	ASSERT(l_bHasColor); // nie ma karty w tym kolorze
	ASSERT((l_nCardLast >= 0) && (l_nCardLast < 13));
	ASSERT(!m_cards[l_nCardLast].IsUsed());
	return l_nCardLast;
}


// ---------------------------------------------------------
//	Czy wœród kart jest król kier
//
BOOL	//WY TRUE - jest król kier
CUserCards::HasKingOfHeart() const
{
	short l_nCard;

	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed())
			continue;
		if (m_cards[l_nCard].GetColor() != E_CC_HEART)
			continue;
		if (m_cards[l_nCard].CardValue() == E_CV_K)
			return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------
//	zwraca kolory wed³ug rankingu z najmniejsz¹ 
//	kart¹  w danym kolorze
//
T_COLOR		//WY kolor z najmniejsz¹ kart¹
CUserCards::GetLowestCardColorRank(
	short a_nRank	//WE ranking
	) const
{
	ASSERT((a_nRank >= 1) && (a_nRank <= c_app_btPlayersCnt));
	struct S_RANK
	{
		T_COLOR enColor;
		short	nLowest;
	} l_aColorCnt[4], l_aColorSort[4];

	short l_nAt = 0;
	l_aColorCnt[l_nAt].enColor = E_CC_CLUB;
	l_aColorCnt[l_nAt].nLowest = FirstInColorVal(l_aColorCnt[l_nAt].enColor);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = E_CC_DIAMOND;
	l_aColorCnt[l_nAt].nLowest = FirstInColorVal(l_aColorCnt[l_nAt].enColor);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = E_CC_SPADE;
	l_aColorCnt[l_nAt].nLowest = FirstInColorVal(l_aColorCnt[l_nAt].enColor);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = E_CC_HEART;
	l_aColorCnt[l_nAt].nLowest = FirstInColorVal(l_aColorCnt[l_nAt].enColor);

	// po wszystkich elementach - najd³u¿szy przepisuj
	short l_nSort = 1;
	short l_nMax = -1;
	short l_nMaxPos = -1;
	while (l_nSort <= 4)
	{
		l_nMax = -1;
		for (l_nAt = 1; l_nAt <= 4; l_nAt++)
		{
			if (l_nMax < l_aColorCnt[l_nAt - 1].nLowest)
			{
				l_nMax = l_aColorCnt[l_nAt - 1].nLowest;
				l_nMaxPos = l_nAt;
			}
		}
		l_aColorSort[l_nSort - 1].enColor = l_aColorCnt[l_nMaxPos - 1].enColor;
		l_aColorSort[l_nSort - 1].nLowest = l_aColorCnt[l_nMaxPos - 1].nLowest;
		l_aColorCnt[l_nMaxPos - 1].nLowest = -1;
		l_nSort++;
	}

	return l_aColorSort[4 - a_nRank].enColor;

}


// ---------------------------------------------------------
//	Zwraca kolor w którym pierwsza karta jest najwiêksza 
//	(wedlug rankingu)
//
T_COLOR		//WY kolor z najwiêksz¹ kart¹
CUserCards::GetBiggestFirstCardColorRank(
	short a_nRank	//WE ranking
	) const
{
	return GetLowestCardColorRank(5 - a_nRank);
}


// ---------------------------------------------------------
//	Ile ma mam kart pomiêdzy podanymi (w³¹cznie) razem z u¿ytymi
//
short	//WY iloœæ kart pomiêdzy podanymi
CUserCards::CardsBetween(
	const CCard& a_card1,	//WE pierwsza karta
	const CCard& a_card2	//WE druga karta
	) const
{
	// karty musz¹ byæ w tym samym kolorze
	ASSERT(a_card1.GetColor() == a_card2.GetColor());
	// druga musi byæ wiêksza od pierwszej
	ASSERT(a_card1 < a_card2);
	short l_nCardCnt = 0;
	short l_iCardNr;

	for (l_iCardNr = 0; l_iCardNr < 13; l_iCardNr++)
	{
		if (!(m_cards[l_iCardNr] == a_card1))
			continue;
		if (m_cards[l_iCardNr].IsUsed())
			continue;
		if (m_cards[l_iCardNr] < a_card1) 
			continue;
		if (m_cards[l_iCardNr] > a_card2) 
			continue;
		l_nCardCnt++;
	}
	return l_nCardCnt;
}


// ---------------------------------------------------------
//	Ile ma mam kart pomiêdzy podanymi (w³¹cznie) bez u¿ytych
//
short	//WY iloœæ kart
CUserCards::CardsBetweenAll(
	const CCard& a_card1,	//WE karta pierwsza
	const CCard& a_card2	//WE karta druga
	) const
{
	// karty musz¹ byæ w tym samym kolorze
	ASSERT(a_card1.GetColor() == a_card2.GetColor());
	// druga musi byæ wiêksza od pierwszej
	ASSERT(a_card1 < a_card2);
	short l_nCardCnt = 0;
	short l_iCardNr;

	for (l_iCardNr = 0; l_iCardNr < 13; l_iCardNr++)
	{
		if (!(m_cards[l_iCardNr] == a_card1))
			continue;
		if (m_cards[l_iCardNr] < a_card1) 
			continue;
		if (m_cards[l_iCardNr] > a_card2) 
			continue;
		l_nCardCnt++;
	}
	return l_nCardCnt;
}


// ---------------------------------------------------------
//	Ile kart w najkrótszym kolorze bez podanego koloru z u¿ytymi
//
short	//WY iloœc kart w najkrótszym kolorze
CUserCards::InShortestColorAllNoOneColor(
	T_COLOR a_enColor	//WE niesprawdzany kolor
	)	const
{
	short l_nRank;
	T_COLOR l_enColor;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = GetShortestColorAllRank(l_nRank);
		if (l_enColor == a_enColor)
			continue;
		return ColorAllCnt(l_enColor); 
	}
	// tu nie mo¿emy siê znaleŸæ
	ASSERT(FALSE);
	return -1;
}


// ---------------------------------------------------------
//	Ile kart w najkrótszym kolorze bez podanego koloru bez
//	u¿ytych
//
short	//WY iloœæ w kolorze
CUserCards::InShortestColorNoOneColor(
	T_COLOR a_enColor	//WE kolor nieuwzglêdniany
	)	const
{
	short l_nRank;
	T_COLOR l_enColor;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = GetShortestColorRank(l_nRank);
		if (l_enColor == a_enColor)
		{
			continue;
		}
		return ColorCnt(l_enColor); 
	}
	// tu nie mo¿emy siê znaleŸæ
	ASSERT(FALSE);
	return -1;
}


// ---------------------------------------------------------
//	Czy ma wszystkie karty o podanym kolorze z wymienionej listy, 
//	a_bUsed okreœla czy nie liczyæ kart u¿ytych
//
BOOL	//WY TRUE - ma wszystkie
CUserCards::HasCards(
	T_COLOR a_enColor,	//WE sprawdzany kolor
	BOOL a_bUsed,		//WE TRUE - bez uwzglêdniania u¿ytych
	short a_nCnt,		//WE iloœæ sprawdzanych kart
	...					//WE sprawdzane karty typu T_CARDVAL
	)	const
{

	ASSERT((a_nCnt >= 1) && (a_nCnt <= 13));
	short l_nArg;
	T_CARDVAL l_cv;

	va_list l_vaList;
	va_start(l_vaList, a_nCnt);
	for (l_nArg = 1; l_nArg <= a_nCnt; l_nArg++)
	{
		l_cv = va_arg(l_vaList, T_CARDVAL);
		if (FindCard(a_enColor, l_cv, a_bUsed) < 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}


// ---------------------------------------------------------
//	Czy ma choæ jedn¹ kartê o podanym kolorze z wymienionej listy, 
//	a_bUsed okreœla czy nie liczyæ kart u¿ytych
//
BOOL	//WY TRUE - ma choæ jedn¹ kartê
CUserCards::HasOneOfCards(
	T_COLOR a_enColor,	//WE kolor
	BOOL a_bUsed,		//WE TRUE - nie uwzglêdniæ u¿ytych
	short a_nCnt,		//WE iloœæ sprawdzanych kart
	...					//WE sprawdzane karty (T_CARDVAL)
	)	const
{

	ASSERT((a_nCnt >= 1) && (a_nCnt <= 13));
	short l_nArg;
	T_CARDVAL l_cv;

	va_list l_vaList;
	va_start(l_vaList, a_nCnt);
	for (l_nArg = 1; l_nArg <= a_nCnt; l_nArg++)
	{
		l_cv = va_arg(l_vaList, T_CARDVAL);
		if (FindCard(a_enColor, l_cv, a_bUsed) >= 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}


// ---------------------------------------------------------
//	Czy ma kolor w którym jest dok³adnie tyle kart co wymieniono
//
T_COLOR		//WY znaleziony kolor lub E_CC_NOTHING
CUserCards::GetColorExactNumNoOneColor(
	short a_nNum,		//WE szukana liczba kart
	T_COLOR a_enColor,	//WE oprócz koloru
	BOOL a_bUsed		//WE TRUE - nie uwzglêdniaæ u¿ywanych
	)	const
{
	short l_nCnt;
	T_COLOR l_enColor = E_CC_CLUB;
	l_nCnt = ColorCnt(a_enColor, a_bUsed);
	if (
		(l_nCnt == a_nNum) &&
		(l_enColor != a_enColor)
	   )
	   return l_enColor;

	l_enColor = E_CC_SPADE;
	l_nCnt = ColorCnt(a_enColor, a_bUsed);
	if (
		(l_nCnt == a_nNum) &&
		(l_enColor != a_enColor)
	   )
	   return l_enColor;

	l_enColor = E_CC_DIAMOND;
	l_nCnt = ColorCnt(a_enColor, a_bUsed);
	if (
		(l_nCnt == a_nNum) &&
		(l_enColor != a_enColor)
	   )
	   return l_enColor;

	l_enColor = E_CC_HEART;
	l_nCnt = ColorCnt(a_enColor, a_bUsed);
	if (
		(l_nCnt == a_nNum) &&
		(l_enColor != a_enColor)
	   )
	   return l_enColor;

	return E_CC_NOTHING;
}


// ---------------------------------------------------------
//	Znajduje pierwsz¹ wiêksz¹ kartê od podanej lub 
//	najmniejsz¹. 
//	Zak³ada, ¿e karty posortowane
//
short	//WY numer karty
CUserCards::FirstBiggerOrLeastInColor(
	const CCard* a_pCard	//WE porównywana karta
	)	const
{
	short l_nCard;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed())
		{
			continue;
		}

		if (m_cards[l_nCard] > *a_pCard)
		{
			return l_nCard;
		}
	}
	// nie ma wiêkszej, to gramy najmniejsz¹
	return FirstInColor(a_pCard->GetColor());
}



// ---------------------------------------------------------
//	Czy ma kartê do zagrania w puzzle
//
BOOL	//WY TRUE - mamy karte do zagrania
CUserCards::HasCardForPuzzle(
	const CPuzzleRows* a_pPuzzleRow	// wszystkie ulozone wiersze 
	) const
{
	if (HasCardForPuzzleColor(a_pPuzzleRow, E_CC_HEART))
		return TRUE;
	if (HasCardForPuzzleColor(a_pPuzzleRow, E_CC_DIAMOND))
		return TRUE;
	if (HasCardForPuzzleColor(a_pPuzzleRow, E_CC_SPADE))
		return TRUE;
	if (HasCardForPuzzleColor(a_pPuzzleRow, E_CC_CLUB))
		return TRUE;
	return FALSE;
}


// ---------------------------------------------------------
//	Czy ma kartê do zagrania w puzzle w podanym kolorze
//
BOOL	//WY TRUE: jest karta do zagrania
CUserCards::HasCardForPuzzleColor(
	const CPuzzleRows* a_pPuzzleRow,	//WE wszystkie wiersze
	T_COLOR a_enColor					//WE sprawdzany kolor
	) const
{
	const CCard* l_pCard;
	l_pCard = a_pPuzzleRow->m_arPuzzleRow[a_enColor - 1].m_pCardTop;
	if (l_pCard == NULL)
	{
		if (FindCard(a_enColor, E_CV_8) >= 0)
			return TRUE;
	}
	else
	{
		if (FindCard(a_enColor, (T_CARDVAL)(l_pCard->CardValue() + 1)) >= 0)
			return TRUE;
	}

	l_pCard = a_pPuzzleRow->m_arPuzzleRow[a_enColor - 1].m_pCardBottom;
	if (l_pCard == NULL)
	{
		if (FindCard(a_enColor, E_CV_8) >= 0)
			return TRUE;
	}
	else
	{
		if (FindCard(a_enColor, (T_CARDVAL)(l_pCard->CardValue() - 1)) >= 0)
			return TRUE;
	}
	return FALSE;
}


//FN+ -----------------------------------------------------------
//	Rozgrywka puzzli - funkcja rekurencyjna
//
short	//WY ile kart zosta³o
CUserCards::CardsLeft() const
//FN-
{
	short l_nCnt = 0;
	short l_nCard;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed())
			continue;
		l_nCnt++;
	}
	return l_nCnt;
}


// -----------------------------------------------------------
// Ustaw kartê jako u¿yt¹
//
void 
CUserCards::SetUsed(
	short a_nCard,	//WE numer karty
	BOOL a_bUsed	//WE czy u¿yta
	)
{
	m_cards[a_nCard].SetUsed(a_bUsed);
}


// -----------------------------------------------------------
// Zwraca czy karta ju¿ u¿yta
//
BOOL	//WY TRUE - u¿yta
CUserCards::IsUsed(
	short a_nCardNr		//WE numer karty
	) const
{
	return m_cards[a_nCardNr].IsUsed();	
}


// -----------------------------------------------------------
// Zwraca kartê
//
const CCard&	//WY karta
CUserCards::GetCard(
	short a_nCard	//WE która karta
	) const
{
	ASSERT(a_nCard >= 0);
	ASSERT(a_nCard < 13);
	return m_cards[a_nCard];	
}


// ---------------------------------------------------------
// Checks if we have least card in color (or we don't 
// have a color)
//
BOOL 
CUserCards::HasLeastInColor(
	T_COLOR a_enColor, 
	const CTakenTricks* a_pTricks
	) const
{
	const CCard* l_pLeastCard = FirstInColorCard(a_enColor);
	if (l_pLeastCard == NULL)
	{
		return TRUE;
	}
	return (!a_pTricks->AreLessInColor(l_pLeastCard, this));

}
