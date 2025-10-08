#include "StdAfx.h"
#include "Kierki.h"
#include "GameWnd.h"
#include "GameWndData.h"
#include "FontFactory.h"
#include <RCards/RCards.h>
#include <RCards/resource.h>
#include <rcommon/RMemDC.h>
#include <rcommon/RKeyboard.h>
#include <rcommon/drawutl.h>
#include <rcommon/RBtnWnd.h>
#include <rcommon/RSystemExc.h>
#include <rcommon/SafeWndProc.hpp>
#include <RCards/resource.h>
#include <commctrl.h>
#include "layout.h"


static const TCHAR cc_sWindowClass[] = _T("GAMEWND");	// game window class name
static GameWndData* GetData(HWND a_hWnd);

static LRESULT GameWnd_WndProc(HWND, UINT, WPARAM, LPARAM);

inline static LRESULT OnCreate(HWND a_hWnd, LPCREATESTRUCT a_pCreateStruct);
inline static void OnNcDestroy(HWND a_hWnd);
inline static void OnSize(HWND a_hWnd);
inline static void OnPaint(HWND a_hWnd);
inline static void OnLButtonDown(HWND a_hWnd, int a_x, int a_y);
inline static void OnLButtonUp(HWND a_hWnd, int a_x, int a_y) ;
inline static void OnMouseMove(HWND a_hWnd, UINT a_nFlags, int a_x, int a_y);
inline static void OnCaptureChanged(HWND a_hWnd);
inline static void OnNotify(HWND a_hWnd, LPNMHDR a_pNmHdr);

inline static void OnSetBrightness(HWND a_hWnd, BYTE a_btBrightness);
inline static BYTE OnGetBrightness(HWND a_hWnd, BOOL* a_pSet);


static void CalculatePositions(HWND a_hWnd);

static void InvalidateAllCards(HWND a_hWnd, BOOL a_bInvTable, BOOL a_bInvPuzzle);

static void InvalidateCards(HWND a_hWnd, short a_iInHand, T_PLAYER a_enPlayer, BOOL a_bPuzzle);
static void InvalidateCardHorz(HWND a_hWnd, short a_iCard, T_PLAYER a_enPlayer,	BOOL a_bAllCard);
static void InvalidateCardVert(HWND	a_hWnd, short a_iCard, T_PLAYER	a_enPlayer);
static void InvalidatePuzzleCard(HWND a_hWnd, const CCard& a_card);
static void InvalidateTableCards(HWND a_hWnd, BOOL a_bAll, T_PLAYER a_enPlayer);
static void InvalidateAllPuzzleRows(HWND a_hWnd);
static void InvalidatePuzzleRow(HWND a_hWnd, T_COLOR a_enColor);
static void InvalidateAllPlayersCards(HWND a_hWnd);
static void InvalidatePlayerCards(HWND a_hWnd, T_PLAYER a_enPlayer);
static void InvalidateResult(HWND a_hWnd, bool a_bAll, T_PLAYER a_enPlayer);
static void InvalidateAfterTrumpsChoice(HWND a_hWnd);

// positions
static int LeftCardEdgePuzzle(T_COLOR a_enColor);
static int TopCardEdgePuzzle(const CCard& a_card);

// bitmaps
static bool LoadBitmaps(HWND a_hWnd);

// drawing
static void Draw(HWND a_hWnd, HDC a_hDC);
static void DrawFelt(HWND a_hWnd, HDC a_hDC);

static void DrawNames(HWND a_hWnd, HDC a_hDC, bool a_bPass, bool a_bDrawTakenTricks);
static void DrawCardsHorz(HWND a_hWnd, HDC a_hDC, const CUserCards& a_pCards, LONG a_y, bool a_bReverse, short a_nStart, bool a_bHighlight);
static void DrawCardsVert(HWND a_hWnd, HDC a_hDC, const CUserCards& a_pCards, LONG a_x, bool a_bReverse, short a_nStart);
static void DrawTableCards(HWND a_hWnd,	HDC a_hDC);
static void DrawTableCard(HDC a_hDC, short a_iCardNr, LONG a_x, LONG a_y);
static void DrawPuzzle(HWND a_hWnd,	HDC a_hDC);
static void DrawPuzzleColor(HWND a_hWnd, HDC a_hDC,	T_COLOR a_enColor);

// mouse 
static void Capture(HWND a_hWnd, bool a_bGet);

// others
static short GetHighlighted(HWND a_hWnd, BOOL a_bHighlight);
static short GetClicked(HWND a_hWnd, const POINT& a_point);
static short GetCardFromPoint(HWND a_hWnd, const POINT& a_point, short a_nStartCard);
static short GetClickedTrumps(HWND a_hWnd, const POINT& a_point);
static void ClickOnCardsTrumps(HWND a_hWnd, const POINT& a_point);
static void ClickOnCards(HWND a_hWnd, const POINT& a_point);
static void ClickConfirmTrick(HWND a_hWnd, const POINT& a_pt);
static void TrumpsChosen(HWND a_hWnd, T_COLOR a_enColor);

static void NewDeal(HWND a_hWnd, BOOL a_bNextGame, BOOL a_bOpen);
static void ThrowCardPuzzle(HWND a_hWnd, short a_nInHand, T_PLAYER a_enPlayer);
static void ThrowCard(HWND a_hWnd, short a_nInHand, T_PLAYER a_enPlayer);
static void PlayPuzzle(HWND a_hWnd);
static void Play(HWND a_hWnd);
static inline POINT PassPos(HWND a_hWnd, T_PLAYER a_enPlayer, const SIZE& a_sizePass);
static void TrumpsChoice(HWND a_hWnd);
static void TrickTaking(HWND a_hWnd);
inline static void ShowPass(HWND a_hWnd, T_PLAYER a_enPlayer);


static bool AreCardsToPlayPuzzle(HWND a_hWnd);
static bool CanPlayCardPuzzle(HWND a_hWnd, short a_nCard, T_PLAYER a_enPlayer);
static bool CanPlayCard(HWND a_hWnd, short a_nCard, T_PLAYER a_enPlayer);
static bool CanPlayFirstCard(HWND a_hWnd, const CCard& a_card, T_PLAYER a_enPlayer);





// TODO consider using resource.h for IDB_... definitions
#define IDB_NOTRUMP  1001

// TODO add this to registry
// only for debug purposes
#ifdef _DEBUG
//#define DEBUG_SHOWCARD
#endif


typedef struct SCreateData
{
	HWND m_hWndApp;
	GameData* m_pGameData;
} RCreateData;


HWND GameWnd_Create(DWORD a_dwStyleEx, DWORD a_dwStyle, int a_x, int a_y,
	int a_dx, int a_dy, HWND a_hWndParent, HINSTANCE a_hInst, HWND a_hWndApp, GameData* a_pGameData)
{
	RCreateData l_data = { a_hWndApp, a_pGameData };
	return ::CreateWindowEx(a_dwStyleEx, cc_sWindowClass, _T(""), a_dwStyle, a_x, a_y, 
		a_dx, a_dy, a_hWndParent, NULL, a_hInst, &l_data);
}


ATOM GameWnd_Register(HINSTANCE a_hInst)
{
	WNDCLASSEX l_wcex;

	l_wcex.cbSize			= sizeof(WNDCLASSEX); 
	l_wcex.style			= CS_HREDRAW | CS_VREDRAW;
	l_wcex.lpfnWndProc		= SafeWndProc<GameWnd_WndProc>;
	l_wcex.cbClsExtra		= 0;
	l_wcex.cbWndExtra		= sizeof(GameWndData*);
	l_wcex.hInstance		= a_hInst;
	l_wcex.hIcon			= NULL;
	l_wcex.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	l_wcex.hbrBackground	= NULL;
	l_wcex.lpszMenuName		= NULL;
	l_wcex.lpszClassName	= cc_sWindowClass;
	l_wcex.hIconSm			= NULL;
	return ::RegisterClassEx(&l_wcex);
}


void GameWnd_NewDeal(HWND a_hWnd, bool a_bOpen)
{
	GameWndData* l_pData = GetData(a_hWnd);
	l_pData->m_bConfirmTrick = false;
	l_pData->m_enPassPlayer = E_DL_NULL;
	::RedrawWindow(a_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW);

	// zaczynamy grê
	if (l_pData->m_pGameData->GetGame() == E_GM_PUZZLE)
	{
		PlayPuzzle(a_hWnd);
	}
	else
	{
		Play(a_hWnd);
	}
}


void GameWnd_Refresh(HWND a_hWnd)
{
	GameWndData* l_pData = GetData(a_hWnd);

	if (!LoadBitmaps(a_hWnd))
		throw RSystemExc(_T("LOAD_BITMAP_FAILED"));
}


GameWndData* GetData(HWND a_hWnd)
{
	return reinterpret_cast<GameWndData*>(::GetWindowLongPtr(a_hWnd, GWLP_USERDATA));
}


//
//  FUNCTION: GameWnd_WndProc(HWND, unsigned, WORD, LONG)
//
LRESULT GameWnd_WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg) 
	{
	case WM_CREATE:
		return OnCreate(a_hWnd, reinterpret_cast<LPCREATESTRUCT>(a_lParam));

	case WM_NCDESTROY:
		OnNcDestroy(a_hWnd);
		break;

	case WM_SIZE:
		OnSize(a_hWnd);
		break;

	case WM_PAINT:
		OnPaint(a_hWnd);
		break;

	case WM_LBUTTONDOWN:
		OnLButtonDown(a_hWnd, LOWORD(a_lParam), HIWORD(a_lParam));
		break;

	case WM_LBUTTONUP:
		OnLButtonUp(a_hWnd, LOWORD(a_lParam), HIWORD(a_lParam));
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(a_hWnd, static_cast<UINT>(a_wParam), LOWORD(a_lParam), HIWORD(a_lParam));
		break;

	case WM_CAPTURECHANGED:
		OnCaptureChanged(a_hWnd);
		break;

	case WM_NOTIFY:
		OnNotify(a_hWnd, reinterpret_cast<LPNMHDR>(a_lParam));
		break;

	case WM_APP_SETBRIGHTNESS:
		OnSetBrightness(a_hWnd, static_cast<BYTE>(a_wParam));
		break;

	case WM_APP_GETBRIGHTNESS:
		return OnGetBrightness(a_hWnd, reinterpret_cast<BOOL*>(a_lParam));

	default:
		return ::DefWindowProc(a_hWnd, a_iMsg, a_wParam, a_lParam);
	}
	return 0;
}


