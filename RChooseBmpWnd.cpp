// RChooseBmpWnd.cpp: implementation of the RChooseBmpWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "RChooseBmpWnd.h"
#include "coverres.h"
#include <rcommon/RMemDC.h>
#include <rcommon/drawutl.h>
#include <commctrl.h>
#include <vector>


typedef struct S_RESENUM
{
	bool    bInt;
	UINT    idRes;
	tstring sRes;
} RResEnum;

typedef std::vector<RResEnum> UResVect;


class RChooseBmpData
{
public:
	RChooseBmpData(void)
	{
		m_hWndSpin = NULL;
		m_bExecutable = false;
		m_iIdx = 0;
		m_hModule = NULL;
		m_hBmp = NULL;
		m_sizePicture.cx = m_sizePicture.cy = 0;
		m_bPictureOnly = false;
	}

	~RChooseBmpData(void)
	{
		SetModule(NULL);
		SetBmp(NULL);
	}

	void SetModule(HMODULE a_hModule)
	{
		if (m_hModule != NULL)
		{
			::FreeLibrary(m_hModule);
		}
		m_hModule = a_hModule;
	}

	HMODULE GetModule() const { return m_hModule; }

	void SetBmp(HBITMAP a_hBmp)
	{
		if (m_hBmp != NULL)
		{
			::DeleteObject(m_hBmp);
		}
		m_hBmp = a_hBmp;
	}

	HBITMAP GetBmp() const { return m_hBmp; }

	HWND m_hWndSpin;
	bool m_bExecutable;
	UINT m_iIdx;
	UResVect m_vectRes;
	SIZE     m_sizePicture;
	tstring  m_sPath;

private:
	HMODULE m_hModule;
	HBITMAP m_hBmp;
	bool    m_bPictureOnly;
};


static const long c_iWindowOfs = sizeof(RChooseBmpData*) - 4;
static inline RChooseBmpData* GetData(HWND a_hWnd);

// messages
static inline BOOL OnCreate(HWND a_hWnd, LPCREATESTRUCT a_lpStruct);
static inline void OnDestroy(HWND a_hWnd);
static inline void OnPaint(HWND a_hWnd);
static inline void OnSetFocus(HWND a_hWnd);
static inline void OnHScroll(HWND a_hWnd, UINT a_iCode);
static inline void OnSize(HWND a_hWnd, long a_dx, long a_dy);
static inline LRESULT OnSetFile(HWND a_hWnd, LPCTSTR a_psFile);
static inline void OnSetPictureSize(HWND a_hWnd, long a_dx, long a_dy);
static inline void OnGetMinSize(HWND a_hWnd, LPSIZE a_pSize);
static inline LRESULT OnGetChosen(HWND a_hWnd, LPRCOVERRES a_pData);
static inline void OnSetChosen(HWND a_hWnd, const LPRCOVERRES a_pData);

static inline void Draw(HWND a_hWnd, HDC a_hDC);
static bool SetFile(HWND a_hWnd, LPCTSTR a_psFile);
static void SetIdRes(HWND a_hWnd, LPCVOID a_idRes);
static void LoadBmp(HWND a_hWnd);
static BOOL CALLBACK EnumResProc(HMODULE a_hModule, LPCTSTR a_sType, 
	LPTSTR a_sName, LONG_PTR a_iParam);

#define HasStyle(a_hWnd, a_iStyle) ((::GetWindowLong(a_hWnd, GWL_STYLE) & a_iStyle) == a_iStyle)
#define IsPictureOnly(a_hWnd) (HasStyle(a_hWnd, RCBS_PICTUREONLY))

#define BACK_COLOR (RGB(255, 255, 255))
static const short cc_dySpin = 20;

