// RecoverDecider.cpp: implementation of the CRecoverDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecoverDecider.h"


#include <time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecoverDecider::CRecoverDecider(
	const CUserCards* a_pUserCards,	//WE karty gracza
	const CTakenTricks* a_pTricks,	//WE wziête lewy
	T_PLAYER a_enPlayer,			//WE gracz
	T_COLOR a_enTrumps				//WE kolor atu
	)
{
	m_pCards = a_pUserCards;	
	m_pTricks = a_pTricks;
	m_enPlayer = a_enPlayer;
	m_enTrumps = a_enTrumps;
}


// ---------------------------------------------------------
//	Destruktor
//
CRecoverDecider::~CRecoverDecider()
{

}


// ---------------------------------------------------------
//	Zwraca kartê, któr¹ nale¿y dorzuciæ
//
short	//WY karta w rêce
CRecoverDecider::GetCardNr(
	short a_nTrick	//WE która lewa
	)	const
{
	short l_nCardNr ;
	if (a_nTrick == 13)
	{
		return m_pCards->FirstInShortest();
	}

	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1] ;

	if (m_enTrumps == Suit::NoTrumps)
	{
		l_nCardNr = (l_trickCurrent.GetCardsCnt() == 0 ? FirstCardNoTrumps(a_nTrick) : NextCardNoTrumps(a_nTrick));
	}
	else
	{
		l_nCardNr = (l_trickCurrent.GetCardsCnt() == 0 ? FirstCard(a_nTrick) : NextCard(a_nTrick));
	}

	ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13)) ;
	ASSERT(!(*m_pCards)[l_nCardNr].IsUsed());
	return l_nCardNr;
}