// ---------------------------------------------------------
// handles creation of window
LRESULT 
OnCreate(
	HWND a_hWnd,					//IN game wnd
	LPCREATESTRUCT a_pCreateStruct	//IN create struct
	)
{
	RCreateData* l_pCreateData = 
		reinterpret_cast<RCreateData*>(a_pCreateStruct->lpCreateParams);
	GameWndData* l_pData = new GameWndData(l_pCreateData->m_hWndApp, l_pCreateData->m_pGameData);
	if (l_pData == NULL)
	{
		return -1;
	}
	::SetWindowLongPtr(a_hWnd, GWLP_USERDATA, (LONG_PTR)l_pData);

	if (!LoadBitmaps(a_hWnd))
	{
		return -1;
	}

	return 0;
}


void OnNcDestroy(HWND a_hWnd)
{
	GameWndData* l_pData = GetData(a_hWnd);
	delete l_pData;
}

// ---------------------------------------------------------
// handles resizing of window
//
void OnSize(HWND a_hWnd)
{
	CalculatePositions(a_hWnd);
}

// ---------------------------------------------------------
// Painting
void OnPaint(
	HWND a_hWnd		//IN game wnd
	)
{
	PAINTSTRUCT l_ps;
	HDC l_hdc = ::BeginPaint(a_hWnd, &l_ps);

	RECT l_rectWin;
	::GetClientRect(a_hWnd, &l_rectWin);
	{	
		// prepare correct DC
#ifdef _DEBUG 
		HDC l_hPaintDC = l_hdc;
#else
		RMemDC l_memDC(l_hdc, &l_rectWin);
		HDC l_hPaintDC = l_memDC;
#endif
		Draw(a_hWnd, l_hPaintDC);
	}
	::EndPaint(a_hWnd, &l_ps);
}


// 
// calculates positions of window elements
//
void CalculatePositions(HWND a_hWnd)
{
	GameWndData* l_pData = GetData(a_hWnd);

	// calculate positions of users cards
	for (short l_iAt = 0; l_iAt < 13; l_iAt++)
	{
		l_pData->m_arrHorzPos[l_iAt].m_iLeft = CP_XH + c_dxCardGap * (l_iAt);
		l_pData->m_arrHorzPos[l_iAt].m_iRight = l_pData->m_arrHorzPos[l_iAt].m_iLeft + c_dxCard;
		l_pData->m_arrVertPos[l_iAt].m_iTop = CP_YV + c_dyCardGap * (l_iAt);
		l_pData->m_arrVertPos[l_iAt].m_iBottom = l_pData->m_arrVertPos[l_iAt].m_iTop + c_dyCard;
	}

	RECT l_rect;
	::GetClientRect(a_hWnd, &l_rect);

	POINT l_ptMid = { RectWidth(l_rect) / 2, RectHeight(l_rect) / 2 };

	// calculate positions of laid cards
	l_pData->m_ptsLaidCards[E_DL_1].x = l_ptMid.x - c_dxCard / 2 - c_iLaidCardsShift;
	l_pData->m_ptsLaidCards[E_DL_1].y = l_ptMid.y - c_iLaidCardsOverlap;
	l_pData->m_ptsLaidCards[E_DL_3].x = l_pData->m_ptsLaidCards[E_DL_1].x + 2 * c_iLaidCardsShift;
	l_pData->m_ptsLaidCards[E_DL_3].y = l_ptMid.y - c_dyCard + c_iLaidCardsOverlap;

	l_pData->m_ptsLaidCards[E_DL_2].x = l_ptMid.x - c_dxCard + c_iLaidCardsOverlap;
	l_pData->m_ptsLaidCards[E_DL_2].y = l_ptMid.y - c_dyCard / 2 - c_iLaidCardsShift;
	l_pData->m_ptsLaidCards[E_DL_4].x = l_ptMid.x - c_iLaidCardsOverlap;
	l_pData->m_ptsLaidCards[E_DL_4].y = l_pData->m_ptsLaidCards[E_DL_2].y + 2 * c_iLaidCardsShift;

	// calculate invalidate rect of laid cards
	l_pData->m_rectLaidCards.left = l_pData->m_ptsLaidCards[E_DL_2].x;
	l_pData->m_rectLaidCards.top = l_pData->m_ptsLaidCards[E_DL_3].y;
	l_pData->m_rectLaidCards.right = l_pData->m_ptsLaidCards[E_DL_4].x + c_dxCard;
	l_pData->m_rectLaidCards.bottom = l_pData->m_ptsLaidCards[E_DL_1].y + c_dyCard;

	// calculate names RECT
	l_pData->m_rectsNames[E_DL_1].left = CP1_LEFT;
	l_pData->m_rectsNames[E_DL_1].top = CP1_BOTTOM + c_dyMargin - c_dyNameHeight;
	l_pData->m_rectsNames[E_DL_1].right = CP1_RIGHT;
	l_pData->m_rectsNames[E_DL_1].bottom = l_pData->m_rectsNames[E_DL_1].top + c_dyNameHeight;

	l_pData->m_rectsNames[E_DL_2].left = 0;
	l_pData->m_rectsNames[E_DL_2].top = CP2_TOP - c_dyNameHeight;
	l_pData->m_rectsNames[E_DL_2].right = CP2_RIGHT;
	l_pData->m_rectsNames[E_DL_2].bottom = l_pData->m_rectsNames[E_DL_2].top + c_dyNameHeight;

	l_pData->m_rectsNames[E_DL_3].left = CP3_LEFT;
	l_pData->m_rectsNames[E_DL_3].top = CP3_TOP - c_dyNameHeight;
	l_pData->m_rectsNames[E_DL_3].right = CP3_RIGHT;
	l_pData->m_rectsNames[E_DL_3].bottom = l_pData->m_rectsNames[E_DL_3].top + c_dyNameHeight;

	l_pData->m_rectsNames[E_DL_4].left = CP4_LEFT;
	l_pData->m_rectsNames[E_DL_4].top = CP4_TOP - c_dyNameHeight;
	l_pData->m_rectsNames[E_DL_4].right = CP4_RIGHT + c_dxMargin;
	l_pData->m_rectsNames[E_DL_4].bottom = l_pData->m_rectsNames[E_DL_4].top + c_dyNameHeight;

	l_pData->m_ptsPass[E_DL_1].x = l_ptMid.x - c_dxPass / 2;
	l_pData->m_ptsPass[E_DL_1].y = CP1_BOTTOM;

	l_pData->m_ptsPass[E_DL_2].x = RectWidth(l_pData->m_rectsNames[E_DL_2]) / 2 - c_dxPass / 2;
	l_pData->m_ptsPass[E_DL_2].y = CP2_TOP - c_dyPass;

	l_pData->m_ptsPass[E_DL_3].x = l_ptMid.x - c_dxPass / 2;
	l_pData->m_ptsPass[E_DL_3].y = CP3_TOP - c_dyPass;

	l_pData->m_ptsPass[E_DL_4].x = l_pData->m_rectsNames[E_DL_4].left + RectWidth(l_pData->m_rectsNames[E_DL_4]) / 2 - c_dxPass / 2;
	l_pData->m_ptsPass[E_DL_4].y = CP4_TOP - c_dyPass;
}


// ---------------------------------------------------------
// Really draws window
void 
Draw(
	 HWND a_hWnd,	//IN game wnd
	 HDC a_hDC		//IN device context
	 )
{
	GameWndData* l_pData = GetData(a_hWnd);
	GameData* l_pGameData = l_pData->m_pGameData;

	DrawFelt(a_hWnd, a_hDC);

	if ((!l_pGameData->IsDealed()) && (!l_pGameData->IsTrumpsChoice()))
	{
		return ;
	}

	short l_nStartCard;
	if (l_pGameData->IsTrumpsChoice())
	{
		l_nStartCard = 6;
	}
	else
	{
		l_nStartCard = 12;
	}
	DrawCardsHorz(a_hWnd, a_hDC, l_pGameData->GetPlayerCards(E_DL_1), CP_YB, FALSE, l_nStartCard, TRUE);
	DrawCardsVert(a_hWnd, a_hDC, l_pGameData->GetPlayerCards(E_DL_2), CP_XL, TRUE, l_nStartCard);
	DrawCardsHorz(a_hWnd, a_hDC, l_pGameData->GetPlayerCards(E_DL_3), CP_YT, TRUE, l_nStartCard, FALSE);
	DrawCardsVert(a_hWnd, a_hDC, l_pGameData->GetPlayerCards(E_DL_4), CP_XR, TRUE, l_nStartCard);

	bool l_bDrawTakenTricks = false;
	if (l_pGameData->GetGame() != E_GM_PUZZLE)
	{
		l_bDrawTakenTricks = true ;
		if (l_pGameData->GetTricksCnt() > 0)
		{
			DrawTableCards(a_hWnd, a_hDC) ;	// cards on the table
		}
	}
	else
	{
		DrawPuzzle(a_hWnd, a_hDC);
	}
	if (l_pData->m_enPassPlayer != E_DL_NULL)
	{	
		RDraw::DrawTransparentAlphaBlend(a_hDC, l_pData->m_ptsPass[l_pData->m_enPassPlayer].x, l_pData->m_ptsPass[l_pData->m_enPassPlayer].y, l_pData->GetBmpPass(), 255);
	}
	DrawNames(a_hWnd, a_hDC, l_pGameData->IsPass(), l_bDrawTakenTricks);
}


