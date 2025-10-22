#include "StdAfx.h"
#include "Kierki.h"
#include "typedefs.h"
#include "ResultWnd.h"
#include "FontFactory.h"
#include <rcommon/RListCtrl.h>
#include <rcommon/RTheme.h>
#include <rcommon/drawutl.h>
#include <rcommon/RSystemExc.h>
#include <rcommon/SafeWndProc.hpp>




#define MAX_LOADSTRING 100

class ResultWndData
{
public:
	ResultWndData(T_SERIE a_enSerie, GameData* a_pGameData)
	{
		m_enSerie = a_enSerie;
		m_pGameData = a_pGameData;
		m_hWndList = NULL;
	}

	ResultWndData(ResultWndData* a_pData)
	{
		m_enSerie = a_pData->m_enSerie;
		m_pGameData = a_pData->m_pGameData;
		m_hWndList = NULL;
		m_hBmpBackground = a_pData->m_hBmpBackground;

	}

	~ResultWndData()
	{
		SetBackground(nullptr);
	}

	void SetBackground(HBITMAP a_hBmpBackground)
	{
		if (m_hBmpBackground != nullptr)
		{
			::DeleteObject(m_hBmpBackground);
		}
		m_hBmpBackground = a_hBmpBackground;

	}


	T_SERIE   m_enSerie;
	GameData* m_pGameData;
	HWND      m_hWndList;
	HBITMAP   m_hBmpBackground{};

};


static const TCHAR cc_sWindowClass[] = _T("RESULTWND");	// game window class name
static const long c_iWindowOfs = sizeof(ResultWndData*) - sizeof(int);
static ResultWndData* GetData(HWND a_hWnd);

static LRESULT ResultWnd_WndProc(HWND, UINT, WPARAM, LPARAM);

inline static LRESULT OnCreate(HWND a_hWnd, LPCREATESTRUCT a_pCreateStruct);
inline static void OnNcDestroy(HWND a_hWnd);
inline static void OnSize(HWND a_hWnd, int a_dxWidth, int a_dyHeight);
inline static void OnShowWindow(HWND a_hWnd, bool a_bShow);
inline static void OnSetFocus(HWND a_hWnd);
inline static void OnAppRefresh(HWND a_hWnd);
inline static void OnSetSerie(HWND a_hWnd, T_SERIE a_enSerie);

inline static void OnSetBrightness(HWND a_hWnd, BYTE a_btBrightness);
inline static BYTE OnGetBrightness(HWND a_hWnd, BOOL* a_pSet);


static void SetColors(HWND a_hWnd);
static void SetBackground(HWND a_hWnd);
static void SetFont(HWND a_hWnd);


static BOOL ListDataProc(void* a_pObj, long a_iRow, long a_iCol, const TCHAR** a_ppData);
static long ListCountProc(void* a_pObj);
static BOOL ListDrawProc(HWND a_hWnd, HDC a_hDC, void* a_pObj, LPRLCELL a_pCell);
static BOOL ListGridProc(HWND a_hWnd, HDC a_hDC, void* a_pObj, LPRLGRID a_pGrid);
static BOOL ListBackgroundProc(HWND a_hWnd, HDC a_hDC, void* a_pObj, LPRECT a_pRect);






HWND ResultWnd_Create(DWORD a_dwStyleEx, DWORD a_dwStyle, int a_x, int a_y,
	int a_dx, int a_dy, HWND a_hWndParent, HINSTANCE a_hInst, T_SERIE a_enSerie, GameData* a_pGameData)
{
	ResultWndData l_data(a_enSerie, a_pGameData);
	return ::CreateWindowEx(a_dwStyleEx, cc_sWindowClass, _T(""), a_dwStyle, a_x, a_y, 
		a_dx, a_dy, a_hWndParent, NULL, a_hInst, &l_data);
}


ATOM ResultWnd_Register(HINSTANCE a_hInst)
{
	WNDCLASSEX l_wcex;

	l_wcex.cbSize			= sizeof(WNDCLASSEX); 
	l_wcex.style			= 0;
	l_wcex.lpfnWndProc		= SafeWndProc<ResultWnd_WndProc>;
	l_wcex.cbClsExtra		= 0;
	l_wcex.cbWndExtra		= sizeof(ResultWndData*);
	l_wcex.hInstance		= a_hInst;
	l_wcex.hIcon			= NULL;
	l_wcex.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	l_wcex.hbrBackground	= NULL;
	l_wcex.lpszMenuName		= NULL;
	l_wcex.lpszClassName	= cc_sWindowClass;
	l_wcex.hIconSm			= NULL;
	return ::RegisterClassEx(&l_wcex);
}


