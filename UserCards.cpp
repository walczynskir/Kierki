// UserCards.cpp: implementation of the CUserCards class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserCards.h"
#include "TakenTricks.h"
#include <algorithm>

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
//	Najm�odsza karta w najkr�tszym kolorze
//
short	//WY numer karty w r�ce
CUserCards::FirstInShortest()	const
{
	// znajd� najm�odsz� kart� w najkr�tszym kolorze
	T_COLOR l_enShortest = ShortestColor() ;
	return FirstInColor(l_enShortest) ;
}


// ---------------------------------------------------------
//	Pierwsza karta w kolorze
//
short	//WY numer najm�odszej karty w kolorze
CUserCards::FirstInColor(
	T_COLOR a_enShortest	//WE kolor w kt�rym szukamy
	)	const
{
	// znajd� najm�odsz� kart� w najkr�tszym kolorze
	short l_nAt ;

	for (l_nAt = 0; l_nAt < 13; l_nAt++)
	{
		if (!m_cards[l_nAt].IsUsed())
		{
			if (m_cards[l_nAt].GetSuit() == a_enShortest)
			{
				return l_nAt;
			}
		}
	}
	ASSERT(FALSE) ;
	return -1 ;
}


// ---------------------------------------------------------
//	Pierwsza karta w kolorze - warto��
//
T_CARDVAL	//WY warto�� najmniejszej karty
CUserCards::FirstInColorVal(
	T_COLOR a_enColor	//WE kolor
	)	const
{
	// znajd� najm�odsz� kart� w najkr�tszym kolorze
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
//	Pierwsza karta w kolorze - warto��
//
const CCard* 
CUserCards::FirstInColorCard(
	T_COLOR a_enColor	//WE kolor
	)	const
{
	// znajd� najm�odsz� kart� w najkr�tszym kolorze
	short l_iAt ;

	for (l_iAt = 0; l_iAt < 13; l_iAt++)
	{
		if (!m_cards[l_iAt].IsUsed())
		{
			if (m_cards[l_iAt].GetSuit() == a_enColor)
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
 	short a_nStart,	// od kt�rej zacz�� sortowanie
	short a_nEnd	// na kt�rej sko�czy�
	)
{
#pragma message(HERE "zmie� algorytm sortowania")
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

void CUserCards::SortSTL(short a_nStart, short a_nEnd)
{
	if (a_nStart < 0 || a_nEnd >= 13 || a_nStart > a_nEnd)
		return; // bounds check

	std::sort(
		m_cards + a_nStart,
		m_cards + a_nEnd + 1,
		[](const CCard& lhs, const CCard& rhs) {
			return lhs.GetNr() < rhs.GetNr();
		}
	);
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
//	Zwraca kart�
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
	short a_nAt,		//WE kt�ra karta
	short a_nCardNr,	//WE numer karty
	T_PLAYER a_enOwner,	//WE w�a�ciciel
	BOOL a_bUsed		//WE czy u�yta
	)
{
	m_cards[a_nAt].SetCard(a_nCardNr, a_enOwner, a_bUsed) ;
}


// ---------------------------------------------------------
//	Najkr�tszy kolor w kartach
//
T_COLOR		//WY kolor najkr�tszy
CUserCards::ShortestColor()	const
{

	short l_iClubsCnt = ColorCnt(Suit::Club);
	short l_iDiamondsCnt = ColorCnt(Suit::Diamond);
	short l_iSpadesCnt = ColorCnt(Suit::Spade);
	short l_iHeartsCnt = ColorCnt(Suit::Heart);

	short l_iShortest = l_iClubsCnt;
	T_COLOR l_enColor = Suit::Club ;

	if (l_iShortest == 0)
	{
		l_iShortest = l_iDiamondsCnt ;
		l_enColor = Suit::Diamond ;
	}
	if (l_iShortest == 0)
	{
		l_iShortest = l_iSpadesCnt ;
		l_enColor = Suit::Spade ;
	}
	if (l_iShortest == 0)
	{
		l_iShortest = l_iHeartsCnt ;
		l_enColor = Suit::Heart ;
	}
	ASSERT(l_iShortest > 0) ;

	if ((l_iDiamondsCnt < l_iShortest) && (l_iDiamondsCnt > 0))
	{
		l_enColor = Suit::Diamond ;
		l_iShortest = l_iDiamondsCnt ;
	}

	if ((l_iSpadesCnt < l_iShortest) && (l_iSpadesCnt > 0))
	{
		l_enColor = Suit::Spade ;
		l_iShortest = l_iSpadesCnt ;
	}

	if ((l_iHeartsCnt < l_iShortest) && (l_iHeartsCnt > 0))
	{
		l_enColor = Suit::Heart ;
		l_iShortest = l_iHeartsCnt ;
	}

	return l_enColor ;

}


// ---------------------------------------------------------
//	Ile user ma w podanym kolorze kart
//
short	//WY ile kart w kartach u�ytk. w podanym kolorze
CUserCards::ColorCnt(
	T_COLOR a_enColor,	//WE kolor 
	BOOL a_bUsed		//WE TRUE nie uwzgl�dniaj u�ytych
	)	const
{
	short l_nCnt = 0 ;
	short l_nAt ;
	for (l_nAt = 0; l_nAt < 13; l_nAt++)
	{	
		if (m_cards[l_nAt].IsUsed() && a_bUsed)
			continue;

		if (m_cards[l_nAt].GetSuit() == a_enColor)
			l_nCnt++ ;
	}
	return l_nCnt ;
}


// ---------------------------------------------------------
//	Ile user ma w podanym kolorze kart uwzgl�dniaj�c zu�yte
//
short	//WY ilo�� kart w kolorze z uzytymi
CUserCards::ColorAllCnt(
	T_COLOR a_enColor	//WE sprawdzany kolor
	)	const
{
	return ColorCnt(a_enColor, FALSE);
}


// ---------------------------------------------------------
//	Najkr�tszy kolor niekierowy
//
T_COLOR		//WY najkr�tszy niekierowy kolor
CUserCards::ShortestColorNoHearts() const
{

	short l_iClubsCnt = ColorCnt(Suit::Club);
	short l_iDiamondsCnt = ColorCnt(Suit::Diamond);
	short l_iSpadesCnt = ColorCnt(Suit::Spade);

	short l_iShortest = l_iClubsCnt;
	T_COLOR l_enColor = Suit::Club ;

	if (l_iShortest == 0)
	{
		l_iShortest = l_iDiamondsCnt ;
		l_enColor = Suit::Diamond ;
	}
	if (l_iShortest == 0)
	{
		l_iShortest = l_iSpadesCnt ;
		l_enColor = Suit::Spade ;
	}
	ASSERT(l_iShortest > 0) ;

	if ((l_iDiamondsCnt < l_iShortest) && (l_iDiamondsCnt > 0))
	{
		l_enColor = Suit::Diamond ;
		l_iShortest = l_iDiamondsCnt ;
	}

	if ((l_iSpadesCnt < l_iShortest) && (l_iSpadesCnt > 0))
	{
		l_enColor = Suit::Spade ;
		l_iShortest = l_iSpadesCnt ;
	}

	return l_enColor ;
}


// ---------------------------------------------------------
//	Czy s� karty nie "kiery"
//
BOOL	//WY s� nie kiery
CUserCards::AreNoHearts() const
{
	short l_iAt ;
	for (l_iAt = 0; l_iAt <= 12; l_iAt++)
	{	
		if (!(m_cards[l_iAt].IsUsed()))
		{
			if (m_cards[l_iAt].GetSuit() != Suit::Heart)
			{
				return TRUE;
			}
		}
	}
	return FALSE ;

}


// ---------------------------------------------------------
//	Zwraca najwi�ksz� kart� w kolorze
//
short	//WY numer karty najwiekszej w kolorze
CUserCards::LastInColor(
	T_COLOR a_enColor	//WE badany kolor
	) const
{
	// znajd� najstarsz� kart� w kolorze
	// karty ju� pouk�adane
	short l_iAt ;

	for (l_iAt = 13; l_iAt > 0; l_iAt--)
	{
		if (!m_cards[l_iAt - 1].IsUsed())
		{
			if (m_cards[l_iAt - 1].GetSuit() == a_enColor)
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
short	//WY numer karty w r�ce
CUserCards::LessOrBiggestInColor(
	const CTrick& a_trick	//lewa sprawdzana
	)	const
{
	// znajd� pierwsz� mniejsz� lub najwi�ksz� w kolorze
	// je�li nie masz - wyrzu� najwy�sz� w najkr�tszym kolorze
	// karty ju� pouk�adane
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
// Czy jest kolor podany. Uwzgl�dniany stan u�ycia - nie 
// liczy si� kart u�ytych.
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

		if (m_cards[l_iCard].GetSuit() != a_enColor)
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
short	//WE pozycja najwi�kszej karty w kolorze
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

		if (m_cards[l_nCard - 1].GetSuit() != a_enColor)
		{
			continue;
		}
		return l_nCard - 1;
	}

	// nie ma najwi�kszej w kolorze
	ASSERT(FALSE);
	return -1;
}


// ---------------------------------------------------------
//	Najwieksza w kolorze
//
const CCard*	//WY karta najwi�ksza w kolorze
CUserCards::BiggestInColorCard(
	T_COLOR a_enColor	//WE sprawdzany kolor
	)	const
{
	return BiggestInColorCardRank(a_enColor, 1);
}


// ---------------------------------------------------------
//	Najwieksza w kolorze wed�ug rankingu
//
const CCard*	//WY karta najwy�sza w kolorze
CUserCards::BiggestInColorCardRank(
	T_COLOR a_enColor,	//WE sprawdzany kolor
	short a_nRank		//WE kt�ra najwy�sza
	)	const
{
	short l_iCnt = 0;
	short l_iCardNr;
	for (l_iCardNr = 13; l_iCardNr > 0; l_iCardNr--)
	{
		if (m_cards[l_iCardNr - 1].IsUsed())
			continue ;

		if (m_cards[l_iCardNr - 1].GetSuit() != a_enColor)
			continue ;
		l_iCnt++;
		if (l_iCnt != a_nRank)
			continue;
		return &(m_cards[l_iCardNr - 1]);
	}

	// nie ma najwi�kszej w kolorze
	return NULL ;
}




// ---------------------------------------------------------
//	Najwieksza w kolorze poni�ej podanej
//
short	//WY numer karty
CUserCards::GetBiggestBelow(
	T_COLOR a_enColor,	//WE kolor
	T_CARDVAL a_cv		//WE warto�� karty
	) const
{
	short l_iCard;
	for (l_iCard = 13; l_iCard > 0; l_iCard--)
	{
		if (m_cards[l_iCard - 1].IsUsed())
			continue;
		if (m_cards[l_iCard - 1].GetSuit() != a_enColor)
			continue;
		if (m_cards[l_iCard - 1].CardValue() <	a_cv)
			return l_iCard - 1;
	}
	return -1;
}


// ---------------------------------------------------------
//	Znajd� podan� kart�
//	a_bUsed, okre�la czy karty u�yte nie s� brane pod uwag�
//
short	//WY numer karty w r�ce
CUserCards::FindCard(
	T_COLOR a_enColor,	//WE kolor
	T_CARDVAL a_cv,		//WE warto�� karty
	BOOL a_bUsed		//WE TRUE - bez u�ywanych
	) const
{
	short l_nCard;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed() == a_bUsed)
			continue;
		if (m_cards[l_nCard].GetSuit() != a_enColor)
			continue;
		if (m_cards[l_nCard].CardValue() != a_cv)
			continue;
		return l_nCard;
	}
	return -1;
}


// ---------------------------------------------------------
//	Znajd� podan� kart�
//	a_bUsed, okre�la czy karty u�yte nie s� brane pod uwag�
//
short	//WY numer karty w r�ce lub -1, gdy nie ma 
CUserCards::FindCard(
	const CCard& a_card,	//WE karta szukana
	BOOL a_bUsed	//WE TRUE - szuka� tylko w nieu�ytych
	)	const
{
	return FindCard(&a_card, a_bUsed);
}


// ---------------------------------------------------------
//	Znajd� podan� kart�
//	a_bUsed, okre�la czy karty u�yte nie s� brane pod uwag�
//
short	//WY numer karty w r�ce lub -1, gdy nie ma 
CUserCards::FindCard(
	const CCard* a_pCard,	//WE karta szukana
	BOOL a_bUsed	//WE TRUE - szuka� tylko w nieu�ytych
	)	const
{
	short l_nCard;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed() == a_bUsed)
			continue;
		if (m_cards[l_nCard].GetSuit() != a_pCard->GetSuit())
			continue;
		if (m_cards[l_nCard].CardValue() != a_pCard->CardValue())
			continue;
		return l_nCard;
	}
	return -1;
}


// ---------------------------------------------------------
//	Zwraca kolor najkr�tszy wed�ug rankingu
//	Ranking 1 oznacza zwr�� kolor w kt�rym jest najmniej kart
//	Ranking 2 oznacza 2 z kolei kolor w kt�rym jest najmniej 
//	kart, itd.
//
T_COLOR		//WY njkr�tszy kolor wed�ug rankingu
CUserCards::GetShortestColorRank(
	short a_nRank,	//WE ranking
	BOOL a_bUsed	//WE TRUE - nie uwzgl�dnia� u�ytych
	)	const
{
	ASSERT((a_nRank >= 1) && (a_nRank <= 4));
	struct S_RANK
	{
		T_COLOR enColor;
		short	nLeft;
	} l_aColorCnt[4], l_aColorSort[4];

	short l_nAt = 0;
	l_aColorCnt[l_nAt].enColor = Suit::Club;
	l_aColorCnt[l_nAt].nLeft = ColorCnt(l_aColorCnt[l_nAt].enColor, a_bUsed);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = Suit::Diamond;
	l_aColorCnt[l_nAt].nLeft = ColorCnt(l_aColorCnt[l_nAt].enColor, a_bUsed);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = Suit::Spade;
	l_aColorCnt[l_nAt].nLeft = ColorCnt(l_aColorCnt[l_nAt].enColor, a_bUsed);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = Suit::Heart;
	l_aColorCnt[l_nAt].nLeft = ColorCnt(l_aColorCnt[l_nAt].enColor, a_bUsed);

	// po wszystkich elementach - najd�u�szy przepisuj
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
//	Zwraca kolor najkr�tszy wed�ug rankingu uwzgl�dniaj�c 
//	wzi�te karty.
//	Ranking 1 oznacza zwr�� kolor w kt�rym jest najmniej kart
//	Ranking 2 oznacza 2 z kolei kolor w kt�rym jest najmniej 
//	kart, itd.
//
T_COLOR		//WY znaleziony kolor
CUserCards::GetShortestColorAllRank(
	short a_nRank	//WE kt�ry najkr�tszy
	)	const
{
	return GetShortestColorRank(a_nRank, FALSE);
}


// ---------------------------------------------------------
//	Zwraca kolor najkr�tszy wed�ug rankingu
//	Ranking 1 oznacza zwr�� kolor w kt�rym jest najmniej kart
//	Ranking 2 oznacza 2 z kolei kolor w kt�rym jest najmniej 
//	kart, itd.
//
T_COLOR		//WY njkr�tszy kolor wed�ug rankingu
CUserCards::GetLongestColorRank(
	short a_nRank,	//WE ranking
	BOOL a_bUsed	//WE TRUE - nie uwzgl�dnia� u�ytych
	)	const
{
	return GetShortestColorRank(5 - a_nRank, a_bUsed);
}


// ---------------------------------------------------------
//	Czy posiada kolor r�ny od kier�w
//
BOOL	//WY TRUE - posiada
CUserCards::HasNoHeartsColor() const
{
	if (HasColor(Suit::Club))
	{
		return TRUE;
	}
	if (HasColor(Suit::Spade))
	{
		return TRUE;
	}
	if (HasColor(Suit::Diamond))
	{
		return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------
//	Czy user ma jeszcze nie u�yt� karte o podanym numerze
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
//	Ile mam nieu�ytych kart w kolorze
//
short	//WY ilos� kart w kolorze
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
		if (m_cards[l_nCard].GetSuit() != a_enColor)
			continue;
		l_nCnt++;
	}
	return l_nCnt;
}


// ---------------------------------------------------------
//	Ile mam wszystkich kart w kolorze
//
short	//WY ilo�� kart
CUserCards::AllCardsInColor(
	T_COLOR a_enColor	//WE sprawdzany kolor
	)	const
{
	short l_nCard;
	short l_nCnt = 0;
	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].GetSuit() != a_enColor)
		{
			continue;
		}
		l_nCnt++;
	}
	return l_nCnt;
}