static void DrawFelt(HWND a_hWnd, HDC a_hDC)
{
	GameWndData* l_pData = GetData(a_hWnd);

	// Create compatible DC and select bitmap
	HDC l_hdcMem = ::CreateCompatibleDC(a_hDC);
	HBITMAP l_hBmpOld = (HBITMAP)::SelectObject(l_hdcMem, l_pData->GetBmpFelt());

	// Get bitmap size
	BITMAP l_bmp;
	GetObject(l_pData->GetBmpFelt(), sizeof(BITMAP), &l_bmp);

	RECT l_rect;
	::GetClientRect(a_hWnd, &l_rect);
	// Stretch or tile the bitmap
	::StretchBlt(a_hDC, 0, 0, RectWidth(l_rect), RectHeight(l_rect),
		l_hdcMem, 0, 0, l_bmp.bmWidth, l_bmp.bmHeight, SRCCOPY);

	// Cleanup
	::SelectObject(l_hdcMem, l_hBmpOld);
	::DeleteDC(l_hdcMem);

	RDraw::BlendOverlay(a_hDC, l_rect, l_pData->m_pGameData->m_regData.m_regHidden.m_clrTintGameBackground, l_pData->m_pGameData->m_regData.m_regAuto.m_btAlphaGameBackground);
}

// ---------------------------------------------------------
// Inwalidacja (bez wymuszenia rysowania) wszystkich kart 
// le¿¹cych na stole.
// Po metodzie nale¿y wywo³aæ UpdateWindow()
// 
void InvalidateAllCards(
	HWND a_hWnd,		//IN game wnd
	BOOL a_bInvTable,	//IN TRUE - invalidate table cards
	BOOL a_bInvPuzzle 	//IN TRUE - inwalidowaæ karty uk³.
	)
{
	if (a_bInvTable)
	{
		InvalidateTableCards(a_hWnd, TRUE, E_DL_1);
	}
	if (a_bInvPuzzle)
	{
		InvalidateAllPuzzleRows(a_hWnd);
	}
	InvalidateAllPlayersCards(a_hWnd);
}


// 
// Calculates position of pass rect in client coordinates
//
POINT PassPos(HWND a_hWnd, T_PLAYER a_enPlayer, const SIZE& a_sizePass)
{
	POINT l_point;
	RECT l_rectView;
	::GetClientRect(a_hWnd, &l_rectView);

	switch (a_enPlayer)
	{
	case E_DL_1:
		l_point.x = CP_XH + (c_dxCard + 12 * c_dxCardGap) + 7;
//		l_point.x = CP_XH - a_sizePass.cx;
		l_point.y = CP_YB + c_dyCard / 2;
		break;

	case E_DL_2:
		l_point.x = CP_XL +30;
		l_point.y = CP_YV + (c_dyCard + 12 * c_dyCardGap) + 20;
		break;

	case E_DL_3:
		l_point.x = CP_XH - 7;
		l_point.y = CP_YT + c_dyCard / 2;
		break;

	case E_DL_4:
		l_point.x = CP_XR + c_dxCard / 2;
		l_point.y = CP_YV - 20 ;
		break;

	default:
		ASSERT(FALSE);
	}
	l_point.x -= (a_sizePass.cx / 2);
	l_point.y -= (a_sizePass.cy / 2);
	return l_point;
}


// ---------------------------------------------------------
//	Invalidates throwed card
//
void InvalidateCards(HWND a_hWnd, short a_iInHand, T_PLAYER a_enPlayer,	BOOL a_bPuzzle)
{
	switch (a_enPlayer)
	{
	case E_DL_1:
	case E_DL_3:
		InvalidateCardHorz(a_hWnd, a_iInHand, a_enPlayer, FALSE);
		break ;
	case E_DL_2:
	case E_DL_4:
		InvalidateCardVert(a_hWnd, a_iInHand, a_enPlayer);
		break ;
	}
	if (a_bPuzzle)
	{
		const CCard& l_card = GetData(a_hWnd)->m_pGameData->GetPlayerCard(a_enPlayer, a_iInHand);
		InvalidatePuzzleCard(a_hWnd, l_card);
	}
	else
	{
		InvalidateTableCards(a_hWnd, FALSE, a_enPlayer);
	}
}


// ---------------------------------------------------------
//	Starts trumps choice
//
void TrumpsChoice(HWND a_hWnd)
{
	GameWndData* l_pData = GetData(a_hWnd);
	::SendMessage(l_pData->m_hWndApp, WM_APP_CHOOSETRUMPS, 0, 0);
	if (l_pData->m_hWndNoTrump == NULL)
	{

		HBITMAP l_hBmp = ::LoadBitmap(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_NOTRUMPS));
		ASSERT(l_hBmp != NULL);
		BITMAP l_bmp;
		VERIFY(::GetObject(l_hBmp, sizeof(l_bmp), &l_bmp) != 0);
		int l_x = CP_XH + 8 * c_dxCardGap + c_dxCard;
		::DeleteObject(l_hBmp);

		RBtnWnd_RegisterClass();
		l_pData->m_hWndNoTrump = RBtnWnd_CreateEx(WS_VISIBLE | WS_CHILD, 0, _T(""),
			l_x, CP_YB + c_dyCard / 2 - 15, l_bmp.bmWidth / 3, l_bmp.bmHeight, a_hWnd, 
			IDB_NOTRUMP);
		TSetBitmap l_setbmp;
		l_setbmp.hInst = ::GetModuleHandle(NULL);
		l_setbmp.idBmp = IDB_NOTRUMPS;
		l_setbmp.iCount = 3;
		l_setbmp.clrMask = RGB(255, 0, 0);
		RBtnWnd_SetBitmap(l_pData->m_hWndNoTrump, &l_setbmp);
	}
	::ShowWindow(l_pData->m_hWndNoTrump, SW_SHOW);
	InvalidateAllPlayersCards(a_hWnd);
	InvalidateTableCards(a_hWnd, TRUE, E_DL_1);
	InvalidateResult(a_hWnd, true, E_DL_1);
	::UpdateWindow(a_hWnd);
}


// ---------------------------------------------------------
//	Invalidates horizontal card
//
void 
InvalidateCardHorz(
	HWND     a_hWnd,		//IN game wnd
	short	 a_iCard,		//IN card nr
	T_PLAYER a_enPlayer,	//IN player
	BOOL     a_bAllCard		//IN invalidate all card
	)
{
	// dla karty 13 inwalidujemy ca³y prostok¹t
	RECT l_rectInv ;
	switch (a_enPlayer)
	{
	case E_DL_1:
		l_rectInv.top = CP_YB ;
		break ;

	case E_DL_3:
		l_rectInv.top = CP_YT ;
		break ;
	default:
		ASSERT(FALSE) ;	// co za babol !!!
	}

	l_rectInv.bottom = l_rectInv.top + c_dyCard;

	GameWndData* l_pData = GetData(a_hWnd);
	if (a_iCard == 12 || a_bAllCard)
	{
		l_rectInv.left = l_pData->m_arrHorzPos[a_iCard].m_iLeft ;
		l_rectInv.right = l_pData->m_arrHorzPos[a_iCard].m_iRight ;
		::InvalidateRect(a_hWnd, &l_rectInv, TRUE);
		return;
	}

	short l_iCard = a_iCard + 1; // karta sprawdzana
	short l_dxWidth = c_dxCardGap ; // szerokoœæ rysowana
	short l_iAt = (short)(c_dxCard / c_dxCardGap) + 1;	// ile kart trzeba sprawdziæ
	// dla reszty kart trzeba sprawdziæ, czy s¹ nastêpne
	while (l_iAt-- > 0)
	{
		// nienarysowana - wychodzimy
		if (!l_pData->m_pGameData->GetPlayerCards(a_enPlayer).IsUsed(l_iCard))
			break ;
		
		// wszystkie nastêpne karty nie pokazane - t¹ te¿ trzeba narysowaæ w ca³oœci 
		if (l_iCard == 12)
		{
			l_dxWidth = c_dxCard ;
			break ;
		}
		l_dxWidth = l_dxWidth + c_dxCardGap ;
		l_iCard++ ; // nastêpna sprawdzana
	}
	
	l_rectInv.left = l_pData->m_arrHorzPos[a_iCard].m_iLeft ;
	l_rectInv.right = l_rectInv.left + l_dxWidth ;
	::InvalidateRect(a_hWnd, &l_rectInv, TRUE);
}


// ---------------------------------------------------------
//	Zinwalidowanie karty horz.
//
void 
InvalidateCardVert(
	HWND		a_hWnd,		//IN game wnd
	short		a_iCard,	//IN card number
	T_PLAYER	a_enPlayer	//IN player
	)
{
	ASSERT((a_iCard >=0) && (a_iCard < 13));
	// dla karty 13 inwalidujemy ca³y prostok¹t
	RECT l_rectInv ;

	switch (a_enPlayer)
	{
	case E_DL_2:
		l_rectInv.left = CP_XL;
		break ;

	case E_DL_4:
		l_rectInv.left = CP_XR;
		break ;
	default:
		ASSERT(FALSE) ;	// co za babol !!!
	}

	l_rectInv.right = l_rectInv.left + c_dxCard ;

	GameWndData* l_pData = GetData(a_hWnd);
	if (a_iCard == 12)
	{
		l_rectInv.top = l_pData->m_arrVertPos[a_iCard].m_iTop ;
		l_rectInv.bottom = l_pData->m_arrVertPos[a_iCard].m_iBottom ;
		::InvalidateRect(a_hWnd, &l_rectInv, TRUE);
		return;
	}

	short l_iCard = a_iCard + 1; // karta sprawdzana
	short l_dyHeight = c_dxCardGap ; // szerokoœæ rysowana
	short l_iAt = (short)(c_dyCard / c_dxCardGap) + 1;	// ile kart trzeba sprawdziæ
	// dla reszty kart trzeba sprawdziæ, czy s¹ nastêpne
	while (l_iAt-- > 0)
	{
		// nienarysowana - wychodzimy
		if (!l_pData->m_pGameData->GetPlayerCards(a_enPlayer).IsUsed(l_iCard))
			break;
		
		// wszystkie nastêpne karty nie pokazane - t¹ te¿ trzeba narysowaæ w ca³oœci 
		if (l_iCard == 12)
		{
			l_dyHeight = c_dyCard ;
			break ;
		}
		l_dyHeight = l_dyHeight + c_dyCardGap ;
		l_iCard++ ; // nastêpna sprawdzana
	}
	
	l_rectInv.top = l_pData->m_arrVertPos[a_iCard].m_iTop ;
	l_rectInv.bottom = l_rectInv.top + l_dyHeight ;
	::InvalidateRect(a_hWnd, &l_rectInv, TRUE);
}


