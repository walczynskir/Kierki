#pragma once

typedef enum {
	E_DL_NULL = -1,
	E_DL_1 = 0,
	E_DL_2,
	E_DL_3,
	E_DL_4
}	T_PLAYER ;

typedef enum 
{
	E_CC_NOTHING = 0,
	E_CC_CLUB = 1,
	E_CC_DIAMOND,
	E_CC_SPADE,
	E_CC_HEART,
	E_CC_NOTRUMPS
} T_COLOR ;

typedef enum {
	E_GM_NOTHING = -1, 
	E_GM_NOTRICKS = 0, 
	E_GM_NOHEARTS, 
	E_GM_NOBOYS, 
	E_GM_NOLADIES,
	E_GM_NOKINGOFHEART,
	E_GM_NOSIXANDLAST,
	E_GM_ROBBER,
	E_GM_RECOVER1,
	E_GM_RECOVER2,
	E_GM_RECOVER3,
	E_GM_RECOVER4,
	E_GM_PUZZLE
}	T_GAMES ;

typedef enum {
	E_CV_NULL = -1,
	E_CV_2 = 1, 
	E_CV_3, 
	E_CV_4, 
	E_CV_5, 
	E_CV_6,
	E_CV_7,
	E_CV_8,
	E_CV_9,
	E_CV_10,
	E_CV_J,
	E_CV_D,
	E_CV_K,
	E_CV_A
}	T_CARDVAL ;

typedef enum {
	E_SR_NULL = -1,
	E_SR_1 = 0,
	E_SR_2,
	E_SR_3,
	E_SR_4,
}	T_SERIE;



const BYTE c_app_btPlayersCnt = 4;
const BYTE c_app_btPlayerCards = 13;

typedef struct S_SCORE
{
	short m_nPoints[12];
} SCORE, *LPSCORE;

typedef struct S_PLAYERSCORE
{
	SCORE m_score[4];
} PLAYERSCORE, *LPPLAYERSCORE;


typedef struct S_SAVERESTORE
{
	T_PLAYER	m_enDealer;
	T_GAMES		m_enGame;
	T_SERIE		m_enSerie;
	T_PLAYER	m_enFirstDealer;
	PLAYERSCORE m_playerscore[4];
} SAVERESTORE, *LPSAVERESTORE;