ResultWndData* GetData(HWND a_hWnd)
{
	return reinterpret_cast<ResultWndData*>(::GetWindowLongPtr(a_hWnd, c_iWindowOfs));
}


//
//  FUNCTION: ResultWnd_WndProc(HWND, unsigned, WORD, LONG)
//
LRESULT ResultWnd_WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_iMsg) 
	{
	case WM_CREATE:
		return OnCreate(a_hWnd, reinterpret_cast<LPCREATESTRUCT>(a_lParam));

	case WM_NCDESTROY:
		OnNcDestroy(a_hWnd);
		break;

	case WM_SIZE:
		OnSize(a_hWnd, LOWORD(a_lParam), HIWORD(a_lParam));
		break;

	case WM_SHOWWINDOW:
		OnShowWindow(a_hWnd, a_wParam == TRUE);
		break;

	case WM_SETFOCUS:
		OnSetFocus(a_hWnd);
		break;

	case WM_APP_REFRESH:
		OnAppRefresh(a_hWnd);
		break;

	case WM_APP_SETBRIGHTNESS:
		OnSetBrightness(a_hWnd, static_cast<BYTE>(a_wParam));
		break;

	case WM_APP_GETBRIGHTNESS:
		return OnGetBrightness(a_hWnd, reinterpret_cast<BOOL*>(a_lParam));

	case WM_APP_SETSERIE:
		OnSetSerie(a_hWnd, static_cast<T_SERIE>(a_wParam));
		break;

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
	ResultWndData* l_pData = 
		reinterpret_cast<ResultWndData*>(a_pCreateStruct->lpCreateParams);
	// we must copy data
	l_pData = new ResultWndData(l_pData);
	if (l_pData == NULL)
	{
		return -1;
	}
	::SetWindowLongPtr(a_hWnd, c_iWindowOfs, (LONG_PTR)l_pData);

	RListCtrl_Register();
	l_pData->m_hWndList = RListCtrl_Create(NULL, WS_VISIBLE | WS_CHILD, 
		0, 0, 0, 0, a_hWnd, 0, NULL);

	// TODO - unify what is passed to these procedures, too prone for errors now
	RListCtrl_SetDataProc(l_pData->m_hWndList, a_hWnd, ListDataProc);
	RListCtrl_SetCountProc(l_pData->m_hWndList, l_pData->m_pGameData, ListCountProc);
	RListCtrl_SetDrawProc(l_pData->m_hWndList, l_pData->m_pGameData, ListDrawProc);
	RListCtrl_SetGridProc(l_pData->m_hWndList, l_pData, ListGridProc);
	RListCtrl_SetDrawBkProc(l_pData->m_hWndList, l_pData, ListBackgroundProc);

	RListCtrl_SetMode(l_pData->m_hWndList, LMB_READONLY, TRUE);
	

	RLCCOLDEF l_col;
	l_col.psColName = _T("");
	l_col.iLength = 100; // doesn't matter, will be resized on WM_SIZE
	RListCtrl_SetCol(l_pData->m_hWndList, &l_col);
	RListCtrl_SetCol(l_pData->m_hWndList, &l_col);
	RListCtrl_SetCol(l_pData->m_hWndList, &l_col);
	RListCtrl_SetCol(l_pData->m_hWndList, &l_col);

	SetColors(a_hWnd);
	SetBackground(a_hWnd);
	SetFont(a_hWnd);

	return 0;
}


void OnNcDestroy(HWND a_hWnd)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	delete l_pData;
}


void OnSize(HWND a_hWnd, int a_dxWidth, int a_dyHeight)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	::SetWindowPos(l_pData->m_hWndList, NULL, 0, 0, a_dxWidth, a_dyHeight, SWP_NOZORDER);
	long l_iColWidth = a_dxWidth / 4;
	RListCtrl_SetColWidth(l_pData->m_hWndList, 0, l_iColWidth);
	RListCtrl_SetColWidth(l_pData->m_hWndList, 1, l_iColWidth);
	RListCtrl_SetColWidth(l_pData->m_hWndList, 2, l_iColWidth);

}


