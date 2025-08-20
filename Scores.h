// Scores.h: interface for the CScores class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCORES_H__AED63D04_183B_11D4_A79A_347710C10000__INCLUDED_)
#define AFX_SCORES_H__AED63D04_183B_11D4_A79A_347710C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Score.h"

class CScores  
{
public:
	void Clear();
	CScores();
	virtual ~CScores();
	CScore	m_arScore[4];
};

#endif // !defined(AFX_SCORES_H__AED63D04_183B_11D4_A79A_347710C10000__INCLUDED_)
