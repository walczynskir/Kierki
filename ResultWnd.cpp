#include "StdAfx.h"
#include "Kierki.h"
#include "typedefs.h"
#include "ResultWnd.h"
#include <rcommon/RListCtrl.h>
#include <rcommon/RTheme.h>
#include <rcommon/drawutl.h>

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
	}

	T_SERIE   m_enSerie;
	GameData* m_pGameData;
	HWND      m_hWndList;
};


static const TCHAR c_sWindowClass[] = _T("RESULTWND");	// game window class name
static const long c_iWindowOfs = sizeof(ResultWndData*) - sizeof(int);
static ResultWndData* GetData(HWND a_hWnd);

static LRESULT CALLBACK	ResultWnd_WndProc(HWND, UINT, WPARAM, LPARAM);

inline static LRESULT OnCreate(HWND a_hWnd, LPCREATESTRUCT a_pCreateStruct);
inline static void OnNcDestroy(HWND a_hWnd);
inline static void OnSize(HWND a_hWnd, int a_dxWidth, int a_dyHeight);
inline static void OnShowWindow(HWND a_hWnd, bool a_bShow);
inline static void OnSetFocus(HWND a_hWnd);
inline static void OnAppRefresh(HWND a_hWnd);
inline static void OnSetSerie(HWND a_hWnd, T_SERIE a_enSerie);

static void SetColors(HWND a_hWnd);

static BOOL ListDataProc(void* a_pObj, long a_iRow, long a_iCol, const TCHAR** a_ppData);
static long ListCountProc(void* a_pObj);
static BOOL ListDrawProc(HWND a_hWnd, HDC a_hDC, void* a_pObj, LPRLCELL a_pCell);


HWND ResultWnd_Create(DWORD a_dwStyleEx, DWORD a_dwStyle, int a_x, int a_y,
	int a_dx, int a_dy, HWND a_hWndParent, HINSTANCE a_hInst, T_SERIE a_enSerie, GameData* a_pGameData)
{
	ResultWndData l_data(a_enSerie, a_pGameData);
	return ::CreateWindowEx(a_dwStyleEx, c_sWindowClass, _T(""), a_dwStyle, a_x, a_y, 
		a_dx, a_dy, a_hWndParent, NULL, a_hInst, &l_data);
}


ATOM ResultWnd_Register(HINSTANCE a_hInst)
{
	WNDCLASSEX l_wcex;

	l_wcex.cbSize			= sizeof(WNDCLASSEX); 
	l_wcex.style			= 0;
	l_wcex.lpfnWndProc		= ResultWnd_WndProc;
	l_wcex.cbClsExtra		= 0;
	l_wcex.cbWndExtra		= sizeof(ResultWndData*);
	l_wcex.hInstance		= a_hInst;
	l_wcex.hIcon			= NULL;
	l_wcex.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	l_wcex.hbrBackground	= NULL;
	l_wcex.lpszMenuName		= NULL;
	l_wcex.lpszClassName	= c_sWindowClass;
	l_wcex.hIconSm			= NULL;
	return ::RegisterClassEx(&l_wcex);
}


ResultWndData* GetData(HWND a_hWnd)
{
#pragma warning(disable: 4312) // I do not understand why compiler issues this warning!
	return reinterpret_cast<ResultWndData*>(::GetWindowLongPtr(a_hWnd, c_iWindowOfs));
#pragma warning(default: 4312)
}


//
//  FUNCTION: ResultWnd_WndProc(HWND, unsigned, WORD, LONG)
//
LRESULT CALLBACK ResultWnd_WndProc(HWND a_hWnd, UINT a_iMsg, WPARAM a_wParam, LPARAM a_lParam)
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
#pragma warning(disable: 4244)
	::SetWindowLongPtr(a_hWnd, c_iWindowOfs, (LONG_PTR)l_pData);