void OnShowWindow(HWND a_hWnd, bool a_bShow)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	::ShowWindow(l_pData->m_hWndList, a_bShow ? SW_SHOW : SW_HIDE);
}


void OnSetFocus(HWND a_hWnd)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	::SetFocus(l_pData->m_hWndList);
}


void OnAppRefresh(HWND a_hWnd)
{
	SetColors(a_hWnd);
	SetFont(a_hWnd);
}


void OnSetBrightness(HWND a_hWnd, BYTE a_btBrightness)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	l_pData->m_pGameData->m_regData.m_regAuto.m_btAlphaResultBackground = a_btBrightness;
	::RedrawWindow(a_hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}

BYTE OnGetBrightness(HWND a_hWnd, BOOL* a_pSet)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	if (l_pData == nullptr)
		return 0;
	*a_pSet = TRUE;
	return l_pData->m_pGameData->m_regData.m_regAuto.m_btAlphaResultBackground;
}

void OnSetSerie(HWND a_hWnd, T_SERIE a_enSerie)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	l_pData->m_enSerie = a_enSerie;
}


BOOL ListDataProc(void* a_pObj, long a_iRow, long a_iCol, const TCHAR** a_ppData)
{
	static TCHAR l_sData[128];
	l_sData[0] = _T('\0');
	ResultWndData* l_pData = GetData(reinterpret_cast<HWND>(a_pObj));
	GameData* l_pGameData = l_pData->m_pGameData;

	#pragma todo("South == 0, West = 1  -- watch out: dangerous")
	if (a_iRow == 0)
	{
		ASSERT(a_iCol < 4);
		_tcsncpy_s(l_sData, ArraySize(l_sData), 
			l_pGameData->m_regData.GetPlayerName(static_cast<T_PLAYER>(a_iCol)).c_str(),
			_TRUNCATE);
	}
	else 
	{

		if (l_pGameData->GetSerie() != E_SR_NULL)
		{
			T_GAMES l_enGame;
			if (l_pGameData->GetSerie() > l_pData->m_enSerie)
			{
				l_enGame = E_GM_PUZZLE;
			}
			else
			{
				l_enGame = l_pGameData->GetGame();
			}
			// summary of previous games

			int l_iScore;
			bool l_bData = true;

			if (a_iRow == 1)
			{
				l_iScore = l_pGameData->SumPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			// minuses
			else if ((a_iRow >= 2) && (a_iRow <= 7) && (a_iRow <= l_enGame + 2))
			{
				l_iScore = l_pGameData->GetPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie, (T_GAMES)(a_iRow - 2));
			}
			// sum of minuses
			else if ((a_iRow == 8) && (l_enGame > E_GM_NOTRICKS))
			{
				l_iScore = l_pGameData->SumPlayerMinuses((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			// robber
			else if ((a_iRow == 9) && (a_iRow <= l_enGame + 3))
			{
					l_iScore = l_pGameData->GetPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie, (T_GAMES)(a_iRow - 3));
			}
			// sum of all minuses
			else if ((a_iRow == 10) && (l_enGame > E_GM_NOTRICKS))
			{
				l_iScore = l_pGameData->SumPlayerMinuses((T_PLAYER)a_iCol, l_pData->m_enSerie, TRUE);
			}
			// pluses
			else if ((a_iRow >= 11) && (a_iRow <= 14) && (a_iRow <= l_enGame + 4))
			{
				l_iScore = l_pGameData->GetPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie, (T_GAMES)(a_iRow - 4));
			}
			// sum of minuses and pluses
			else if ((a_iRow == 15) && (l_enGame > E_GM_NOTRICKS))
			{
				l_iScore = l_pGameData->SumPlayer((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			// puzzle
			else if ((a_iRow == 16) && (a_iRow <= l_enGame + 5))
			{
				l_iScore = l_pGameData->GetPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie, (T_GAMES)(a_iRow - 5));
			}
			// after puzzle
			else if ((a_iRow == 17) && (l_enGame > E_GM_NOTRICKS))
			{
				l_iScore = l_pGameData->SumPlayerAll((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			// sum of all games
			else if ((a_iRow == 18) && (l_enGame > E_GM_NOTRICKS))
			{
				l_iScore = l_pGameData->SumPlayerAllScore((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			else
			{
				l_bData = false; // no data
			}

			if (l_bData)
			{
				_sntprintf_s(l_sData, ArraySize(l_sData), _TRUNCATE, _T("%d"), l_iScore);
			}
		}
	}

	*a_ppData = l_sData;
	return TRUE;
}


long ListCountProc(void* a_pObj)
{
	return 19;
}


BOOL ListDrawProc(HWND a_hWnd, HDC a_hDC, void* a_pObj, LPRLCELL a_pCell)
{
	long l_iRow = a_pCell->pos.cell.iRow;
	if (l_iRow == 0)
	{
// TODO correct header drawing, at least put the colors into the registry
		RListCtrl_DrawCell(a_hWnd, a_hDC, a_pObj, a_pCell);
		RECT l_rect;
		l_rect.top = a_pCell->pos.rect.bottom - 2;
		l_rect.bottom = a_pCell->pos.rect.bottom;
		l_rect.left = a_pCell->pos.rect.left;
		l_rect.right = a_pCell->pos.rect.right;
		RDraw::FillSolidRect(a_hDC, l_rect, RGB(230, 139, 44));
		l_rect.bottom = l_rect.top;
		l_rect.top = l_rect.bottom - 1;
		RDraw::FillSolidRect(a_hDC, l_rect, RGB(255, 199, 60));
		return TRUE;
	}
	return RListCtrl_DrawCell(a_hWnd, a_hDC, a_pObj, a_pCell);
}

BOOL ListGridProc(HWND a_hWnd, HDC a_hDC, void* a_pObj, LPRLGRID a_pGrid)
{

	ResultWndData* l_pData = reinterpret_cast<ResultWndData*>(a_pObj);
	if (l_pData->m_pGameData->m_regData.m_regView.m_bFancyStyle)
	{
		if (a_pGrid->bVert)
		{
			if (a_pGrid->iRowCol < 3)
				RDraw::DrawSketchLine(a_hDC, { a_pGrid->rect.right, a_pGrid->rect.top }, { a_pGrid->rect.right, a_pGrid->rect.bottom }, 1, 60, 1.5f, RGB(0, 0, 0), 255);
		}
		else
		{
			int l_iRow = a_pGrid->iRowCol;
			float l_fThickness = ((l_iRow == 1) || (l_iRow == 7) || (l_iRow == 9) || (l_iRow == 14) || (l_iRow == 16) || (l_iRow == 18)) ? 3.0f : 1.5f;
			{
				RDraw::DrawSketchLine(a_hDC, { a_pGrid->rect.left, a_pGrid->rect.bottom }, { a_pGrid->rect.right, a_pGrid->rect.bottom }, 1, 60, l_fThickness, RGB(0, 0, 0), 255);
			}
		}
	}
	else
	{
		if (a_pGrid->bVert)
		{
			if (a_pGrid->iRowCol < 3)
				RListCtrl_DrawGrid(a_hWnd, a_hDC, a_pObj, a_pGrid);
		}
		else
		{
			int l_iRow = a_pGrid->iRowCol;
			int l_iThickness = ((l_iRow == 1) || (l_iRow == 7) || (l_iRow == 9) || (l_iRow == 14) || (l_iRow == 16) || (l_iRow == 18)) ? 3 : 1;
			a_pGrid->hPen = ::CreatePen(PS_SOLID, l_iThickness, RGB(0, 0, 0));
			HPEN l_hPenOld = (HPEN)::SelectObject(a_hDC, a_pGrid->hPen);
			RListCtrl_DrawGrid(a_hWnd, a_hDC, a_pObj, a_pGrid);
			// reset and delete pen
			a_pGrid->hPen = l_hPenOld;
			::SelectObject(a_hDC, l_hPenOld);
			::DeleteObject(a_pGrid->hPen);
		}

	}

	return TRUE;
}


static BOOL ListBackgroundProc(HWND a_hWnd, HDC a_hDC, void* a_pObj, LPRECT a_pRect)
{
	ResultWndData* l_pData = reinterpret_cast<ResultWndData*>(a_pObj);
	if (l_pData->m_hBmpBackground)
	{
		HDC l_hdcMem = ::CreateCompatibleDC(a_hDC);
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(l_hdcMem, l_pData->m_hBmpBackground);

		BITMAP l_bmp;
		::GetObject(l_pData->m_hBmpBackground, sizeof(BITMAP), &l_bmp);

		::StretchBlt(a_hDC, 0, 0, a_pRect->right, a_pRect->bottom,
			l_hdcMem, 0, 0, l_bmp.bmWidth, l_bmp.bmHeight, SRCCOPY);

		::SelectObject(l_hdcMem, hOldBmp);
		::DeleteDC(l_hdcMem);
	}

	RDraw::BlendOverlay(a_hDC, *a_pRect, l_pData->m_pGameData->m_regData.m_regHidden.m_clrTintResultBackground, l_pData->m_pGameData->m_regData.m_regAuto.m_btAlphaResultBackground);

	return TRUE;
}


void SetColors(HWND a_hWnd)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	RListCtrl_SetFixedCols(l_pData->m_hWndList, 0);
	RListCtrl_SetMode(l_pData->m_hWndList, LMB_FILLLASTCOL | LMB_CONSTGRIDSIZE | LMB_DRAWTRANSP, TRUE);
	RListCtrl_SetMode(l_pData->m_hWndList, LMB_DRAWCELLBORDER, FALSE);
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_CELL_CELLACTSEL, RListCtrl_GetColor(l_pData->m_hWndList, RLC_CELL_CELLACT));
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_TEXT_CELLACTSEL, RListCtrl_GetColor(l_pData->m_hWndList, RLC_TEXT_CELLACT));
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_TEXT_CELLNOACTSEL, RListCtrl_GetColor(l_pData->m_hWndList, RLC_TEXT_CELLACT));
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_LBOR_CELLACTSEL, RListCtrl_GetColor(l_pData->m_hWndList, RLC_LBOR_CELLACT));
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_RBOR_CELLACTSEL, RListCtrl_GetColor(l_pData->m_hWndList, RLC_RBOR_CELLACT));
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_UBOR_CELLACTSEL, RListCtrl_GetColor(l_pData->m_hWndList, RLC_UBOR_CELLACT));
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_DBOR_CELLACTSEL, RListCtrl_GetColor(l_pData->m_hWndList, RLC_DBOR_CELLACT));
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_CELL_FIXEDACT, ::GetSysColor(COLOR_BTNFACE));
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_CELL_FIXEDNOACTNOSEL, ::GetSysColor(COLOR_BTNFACE));
}