//	---------------------------------------------------------------------------------------
//	Main RChooseBmpWnd procedure
//
LRESULT CALLBACK 
RChooseBmpWnd_WndProc(
	HWND a_hWnd, 
	UINT a_iMsg, 
	WPARAM a_wParam, 
	LPARAM a_lParam
	)
{

	switch (a_iMsg) 
	{
		case WM_CREATE:
			return (OnCreate(a_hWnd, (LPCREATESTRUCT)a_lParam) ? 0 : -1);

		case WM_PAINT:
			OnPaint(a_hWnd);
			break;

		case WM_LBUTTONDOWN:
			::SetFocus(a_hWnd);
			break;

		case WM_SETFOCUS:
			OnSetFocus(a_hWnd);
			break;

		case WM_HSCROLL:
			OnHScroll(a_hWnd, LOWORD(a_wParam));
			break;

		case WM_SIZE:
			OnSize(a_hWnd, LOWORD(a_lParam), HIWORD(a_lParam));
			break;

		case WM_DESTROY:
			OnDestroy(a_hWnd);
			break;

		case RCBM_SETFILE:
			return OnSetFile(a_hWnd, reinterpret_cast<LPCTSTR>(a_lParam));

		case RCBM_SETPICTURESIZE:
			OnSetPictureSize(a_hWnd, static_cast<long>(a_wParam), static_cast<long>(a_lParam));
			break;

		case RCBM_GETMINSIZE:
			OnGetMinSize(a_hWnd, reinterpret_cast<LPSIZE>(a_lParam));
			break;

		case RCBM_GETCHOSEN:
			return OnGetChosen(a_hWnd, reinterpret_cast<LPRCOVERRES>(a_lParam));

		case RCBM_SETCHOSEN:
			OnSetChosen(a_hWnd, reinterpret_cast<const LPRCOVERRES>(a_lParam));
			break;

		default:
			return ::DefWindowProc(a_hWnd, a_iMsg, a_wParam, a_lParam);
   }
   return 0;
}


//	---------------------------------------------------------------------------------------
//	Creation of RChooseBmpWnd window
//
HWND	// Handle of created window or NULL if failed
RChooseBmpWnd_CreateEx(
	DWORD		a_iStyle,		// style
	DWORD		a_iStyleEx,		// extended style
	int			a_x,			// horizontal position of window
	int			a_y,			// vertical position of window
	int			a_iWidth,		// window width
	int			a_iHeight,		// window height
	HWND		a_hWndParent,   // handle to parent or owner window
	LPVOID		a_lpParam			// pointer to window-creation data
	)
{
	HWND l_hWnd = ::CreateWindowEx(a_iStyleEx, RChooseBmpWnd_ClassName, _T(""), a_iStyle,
		a_x, a_y, a_iWidth, a_iHeight, a_hWndParent, NULL, ::GetModuleHandle(NULL), a_lpParam);
	return l_hWnd;
}


//	---------------------------------------------------------------------------------------
//	Returns control's data
//
RChooseBmpData*	GetData(HWND a_hWnd)
{
#pragma warning(disable: 4312)
	return reinterpret_cast<RChooseBmpData*>(::GetWindowLongPtr(a_hWnd, c_iWindowOfs));
#pragma warning(default: 4312)
}


//	---------------------------------------------------------------------------------------
//	Sets control's data
//
void SetData(HWND a_hWnd, RChooseBmpData* a_pData)
{
#pragma warning(disable: 4311)
	::SetWindowLongPtr(a_hWnd, c_iWindowOfs, reinterpret_cast<LONG>(a_pData));
#pragma warning(default: 4311)
}


//	---------------------------------------------------------------------------------------
//	Initiation of window data
//
BOOL OnCreate(
	HWND a_hWnd,	//IN 
	LPCREATESTRUCT a_lpStruct
	)
{
	RChooseBmpData* l_pData = new RChooseBmpData();
	SetData(a_hWnd, l_pData);

	if (!IsPictureOnly(a_hWnd))
	{
		INITCOMMONCONTROLSEX l_icc;
		l_icc.dwSize = sizeof(l_icc);
		l_icc.dwICC = ICC_UPDOWN_CLASS;
		::InitCommonControlsEx(&l_icc);
		l_pData->m_hWndSpin = ::CreateWindow(UPDOWN_CLASS, NULL, WS_CHILD | UDS_HORZ, 
			0, 0, 50, cc_dySpin, a_hWnd,
			NULL, ::GetModuleHandle(NULL), NULL);
	}
	return TRUE;
}


//	---------------------------------------------------------------------------------------
//	Postmortem:-)
//
void OnDestroy(
	HWND a_hWnd		//IN
	)
{
	const RChooseBmpData* l_pData = GetData(a_hWnd);
	delete l_pData;
}


//	---------------------------------------------------------------------------------------
//	Drawing - WM_PAINT message handler (all drawing here)
//
void OnPaint(
	HWND a_hWnd	//WE okno
	)
{
	PAINTSTRUCT l_ps;
	HDC l_hdc = ::BeginPaint(a_hWnd, &l_ps);

	RECT l_rectWin;
	::GetClientRect(a_hWnd, &l_rectWin);
	{	
		// prepare correct DC
#ifdef _DEBUG 
		HDC l_hMemDC = l_hdc;
#else
		RMemDC l_memDC(l_hdc, &l_rectWin);
		HDC l_hMemDC = l_memDC;
#endif
		
		Draw(a_hWnd, l_hMemDC);
	}
	::EndPaint(a_hWnd, &l_ps);
}


