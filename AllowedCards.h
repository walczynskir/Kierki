// AllowedCards.h: interface for the CAllowedCards class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALLOWEDCARDS_H__863025E9_E0D3_11D3_A79A_A07610C10000__INCLUDED_)
#define AFX_ALLOWEDCARDS_H__863025E9_E0D3_11D3_A79A_A07610C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CAllowedCards
{
public:
	CAllowedCards();

	short GetPoints(short a_nAt) const;
	void SetPoints(short a_nCard, short a_nPoints);
	
	short GetCard(short a_nPos) const;
	short GetAllowedCount() const;
	void Sort();
	void AddCard(short a_nInHand);

private:
	class CCardsPoints
	{
	public:
		short GetPoints() const;
		short GetCard() const;
		void SetCardPoints(short a_nInHand, short a_nPoints = 0);
		void SetPoints(short a_nPoints);
	private:
		short m_nInHand;
		short m_nPoints;
	};
	CCardsPoints	m_arrCardsPoints[8];
	short			m_nCards;
};


#endif // !defined(AFX_ALLOWEDCARDS_H__863025E9_E0D3_11D3_A79A_A07610C10000__INCLUDED_)


