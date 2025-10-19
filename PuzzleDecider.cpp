// PuzzleDecider.cpp: implementation of the CPuzzleDecider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PuzzleDecider.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CPuzzleDecider::CPuzzleDecider(
	const CUserCards*  a_pUserCards,	//WE karty gracza
	const CPuzzleRows* a_pPuzzleRows, //WE po�o�one wiersze
	T_PLAYER a_enPlayer			//WE gracz
	)
{
	m_pCards = a_pUserCards;	
	m_pTricks = NULL;
	m_enPlayer = a_enPlayer;
	m_pPuzzleRows = a_pPuzzleRows;
}

CPuzzleDecider::~CPuzzleDecider()
{

}


// ---------------------------------------------------------
//	Zwraca kart� kt�r� trzeba zagra�, 
//
short	//WY numer karty
CPuzzleDecider::GetCardNr(
	short a_nTrick	//WE numer lewy
	) const
{
	short l_nCardNr = -1;

	CAllowedCards l_allCards;
	FillAllowedCards(&l_allCards);

	// nie ma �adnej
	if (l_allCards.GetAllowedCount() == 0)
	{
		return l_nCardNr;
	}

	// jest tylko jedna
	if (l_allCards.GetAllowedCount() == 1)
	{
		l_nCardNr = l_allCards.GetCard(0);
	}
	else
	{
		l_nCardNr = DecideCard(&l_allCards);
	}

#ifdef _DEBUG
	if (l_nCardNr >= 0)
		ASSERT(m_pPuzzleRows->CanPutCard((*m_pCards)[l_nCardNr]));
	ASSERT((l_nCardNr >= 0) && (l_nCardNr < 13));
#endif

	return l_nCardNr;
}


// ---------------------------------------------------------
//	Zwr�� kart� kt�ra pasuje na st�
//
short	// WY	numer karty w r�ce
CPuzzleDecider::GetCardInColor(
	T_SUIT a_enColor,  //WE kolor karty
	BOOL a_bTop			//WE karta powy�ej 8 czy poni�ej 
	) const
{
	const CCard* l_pCard;
	T_RANK l_val;
	if (a_bTop)
	{
		l_pCard = m_pPuzzleRows->m_arPuzzleRow[a_enColor - 1].m_pCardTop;
		if (l_pCard == NULL)
			l_val = E_CV_8;
		else
		{
			l_val = l_pCard->CardValue();
			if (l_val == E_CV_A)
				return - 1;
			l_val = (T_RANK)(l_val + 1);
		}
	}
	else
	{
		l_pCard = m_pPuzzleRows->m_arPuzzleRow[a_enColor - 1].m_pCardBottom;
		if (l_pCard == NULL)
			return -1;
		else
		{
			l_val = l_pCard->CardValue();
			if (l_val == E_CV_2)
				return - 1;
			l_val = (T_RANK)(l_val - 1);
		}
	}
	return m_pCards->FindCard(a_enColor, l_val);
}


// ---------------------------------------------------------
// Wype�nienie listy kart, kt�re user mo�e po�o�y�
//
void	
CPuzzleDecider::FillAllowedCards(
	CAllowedCards* a_pAllowedCards	//WE karty dopuszczalne
	) const
{
	a_pAllowedCards->AddCard(GetCardInColor(E_CC_HEART, TRUE));
	a_pAllowedCards->AddCard(GetCardInColor(E_CC_HEART, FALSE));
	a_pAllowedCards->AddCard(GetCardInColor(E_CC_CLUB, TRUE));
	a_pAllowedCards->AddCard(GetCardInColor(E_CC_CLUB, FALSE));
	a_pAllowedCards->AddCard(GetCardInColor(E_CC_SPADE, TRUE));
	a_pAllowedCards->AddCard(GetCardInColor(E_CC_SPADE, FALSE));
	a_pAllowedCards->AddCard(GetCardInColor(E_CC_DIAMOND, TRUE));
	a_pAllowedCards->AddCard(GetCardInColor(E_CC_DIAMOND, FALSE));
	a_pAllowedCards->Sort();
}