//	---------------------------------------------------------------------------
//	Really draws KeyWnd control
//
void Draw(
	HWND a_hWnd, 
	HDC a_hDC
	)
{
	const RChooseBmpData* l_pData = GetData(a_hWnd);
	RECT l_rectClient;
	::GetClientRect(a_hWnd, &l_rectClient);
	if (l_pData->GetBmp() != NULL)
	{

		RECT l_rect;
		::SetRect(&l_rect, 0, 0, l_pData->m_sizePicture.cx, l_pData->m_sizePicture.cy);
		RECT l_rectDraw;
		::IntersectRect(&l_rectDraw, &l_rectClient, &l_rect);
		if (l_pData->m_bExecutable)
		{
			RDraw::DrawStretchedBitmap(a_hDC, l_rectDraw, l_pData->GetBmp(), SRCCOPY);
		}
		else
		{
			RDraw::DrawStretchedBitmapTransparent(a_hDC, l_rectDraw, l_pData->GetBmp(), BACK_COLOR);
		}
	}
	if (!IsPictureOnly(a_hWnd) && (l_pData->m_vectRes.size() > 0))
	{
		TCHAR l_sText[64];
		_sntprintf_s(l_sText, ArraySize(l_sText), _TRUNCATE, _T("%d / %d"), l_pData->m_iIdx + 1, l_pData->m_vectRes.size());
		SIZE l_size;
		::GetTextExtentPoint32(a_hDC, l_sText, static_cast<int>(_tcslen(l_sText)), &l_size);
		RECT l_rect;
		::SetRect(&l_rect, 0, l_pData->m_sizePicture.cy + cc_dySpin, 
			l_pData->m_sizePicture.cx, l_pData->m_sizePicture.cy + cc_dySpin + l_size.cy);
		RECT l_rectDraw;
		::IntersectRect(&l_rectDraw, &l_rectClient, &l_rect);
		HFONT l_hFont = static_cast<HFONT>(::GetStockObject(DEFAULT_GUI_FONT));
		HFONT l_hFontOld = static_cast<HFONT>(::SelectObject(a_hDC, l_hFont));
		int l_iBkModeOld = ::SetBkMode(a_hDC, TRANSPARENT);
		::DrawText(a_hDC, l_sText, static_cast<int>(_tcslen(l_sText)), &l_rect, DT_CENTER | DT_VCENTER);
		::SetBkMode(a_hDC, l_iBkModeOld);
		::SelectObject(a_hDC, l_hFontOld);
	}
}


//	---------------------------------------------------------------------------
//	Rejestracja klasy
//
ATOM RChooseBmpWnd_RegisterClass()
{

	WNDCLASSEX l_wcex;
	l_wcex.cbSize			= sizeof(WNDCLASSEX); 
	l_wcex.style			= CS_DBLCLKS | CS_GLOBALCLASS | CS_PARENTDC;
	l_wcex.lpfnWndProc		= RChooseBmpWnd_WndProc;
	l_wcex.cbClsExtra		= 0;
	l_wcex.cbWndExtra		= sizeof(RChooseBmpData*);
	l_wcex.hInstance		= ::GetModuleHandle(NULL);
	l_wcex.hIcon			= NULL;
	l_wcex.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	l_wcex.hbrBackground	= reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
	l_wcex.lpszMenuName		= NULL;
	l_wcex.lpszClassName	= RChooseBmpWnd_ClassName;
	l_wcex.hIconSm			= NULL;

	return ::RegisterClassEx(&l_wcex);
}


void OnSetFocus(HWND a_hWnd)
{
	RChooseBmpData* l_pData = GetData(a_hWnd);
	if (l_pData->m_hWndSpin != NULL)
	{
		::SetFocus(l_pData->m_hWndSpin);
	}
}


void OnHScroll(HWND a_hWnd, UINT a_iCode)
{
	if (a_iCode != SB_ENDSCROLL)
	{
		RChooseBmpData* l_pData = GetData(a_hWnd);
		UINT l_iIdx = static_cast<UINT>(::SendMessage(l_pData->m_hWndSpin, UDM_GETPOS32, 0, 0));
		if (l_pData->m_iIdx != l_iIdx)
		{
			l_pData->m_iIdx = l_iIdx;
			LoadBmp(a_hWnd);
			::InvalidateRect(a_hWnd, NULL, TRUE);
			::RedrawWindow(a_hWnd, NULL, NULL, RDW_ERASENOW | RDW_UPDATENOW);
		}
	}
}