// ---------------------------------------------------------
//	Wist
//
short	//WY karta w rêce
CRecoverDecider::FirstCard(
	short a_nTrick	//WE która lewa
	)	const
{
	// je¿eli mamy tylko mniej ni¿ trzy karty w atucie
	// i je¿eli te karty to karty poni¿ej J
	// i je¿eli nie mamy ¿adnego koloru w którym mieliœmy 
	// mniej ni¿ dwie karty, to gramy w najni¿sze atu
	if  (
		(m_pCards->HasColor(m_enTrumps)) &&
		(m_pCards->ColorAllCnt(m_enTrumps) < 3) && 
		(m_pCards->BiggestInColorCard(m_enTrumps)->CardValue() < E_CV_J) &&
		(m_pCards->InShortestColorAllNoOneColor(m_enTrumps) > 1)
		)
		return m_pCards->FirstInColor(m_enTrumps);		

	// je¿eli w atucie mamy wiêcej ni¿ 4 karty 
	// i mamy (lub mieliœmy) w tym kolorze A, K i D lub A, K, J
	// i inni maj¹ jeszcze atu to gramy najwy¿sze atu
	if  (
		(m_pCards->ColorAllCnt(m_enTrumps) > 4) && 
		((m_pCards->HasCards(m_enTrumps, FALSE, 3, E_CV_A, E_CV_K, E_CV_D)) || (m_pCards->HasOneOfCards(m_enTrumps, FALSE, 3, E_CV_A, E_CV_K, E_CV_J))) &&
		(LeftInColorOthers(m_enTrumps) > 0) &&
		(m_pCards->HasColor(m_enTrumps))
		)
		return m_pCards->Biggest(m_enTrumps);

	// je¿eli mamy najwy¿sze atu w rêku i 
	// zosta³o mniej ni¿ trzy atu, ale wiêcej ni¿ 0 to gramy najwy¿sze atu
	const CCard* l_pCard = m_pCards->BiggestInColorCard(m_enTrumps);
	if (
		(m_pCards->HasColor(m_enTrumps)) &&
		(!m_pTricks->AreBiggerInColor(l_pCard, m_pCards)) &&
		(LeftInColorOthers(m_enTrumps) > 0) &&
		(LeftInColorOthers(m_enTrumps) < 3) 
		)
		return m_pCards->Biggest(m_enTrumps);

	// je¿eli mamy dwa najwy¿sze atu w rêku i 
	// zosta³o mniej ni¿ szeœæ atu, ale wiêcej ni¿ 0 to gramy najwy¿sze atu
	l_pCard = m_pCards->BiggestInColorCard(m_enTrumps);
	const CCard* l_pCard2 = m_pCards->BiggestInColorCardRank(m_enTrumps, 2);
	if (
		(m_pCards->ColorCnt(m_enTrumps) >= 2) &&
		(!m_pTricks->AreBiggerInColor(l_pCard2, m_pCards)) &&
		(LeftInColorOthers(m_enTrumps) > 0) &&
		(LeftInColorOthers(m_enTrumps) < 6) 
		)
		return m_pCards->Biggest(m_enTrumps);

	// je¿eli mamy jakiœ kolor ró¿ny od atu w którym mamy jedn¹ kartê
	// to go zagrajmy, chyba ¿e to A
	T_COLOR l_enColor = m_pCards->GetColorExactNumNoOneColor(1, m_enTrumps, FALSE);
	if (
		(l_enColor != Suit::Nothing) &&
		(m_pCards->HasColor(l_enColor)) &&
		(m_pCards->FirstInColorVal(l_enColor) != E_CV_A)
		)
		return m_pCards->FirstInColor(l_enColor);

	// gramy w kolor najkrótszy, 
	// w którym nie mamy karty wy¿szej od J 
	// i nie bêd¹cy atutowym
	// zaczynaj¹c od najkrótszego
	short l_nRank;
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank, FALSE);
		if (l_enColor == m_enTrumps)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (m_pCards->HasOneOfCards(l_enColor, TRUE, 3, E_CV_A, E_CV_K, E_CV_D))
			continue;
		return m_pCards->FirstInColor(l_enColor);
	}
	
	// gramy w kolor najkrótszy, 
	// nie atutowy
	// w którym mamy kartê najwy¿sz¹ w kolorze
	// jeœli by³ grany 0 lub 1 razy
	// i sami mamy w tym kolorze nie wiêcej ni¿ 5 karty
	// oraz zosta³o jeszcze co najmniej 4 karty w tym kolorze
	// wszyscy w poprzedniej lewie dorzucili do koloru
	// gramy w najwy¿sz¹
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank, FALSE);
		if (l_enColor == m_enTrumps)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;

		const CCard* l_pCard = m_pCards->BiggestInColorCard(l_enColor);
		if (m_pTricks->AreBiggerInColor(l_pCard, m_pCards))
			continue;
		if (m_pTricks->TakenInColor(l_enColor) <= 1)
			continue;
		if (m_pCards->ColorCnt(l_enColor, FALSE) <= 5)
			continue;
		if (LeftInColorOthers(l_enColor) > 4)
			continue;
		if (!m_pTricks->FullTricksInColor(l_enColor))
			continue;
		return m_pCards->Biggest(l_enColor);
	}

	// zagrajmy w kolor najkrótszy w którym nie mamy A, K, D
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank, FALSE);
		if (l_enColor == m_enTrumps)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		if (m_pCards->HasCards(l_enColor, TRUE, 1, E_CV_A))
			continue;
		if (m_pCards->HasCards(l_enColor, TRUE, 1, E_CV_K))
			continue;
		if (m_pCards->HasCards(l_enColor, TRUE, 1, E_CV_D))
			continue;

		return m_pCards->FirstInColor(l_enColor);
	}

	// zagrajmy w najkrótszy kolor nie atutowy
	// jeœli mamy najwy¿sz¹ to najwy¿sz¹
	// jeœli nie mamy najwy¿szej to najni¿sz¹
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank, FALSE);
		if (l_enColor == m_enTrumps)
			continue;
		if (!m_pCards->HasColor(l_enColor))
			continue;
		
		const CCard* l_pCard = m_pCards->BiggestInColorCard(l_enColor);
		if (m_pTricks->AreBiggerInColor(l_pCard, m_pCards))
			return m_pCards->FirstInColor(l_enColor);
		
		// trzeba siê jeszcze zastanowiæ czy ma sens zagraæ najwy¿sz¹
		// zagramy najwy¿sz¹, gdy inni maj¹ conajmniej 
		if	(
			(m_pTricks->FullTricksInColor(l_enColor) || (LeftInColorOthers(m_enTrumps) < 2)) &&
			((LeftInColorOthers(l_enColor) > 3) || (LeftInColorOthers(m_enTrumps) < 2))
			)
			return m_pCards->Biggest(l_enColor);
		return m_pCards->FirstInColor(l_enColor);
	}

	// gramy pierwsz¹ lepsz¹ najwy¿sz¹
	for (l_nRank = 1; l_nRank <= 4; l_nRank++)
	{
		l_enColor = m_pCards->GetShortestColorRank(l_nRank, FALSE);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		
		return m_pCards->Biggest(l_enColor);
	}
	
	// tu nigdy
	ASSERT(FALSE);
	return a_nTrick;
}