void SetBackground(HWND a_hWnd)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	if (l_pData->m_hBmpBackground == nullptr)
	{
		l_pData->SetBackground(reinterpret_cast<HBITMAP>(::LoadImage(
			::GetModuleHandle(nullptr),
			MAKEINTRESOURCE(IDB_NOTEBOOK),
			IMAGE_BITMAP,
			0, 0, LR_CREATEDIBSECTION)));
		if (l_pData->m_hBmpBackground == nullptr)
			throw RSystemExc(_T("LOAD_BACKGROUND_RESULT"));
	}
}


void SetFont(HWND a_hWnd)
{
	ResultWndData* l_pData = GetData(a_hWnd);
	HFONT l_hFont = CFontFactory::Instance().GetFont(a_hWnd);
	RListCtrl_SetFont(l_pData->m_hWndList, l_hFont);
}

/* maybe instead of bitmap? Then I can draw results on the right side of the vertical red line ;-)
void DrawNotebookBackground(HDC a_hDC, RECT a_rect, COLORREF lineColor = RGB(180, 200, 255), COLORREF marginColor = RGB(255, 100, 100), int lineSpacing = 20)
{
	// Wype³nij t³o na bia³o
	HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(a_hDC, &a_rect, bgBrush);
	DeleteObject(bgBrush);

	// Pióro do linii poziomych
	HPEN hLinePen = CreatePen(PS_SOLID, 1, lineColor);
	HPEN hOldPen = (HPEN)SelectObject(a_hDC, hLinePen);

	// Rysuj linie poziome
	for (int y = a_rect.top + lineSpacing; y < a_rect.bottom; y += lineSpacing)
	{
		MoveToEx(a_hDC, a_rect.left, y, nullptr);
		LineTo(a_hDC, a_rect.right, y);
	}

	// Pióro do marginesu
	HPEN hMarginPen = CreatePen(PS_SOLID, 2, marginColor);
	SelectObject(a_hDC, hMarginPen);

	// Rysuj pionow¹ liniê marginesu
	int marginX = a_rect.left + 50;
	MoveToEx(a_hDC, marginX, a_rect.top, nullptr);
	LineTo(a_hDC, marginX, a_rect.bottom);

	// Przywróæ poprzednie pióro
	SelectObject(a_hDC, hOldPen);
	DeleteObject(hLinePen);
	DeleteObject(hMarginPen);
}
*/