void OnSize(HWND a_hWnd, long a_dx, long a_dy)
{
	const RChooseBmpData* l_pData = GetData(a_hWnd);
	if (l_pData->m_hWndSpin != NULL)
	{
		::SetWindowPos(l_pData->m_hWndSpin, NULL, 0, l_pData->m_sizePicture.cy, 
			l_pData->m_sizePicture.cx, cc_dySpin, SWP_NOZORDER);
	}
}


LRESULT OnSetFile(HWND a_hWnd, LPCTSTR a_psFile)
{
	bool l_bSet = SetFile(a_hWnd, a_psFile);

	::InvalidateRect(a_hWnd, NULL, TRUE);
	::RedrawWindow(a_hWnd, NULL, NULL, RDW_ERASENOW | RDW_UPDATENOW);
	RChooseBmpData* l_pData = GetData(a_hWnd);
	if (l_pData->m_hWndSpin != NULL)
	{
		if (l_pData->m_vectRes.size() > 1)
		{
			::ShowWindow(l_pData->m_hWndSpin, SW_SHOW);
			::SendMessage(l_pData->m_hWndSpin, UDM_SETRANGE32, 0, l_pData->m_vectRes.size() - 1);
			::SendMessage(l_pData->m_hWndSpin, UDM_SETPOS32, 0, l_pData->m_iIdx);
		}
		else
		{
			::ShowWindow(l_pData->m_hWndSpin, SW_HIDE);
		}
	}
	return l_bSet ? TRUE : FALSE;
}


void OnSetPictureSize(HWND a_hWnd, long a_dx, long a_dy)
{
	RChooseBmpData* l_pData = GetData(a_hWnd);
	l_pData->m_sizePicture.cx = a_dx;
	l_pData->m_sizePicture.cy = a_dy;
}


void OnGetMinSize(HWND a_hWnd, LPSIZE a_pSize)
{
	const RChooseBmpData* l_pData = GetData(a_hWnd);
	a_pSize->cx = l_pData->m_sizePicture.cx;
	if (IsPictureOnly(a_hWnd))
	{
		a_pSize->cy = l_pData->m_sizePicture.cy;
	}
	else
	{
		HDC l_hDC = ::GetDC(a_hWnd);
		TCHAR l_sTest[] = _T("/0123456789");
		SIZE l_size;
		::GetTextExtentPoint32(l_hDC, l_sTest, ArraySize(l_sTest), &l_size);
		::ReleaseDC(a_hWnd, l_hDC);
		a_pSize->cy = l_pData->m_sizePicture.cy + cc_dySpin + l_size.cy;
	}
}


LRESULT OnGetChosen(HWND a_hWnd, LPRCOVERRES a_pData)
{
	const RChooseBmpData* l_pData = GetData(a_hWnd);
	if ((l_pData->m_sPath.length() == 0) || (l_pData->GetBmp() == NULL))
	{
		return FALSE;
	}

	if (l_pData->m_vectRes.size() > 0)
	{
		LPCVOID l_p;
		if (l_pData->m_vectRes[l_pData->m_iIdx].bInt)
		{
			l_p = reinterpret_cast<LPCVOID>(static_cast<LONG_PTR>(l_pData->m_vectRes[l_pData->m_iIdx].idRes));
		}
		else
		{
			l_p = reinterpret_cast<LPCVOID>(l_pData->m_vectRes[l_pData->m_iIdx].sRes.c_str());
		}
		RCoverRes_Set(a_pData, l_pData->m_bExecutable, l_pData->m_sPath.c_str(), 
			l_pData->m_vectRes[l_pData->m_iIdx].bInt, l_p);
	}
	else
	{
		RCoverRes_Set(a_pData, l_pData->m_bExecutable, l_pData->m_sPath.c_str(), 
			true, NULL);
	}
	if (l_pData->m_bExecutable && (l_pData->m_vectRes.size() <= 0))
	{
		return FALSE;
	}
	return TRUE;
}


void OnSetChosen(HWND a_hWnd, const LPRCOVERRES a_pData)
{
	SetFile(a_hWnd, a_pData->psPath);
	SetIdRes(a_hWnd, RCoverRes_GetIntResource(a_pData));
	RChooseBmpData* l_pData = GetData(a_hWnd);
	::InvalidateRect(a_hWnd, NULL, TRUE);
	::RedrawWindow(a_hWnd, NULL, NULL, RDW_ERASENOW | RDW_UPDATENOW);
	if (l_pData->m_hWndSpin != NULL)
	{
		if (l_pData->m_vectRes.size() > 1)
		{
			::ShowWindow(l_pData->m_hWndSpin, SW_SHOW);
			::SendMessage(l_pData->m_hWndSpin, UDM_SETRANGE32, 0, l_pData->m_vectRes.size() - 1);
			::SendMessage(l_pData->m_hWndSpin, UDM_SETPOS32, 0, l_pData->m_iIdx);
		}
		else
		{
			::ShowWindow(l_pData->m_hWndSpin, SW_HIDE);
		}
	}
}