// ---------------------------------------------------------
//	Dorzucenie karty
//
short	//WY numer kart w rêce
CRecoverDecider::NextCard(
	short a_nTrick	//WE która lewa
	)	const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1] ;
	T_COLOR l_enColor = l_trickCurrent.GetCardColor(0);

	if (m_pCards->HasColor(l_enColor))
		return NextCardInColor(a_nTrick);
	return NextCardNotInColor(a_nTrick);
}


// ---------------------------------------------------------
//	Dorzucenie karty do koloru
//
short	//WY numer karty w rêce
CRecoverDecider::NextCardInColor(
	short a_nTrick	//WE która lewa
	)	const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1] ;
	
	// obs³uga w zale¿noœci która to lewa
	switch (l_trickCurrent.GetCardsCnt())
	{
	case 3:
		return NextCardInColor3(a_nTrick);
	case 2:
		return NextCardInColor2(a_nTrick);
	case 1:
		return NextCardInColor1(a_nTrick);
	default:
		ASSERT(FALSE); // jakim cudem tutaj?
	}

	// tu nigdy
	ASSERT(FALSE); 
	return a_nTrick;
}


// ---------------------------------------------------------
//	Dorzucenie karty do nie koloru
//
short	//WY numer karty w rêce
CRecoverDecider::NextCardNotInColor(
	short a_nTrick	//WE numer lewy
	)	const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1] ;
	
	// jeœli to czwarta lewa, to sprawdŸmy czy nie ma
	switch (l_trickCurrent.GetCardsCnt())
	{
	case 3:
		return NextCardNotInColor3(a_nTrick);
	case 2:
		return NextCardNotInColor2(a_nTrick);
	case 1:
		return NextCardNotInColor1(a_nTrick);
	default:
		ASSERT(FALSE); // jakim cudem tutaj?
	}

	// tu nigdy
	ASSERT(FALSE); 
	return a_nTrick;
}


// ---------------------------------------------------------
//	Dorzucenie karty do koloru, gdy jesteœmy ostatnim graczem
//	(trzy karty w lewie)
//
short	//WY numer karty w rêce
CRecoverDecider::NextCardInColor3(
	short a_nTrick	//WE która lewa
	)	const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1] ;
	const CCard* l_pBiggest = l_trickCurrent.Biggest(m_enTrumps);
	T_COLOR l_enColor = l_trickCurrent.GetCardColor(0);

	// jeœli lewa w kolorze atutowym to trzba zagraæ wy¿sz¹ jeœli 
	// mamy
	if (l_enColor == m_enTrumps)
		return m_pCards->FirstBiggerOrLeastInColor(l_pBiggest);
	// je¿eli najwiêksza karta to atu, to gramy najmniejsz¹
	// w kolorze
	if (l_pBiggest->GetSuit() == m_enTrumps)
		return m_pCards->FirstInColor(l_enColor);

	// je¿eli nie to gramy pierwsz¹ wiêksz¹ lub najmniejsz¹
	return m_pCards->FirstBiggerOrLeastInColor(l_pBiggest);
}