#pragma warning(default: 4244)

	RListCtrl_Register();
	l_pData->m_hWndList = RListCtrl_Create(NULL, WS_VISIBLE | WS_CHILD, 
		0, 0, 0, 0, a_hWnd, 0, NULL);
	RListCtrl_SetDataProc(l_pData->m_hWndList, a_hWnd, ListDataProc);
	RListCtrl_SetCountProc(l_pData->m_hWndList, l_pData->m_pGameData, ListCountProc);
	RListCtrl_SetDrawProc(l_pData->m_hWndList, l_pData->m_pGameData, ListDrawProc);

	RLCCOLDEF l_col;
	l_col.psColName = _T("");
	l_col.iLength = 100; // doesn't matter
	RListCtrl_SetCol(l_pData->m_hWndList, &l_col);
	RListCtrl_SetCol(l_pData->m_hWndList, &l_col);
	RListCtrl_SetCol(l_pData->m_hWndList, &l_col);
	RListCtrl_SetCol(l_pData->m_hWndList, &l_col);

	SetColors(a_hWnd);

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

	// E_DL_1 == 0, E_DL_2 = 1  -- watch out: dangerous
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
			bool l_bSprintf = true;

			if (a_iRow == 1)
			{
				l_iScore = l_pGameData->SumPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			// minuses
			// TODO - check why this condition is different from others?
			else if ((a_iRow >= 2) && (a_iRow <= 7) && (a_iRow <= l_enGame + 2))
			{
				l_iScore = l_pGameData->GetPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie, (T_GAMES)(a_iRow - 2));
			}
			// sum of minuses
			else if ((a_iRow == 8))
			{
				l_iScore = l_pGameData->SumPlayerMinuses((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			// robber
			else if ((a_iRow == 9) && (a_iRow <= l_enGame + 3))
			{
				l_iScore = l_pGameData->GetPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie, (T_GAMES)(a_iRow - 3));
			}
			// sum of all minuses
			else if ((a_iRow == 10))
			{
				l_iScore = l_pGameData->SumPlayerMinuses((T_PLAYER)a_iCol, l_pData->m_enSerie, TRUE);
			}
			// pluses
			else if ((a_iRow >= 11) && (a_iRow <= 14) && (a_iRow <= l_enGame + 4))
			{
				l_iScore = l_pGameData->GetPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie, (T_GAMES)(a_iRow - 4));
			}
			// sum of minuses and pluses
			else if (a_iRow == 15)
			{
				l_iScore = l_pGameData->SumPlayer((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			// puzzle
			else if ((a_iRow == 16) && (a_iRow <= l_enGame + 5))
			{
				l_iScore = l_pGameData->GetPlayerScore((T_PLAYER)a_iCol, l_pData->m_enSerie, (T_GAMES)(a_iRow - 5));
			}
			// after puzzle
			else if (a_iRow == 17)
			{
				l_iScore = l_pGameData->SumPlayerAll((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			// sum of all games
			else if (a_iRow == 18)
			{
				l_iScore = l_pGameData->SumPlayerAllScore((T_PLAYER)a_iCol, l_pData->m_enSerie);
			}
			else
			{
				l_bSprintf = false; // no data
			}

			if (l_bSprintf)
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
		a_pCell->iState |= RLS_DRAWBK;
	}
	else
	{
		a_pCell->iState &= ~RLS_DRAWBK;
	}
	if (l_iRow == 0)
	{
		RTheme l_theme;
		if (l_theme.OpenData(a_hWnd, L"HEADER") != NULL)
		{
			HRESULT l_hr = l_theme.DrawBackground(a_hDC, HP_HEADERITEM, HIS_NORMAL, &(a_pCell->pos.rect));
			RECT l_rectContent;
			l_hr = l_theme.GetBackgroundContentRect(a_hDC, HP_HEADERITEM, 
					HIS_NORMAL, &(a_pCell->pos.rect), &l_rectContent);
 
			l_hr = l_theme.DrawText(a_hDC, HP_HEADERITEM, HIS_NORMAL,
							a_pCell->sText.c_str(), static_cast<int>(a_pCell->sText.length()),
							DT_CENTER | DT_VCENTER | DT_SINGLELINE,
							0, &l_rectContent);
		}
		else
		{
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
		}
		return TRUE;
	}
	if ((l_iRow == 1) || (l_iRow == 7) || (l_iRow == 9) || (l_iRow == 14) || (l_iRow == 16) || (l_iRow == 18))
	{
		RListCtrl_DrawCell(a_hWnd, a_hDC, a_pObj, a_pCell);
		RECT l_rect;
		l_rect.top = a_pCell->pos.rect.bottom - 1;
		l_rect.bottom = a_pCell->pos.rect.bottom - 1;
		l_rect.left = a_pCell->pos.rect.left;
		l_rect.right = a_pCell->pos.rect.right;
		RDraw::Draw3DRect(a_hDC, l_rect, RGB(0, 0, 0), RGB(0, 0, 0));
		return TRUE;
	}
	return RListCtrl_DrawCell(a_hWnd, a_hDC, a_pObj, a_pCell);
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
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_BK_ACT, l_pData->m_pGameData->m_regData.m_regView.m_colorResult);
	RListCtrl_SetColor(l_pData->m_hWndList, RLC_BK_NOACT, l_pData->m_pGameData->m_regData.m_regView.m_colorResult);
}