bool SetFile(HWND a_hWnd, LPCTSTR a_psFile)
{
	RChooseBmpData* l_pData = GetData(a_hWnd);
	l_pData->SetModule(NULL);
	l_pData->SetBmp(NULL);
	l_pData->m_vectRes.clear();
	l_pData->m_iIdx = 0;
	l_pData->m_sPath.clear();

	// check if it is exe or dll
	l_pData->SetModule(::LoadLibraryEx(a_psFile, NULL, LOAD_LIBRARY_AS_DATAFILE));
	if (l_pData->GetModule() != NULL)
	{
		l_pData->m_bExecutable = true;
	}
	// maybe its an picture?
	else
	{
		l_pData->SetBmp(RDraw::LoadPicture(a_psFile, BACK_COLOR, NULL));
		// not picture, and not executable
		if (l_pData->GetBmp() == NULL)
		{
			if (l_pData->m_hWndSpin != NULL)
			{
				::ShowWindow(l_pData->m_hWndSpin, SW_HIDE);
			}
			return false;
		}
		l_pData->m_bExecutable = false;
	}

	l_pData->m_sPath = a_psFile;
	if (l_pData->m_bExecutable)
	{
		if (!::EnumResourceNames(l_pData->GetModule(), RT_BITMAP, EnumResProc, reinterpret_cast<LONG_PTR>(&(l_pData->m_vectRes))))
		{
			l_pData->SetModule(NULL);
		}
		else
		{
			if (l_pData->m_vectRes.size() > 0)
			{
				LoadBmp(a_hWnd);
			}
		}
	}
	return true;
}


void SetIdRes(HWND a_hWnd, LPCVOID a_idRes)
{
	RChooseBmpData* l_pData = GetData(a_hWnd);
	if (l_pData->m_vectRes.size() <= 1)
	{
		return;
	}

	for (UINT l_iAt = 0; l_iAt < l_pData->m_vectRes.size(); l_iAt++)
	{
		if ((IS_INTRESOURCE(a_idRes) && l_pData->m_vectRes[l_iAt].bInt && l_pData->m_vectRes[l_iAt].idRes == static_cast<UINT>(reinterpret_cast<LONG_PTR>(a_idRes))) ||
			(!IS_INTRESOURCE(a_idRes) && !(l_pData->m_vectRes[l_iAt].bInt) && (_tcscmp(static_cast<LPCTSTR>(a_idRes), l_pData->m_vectRes[l_iAt].sRes.c_str()) == 0)))
		{
			l_pData->m_iIdx = l_iAt;
			LoadBmp(a_hWnd);
			if (l_pData->m_hWndSpin != NULL)
			{
				::SendMessage(l_pData->m_hWndSpin, UDM_SETPOS32, 0, l_pData->m_iIdx);
			}
			return;
		}

	}
}


// ---------------------------------------------------------
// loads identifiers of all bitmaps from executable
//
BOOL CALLBACK EnumResProc(HMODULE a_hModule, LPCTSTR a_sType,
	LPTSTR a_sName,	LONG_PTR a_iParam)
{
	UResVect* l_pVect = reinterpret_cast<UResVect*>(a_iParam);

	if (a_sType == RT_BITMAP)
	{
		RResEnum l_res;
		l_res.bInt = IS_INTRESOURCE(a_sName);
		if (l_res.bInt)
		{
			l_res.idRes = static_cast<UINT>(reinterpret_cast<LONG_PTR>(a_sName));
		}
		else
		{
			l_res.sRes = a_sName;
		}
		l_pVect->push_back(l_res);
	}
	return TRUE;
}


void LoadBmp(HWND a_hWnd)
{
	RChooseBmpData* l_pData = GetData(a_hWnd);
	LPCTSTR l_psBmp = 
		l_pData->m_vectRes[l_pData->m_iIdx].bInt ? 
			MAKEINTRESOURCE(l_pData->m_vectRes[l_pData->m_iIdx].idRes) :
			l_pData->m_vectRes[l_pData->m_iIdx].sRes.c_str();
	l_pData->SetBmp(::LoadBitmap(l_pData->GetModule(), l_psBmp));
}
