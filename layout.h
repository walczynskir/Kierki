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


static const int c_dxMargin = 90;			//margines od brzegu do karty
static const int c_dyMargin = c_dxMargin;	//margines od brzegu do karty
static const int c_dxCardGap = 15;			// odleg³oœæ miêdzy kartami
static const int c_dyCardGap = c_dxCardGap;			// odleg³oœæ miêdzy kartami
static const int c_dxInterGap = 30;		// odleg³oœæ miêdzy kartami pionowymi i poziomymi
static const int c_dyInterGap = 20;			// odleg³oœæ miêdzy kartami poziomymi i pionowymi
static const int c_dyNameHeight = 20;		// height of rect for drawing names
static const int c_dxCard = 70;				// cards width
static const int c_dyCard = 90;				// cards height
static const int c_iLaidCardsOverlap = 4;	// overlap of laid cards
static const int c_iLaidCardsShift = 4;		// shift of laid cards
static const int c_dxPass = 80;
static const int c_dyPass = 80;
static const int c_dxHelp = 120;			// width of help / instructions panel



#define CP_XL	(c_dxMargin)							// po³o¿enie kart X lewych
#define CP_XR	(CP_XL + c_dxCard + c_dxInterGap + c_dxCard + c_dxCardGap * 12 + c_dxInterGap)		// po³o¿enie kart X prawych (right)
#define CP_XH	(c_dxMargin + c_dxCard + c_dxInterGap)	// po³o¿enie kart X horyzontalnych
#define CP_YT	(c_dyMargin)		// po³o¿enie Y kart górnych (top)
#define CP_YB	(c_dxMargin + c_dyCard + c_dyInterGap + c_dyCard + 12 * c_dyCardGap + c_dyInterGap)		// po³o¿enie Y kart dolnych (bottom)
#define	CP_YV	(CP_YT + c_dyCard + c_dyInterGap) // po³o¿enie Y kart vertykalnych

// definition of rectangles of all players cards
#define CP1_LEFT		(CP_XH)
#define CP1_TOP			(CP_YB)
#define CP1_RIGHT		(CP1_LEFT + c_dxCard + 12 * c_dxCardGap)
#define CP1_BOTTOM		(CP1_TOP + c_dyCard)

#define CP2_LEFT		(CP_XL)
#define CP2_TOP			(CP_YV)
#define CP2_RIGHT		(CP2_LEFT + c_dxCard)
#define CP2_BOTTOM		(CP2_TOP + c_dyCard + 12 * c_dyCardGap)

#define CP3_LEFT		(CP_XH)
#define CP3_TOP			(CP_YT)
#define CP3_RIGHT		(CP3_LEFT + c_dxCard + 12 * c_dxCardGap)
#define CP3_BOTTOM		(CP3_TOP + c_dyCard)

#define CP4_LEFT		(CP_XR)
#define CP4_TOP			(CP_YV)
#define CP4_RIGHT		(CP4_LEFT + c_dxCard)
#define CP4_BOTTOM		(CP4_TOP + c_dyCard + 12 * c_dyCardGap)


#define C_VIEW_WIDTH   (CP_XR + c_dxCard + c_dxMargin)
#define C_VIEW_HEIGHT  (CP_YB + c_dyCard + c_dyMargin)