// ---------------------------------------------------------
// Wybiera najlepsz� kart� do zagrania z dozwolonych
//
short	//WY numer karty w r�ce
CPuzzleDecider::DecideCard(
	CAllowedCards* a_pAllowedCards	//WE dozwolone
	) const
{
	short l_nInHand;

	// najpierw kart� najbardziej potrzebn�
	// czyli tak� dla kt�rej p�niej mamy ASA, 2 itp.
	l_nInHand = FindFurthest(a_pAllowedCards);
	if (l_nInHand >= 0)
	{
		return l_nInHand;
	}

	// teraz kart�, po kt�rej nikt nie po�o�y karty
	// czyli A, 2, lub takie po kt�rych mamy nast�pne karty
	l_nInHand = FindForNoOne(a_pAllowedCards);
	if (l_nInHand >= 0)
	{
		return l_nInHand;
	}

	return FindBestRest(a_pAllowedCards);
}


// ---------------------------------------------------------
// Odnalezienie karty najlepszej
//
short	//WY karta w r�ce
CPuzzleDecider::FindFurthest(
	CAllowedCards* a_pAllowedCards	//WE dozwolone karty
	) const
{
	short l_nAt;
	for (l_nAt = 0; l_nAt < a_pAllowedCards->GetAllowedCount(); l_nAt++)
	{
		DecidePointsFurthest(a_pAllowedCards, l_nAt);
	}

	// teraz trzeba wybra� kart� z najwy�sz� punktacj�
	short l_nMaxPoints = 0;
	short l_nPoints = 0;
	short l_nMaxPos = 0;
	for (l_nAt = 0; l_nAt < a_pAllowedCards->GetAllowedCount(); l_nAt++)
	{
		l_nPoints = a_pAllowedCards->GetPoints(l_nAt);
		if (l_nPoints > l_nMaxPoints)
		{
			l_nMaxPoints = l_nPoints;
			l_nMaxPos = l_nAt;
		}
	}

	if (l_nMaxPoints > 0)
	{
		return a_pAllowedCards->GetCard(l_nMaxPos);
	}
	return -1;
}


