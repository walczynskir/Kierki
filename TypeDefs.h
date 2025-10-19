#pragma once

typedef enum Player {
	E_DL_NULL = -1,
	E_DL_1 = 0,		// has to start with zero, as used as array index
	E_DL_2,
	E_DL_3,
	E_DL_4
}	T_PLAYER ;

typedef enum Suit
{
	E_CC_NULL = 0,
	E_CC_CLUB = 1,
	E_CC_DIAMOND,
	E_CC_SPADE,
	E_CC_HEART,
	E_CC_NOTRUMPS
} T_SUIT;

// for further use
enum class SuitNew { Clubs, Diamonds, Hearts, Spades };
enum class Trump { Clubs, Diamonds, Hearts, Spades, NoTrump };

typedef enum GameType {
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
	E_GM_PUZZLE,
	E_GM_NULL
}	T_GAMES;

typedef enum Rank {
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
	E_CV_A,
	E_CV_NULL
}	T_RANK ;

enum class RankNew { Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };

typedef enum Serie {
	E_SR_1 = 0,
	E_SR_2,
	E_SR_3,
	E_SR_4,
	E_SR_NULL
}	T_SERIE;

template<typename T, T First, T Last, T Sentinel>
class CircularEnum {
public:
	static T Next(T value) {
		if (value == Sentinel)
			return First;
		if (value < First || value > Last)
			return Sentinel;

		return (value == Last) ? First : static_cast<T>(value + 1);
	}

	static T Prev(T value) {
		if (value == Sentinel)
			return Last;
		if (value < First || value > Last)
			return Sentinel;

		return (value == First) ? Last : static_cast<T>(value - 1);
	}
};


// usage example: 	Player l_plauer = PlayerCycle::Next(a_enCurrent);
using PlayerCycle = CircularEnum<Player, E_DL_1, E_DL_4, E_DL_NULL>;
using SerieCycle = CircularEnum<Serie, E_SR_1, E_SR_4, E_SR_NULL>;
using GameCycle = CircularEnum<GameType, E_GM_NOTRICKS, E_GM_PUZZLE, E_GM_NULL>;


const BYTE c_app_btPlayersCnt = 4;
const BYTE c_app_btPlayerCards = 13;

typedef struct S_SCORE
{
	short m_arScore[12];
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