// ---------------------------------------------------------
//	Invalidates cards on table
//
void 
InvalidateTableCards(
	HWND a_hWnd,		//IN card on table
	BOOL a_bAll,		//IN all cards
	T_PLAYER a_enPlayer	//WE for player
	)
{
	GameWndData* l_pData = GetData(a_hWnd);
	if (a_bAll)
	{
		::InvalidateRect(a_hWnd, &(l_pData->m_rectLaidCards), TRUE);
	}
	else
	{
		RECT l_rect; 
		::SetRect(&l_rect, l_pData->m_ptsLaidCards[a_enPlayer].x, l_pData->m_ptsLaidCards[a_enPlayer].y, l_pData->m_ptsLaidCards[a_enPlayer].x + c_dxCard, l_pData->m_ptsLaidCards[a_enPlayer].y + c_dyCard);
		::InvalidateRect(a_hWnd, &l_rect, TRUE) ;
	}
}


// ---------------------------------------------------------
//	Invalidates card on table - puzzle
//
void 
InvalidatePuzzleCard(
	HWND  a_hWnd,		//IN game wnd
	const CCard& a_card	//WE card to invalidate
	)
{
	RECT l_rect = {
		LeftCardEdgePuzzle(a_card.GetColor()),
		TopCardEdgePuzzle(a_card),
		LeftCardEdgePuzzle(a_card.GetColor()) + c_dxCard,
		TopCardEdgePuzzle(a_card) + c_dyCard};

	::InvalidateRect(a_hWnd, &l_rect, TRUE);
}


// ------------------------------------------------------------
//	Calculates left corner for color in puzzle game
//
int		//IN left edge
LeftCardEdgePuzzle(
	T_COLOR a_enColor	//WE dla koloru
	)
{
	int l_dxStart = C_VIEW_WIDTH / 2 + c_dxPuzzleGap / 2 - 
		(3 - a_enColor) * c_dxCard;

	return l_dxStart;
}


// ------------------------------------------------------------
//	Calculates top corner for color in puzzle game
//
int		//WY górna krawêdŸ
TopCardEdgePuzzle(
	const CCard& a_card	//WE karta
	)
{
	T_CARDVAL l_val = a_card.CardValue();

	// po³o¿enie E_CV_8
	int l_y8 = (C_VIEW_HEIGHT - c_dyCard) / 2;
	if (l_val == E_CV_8)
	{
		return l_y8;
	}
	else if (l_val > E_CV_8)
	{
		int l_y9 = l_y8 - c_dyCard / 2 - c_dxPuzzleGap;
		return l_y9 - (l_val - E_CV_9) * c_dyPuzzleGap;
	}
	else
	{
		int l_y7 = l_y8 + c_dyCard / 2 + c_dxPuzzleGap;
		return l_y7 + (E_CV_7 - l_val) * c_dyPuzzleGap;
	}

	ASSERT(FALSE);
	return -1;
}


// ---------------------------------------------------------
// Invalidate all puzzle rows
// Does not redraw window you must call UpdateWindow()
void 
InvalidateAllPuzzleRows(
	HWND a_hWnd		//IN game wnd
	)
{
	InvalidatePuzzleRow(a_hWnd, E_CC_CLUB);
	InvalidatePuzzleRow(a_hWnd, E_CC_DIAMOND);
	InvalidatePuzzleRow(a_hWnd, E_CC_SPADE);
	InvalidatePuzzleRow(a_hWnd, E_CC_HEART);
}

// ---------------------------------------------------------
// Invalidates one color row of puzzle game
// Does not redraw window you must call UpdateWindow()
void 
InvalidatePuzzleRow(
	HWND a_hWnd,		//IN game wnd
	T_COLOR a_enColor	//IN color
	)
{
	RECT l_rect;
	l_rect.left = LeftCardEdgePuzzle(a_enColor);
	l_rect.top = TopCardEdgePuzzle(CCard(a_enColor, E_CV_A));
	l_rect.right =  l_rect.left + c_dxCard;
	l_rect.bottom = TopCardEdgePuzzle(CCard(a_enColor, E_CV_2)) + c_dyCard;
	::InvalidateRect(a_hWnd, &l_rect, TRUE);
}


// ---------------------------------------------------------
// Invalidates all players cards
// Does not redraw window you must call UpdateWindow()
void 
InvalidateAllPlayersCards(
	HWND a_hWnd	//IN game wnd
	)
{
	InvalidatePlayerCards(a_hWnd, E_DL_1);
	InvalidatePlayerCards(a_hWnd, E_DL_2);
	InvalidatePlayerCards(a_hWnd, E_DL_3);
	InvalidatePlayerCards(a_hWnd, E_DL_4);
}


// ---------------------------------------------------------
// Invalidates player's cards
// Does not redraw window you must call UpdateWindow()
void InvalidatePlayerCards(HWND a_hWnd, T_PLAYER a_enPlayer)
{
	GameWndData* l_pData = GetData(a_hWnd);
	RECT l_rect;
	switch (a_enPlayer)
	{
		case E_DL_1:
		case E_DL_3:
			l_rect.left = l_pData->m_arrHorzPos[0].m_iLeft;
			l_rect.right = l_pData->m_arrHorzPos[12].m_iRight;
			break;

		case E_DL_2:
		case E_DL_4:
			l_rect.top = l_pData->m_arrVertPos[0].m_iTop;
			l_rect.bottom = l_pData->m_arrVertPos[12].m_iBottom;
			break;

		default:
			ASSERT(FALSE); // nie powinno siê zdarzyæ
	}

	switch (a_enPlayer)
	{
		case E_DL_1:
			l_rect.top = CP_YB;
			l_rect.bottom = l_rect.top + c_dyCard;
			break;

		case E_DL_3:
			l_rect.top = CP_YT;
			l_rect.bottom = l_rect.top + c_dyCard;
			break;

		case E_DL_2:
			l_rect.left = CP_XL;
			l_rect.right = l_rect.left + c_dxCard;
			break;

		case E_DL_4:
			l_rect.left = CP_XR;
			l_rect.right = l_rect.left + c_dxCard;
			break;

		default:
			ASSERT(FALSE); // nie powinno siê zdarzyæ
	}
	::InvalidateRect(a_hWnd, &l_rect, TRUE);
}



// --------------------------------------------------------
// Draws names of players
//
void 
DrawNames(
	HWND a_hWnd,	//IN game wnd
	HDC  a_hDC,		//IN device context
	bool a_bPass,	//IN is pass
	bool a_bTakenTricksCount //IN draw taken tricks count
	)
{
	GameWndData* l_pData = GetData(a_hWnd);
	tstring l_sDrawText;

	// all players

	for (short l_iAt = E_DL_1; l_iAt <= E_DL_4; l_iAt++)
	{
		tstring l_sDrawText;
		const CRegData& l_regData = l_pData->m_pGameData->m_regData;
		const tstring& l_sPlayerName = l_regData.GetPlayerName(static_cast<T_PLAYER>(l_iAt));
		if (a_bTakenTricksCount)
		{
			l_sDrawText = FormatTextT("{} ({})", l_sPlayerName, l_pData->m_pGameData->GetPlayerTricksCnt(static_cast<T_PLAYER>(l_iAt)));
		}
		else
		{
			l_sDrawText = l_sPlayerName;
		}

		HFONT l_hFont = CFontFactory::Instance().GetFont(a_hWnd, l_regData.m_regHidden.m_iGameFontSize);
		RDraw::DrawSmartText(a_hDC, l_hFont, l_pData->m_rectsNames[l_iAt], l_sDrawText.c_str());
	
	}

}


