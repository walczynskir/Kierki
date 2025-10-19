// Score.h: interface for the CScore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCORE_H__6DCA56C1_80FB_11D3_B413_E87410C10000__INCLUDED_)
#define AFX_SCORE_H__6DCA56C1_80FB_11D3_B413_E87410C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "TypeDefs.h"

class CScore : private S_SCORE
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

	void FillScore(SCORE& a_score) const;
	void RestoreScore(const SCORE& a_score);

};

#endif // !defined(AFX_SCORE_H__6DCA56C1_80FB_11D3_B413_E87410C10000__INCLUDED_)