// ---------------------------------------------------------
//	Dorzucenie karty do koloru, gdy jesteœmy przedostatnim graczem
//	(dwie karty w lewie)
//
short	//WY karta w rêce
CRecoverDecider::NextCardInColor2(
	short a_nTrick	//WE któa lewa
	)	const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1] ;
	const CCard* l_pBiggest = l_trickCurrent.Biggest(m_enTrumps);
	T_COLOR l_enColor = l_trickCurrent.GetCardColor(0);

	// jeœli lewa w kolorze atutowym to trzeba zagraæ wy¿sz¹ jeœli 
	// mamy
	if (l_enColor == m_enTrumps)
		return m_pCards->FirstBiggerOrLeastInColor(l_pBiggest);

	// je¿eli najwiêksza karta to atu, to gramy najmniejsz¹
	// w kolorze
	if (l_pBiggest->GetSuit() == m_enTrumps)
		return m_pCards->FirstInColor(l_enColor);

	// jeœli karta najwy¿sza jest najwy¿sz¹ z mo¿liwych 
	// to gramy najmniejsz¹ w tym kolorze
	if (!m_pTricks->AreBiggerInColor(l_pBiggest, m_pCards))
		return m_pCards->FirstInColor(l_enColor);

	// jeœli mamy ASA i DAMÊ, 
	// i to pierwsza lewa
	// i mamy mniej ni¿ 5 kart w tym kolorze
	// i najwiêksza karta nie jest królem
	// to od czasu ryzykujemy po³o¿enie D
	if  (
		(m_pCards->HasCards(l_enColor, TRUE, 2, E_CV_A, E_CV_D)) &&
		(m_pTricks->TricksInColor(l_enColor) == 1) &&
		(m_pCards->ColorAllCnt(l_enColor) < 5) &&
		(l_pBiggest->CardValue() < E_CV_K)
		)
	{
	    srand((unsigned)time(NULL)); // inicjalizacja generatora
		long l_nRand = rand() *  10 / RAND_MAX - 1;
		if (l_nRand == 0)
			return m_pCards->FindCard(l_enColor, E_CV_D);
		return m_pCards->FindCard(l_enColor, E_CV_A);
	}

	// jeœli mamy ASA i WALETA, 
	// i to pierwsza lewa
	// i mamy mniej ni¿ 5 kart w tym kolorze
	// i najwiêksza karta nie jest królem
	// to od czasu ryzykujemy po³o¿enie J
	if  (
		(m_pCards->HasCards(l_enColor, TRUE, 2, E_CV_A, E_CV_J)) &&
		(m_pTricks->TricksInColor(l_enColor) == 0) &&
		(m_pCards->ColorAllCnt(l_enColor) < 5) &&
		(l_pBiggest->CardValue() < E_CV_K)
		)
	{
	    srand((unsigned)time(NULL)); // inicjalizacja generatora
		long l_nRand = rand() *  15 / RAND_MAX - 1;
		if (l_nRand == 0)
			return m_pCards->FindCard(l_enColor, E_CV_J);
		return m_pCards->FindCard(l_enColor, E_CV_A);
	}

	// jeœli wczeœniej grano w tym kolorze atu
	if (m_pTricks->TrumpsInLastTrickInColor(l_enColor, m_enTrumps))
		return m_pCards->FirstInColor(l_enColor);

	// jeœli to pierwszy raz 
	// i mamy najwy¿sz¹ kartê
	// gramy nasz¹ najwy¿sz¹
	const CCard* l_pMyBiggest = m_pCards->BiggestInColorCard(l_enColor);
	short l_nTrickColor = m_pTricks->TricksInColor(l_enColor);
	if  (
		(l_nTrickColor == 1) &&
		(!m_pTricks->AreBiggerInColor(l_pMyBiggest, m_pCards))
		)
		return m_pCards->Biggest(l_enColor);

	// jeœli to drugi lub trzeci raz 
	// kart w tym kolorze mniej ni¿ 7
	// i mamy najwy¿sz¹ kartê
	// i w poprzedniej lewie nie by³o atu
	// gramy nasz¹ najwy¿sz¹
	if  (
		((l_nTrickColor == 2) || (l_nTrickColor == 3)) &&
		(m_pCards->AllCardsInColor(l_enColor) < 7) &&
		(!m_pTricks->AreBiggerInColor(l_pMyBiggest, m_pCards)) &&
		(!m_pTricks->TrumpsInLastTrickInColor(l_enColor, m_enTrumps))
		)
		return m_pCards->Biggest(l_enColor);

	// jeœli inni nie maj¹ ju¿ atu
	// i mamy najwy¿sz¹ w tym kolorze to j¹ gramy
	if  (
		(LeftInColorOthers(m_enTrumps) == 0) &&
		(!m_pTricks->AreBiggerInColor(l_pMyBiggest, m_pCards))
		)
		return m_pCards->Biggest(l_enColor);

	// jeœli mamy K, 
	// a A jeszcze nie poszed³ to gramy 
	// nasza druga karta jest wy¿sza od najwy¿szej
	// i w poprzedniej lewie nie sz³o atu
	// drug¹ najwiêksz¹ w tym kolorze
	CCard l_cardKing(l_enColor, E_CV_K);
	if  (
		(m_pCards->HasCards(l_enColor, TRUE, 1, E_CV_K)) &&
		(m_pTricks->AreBiggerInColor(&l_cardKing, m_pCards)) &&
		(m_pTricks->FullTricksInColor(l_enColor)) &&
		(m_pCards->ColorCnt(l_enColor) >= 2) &&
		((*(m_pCards->BiggestInColorCardRank(l_enColor, 2))) > *l_pBiggest)
		)
	{
		const CCard* l_pCard = m_pCards->BiggestInColorCardRank(l_enColor, 2);
		return m_pCards->FindCard(l_pCard);
	}

	// jeœli mamy D, 
	// a A lub/i K jeszcze nie poszed³ to gramy 
	// drug¹ najwiêksz¹ w tym kolorze
	// i w poprzedniej lewie nie sz³o atu
	CCard l_cardQueen(l_enColor, E_CV_D);
	if  (
		(m_pCards->HasCards(l_enColor, TRUE, 1, E_CV_D)) &&
		(m_pTricks->AreBiggerInColor(&l_cardQueen, m_pCards)) &&
		(m_pTricks->FullTricksInColor(l_enColor)) &&
		(m_pCards->ColorCnt(l_enColor) >= 2)
		)
	{
		const CCard* l_pCard = m_pCards->BiggestInColorCardRank(l_enColor, 2);
		return m_pCards->FindCard(l_pCard);
	}

	return m_pCards->FirstInColor(l_enColor);
}