// ---------------------------------------------------------
//	draws horizontal cards
//
void 
DrawCardsHorz(
	HWND a_hWnd,				//IN game wnd
	HDC a_hDC,					//IN device context
	const CUserCards& a_pCards, //IN user cards
	LONG  a_y,					//IN position y
	bool a_bReverse,			//IN show cover
	short a_nStart,				//IN start card (from 0)
	bool a_bHighlight			//IN highlight card 
	)
{

	const CRegData& l_reg = GetData(a_hWnd)->m_pGameData->m_regData;
	if (l_reg.m_regHidden.m_bShowAllCards)
		a_bReverse = false;

	short l_nAt ;
	short l_x = CP_XH + (a_nStart + 1) * c_dxCardGap;
	short l_iCardNr ;
	short l_dxWidth = c_dxCard ;	// na jakiej szerokoœci rysowaæ kartê
	short l_nHighlighted = GetHighlighted(a_hWnd, a_bHighlight); // która podœwietlona
	int l_iHighlightedStart = l_x - (13 - l_nHighlighted) * c_dxCardGap;
	int l_iHighlightedEnd = l_iHighlightedStart + c_dxCard;

	// trzeba zacz¹æ od karty ostatniej - bo to jest na pewno nie zakryta
	// problem jest tylko w rysowaniu karty podœwietlonej, gdy siê od ty³u
	// rysuje.
	for (l_nAt = a_nStart; l_nAt >= 0; l_nAt--)
	{

		BOOL l_bDrawHighlighted = (l_nHighlighted == l_nAt);
		DWORD l_dwRop = l_bDrawHighlighted ? /*(SRCERASE | SRCAND)*/NOTSRCCOPY : SRCAND;
		l_x = l_x - c_dxCardGap;

		// jeœli kartê podœwietlamy to na ca³ej szerokoœci
		if (l_bDrawHighlighted)
		{
			l_dxWidth = c_dxCard;
		}
		// u¿ytej siê nie rysuje
		else if (a_pCards[l_nAt].IsUsed())
		{
			l_dxWidth = l_dxWidth + c_dxCardGap ;
			if (l_dxWidth > c_dxCard)
				l_dxWidth = c_dxCard ;
			continue ;
		}

		// czy w ogóle rysujemy kartê - czy nie przes³oniêta 
		// przez podœwietlon¹
		if ((l_nHighlighted >= 0) && (l_nAt != l_nHighlighted))
		{
			//czy przes³oniêty pocz¹tek naszej karty
			if ((l_x <= l_iHighlightedEnd) && (l_x > l_iHighlightedStart))
			{
				// jeœli koniec te¿ przes³oniêty to karty nie rysujemy
				if (l_x + l_dxWidth <= l_iHighlightedEnd)
				{
					l_dxWidth = c_dxCardGap ;
					continue;
				}
				// przesloniêta czêœciowo
				else
				{

				}
			}
		}

		
		BOOL l_bOk;
		l_iCardNr = a_pCards[l_nAt].GetNr();
		if (a_bReverse)
		{
			l_bOk = ::Cards_DrawCover(a_hDC, GetData(a_hWnd)->GetBmpCover(), 
				l_x, a_y, l_dxWidth, c_dyCard, c_dxCard, c_dyCard);
		}
		else
		{
			l_bOk = ::Cards_DrawCard(a_hDC, l_iCardNr - 1, 
				l_x, a_y, l_dxWidth, c_dyCard, c_dxCard, c_dyCard, l_bDrawHighlighted);
		}

		ASSERT(l_bOk);
		l_dxWidth = c_dxCardGap ;

	}
}


// ---------------------------------------------------------
//	draws vertical cards
//
void 
DrawCardsVert(
	HWND a_hWnd,				//IN game wnd
	HDC a_hDC,					//IN device context
	const CUserCards& a_pCards, //IN user cards
	LONG  a_x,					//IN position x
	bool a_bReverse,			//IN reverse
	short a_nStart				//IN start card (from 0)
	)	
{
	const CRegData& l_reg = GetData(a_hWnd)->m_pGameData->m_regData;
	if (l_reg.m_regHidden.m_bShowAllCards)
		a_bReverse = false;

	short l_iAt;
	short l_y = CP_YV + (a_nStart + 1) * c_dxCardGap;
	short l_iCardNr;
	short l_dyHeight = c_dyCard;	// na jakiej wysokoœci rysowaæ kartê

	// trzeba zacz¹æ od karty ostatniej - bo to jest na pewno nie zakryta
	for (l_iAt = a_nStart; l_iAt >= 0; l_iAt--)
	{
		// u¿ytej siê nie rysuje
		l_y = l_y - c_dxCardGap;
		if (a_pCards[l_iAt].IsUsed())
		{
			l_dyHeight = l_dyHeight + c_dxCardGap;
			if (l_dyHeight > c_dyCard)
				l_dyHeight = c_dyCard;
			continue ;
		}

		BOOL l_bOk;

		l_iCardNr = a_pCards[l_iAt].GetNr() ;

		if (a_bReverse)
		{
			l_bOk = ::Cards_DrawCover(a_hDC, GetData(a_hWnd)->GetBmpCover(), 
			  a_x, l_y, c_dxCard, l_dyHeight, c_dxCard, c_dyCard);
		}
		else
		{
			l_bOk = ::Cards_DrawCard(a_hDC, l_iCardNr - 1, 
			  a_x, l_y, c_dxCard, l_dyHeight, c_dxCard, c_dyCard, FALSE);
		}

		ASSERT(l_bOk);
		l_dyHeight = c_dxCardGap;
	}
}


// ---------------------------------------------------------
//	draws cards on table
//
void 
DrawTableCards(
	HWND a_hWnd,	//IN game wnd
	HDC  a_hDC		//IN device context
	)
{
	GameWndData* l_pData = GetData(a_hWnd);
	// last trick
	ASSERT(l_pData->m_pGameData->GetTricksCnt() <= 13) ;
	ASSERT(l_pData->m_pGameData->GetTricksCnt() >= 1) ;
	const CTrick& l_trick = l_pData->m_pGameData->GetLastTrick();
	
	// for all cards
	short l_iAt ;
	for (l_iAt = 0; l_iAt < l_trick.GetCardsCnt(); l_iAt++)
	{

		T_PLAYER l_enPlayer = l_trick.GetCardOwner(l_iAt);

		DrawTableCard(a_hDC, l_trick.GetCardNr(l_iAt), l_pData->m_ptsLaidCards[l_enPlayer].x, l_pData->m_ptsLaidCards[l_enPlayer].y);
	}
}


// ---------------------------------------------------------
//	Draws one card on table
//
void 
DrawTableCard(
	HDC   a_hDC,		//IN device context
	short a_iCardNr,	//IN card number
	LONG  a_x,			//IN x
	LONG  a_y			//IN y
	)
{

	ASSERT(a_iCardNr >= 1);
	BOOL l_bOk = ::Cards_DrawCard(a_hDC, a_iCardNr - 1, a_x, a_y, 
		c_dxCard, c_dyCard, c_dxCard, c_dyCard, FALSE);
	ASSERT(l_bOk) ;
}

// ------------------------------------------------------------
//	draws puzzle cards on table
//
void
DrawPuzzle(
	HWND a_hWnd,	//IN game wnd
	HDC  a_hDC		//IN device context
	)
{
	ASSERT(GetData(a_hWnd)->m_pGameData->GetGame() == E_GM_PUZZLE);
	DrawPuzzleColor(a_hWnd, a_hDC, E_CC_HEART);
	DrawPuzzleColor(a_hWnd, a_hDC, E_CC_DIAMOND);
	DrawPuzzleColor(a_hWnd, a_hDC, E_CC_SPADE);
	DrawPuzzleColor(a_hWnd, a_hDC, E_CC_CLUB);
}


// ------------------------------------------------------------
//	draws one color of puzzles on table
//
void 
DrawPuzzleColor(
	HWND a_hWnd,		//IN game wnd
	HDC  a_hDC,			//IN device context
	T_COLOR a_enColor	//IN color
	)
{
	ASSERT(GetData(a_hWnd)->m_pGameData->GetGame() == E_GM_PUZZLE);
	const CPuzzleRows& l_PuzzleRows = GetData(a_hWnd)->m_pGameData->GetPuzzleRows();
	const CCard* l_pCardTop = l_PuzzleRows.GetTopCard(a_enColor);
	// jeszcze nie ma ¿adnej karty w tym kolorze
	if (l_pCardTop == NULL) 
	{
		return;
	}

	const CCard* l_pCardBottom = l_PuzzleRows.GetBottomCard(a_enColor);
	if (l_pCardBottom == NULL)
	{
		return;
	}

	int l_dxStart = LeftCardEdgePuzzle(a_enColor);
	int l_dyStart;
	BOOL l_bTop = (l_pCardTop->CardValue() > E_CV_8);
	BOOL l_bBottom = (l_pCardBottom->CardValue() < E_CV_8);

	// najpierw 8
	CCard l_card8(a_enColor, E_CV_8);
	l_dyStart = TopCardEdgePuzzle(l_card8);
	
	VERIFY(::Cards_DrawCard(a_hDC, l_card8.GetNr() - 1,
		l_dxStart, l_dyStart, c_dxCard, c_dyCard, c_dxCard, c_dyCard, FALSE));

	// potem górê
	if (l_bTop)
	{
		int l_iVal;
		for (l_iVal = E_CV_9; l_iVal <= l_pCardTop->CardValue(); l_iVal++)
		{
			CCard l_cardTop(a_enColor, (T_CARDVAL)l_iVal);
			l_dyStart = TopCardEdgePuzzle(l_cardTop);
			VERIFY(::Cards_DrawCard(a_hDC, l_cardTop.GetNr() - 1,
				l_dxStart, l_dyStart, c_dxCard, c_dyCard, c_dxCard, c_dyCard, FALSE));
		}
	}


	// teraz dó³
	if (l_bBottom)
	{
		int l_iVal;
		for (l_iVal = E_CV_7; l_iVal >= l_pCardBottom->CardValue(); l_iVal--)
		{
			CCard l_cardBottom(a_enColor, (T_CARDVAL)l_iVal);
			l_dyStart = TopCardEdgePuzzle(l_cardBottom);
			VERIFY(::Cards_DrawCard(a_hDC, l_cardBottom.GetNr() - 1,
				l_dxStart, l_dyStart, c_dxCard, c_dyCard, c_dxCard, c_dyCard, FALSE));
		}
	}
}


// ---------------------------------------------------------
//	Invalidates player result
//
void InvalidateResult(HWND a_hWnd, bool a_bAll,	T_PLAYER a_enPlayer)
{
	GameWndData* l_pData = GetData(a_hWnd);
	if (a_bAll)
	{
		::InvalidateRect(a_hWnd, &(l_pData->m_rectsNames[E_DL_1]), TRUE);
		::InvalidateRect(a_hWnd, &(l_pData->m_rectsNames[E_DL_2]), TRUE);
		::InvalidateRect(a_hWnd, &(l_pData->m_rectsNames[E_DL_3]), TRUE);
		::InvalidateRect(a_hWnd, &(l_pData->m_rectsNames[E_DL_4]), TRUE);
	}
	else
	{
		::InvalidateRect(a_hWnd, &(l_pData->m_rectsNames[a_enPlayer]), TRUE) ;
	}
}