// ---------------------------------------------------------
//	Karta mniejsza od podanej ale najwi�ksza z mo�liwych 
//	lub pierwsza wi�ksza
//
short	//WY numer karty
CUserCards::LessOrFirstBiggerInColor(
	const CCard* a_pCard	//WE por�wnywana karta
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
		if (m_cards[l_nCard].GetSuit() != a_pCard->GetSuit())
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
//	Czy w�r�d kart jest kr�l kier
//
BOOL	//WY TRUE - jest kr�l kier
CUserCards::HasKingOfHeart() const
{
	short l_nCard;

	for (l_nCard = 0; l_nCard < 13; l_nCard++)
	{
		if (m_cards[l_nCard].IsUsed())
			continue;
		if (m_cards[l_nCard].GetSuit() != Suit::Heart)
			continue;
		if (m_cards[l_nCard].CardValue() == E_CV_K)
			return TRUE;
	}
	return FALSE;
}


// ---------------------------------------------------------
//	zwraca kolory wed�ug rankingu z najmniejsz� 
//	kart�  w danym kolorze
//
T_COLOR		//WY kolor z najmniejsz� kart�
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
	l_aColorCnt[l_nAt].enColor = Suit::Club;
	l_aColorCnt[l_nAt].nLowest = FirstInColorVal(l_aColorCnt[l_nAt].enColor);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = Suit::Diamond;
	l_aColorCnt[l_nAt].nLowest = FirstInColorVal(l_aColorCnt[l_nAt].enColor);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = Suit::Spade;
	l_aColorCnt[l_nAt].nLowest = FirstInColorVal(l_aColorCnt[l_nAt].enColor);
	l_nAt++;

	l_aColorCnt[l_nAt].enColor = Suit::Heart;
	l_aColorCnt[l_nAt].nLowest = FirstInColorVal(l_aColorCnt[l_nAt].enColor);

	// po wszystkich elementach - najd�u�szy przepisuj
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
//	Zwraca kolor w kt�rym pierwsza karta jest najwi�ksza 
//	(wedlug rankingu)
//
T_COLOR		//WY kolor z najwi�ksz� kart�
CUserCards::GetBiggestFirstCardColorRank(
	short a_nRank	//WE ranking
	) const
{
	return GetLowestCardColorRank(5 - a_nRank);
}


// ---------------------------------------------------------
//	Ile ma mam kart pomi�dzy podanymi (w��cznie) razem z u�ytymi
//
short	//WY ilo�� kart pomi�dzy podanymi
CUserCards::CardsBetween(
	const CCard& a_card1,	//WE pierwsza karta
	const CCard& a_card2	//WE druga karta
	) const
{
	// karty musz� by� w tym samym kolorze
	ASSERT(a_card1.GetSuit() == a_card2.GetSuit());
	// druga musi by� wi�ksza od pierwszej
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
//	Ile ma mam kart pomi�dzy podanymi (w��cznie) bez u�ytych
//
short	//WY ilo�� kart
CUserCards::CardsBetweenAll(
	const CCard& a_card1,	//WE karta pierwsza
	const CCard& a_card2	//WE karta druga
	) const
{
	// karty musz� by� w tym samym kolorze
	ASSERT(a_card1.GetSuit() == a_card2.GetSuit());
	// druga musi by� wi�ksza od pierwszej
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
//	Ile kart w najkr�tszym kolorze bez podanego koloru z u�ytymi
//
short	//WY ilo�c kart w najkr�tszym kolorze
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
	// tu nie mo�emy si� znale��
	ASSERT(FALSE);
	return -1;
}


// ---------------------------------------------------------
//	Ile kart w najkr�tszym kolorze bez podanego koloru bez
//	u�ytych
//
short	//WY ilo�� w kolorze
CUserCards::InShortestColorNoOneColor(
	T_COLOR a_enColor	//WE kolor nieuwzgl�dniany
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
	// tu nie mo�emy si� znale��
	ASSERT(FALSE);
	return -1;
}


// ---------------------------------------------------------
//	Czy ma wszystkie karty o podanym kolorze z wymienionej listy, 
//	a_bUsed okre�la czy nie liczy� kart u�ytych
//
BOOL	//WY TRUE - ma wszystkie
CUserCards::HasCards(
	T_COLOR a_enColor,	//WE sprawdzany kolor
	BOOL a_bUsed,		//WE TRUE - bez uwzgl�dniania u�ytych
	short a_nCnt,		//WE ilo�� sprawdzanych kart
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
//	Czy ma cho� jedn� kart� o podanym kolorze z wymienionej listy, 
//	a_bUsed okre�la czy nie liczy� kart u�ytych
//
BOOL	//WY TRUE - ma cho� jedn� kart�
CUserCards::HasOneOfCards(
	T_COLOR a_enColor,	//WE kolor
	BOOL a_bUsed,		//WE TRUE - nie uwzgl�dni� u�ytych
	short a_nCnt,		//WE ilo�� sprawdzanych kart
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
//	Czy ma kolor w kt�rym jest dok�adnie tyle kart co wymieniono
//
T_COLOR		//WY znaleziony kolor lub Suit::Nothing
CUserCards::GetColorExactNumNoOneColor(
	short a_nNum,		//WE szukana liczba kart
	T_COLOR a_enColor,	//WE opr�cz koloru
	BOOL a_bUsed		//WE TRUE - nie uwzgl�dnia� u�ywanych
	)	const
{
	short l_nCnt;
	T_COLOR l_enColor = Suit::Club;
	l_nCnt = ColorCnt(a_enColor, a_bUsed);
	if (
		(l_nCnt == a_nNum) &&
		(l_enColor != a_enColor)
	   )
	   return l_enColor;

	l_enColor = Suit::Spade;
	l_nCnt = ColorCnt(a_enColor, a_bUsed);
	if (
		(l_nCnt == a_nNum) &&
		(l_enColor != a_enColor)
	   )
	   return l_enColor;

	l_enColor = Suit::Diamond;
	l_nCnt = ColorCnt(a_enColor, a_bUsed);
	if (
		(l_nCnt == a_nNum) &&
		(l_enColor != a_enColor)
	   )
	   return l_enColor;

	l_enColor = Suit::Heart;
	l_nCnt = ColorCnt(a_enColor, a_bUsed);
	if (
		(l_nCnt == a_nNum) &&
		(l_enColor != a_enColor)
	   )
	   return l_enColor;

	return Suit::Nothing;
}


// ---------------------------------------------------------
//	Znajduje pierwsz� wi�ksz� kart� od podanej lub 
//	najmniejsz�. 
//	Zak�ada, �e karty posortowane
//
short	//WY numer karty
CUserCards::FirstBiggerOrLeastInColor(
	const CCard* a_pCard	//WE por�wnywana karta
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
	// nie ma wi�kszej, to gramy najmniejsz�
	return FirstInColor(a_pCard->GetSuit());
}



// ---------------------------------------------------------
//	Czy ma kart� do zagrania w puzzle
//
BOOL	//WY TRUE - mamy karte do zagrania
CUserCards::HasCardForPuzzle(
	const CPuzzleRowSet* a_pPuzzleRow	// wszystkie ulozone wiersze 
	) const
{
	if (HasCardForPuzzleColor(a_pPuzzleRow, Suit::Heart))
		return TRUE;
	if (HasCardForPuzzleColor(a_pPuzzleRow, Suit::Diamond))
		return TRUE;
	if (HasCardForPuzzleColor(a_pPuzzleRow, Suit::Spade))
		return TRUE;
	if (HasCardForPuzzleColor(a_pPuzzleRow, Suit::Club))
		return TRUE;
	return FALSE;
}


// ---------------------------------------------------------
//	Czy ma kart� do zagrania w puzzle w podanym kolorze
//
BOOL	//WY TRUE: jest karta do zagrania
CUserCards::HasCardForPuzzleColor(
	const CPuzzleRowSet* a_pPuzzleRow,	//WE wszystkie wiersze
	Suit a_suit					//WE sprawdzany kolor
	) const
{
	const CCard* l_pCard;
	l_pCard = (*a_pPuzzleRow)[a_suit].m_pCardTop;
	if (l_pCard == NULL)
	{
		if (FindCard(a_suit, E_CV_8) >= 0)
			return TRUE;
	}
	else
	{
		if (FindCard(a_suit, (T_CARDVAL)(l_pCard->CardValue() + 1)) >= 0)
			return TRUE;
	}

	l_pCard = (*a_pPuzzleRow)[a_suit].m_pCardBottom;
	if (l_pCard == NULL)
	{
		if (FindCard(a_suit, E_CV_8) >= 0)
			return TRUE;
	}
	else
	{
		if (FindCard(a_suit, (T_CARDVAL)(l_pCard->CardValue() - 1)) >= 0)
			return TRUE;
	}
	return FALSE;
}


//FN+ -----------------------------------------------------------
//	Rozgrywka puzzli - funkcja rekurencyjna
//
short	//WY ile kart zosta�o
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
// Ustaw kart� jako u�yt�
//
void 
CUserCards::SetUsed(
	short a_nCard,	//WE numer karty
	BOOL a_bUsed	//WE czy u�yta
	)
{
	m_cards[a_nCard].SetUsed(a_bUsed);
}


// -----------------------------------------------------------
// Zwraca czy karta ju� u�yta
//
BOOL	//WY TRUE - u�yta
CUserCards::IsUsed(
	short a_nCardNr		//WE numer karty
	) const
{
	return m_cards[a_nCardNr].IsUsed();	
}


// -----------------------------------------------------------
// Zwraca kart�
//
const CCard&	//WY karta
CUserCards::GetCard(
	short a_nCard	//WE kt�ra karta
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