// ---------------------------------------------------------
// Przydzielenie ilo�ci punkt�w w zale�no�ci od karty
// Funkcja d�uga, bo r�cznie decyduj� o wyborze karty
// Decyzja o po�o�eniu karty, po kt�rej wyst�puj� jakie� 
// karty. Je�li po karcie nie wystepuj� �adne karty karta
// zdobywa 0 punkt�w
//
void 
CPuzzleDecider::DecidePointsFurthest(
	CAllowedCards* a_pAllowedCards,	//WE dozwolone karty
	short a_nCard					//WE kt�ra karta z allowed
	) const
{
	const CCard& l_card = (*m_pCards)[a_pAllowedCards->GetCard(a_nCard)];
	// asy, kr�le, dw�jki i tr�jki na tym etapie nas nie inetersuj�
	if (
		(l_card.CardValue() == E_CV_A) || 
		(l_card.CardValue() == E_CV_K) || 
		(l_card.CardValue() == E_CV_2) || 
		(l_card.CardValue() == E_CV_3))
	{
		a_pAllowedCards->SetPoints(a_nCard, 0);						
	}

	// poszukuj� r�cznie - dla ka�dej karty oddzielnie
	// je�li ma K i A lub tylko K to nas nie interesuje
	if (l_card.CardValue() == E_CV_D)
	{
		if (m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_K, E_CV_A) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_K)))
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if (m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_A))
		{
			a_pAllowedCards->SetPoints(a_nCard, 1);
		}
		else
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
	}
	// je�li ma 3 i 2 lub tylko 3 to nas nie interesuje
	else if (l_card.CardValue() == E_CV_4)
	{
		if (m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_2, E_CV_3) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_3)))
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if (m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_2))
		{
			a_pAllowedCards->SetPoints(a_nCard, 1);
		}
		else
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
	}

	// je�li ma D i K, A lub tylko D to nas nie interesuje
	else if (l_card.CardValue() == E_CV_J)
	{

		if (!m_pCards->HasOneOfCards(l_card.GetColor(), TRUE, 3, E_CV_D, E_CV_K, E_CV_A))
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if  (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 3, E_CV_D, E_CV_K, E_CV_A)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_D, E_CV_K)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_D))
			)
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_D, E_CV_A)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_K, E_CV_A))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 1);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_K))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 2);
		}
		else
		{
			a_pAllowedCards->SetPoints(a_nCard, 3);
		}
	}

	else if (l_card.CardValue() == E_CV_5)
	{
		if (!m_pCards->HasOneOfCards(l_card.GetColor(), TRUE, 3, E_CV_2, E_CV_3, E_CV_4))
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if  (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 3, E_CV_4, E_CV_3, E_CV_2)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_4, E_CV_3)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_4))
			)
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_4, E_CV_2)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_3, E_CV_2))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 1);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_3))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 2);
		}
		else 
		{
			a_pAllowedCards->SetPoints(a_nCard, 3);
		}

	}

	else if (l_card.CardValue() == E_CV_10)
	{
		if (!m_pCards->HasOneOfCards(l_card.GetColor(), TRUE, 4, E_CV_J, E_CV_D, E_CV_K, E_CV_A))
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if  (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 4, E_CV_J, E_CV_D, E_CV_K, E_CV_A)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 3, E_CV_J, E_CV_D, E_CV_K)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_J, E_CV_D)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_J))
			)
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_A))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 3);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_K))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 2);
		}
		else
		{
			a_pAllowedCards->SetPoints(a_nCard, 1);
		}
	}

	else if (l_card.CardValue() == E_CV_6)
	{
		if (!m_pCards->HasOneOfCards(l_card.GetColor(), TRUE, 4, E_CV_2, E_CV_3, E_CV_4, E_CV_5))
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if  (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 4, E_CV_5, E_CV_4, E_CV_3, E_CV_2)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 3, E_CV_5, E_CV_4, E_CV_3)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_5, E_CV_4)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_5))
			)
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_2))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 3);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_3))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 2);
		}
		else
		{
			a_pAllowedCards->SetPoints(a_nCard, 1);
		}
	}

	else if (l_card.CardValue() == E_CV_9)
	{
		if (!m_pCards->HasOneOfCards(l_card.GetColor(), TRUE, 5, E_CV_10, E_CV_J, E_CV_D, E_CV_K, E_CV_A))
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if  (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 5, E_CV_10, E_CV_J, E_CV_D, E_CV_K, E_CV_A)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 4, E_CV_10, E_CV_J, E_CV_D, E_CV_K)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 3, E_CV_10, E_CV_J, E_CV_D)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_10, E_CV_J)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_10))
			)
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_A))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 4);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_K))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 3);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_D))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 2);
		}
		else
		{
			a_pAllowedCards->SetPoints(a_nCard, 1);
		}
	}

	else if (l_card.CardValue() == E_CV_7)
	{
		if (!m_pCards->HasOneOfCards(l_card.GetColor(), TRUE, 5, E_CV_2, E_CV_3, E_CV_4, E_CV_5, E_CV_6))
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if  (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 5, E_CV_6, E_CV_5, E_CV_4, E_CV_3, E_CV_2)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 4, E_CV_6, E_CV_5, E_CV_4, E_CV_3)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 3, E_CV_6, E_CV_5, E_CV_4)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_6, E_CV_5)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_6))
			)
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_2))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 4);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_3))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 3);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_4))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 2);
		}
		else
		{
			a_pAllowedCards->SetPoints(a_nCard, 1);
		}
	}

	else if (l_card.CardValue() == E_CV_8)
	{
		if (!m_pCards->HasOneOfCards(l_card.GetColor(), TRUE, 12, E_CV_2, E_CV_3, E_CV_4, E_CV_5, E_CV_6, E_CV_7, E_CV_9, E_CV_10, E_CV_J, E_CV_D, E_CV_K, E_CV_A))
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if  (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 6, E_CV_9, E_CV_10, E_CV_J, E_CV_D, E_CV_K, E_CV_A)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 5, E_CV_9, E_CV_10, E_CV_J, E_CV_D, E_CV_K)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 4, E_CV_9, E_CV_10, E_CV_J, E_CV_D)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 3, E_CV_9, E_CV_10, E_CV_J)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_9, E_CV_10)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_9)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 6, E_CV_7, E_CV_6, E_CV_5, E_CV_4, E_CV_3, E_CV_2)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 5, E_CV_7, E_CV_6, E_CV_5, E_CV_4, E_CV_3)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 4, E_CV_7, E_CV_6, E_CV_J, E_CV_D)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 3, E_CV_7, E_CV_6, E_CV_J)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 2, E_CV_7, E_CV_6)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_7))
			)
		{
			a_pAllowedCards->SetPoints(a_nCard, 0);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_A)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_2))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 5);
		}
		else if (
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_K)) ||
			(m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_3))
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 4);
		}
		else if (
			m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_D) ||
			m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_4)
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 3);
		}
		else if (
			m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_J) ||
			m_pCards->HasCards(l_card.GetColor(), TRUE, 1, E_CV_5)
				)
		{
			a_pAllowedCards->SetPoints(a_nCard, 2);
		}
		else
		{
			a_pAllowedCards->SetPoints(a_nCard, 1);
		}
	}

}


