// UserCards.h: interface for the CUserCards class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Card.h"	// Added by ClassView
#include "Trick.h"
#include "PuzzleRows.h"	

class CTakenTricks;


class CUserCards  
{
public:
	CUserCards();
	virtual ~CUserCards();

	const CCard& operator[](short a_iAt) const;

	const CCard& GetCard(short a_nCard) const;
	BOOL IsUsed(short a_nCardNr) const;
	void SetUsed(short a_nCard, BOOL a_bUsed);

	void Sort(short a_nStart = 0, short a_nEnd = 12);
	void SortSTL(short a_nStart = 0, short a_nEnd = 12);
	void SetCard(short a_iAt, short a_iCardNr, T_PLAYER a_enOwner, BOOL a_bUsed = FALSE);

	short CardsLeft() const;
	BOOL HasCardForPuzzle(const CPuzzleRows* a_pPuzzleRow) const;
	BOOL HasCardForPuzzleColor(const CPuzzleRows* a_pPuzzleRow, T_COLOR a_enColor) const;
	short FirstBiggerOrLeastInColor(const CCard* a_pCard) const;
	T_COLOR GetColorExactNumNoOneColor(short a_nNum, T_COLOR a_enColor, BOOL a_bUsed = TRUE) const;
	BOOL HasCards(T_COLOR a_enColor, BOOL a_bUsed, short a_nCnt, ...) const;
	BOOL HasOneOfCards(T_COLOR a_enColor, BOOL a_bUsed, short a_nCnt, ...) const;
	short InShortestColorNoOneColor(T_COLOR a_enColor) const;
	short InShortestColorAllNoOneColor(T_COLOR a_enColor) const;
	short CardsBetween(const CCard& a_card1, const CCard& a_card2) const;
	short CardsBetweenAll(const CCard& a_card1, const CCard& a_card2) const;
	T_COLOR GetBiggestFirstCardColorRank(short a_nRank) const;
	T_COLOR GetLowestCardColorRank(short a_nRank) const;
	BOOL HasKingOfHeart() const;
	short LessOrFirstBiggerInColor(const CCard* a_pCard) const;
	short AllCardsInColor(T_COLOR a_enColor) const;
	short CardsInColor(T_COLOR a_enColor) const;
	BOOL IsCard(short a_nCardNr) const;
	BOOL HasNoHeartsColor() const;
	T_COLOR GetLongestColorRank(short a_nRank, BOOL a_bUsed = TRUE) const;
	T_COLOR GetShortestColorRank(short a_nRank, BOOL a_bUsed = TRUE) const;
	T_COLOR GetShortestColorAllRank(short a_nRank) const;
	short FindCard(T_COLOR a_enColor, T_CARDVAL a_cv, BOOL a_bUsed = TRUE) const;
	short FindCard(const CCard& a_card, BOOL a_bUsed = TRUE) const;
	short FindCard(const CCard* a_pCard, BOOL a_bUsed = TRUE) const;
	short GetBiggestBelow(T_COLOR a_enColor, T_CARDVAL a_cv) const;
	short Biggest(T_COLOR a_enColor) const;
	const CCard* BiggestInColorCard(T_COLOR a_enColor) const;
	const CCard* BiggestInColorCardRank(T_COLOR a_enColor, short a_nRank) const;
	BOOL HasColor(T_COLOR a_enColor) const;
	BOOL AreNoHearts() const;
	short ColorCnt(T_COLOR a_enColor, BOOL a_bUsed = TRUE) const;
	short ColorAllCnt(T_COLOR a_enColor) const;
	T_COLOR ShortestColor()	const;
	T_COLOR ShortestColorNoHearts() const;
	short CardNr(short a_iInHand) const;
	short FirstInShortest()	const;
	short FirstInColor(T_COLOR a_enShortest)	const;
	T_CARDVAL FirstInColorVal(T_COLOR a_enColor)	const;
	const CCard* FirstInColorCard(T_COLOR a_enColor)	const;
	short LastInColor(T_COLOR a_enColor) const;
	short LessOrBiggestInColor(const CTrick& a_pTrick)	const;

	BOOL HasLeastInColor(T_COLOR a_enColor, const CTakenTricks* a_pTricks) const;

private:
	CCard m_cards[13];

};


