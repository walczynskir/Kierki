#pragma once

// cards positions
static const int c_dxPuzzleGap = 4;
static const int c_dyPuzzleGap = 5;

/* cards order
	
	3
2		4
	1
*/

static const char c_sClassName[] = "CardsClass";


static const int c_dxMargin = 10;			//margines od brzegu do karty
static const int c_dyMargin = c_dxMargin;	//margines od brzegu do karty
static const int c_dxCardGap = 15;			// odleg³oœæ miêdzy kartami
static const int c_dyCardGap = c_dxCardGap;			// odleg³oœæ miêdzy kartami
static const int c_dxInterGap = 30;		// odleg³oœæ miêdzy kartami pionowymi i poziomymi
static const int c_dyInterGap = 5;			// odleg³oœæ miêdzy kartami poziomymi i pionowymi
static const int c_dxCard = 70;
static const int c_dyCard = 90;


#define CP_XL	(c_dxMargin)							// po³o¿enie kart X lewych
#define CP_XR	(c_dxMargin + c_dxCard + c_dxInterGap + c_dxCard + c_dxCardGap * 12 + c_dxInterGap)		// po³o¿enie kart X prawych (right)
#define CP_XH	(c_dxMargin + c_dxCard + c_dxInterGap)	// po³o¿enie kart X horyzontalnych
#define CP_YT	(c_dyMargin)		// po³o¿enie Y kart górnych (top)
#define CP_YB	(c_dxMargin + c_dyCard + c_dyCard + 12 * c_dyCardGap)		// po³o¿enie Y kart dolnych (bottom)
#define CP_YV	(c_dyMargin + c_dyCard) // po³o¿enie Y kart vertykalnych

#define C_VIEW_WIDTH   (CP_XR + c_dxCard + c_dxMargin)
#define C_VIEW_HEIGHT  (CP_YB + c_dyCard + c_dyMargin)


#define CP_XT1	200		// karty le¿¹ce na stole
#define CP_XT3	CP_XT1
#define CP_YT1	195
#define CP_XT2	170
#define CP_YT2	150
#define CP_YT3	125
#define CP_XT4	230
#define CP_YT4	CP_YT2


/*
#ifdef _DEBUG
	const char c_sBitmapModule[] = _T("RCardsd.dll"); 
#else
	const char c_sBitmapModule[] = _T("RCards.dll"); 
#endif
	*/