// ---------------------------------------------------------
//	Dorzucenie karty do koloru, gdy jesteœmy drugim graczem
//	(jedna karty w lewie)
//
short	//WY numer karty w rêce
CRecoverDecider::NextCardInColor1(
	short a_nTrick	//WE która lewa
	)	const
{
	// na razie tak jak byœmy byli przedostatnim
	return NextCardInColor2(a_nTrick);
}


// ---------------------------------------------------------
//	Dorzucenie karty do nie koloru - ostatni gracz
//
short	//WY która karta w rêce
CRecoverDecider::NextCardNotInColor3(
	short a_nTrick	//WE która lewa
	)	const
{
	// jeœli nie mamy atu dorzucamy kartê w innym kolorze
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1] ;
	const CCard* l_pBiggest = l_trickCurrent.Biggest(m_enTrumps);
	if (m_pCards->HasColor(m_enTrumps))
		if (l_pBiggest->GetSuit() == m_enTrumps)
			return m_pCards->FirstBiggerOrLeastInColor(l_pBiggest);
		else
			return m_pCards->FirstInColor(m_enTrumps);
	else
		return DecideNotNeededCard();
}


// ---------------------------------------------------------
//	Dorzucenie karty do nie koloru - przedostatni gracz
//
short	//WY któa karta w rêce
CRecoverDecider::NextCardNotInColor2(
	short a_nTrick	//WE która lewa
	)	const
{
	// na razie prosto - nie zastanawiamy siê czy graæ 
	// najwy¿sze atu - zawsze gramy wy¿sze lub najni¿sze jeœli
	// mamy atu
	// czyli tak jak poprzedni 
	return NextCardNotInColor3(a_nTrick);
}


