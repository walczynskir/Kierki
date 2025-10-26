#pragma once

#include <array>

typedef enum class Player : int 
{
	E_DL_NULL = -1,
	South = 0,		// has to start with zero, as used as array index
	West,
	North,
	East
}	T_PLAYER ;

template<typename T>
class PlayerArray {
	T m_data[4];

public:
	T& operator[](Player a_player) {
		return m_data[static_cast<int>(a_player)];
	}

	const T& operator[](Player a_player) const {
		return m_data[static_cast<int>(a_player)];
	}
};

constexpr Player constAllPlayers[] = {
	Player::South,
	Player::West,
	Player::North,
	Player::East
};


typedef enum class Suit : int
{
	Nothing = 0,
	Club = 1,
	Diamond,
	Spade,
	Heart,
	NoTrumps
} T_COLOR ;

inline constexpr short SuitToIndex(Suit a_suit)
{
	return static_cast<short>(a_suit) - 1;
}

inline constexpr std::array<Suit, 4> ConstStandardSuits = {
	Suit::Heart, Suit::Spade, Suit::Diamond, Suit::Club
};

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



constexpr BYTE c_app_btPlayersCnt = 4;
constexpr BYTE c_app_btPlayerCards = 13;
constexpr BYTE c_app_btRounds = 4;
constexpr BYTE c_app_btGamesCnt = 12;

typedef struct S_SCORE
{
	short m_nPoints[c_app_btGamesCnt];
} SCORE, *LPSCORE;

typedef struct S_PLAYERSCORE
{
	SCORE m_score[c_app_btRounds];
} PLAYERSCORE, *LPPLAYERSCORE;


typedef struct S_SAVERESTORE
{
	T_PLAYER	m_enDealer;
	T_GAMES		m_enGame;
	T_SERIE		m_enSerie;
	T_PLAYER	m_enFirstDealer;
	PLAYERSCORE m_playerscore[4];
} SAVERESTORE, *LPSAVERESTORE;