// ---------------------------------------------------------
// Karta dobra dla nas (inni po niej nic nie po�o��)
//
short	//WY wybrana karta
CPuzzleDecider::FindForNoOne(
	CAllowedCards* a_pAllowedCards	//WE dozwolone karty
	) const
{
	short l_nAt;

	for (l_nAt = 0; l_nAt < a_pAllowedCards->GetAllowedCount(); l_nAt++)
	{
		const CCard& l_card = (*m_pCards)[a_pAllowedCards->GetCard(l_nAt)];
		if  (
			(l_card.CardValue() == E_CV_A) || 
			(l_card.CardValue() == E_CV_2)
			)
		{
			return a_pAllowedCards->GetCard(l_nAt);
		}

		if (l_card.CardValue() >= E_CV_8)
		{
			if (m_pCards->FindCard(l_card.GetColor(), (T_RANK)(l_card.CardValue() + 1), TRUE) >= 0)
			{
				return a_pAllowedCards->GetCard(l_nAt);
			}
		}
		else
		{
			if (m_pCards->FindCard(l_card.GetColor(), (T_RANK)(l_card.CardValue() - 1), TRUE) >= 0)
			{
				return a_pAllowedCards->GetCard(l_nAt);
			}
		}
	}
	return -1;
}


// ---------------------------------------------------------
// Karta niedobra dla nas (inni po niej po�o��), my potem 
// nie mamy karty. Wyk�adamy najblizsz� ko�ca.
//
short 
CPuzzleDecider::FindBestRest(
	CAllowedCards* a_pAllowedCards	//WE dozwolone karty
	) const
{
	short l_nAt;
	short l_nDiff = -1;
	short l_nBestDiff = -1;
	short l_nBestPos = -1;
	for (l_nAt = 0; l_nAt < a_pAllowedCards->GetAllowedCount(); l_nAt++)
	{
		const CCard& l_card = (*m_pCards)[a_pAllowedCards->GetCard(l_nAt)];
		if (l_card.CardValue() >= E_CV_8)
		{
			l_nDiff = l_card.CardValue() - E_CV_8;
			if (l_nDiff > l_nBestDiff)
			{
				l_nBestDiff = l_nDiff;
				l_nBestPos = l_nAt;
			}
		}
		else
		{
			l_nDiff = E_CV_8 - l_card.CardValue();
			if (l_nDiff > l_nBestDiff)
			{
				l_nBestDiff = l_nDiff;
				l_nBestPos = l_nAt;
			}
		}
	}
	return a_pAllowedCards->GetCard(l_nBestPos);
}