// ---------------------------------------------------------
//	Dorzucenie karty do nie koloru - drugi gracz
//
short	//WY która karta w rêce
CRecoverDecider::NextCardNotInColor1(
	short a_nTrick	//WE która lewa
	)	const
{	
	// na razie tak samo
	return NextCardNotInColor2(a_nTrick);
}


// ---------------------------------------------------------
//	Decyzja jak¹ kartê z tych które mamy wyrzucamy
//
short	//WY która karta w rêce
CRecoverDecider::DecideNotNeededCard()	const
{
	// jeœli mamy atu to to jest jakiœ babol
	ASSERT(!m_pCards->HasColor(m_enTrumps));

	// wyrzucamy najni¿sz¹ kartê z koloru w którym nam
	// najwiêcej zosta³o
	short l_nRank;
	for (l_nRank = 4; l_nRank >= 1; l_nRank--)
	{
		T_COLOR l_enColor = m_pCards->GetShortestColorRank(l_nRank);
		if (!m_pCards->HasColor(l_enColor))
			continue;
		return m_pCards->FirstInColor(l_enColor);

	}
	// tu nigdy
	ASSERT(FALSE);
	return -1;
}


// ---------------------------------------------------------
//	Decyzja jak¹ kartê pierwsz¹ wyrzucamy przy bez atu
//
short 
CRecoverDecider::FirstCardNoTrumps(
	short a_nTrick
	) const
{
	// lets check if we have five or more cards color
	T_COLOR l_enLongest = m_pCards->GetLongestColorRank(1, TRUE);
	short l_nCardsAll = m_pCards->ColorAllCnt(l_enLongest);
	short l_nCards = m_pCards->ColorCnt(l_enLongest);

	if  (
		(l_nCardsAll > 4) && 
		(l_nCards > 0)
		)
	{
		const CCard* l_pBiggest = m_pCards->BiggestInColorCard(l_enLongest);
		if  (
			(l_nCards <= 2) && 
			(!m_pTricks->AreBiggerInColor(l_pBiggest, m_pCards))
			)
		{
			return m_pCards->LastInColor(l_enLongest);
		}
	}

	short l_nRank;
	for (l_nRank = 2; l_nRank < 4; l_nRank++)
	{
		T_COLOR l_enColor = m_pCards->GetLongestColorRank(l_nRank, TRUE);
		l_nCards = m_pCards->ColorCnt(l_enColor);
		// lets play in longest possible color
		if (l_nCards > 0)
		{
			const CCard* l_pBiggest = m_pCards->BiggestInColorCard(l_enColor);
			if (!m_pTricks->AreBiggerInColor(l_pBiggest, m_pCards))
			{
				return m_pCards->Biggest(l_enLongest);
			}
			else
			{
				return m_pCards->FirstInColor(l_enColor);
			}
		}
	}

	ASSERT(FALSE);
	return 0;
}


// ---------------------------------------------------------
//	Decyzja jak¹ kartê nastêpn¹ wyrzucamy przy bez atu
//
short 
CRecoverDecider::NextCardNoTrumps(
	short a_nTrick
	) const
{
	// check if we have card in trick color
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1] ;
	T_COLOR l_enColor = l_trickCurrent.GetCardColor(0);

	if (m_pCards->HasColor(l_enColor))
		return NextCardInColorNoTrumps(a_nTrick);
	return NextCardNotInColorNoTrumps(a_nTrick);
}



// ---------------------------------------------------------
//	Dorzucenie karty do koloru
//
short	//WY numer karty w rêce
CRecoverDecider::NextCardInColorNoTrumps(
	short a_nTrick	//WE która lewa
	)	const
{
	const CTrick& l_trickCurrent = (*m_pTricks)[a_nTrick - 1];
	T_COLOR l_enColor = l_trickCurrent.GetCardColor(0);

	const CCard* l_pBiggest = l_trickCurrent.Biggest(m_enTrumps);
	return m_pCards->FirstBiggerOrLeastInColor(l_pBiggest);
	
}


// ---------------------------------------------------------
//	Dorzucenie karty do nie koloru
//
short	//WY numer karty w rêce
CRecoverDecider::NextCardNotInColorNoTrumps(
	short a_nTrick	//WE numer lewy
	)	const
{
	return DecideNotNeededCard();	
}