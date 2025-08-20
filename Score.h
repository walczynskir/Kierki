// Score.h: interface for the CScore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCORE_H__6DCA56C1_80FB_11D3_B413_E87410C10000__INCLUDED_)
#define AFX_SCORE_H__6DCA56C1_80FB_11D3_B413_E87410C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "TypeDefs.h"

class CScore  
{
public:
	CScore();
	virtual ~CScore();

	void AddScore(T_GAMES a_enGame, short a_nScore);
	void SetScore(T_GAMES a_enGame, short a_nScore);
	short GetScore(T_GAMES a_enGame) const;
	short GetScore(short a_enGame) const;

	short SumMinuses(BOOL a_bRobber = FALSE) const;
	short SumPluses() const;
	short Sum() const;
	short SumAll()const;

	void Clear();
	short operator[](T_GAMES a_enGame) const { return m_arScore[a_enGame] ; };

	void SaveState(LPSCORE a_pScore) const;
	void RestoreState(const LPSCORE a_pScore);

private:
	short m_arScore[12] ;
};

#endif // !defined(AFX_SCORE_H__6DCA56C1_80FB_11D3_B413_E87410C10000__INCLUDED_)