// ---------------------------------------------------------
// Load bitmaps - felt and cover
//
bool LoadBitmaps(HWND a_hWnd)
{
	GameWndData* l_pData = GetData(a_hWnd);

	l_pData->SetBmpCover(reinterpret_cast<HBITMAP>(::LoadBitmap(RCards_GetInstance(),
		MAKEINTRESOURCE(l_pData->m_pGameData->m_regData.m_regView.m_idCover))));
	if (l_pData->GetBmpCover() == nullptr)
		return false;

	l_pData->SetBmpFelt(reinterpret_cast<HBITMAP>(::LoadImage(RCards_GetInstance(),
		MAKEINTRESOURCE(IDB_FELT_DEFAULT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)));

	if (l_pData->GetBmpFelt() == NULL)
		return false;

	return true;
}





// ---------------------------------------------------------
// returns selected card or -1
//
short GetHighlighted(HWND a_hWnd, BOOL a_bHighlight)
{
	if (!a_bHighlight)
	{
		return -1;
	}
	if (!IsVKeyPressed(VK_LBUTTON))
	{
		return -1;
	}
	POINT l_point;
	::GetCursorPos(&l_point);
	::ScreenToClient(a_hWnd, &l_point);
	return GetClicked(a_hWnd, l_point);
}


// ---------------------------------------------------------
//	returns clciked card
//
short GetClicked(HWND a_hWnd, const POINT& a_point)
{
	GameData* l_pData = GetData(a_hWnd)->m_pGameData;
	short l_iAt;
	if (l_pData->IsTrumpsChoice())
	{
		l_iAt = GetCardFromPoint(a_hWnd, a_point, 6);
	}
	else
	{
		l_iAt = GetCardFromPoint(a_hWnd, a_point, 12);
	}
	if (l_iAt < 0)
	{
		return l_iAt;
	}

	// sprawdŸ czy karta w dobrym kolorze (lub dobra
	BOOL l_bOk;
	// w przypadku wyboru atu ka¿da karta dobra
	if (l_pData->IsTrumpsChoice())
	{
		return l_iAt;
	}
	if (l_pData->GetGame() == E_GM_PUZZLE)
	{
		l_bOk = CanPlayCardPuzzle(a_hWnd, l_iAt, E_DL_1);
	}
	else
	{
		l_bOk = CanPlayCard(a_hWnd, l_iAt, E_DL_1);
	}
	if (l_bOk)
	{
		return l_iAt;
	}
	return -1 ;	// nie znaleziono
}


// ---------------------------------------------------------
// returns card nr of E_DL_1 player, or -1
//
short GetCardFromPoint(HWND a_hWnd, const POINT& a_point, short a_nStartCard)
{
	GameWndData* l_pData = GetData(a_hWnd);
	short l_iAt;
	const CUserCards& l_usercards = l_pData->m_pGameData->GetPlayerCards(E_DL_1);
	for (l_iAt = a_nStartCard; l_iAt >= 0; l_iAt--)
	{
		if  (
			(l_pData->m_arrHorzPos[l_iAt].m_iLeft < a_point.x) && 
			(l_pData->m_arrHorzPos[l_iAt].m_iRight > a_point.x) && 
			(CP_YB <= a_point.y) &&
			(CP_YB + c_dyCard >= a_point.y) &&
			(!l_usercards[l_iAt].IsUsed())
			)
		{
			return l_iAt;
		}
	}
	return -1;
}


// ------------------------------------------------------------
//	might play card in puzzle game
//
bool CanPlayCardPuzzle(HWND a_hWnd, short a_nCard, T_PLAYER a_enPlayer)
{
	GameData* l_pData = GetData(a_hWnd)->m_pGameData;
	const CCard& l_card = l_pData->GetPlayerCard(a_enPlayer, a_nCard);
	return (l_pData->GetPuzzleRows().CanPutCard(l_card) == TRUE);
}


// ------------------------------------------------------------
//	might play card
//
bool CanPlayCard(HWND a_hWnd, short a_nCard, T_PLAYER a_enPlayer)
{
	GameData* l_pData = GetData(a_hWnd)->m_pGameData;
	// first card
	const CCard& l_card = l_pData->GetPlayerCard(a_enPlayer, a_nCard);
	if (l_pData->CardsOnTable() == 0)
	{
		return CanPlayFirstCard(a_hWnd, l_card, a_enPlayer);
	}
	else
	{
		// czy zagrana w kolorze
		const CTrick& l_trick = l_pData->GetLastTrick();
		if (l_trick.GetCardColor(0) != l_card.GetColor())
		{
			if (l_pData->GetPlayerCards(a_enPlayer).HasColor(l_trick.GetCardColor(0)))
			{
				return false;
			}
		}
		switch (l_pData->GetGame())
		{
		case E_GM_NOKINGOFHEART:
			// je¿eli nie ma koloru, a ma króla kier to musi go zagraæ
			// jeœli nie braæ króla kier to trzeba przy pierwszej 
			// okazji go zagraæ. Tutaj juz wiemy, ¿e gra w kolorze
			// albo nie ma koloru
			if (l_trick.GetCardColor(0) != l_card.GetColor())
			{
				if (l_pData->GetPlayerCards(a_enPlayer).HasKingOfHeart())
					if (!((l_card.GetColor() == E_CC_HEART) && (l_card.CardValue() == E_CV_K)))
						return false;
			}
			break;
		
		// przy odgrywce trzeba zagraæ atu jeœli 
		// siê nie ma do koloru
		// oraz trzeba zagraæ wy¿sze atu jeœli siê je ma
		case E_GM_RECOVER1:
		case E_GM_RECOVER2:
		case E_GM_RECOVER3:
		case E_GM_RECOVER4:
			{
				T_COLOR l_enColor = l_trick.GetCardColor(0);
				// jeœli zagrana w atutowym
				// i mamy atuty
				// to znajd¿my najwy¿sz¹ kartê w tej lewie
				// sprawdŸmy czy mamy wy¿sz¹ od zagranej
				// jeœli tak to sprawdŸmy czy zagraliœmy
				// wy¿sz¹
				if (l_pData->GetTrumps() == E_CC_NOTRUMPS)
				{
					const CCard* l_pBiggest = l_trick.Biggest();
					if	(
						(l_pData->GetPlayerCards(a_enPlayer).HasColor(l_enColor)) &&
						((*(l_pData->GetPlayerCards(a_enPlayer).BiggestInColorCard(l_enColor))) > (*l_pBiggest)) &&
						(l_card < (*l_pBiggest))
						)
						return false;
				}
				else
				{
					const CCard* l_pBiggest = l_trick.Biggest(l_pData->GetTrumps());
					if	(
						(l_enColor == l_pData->GetTrumps()) &&
						(l_pData->GetPlayerCards(a_enPlayer).HasColor(l_pData->GetTrumps())) &&
						(l_card < (*l_pBiggest)) &&
						((*(l_pData->GetPlayerCards(a_enPlayer).BiggestInColorCard(l_pData->GetTrumps()))) > (*l_pBiggest))
						)
						return false;
					// jesli karta nie atutowa, a my nie mamy koloru
					// to jeœli mamy atu to musimy je zagraæ
					// co wiêcej jeœli najwy¿sza jest atu 
					// to musimy zagraæ wy¿sze atu jeœli mamy
					if  (
						(l_enColor != l_pData->GetTrumps()) &&
						(!l_pData->GetPlayerCards(a_enPlayer).HasColor(l_enColor)) &&
						(l_pData->GetPlayerCards(a_enPlayer).HasColor(l_pData->GetTrumps()))
						)
					{
						if (l_card.GetColor() != l_pData->GetTrumps())
							return false;
						if  (
							(l_pBiggest->GetColor() == l_pData->GetTrumps()) &&
							(l_card < (*l_pBiggest)) &&
							((*(l_pData->GetPlayerCards(a_enPlayer).BiggestInColorCard(l_pData->GetTrumps()))) > (*l_pBiggest))
							)
							return false;
					}
				}
			}
			break;

		default:
			break;
		}
		return true;
	}
}


// ---------------------------------------------------------
// does carc can by played as first
//
bool CanPlayFirstCard(HWND a_hWnd, const CCard& a_card, T_PLAYER a_enPlayer)
{
	GameData* l_pData = GetData(a_hWnd)->m_pGameData;
	switch (l_pData->GetGame())
	{
	case E_GM_NOHEARTS:
	case E_GM_NOKINGOFHEART:
	case E_GM_ROBBER:
		if (a_card.GetColor() == E_CC_HEART)
		{
			if (l_pData->GetPlayerCards(a_enPlayer).HasNoHeartsColor())
			{
				return false;
			}
		}
	}
	return true;
}


// ---------------------------------------------------------
// on WM_LBUTONDOW
//
void OnLButtonDown(HWND a_hWnd, int a_x, int a_y)
{
	GameWndData* l_pData = GetData(a_hWnd);
// TODO to co poni¿ej powinno byæ uruchamiane tylko gdy czas na klikniêcie przez E_DL_1
	if (!l_pData->m_bConfirmTrick && l_pData->m_pGameData->IsDealed() && l_pData->m_pGameData->GetThrower() == E_DL_1)
	{
		Capture(a_hWnd, true);
		POINT l_pt = {a_x, a_y};
		l_pData->m_nLastHighlighted = GetClicked(a_hWnd, l_pt);
		if (l_pData->m_nLastHighlighted != -1)
		{
			InvalidateCardHorz(a_hWnd, l_pData->m_nLastHighlighted, E_DL_1, TRUE);
			::UpdateWindow(a_hWnd);
		}
	}
}


// ---------------------------------------------------------
// Obs³uga klikniêcia zwolnienia klawisza myszy.
//
void OnLButtonUp(HWND a_hWnd, int a_x, int a_y) 
{
	GameWndData* l_pData = GetData(a_hWnd);
	l_pData->m_nLastHighlighted = -1;
	Capture(a_hWnd, false);
	// jeœli to wybór atu
	POINT l_pt = { a_x, a_y };
	if (l_pData->m_pGameData->IsTrumpsChoice())
	{
		ClickOnCardsTrumps(a_hWnd, l_pt);
	}
	else if (l_pData->m_bConfirmTrick)
	{
		ClickConfirmTrick(a_hWnd, l_pt);
	}
	else if (l_pData->m_pGameData->IsDealed() && (l_pData->m_pGameData->GetThrower() == E_DL_1))
	{
		ClickOnCards(a_hWnd, l_pt);
	}
}


// ---------------------------------------------------------
// OnMouseMove
//
void OnMouseMove(HWND a_hWnd, UINT a_nFlags, int a_x, int a_y)
{
	GameWndData* l_pData = GetData(a_hWnd);
	// czy wciœniêty przycisk lewy myszy
	if (l_pData->m_pGameData->IsDealed())
	{
		BOOL l_bLButtonDown = ((a_nFlags & MK_LBUTTON) == MK_LBUTTON);
		if (l_bLButtonDown)
		{
			POINT l_pt = {a_x, a_y};
			short l_nClicked = GetClicked(a_hWnd, l_pt);

			if ((l_nClicked == -1) && (l_pData->m_nLastHighlighted != -1))
			{
				InvalidateCardHorz(a_hWnd, l_pData->m_nLastHighlighted, E_DL_1, TRUE);
				l_pData->m_nLastHighlighted = -1;
			}
			else if ((l_nClicked != -1) && (l_nClicked != l_pData->m_nLastHighlighted))
			{
				InvalidateCardHorz(a_hWnd, l_pData->m_nLastHighlighted, E_DL_1, TRUE);
				InvalidateCardHorz(a_hWnd, l_nClicked, E_DL_1, TRUE);
				l_pData->m_nLastHighlighted = l_nClicked;
			}
		}
	}
}



// ---------------------------------------------------------
// capture released
//
void OnCaptureChanged(HWND a_hWnd)
{
	Capture(a_hWnd, false);
}


void OnNotify(HWND a_hWnd, LPNMHDR a_pNmHdr)
{
	if ((a_pNmHdr->idFrom == IDB_NOTRUMP) && (a_pNmHdr->code == NM_CLICK))
	{
		TrumpsChosen(a_hWnd, E_CC_NOTRUMPS);
	}
}


void OnSetBrightness(HWND a_hWnd, BYTE a_btBrightness)
{
	GameWndData* l_pData = GetData(a_hWnd);
	l_pData->m_pGameData->m_regData.m_regAuto.m_btAlphaGameBackground = a_btBrightness;
	::RedrawWindow(a_hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}


BYTE OnGetBrightness(HWND a_hWnd, BOOL* a_pSet)
{
	GameWndData* l_pData = GetData(a_hWnd);
	if (l_pData == nullptr)
		return 0;
	*a_pSet = TRUE;
	return l_pData->m_pGameData->m_regData.m_regAuto.m_btAlphaGameBackground;

}


// ---------------------------------------------------------
// mouse capture
//
void Capture(HWND a_hWnd, bool a_bGet)
{
	if (a_bGet)
	{
		::SetCapture(a_hWnd);
	}
	else
	{
		GetData(a_hWnd)->m_nLastHighlighted = -1;	
		::ReleaseCapture();
	}
}

// ---------------------------------------------------------
// Trumps choice
//
void ClickOnCardsTrumps(HWND a_hWnd, const POINT& a_point)
{
	short l_nCardNr = GetClickedTrumps(a_hWnd, a_point);
	if (l_nCardNr < 0)
	{
		::MessageBeep(0);
		return;
	}

	GameWndData* l_pData = GetData(a_hWnd);
	const CUserCards& l_usercards = l_pData->m_pGameData->GetPlayerCards(E_DL_1);
	T_COLOR l_enColor = l_usercards[l_nCardNr].GetColor();

	TrumpsChosen(a_hWnd, l_enColor);
}


// ---------------------------------------------------------
//	click on card
//
void ClickOnCards(HWND a_hWnd, const POINT& a_point)
{
	short l_iClicked = GetClicked(a_hWnd, a_point);

	// nie klikniêto na ¿adnej widocznej
	if (l_iClicked < 0)
	{
		::MessageBeep(1);
		return ;
	}
	ASSERT((l_iClicked >= 0) && (l_iClicked < 13));
	InvalidateCardHorz(a_hWnd, l_iClicked, E_DL_1, TRUE) ;

	GameWndData* l_pData = GetData(a_hWnd);
	if (l_pData->m_pGameData->GetGame() == E_GM_PUZZLE)
	{
		ThrowCardPuzzle(a_hWnd, l_iClicked, E_DL_1) ;
	}
	else
	{
		ThrowCard(a_hWnd, l_iClicked, E_DL_1) ;
	}
	if (!l_pData->m_bConfirmTrick)
	{
		::SendMessage(l_pData->m_hWndApp, WM_APP_READY, 0, 0) ;
		if (l_pData->m_pGameData->GetGame() == E_GM_PUZZLE)
		{
			PlayPuzzle(a_hWnd);
		}
		else
		{
			Play(a_hWnd);
		}
	}
}


// ---------------------------------------------------------
//	returns clicked card
//
short GetClickedTrumps(HWND a_hWnd, const POINT& a_point)
{
	// czy po³o¿enie Y siê zgadza?
	if ((a_point.y < CP_YB) || (a_point.y > CP_YB + c_dyCard))
	{
		return -1;	// nie znaleziono
	}
			
	// czy po³o¿enie X siê zgadza?
	if ((a_point.x < CP_XH) || (a_point.x > (CP_XH + 6 * c_dxCardGap + c_dxCard)))
	{
		return -1;	// nie znaleziono
	}

	short l_nAt;	// iterator kart
	GameWndData* l_pData = GetData(a_hWnd);
	for (l_nAt = 6; l_nAt >= 0; l_nAt--)
	{
		if ((l_pData->m_arrHorzPos[l_nAt].m_iLeft < a_point.x) && (l_pData->m_arrHorzPos[l_nAt].m_iRight > a_point.x))
		{
			return l_nAt;
		}
	}

	// tu nigdy nie powinniœmy siê znaleŸæ
	ASSERT(FALSE);
	return -1;	// nie znaleziono
}


// ---------------------------------------------------------
// After trumps were chosen
//
void TrumpsChosen(HWND a_hWnd, T_COLOR a_enColor)
{
	GameWndData* l_pData = GetData(a_hWnd);
	::ShowWindow(l_pData->m_hWndNoTrump, SW_HIDE);
	l_pData->m_pGameData->EndTrumpsChoice(a_enColor);

	::SendMessage(l_pData->m_hWndApp, WM_APP_TRUMPSCHOSEN, 0, 0);
	::SendMessage(l_pData->m_hWndApp, WM_APP_SETTITLE, 0, 0);


	l_pData->m_pGameData->DealEnd();
	InvalidateAfterTrumpsChoice(a_hWnd);
	::UpdateWindow(a_hWnd);
}


// ---------------------------------------------------------
//	puzzle - throwing card 
//
void ThrowCardPuzzle(HWND a_hWnd, short a_nInHand, T_PLAYER a_enPlayer)
{
	GameWndData* l_pData = GetData(a_hWnd);
	// pobranie numeru karty
	if (a_nInHand == -1)
	{
		// najpierw wyœwietlenie pas
		if (l_pData->m_pGameData->GetPlayerCards(a_enPlayer).CardsLeft() > 0)
		{
			// chyba, ¿e gracz E_Dl_1 nie ma ju¿ kart
			if (l_pData->m_pGameData->GetPlayerCards(E_DL_1).CardsLeft() > 0)
			{
				ShowPass(a_hWnd, a_enPlayer);
			}
		}
		// tu musze ustawiæ nastêpnego graj¹cego (no bo gdzie indziej?)
		l_pData->m_pGameData->SetNextPlayer();
		return;
	}

	// zapamiêtanie karty rzuconej
	l_pData->m_pGameData->SetPlayerCard2Puzzle(a_enPlayer, a_nInHand);

	InvalidateCards(a_hWnd, a_nInHand, a_enPlayer, TRUE);
	::UpdateWindow(a_hWnd);
	if (l_pData->m_pGameData->CardsOnTable() == 52)
	{
		Sleep(l_pData->m_pGameData->m_regData.m_regTime.m_iWaitGetTime);
	}
	else
	{
		Sleep(l_pData->m_pGameData->m_regData.m_regTime.m_iWaitThrowTime);
	}
}


// ---------------------------------------------------------
// Wyrzucenie lewy na stó³
//
void ThrowCard(HWND a_hWnd, short a_nInHand, T_PLAYER a_enPlayer)
{
	GameWndData* l_pData = GetData(a_hWnd);
	GameData* l_pGameData = l_pData->m_pGameData;
	l_pGameData->SetPlayerCard2Trick(a_enPlayer, a_nInHand);

	InvalidateCards(a_hWnd, a_nInHand, a_enPlayer, FALSE);
	::UpdateWindow(a_hWnd);
	if (l_pGameData->CardsOnTable() == 4)
	{
		if (l_pGameData->m_regData.m_regRules.m_bConfirmTrick)
		{
			::SendMessage(l_pData->m_hWndApp, WM_APP_CONFIRMTRICK, 0, 0L);
			l_pData->m_bConfirmTrick = true;
		}
		else
		{
			Sleep(l_pGameData->m_regData.m_regTime.m_iWaitGetTime);
			TrickTaking(a_hWnd);
			::UpdateWindow(a_hWnd);
		}
	}
	else
	{
		Sleep(l_pGameData->m_regData.m_regTime.m_iWaitThrowTime);
	}
}


//	--------------------------------------------------------
//	Rozgrywka puzzli
//
void PlayPuzzle(HWND a_hWnd)
{
	GameWndData* l_pData = GetData(a_hWnd);
	GameData* l_pGameData = l_pData->m_pGameData;
	if (!l_pGameData->IsDealed())
	{
		return ;
	}

	// nastêpna karta
	// w przeciwnym przypadku wywo³ujemy kolejnych Deciderów
	short l_iInHand;	// uwaga zmienna niezainicjowana
						// dla m_enThrower = E_DL_1
	while (TRUE)
	{
		if (l_pGameData->GetThrower() == E_DL_1)
		{
			if (!l_pGameData->HasCardForPuzzle(E_DL_1))
			{
				l_iInHand = -1;
			}
			else
			{
				::SendMessage(l_pData->m_hWndApp, WM_APP_PLAYCARD, 0, 0);
				return;
			}
		}
		else
		{
			l_iInHand = l_pGameData->DecideCardNr() ;
		}
		// rzuæ kartê na stó³ - chyba ¿e czekamy
		ASSERT((l_iInHand >= 0) || (l_iInHand == -1));
		ASSERT(l_iInHand < 13);
		ThrowCardPuzzle(a_hWnd, l_iInHand, l_pGameData->GetThrower()) ;
		if (AreCardsToPlayPuzzle(a_hWnd))
		{
			continue;
		}
		break;
	}
	
	// czy kontynuowanie gry
	if (l_pGameData->NextSerie())
	{
		::SendMessage(l_pData->m_hWndApp, WM_APP_NEXTSERIE, 0, 0);

		InvalidateAllCards(a_hWnd, FALSE, TRUE);
		InvalidateResult(a_hWnd, TRUE, E_DL_1);
		::UpdateWindow(a_hWnd);
#ifdef _DEBUG
//w trybie debug mogê chcieæ graæ tylko w puzzle
		if (l_pGameData->GetGame() == E_GM_PUZZLE)
		{
			PlayPuzzle(a_hWnd);
		}
		else
		{
			Play(a_hWnd);
		}
#else
		Play(a_hWnd);
#endif
	}
	else
	{
		::SendMessage(l_pData->m_hWndApp, WM_APP_GAMEOVER, 0, 0);
	}
}


// ---------------------------------------------------------
//	play
//
void Play(HWND a_hWnd)
{
	GameWndData* l_pData = GetData(a_hWnd);
	GameData* l_pGameData = l_pData->m_pGameData;
	// jeœli nie rozdane to mo¿e wybór atu
	if (!l_pGameData->IsDealed())
	{
		if (l_pGameData->IsTrumpsChoice())
		{
			TrumpsChoice(a_hWnd);
		}
		return;
	}
	ASSERT(l_pGameData->CardsOnTable() <= 4) ;

	short l_iInHand;
	while (TRUE)
	{
		if (l_pGameData->GetThrower() == E_DL_1)
		{
			::SendMessage(l_pData->m_hWndApp, WM_APP_PLAYCARD, 0, 0);
			break ;
		}

		l_iInHand = l_pGameData->DecideCardNr();
#ifdef _DEBUG
		if (l_pGameData->GetGame() == E_GM_PUZZLE)
			ASSERT(CanPlayCardPuzzle(a_hWnd, l_iInHand, l_pGameData->GetThrower()));
		else
			ASSERT(CanPlayCard(a_hWnd, l_iInHand, l_pGameData->GetThrower()));
#endif

		ASSERT((l_iInHand >= 0) && (l_iInHand < 13));
		ThrowCard(a_hWnd, l_iInHand, l_pGameData->GetThrower()) ;

		// albo koñczymy w ogóle grê albo bêdzie wybierane atu
		if (l_pData->m_bConfirmTrick)
		{
			break;
		}
		else if (l_pGameData->GetTricksCnt() == 0)
		{
			if (l_pGameData->IsTrumpsChoice())
			{
				TrumpsChoice(a_hWnd);
			}
			else if (l_pGameData->GetGame() == E_GM_PUZZLE)
			{
				PlayPuzzle(a_hWnd);
			}
			break;
		}
	}

}


// ---------------------------------------------------------
// Zinwalidowanie kart po wyborze atu
// Inwaliduje siê karty od 8 do 13 wszystkich graczy
//
void InvalidateAfterTrumpsChoice(HWND a_hWnd)
{
	GameWndData* l_pData = GetData(a_hWnd);
	RECT l_rect;
	// gracz E_DL_1 - wszystkie karty
	l_rect.left = l_pData->m_arrHorzPos[0].m_iLeft;
	l_rect.right = l_pData->m_arrHorzPos[12].m_iRight;
	l_rect.top = CP_YB;
	l_rect.bottom = l_rect.top + c_dyCard;
	::InvalidateRect(a_hWnd, &l_rect, TRUE);

	// gracz E_DL_3 - ju¿ tylko pozosta³e
	l_rect.left = l_pData->m_arrHorzPos[7].m_iLeft;
	l_rect.top = CP_YT;
	l_rect.bottom = l_rect.top + c_dyCard;
	::InvalidateRect(a_hWnd, &l_rect, TRUE);

	// gracz E_DL_2
	l_rect.left = CP_XL;
	l_rect.right = l_rect.left + c_dxCard;
	l_rect.top = l_pData->m_arrVertPos[7].m_iTop;
	l_rect.bottom = l_pData->m_arrVertPos[12].m_iBottom;
	::InvalidateRect(a_hWnd, &l_rect, TRUE);

	// gracz E_DL_4
	l_rect.left = CP_XR;
	l_rect.right = l_rect.left + c_dxCard;
	::InvalidateRect(a_hWnd, &l_rect, TRUE);
}


// ---------------------------------------------------------
//	Wziêcie lewy 
//
void TrickTaking(HWND a_hWnd)
{
	GameData* l_pData = GetData(a_hWnd)->m_pGameData;
	BOOL l_bContinue = l_pData->GetPlayedTrick();

	// kontynuacja tej samej gry
	if (l_bContinue)
	{
		InvalidateResult(a_hWnd, FALSE, l_pData->GetThrower());
		InvalidateTableCards(a_hWnd, TRUE, E_DL_1);
	}

	// zmieni³a siê gra, ale nie czas na wybór atu
	// przy wyborze atu czekamy jeszcze
	else 
	{
		::SendMessage(GetData(a_hWnd)->m_hWndApp, WM_APP_SETTITLE, 0, 0);
		if (!l_pData->IsTrumpsChoice())
		{
			if (l_pData->GetGame() == E_GM_PUZZLE)
			{
				InvalidateAllCards(a_hWnd, TRUE, TRUE);
				InvalidateResult(a_hWnd, TRUE, E_DL_1);
			}
			else
			{
				InvalidateAllCards(a_hWnd, TRUE, FALSE);
				InvalidateResult(a_hWnd, TRUE, E_DL_1);
			}
		}
	}
}


bool AreCardsToPlayPuzzle(HWND a_hWnd)
{
	GameData* l_pData = GetData(a_hWnd)->m_pGameData;
	if (l_pData->GetPlayerCards(E_DL_1).CardsLeft() > 0)
		return true;
	if (l_pData->GetPlayerCards(E_DL_2).CardsLeft() > 0)
		return true;
	if (l_pData->GetPlayerCards(E_DL_3).CardsLeft() > 0)
		return true;
	if (l_pData->GetPlayerCards(E_DL_4).CardsLeft() > 0)
		return true;
	return false;
}


void ShowPass(HWND a_hWnd, T_PLAYER a_enPlayer)
{
	GameWndData* l_pData = GetData(a_hWnd);
	GameData* l_pGameData = l_pData->m_pGameData;
	if (l_pData->GetBmpPass() == NULL)
	{
		HBITMAP l_hBmpPass = RDraw::LoadImageFromResource(::GetModuleHandle(NULL), IDR_PNG_PASS, _T("PNG"));
		if (l_hBmpPass == NULL)
			throw RSystemExc(_T("Load pass resource"));
		l_pData->SetBmpPass(l_hBmpPass);
		BITMAP l_bmp;
		VERIFY(::GetObject(l_pData->GetBmpPass(), sizeof(l_bmp), &l_bmp) != 0);

		l_pData->m_sizeBmpPass.cx = l_bmp.bmWidth;
		l_pData->m_sizeBmpPass.cy = l_bmp.bmHeight;
		ASSERT(l_bmp.bmWidth == c_dxPass);
		ASSERT(l_bmp.bmWidth == c_dyPass);
	}

	l_pData->m_enPassPlayer = a_enPlayer;
	RECT l_rect = { l_pData->m_ptsPass[a_enPlayer].x, l_pData->m_ptsPass[a_enPlayer].y,
		l_pData->m_ptsPass[a_enPlayer].x + c_dxPass, l_pData->m_ptsPass[a_enPlayer].y + c_dyPass };
	::InvalidateRect(a_hWnd, &l_rect, TRUE);
	::UpdateWindow(a_hWnd);
	::Sleep(l_pGameData->m_regData.m_regTime.m_iWaitPassTime);
	l_pData->m_enPassPlayer = E_DL_NULL;
	::InvalidateRect(a_hWnd, &l_rect, TRUE);
	::UpdateWindow(a_hWnd);
}


void ClickConfirmTrick(HWND a_hWnd, const POINT& a_pt)
{
	GameWndData* l_pData = GetData(a_hWnd);
	ASSERT(l_pData->m_bConfirmTrick);
	l_pData->m_bConfirmTrick = false;
	TrickTaking(a_hWnd);
	::UpdateWindow(a_hWnd);
	if (l_pData->m_pGameData->GetThrower() == E_DL_1)
	{
		::SendMessage(l_pData->m_hWndApp, WM_APP_PLAYCARD, 0, 0L);
	}
	if (l_pData->m_pGameData->GetGame() == E_GM_PUZZLE)
	{
		PlayPuzzle(a_hWnd);
	}
	else
	{
		Play(a_hWnd);
	}
}


