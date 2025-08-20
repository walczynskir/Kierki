#include "StdAfx.h"
#include "resource.h"
#include "RRegData.h"
#include <rcommon/RString.h>
#include <rcommon/registry_value.h>
#include <Psapi.h>

typedef registry_string<tstring> RRegTString;
typedef registry_string_def<tstring> RRegTStringDef;
typedef registry_int<int> RRegInt;
typedef registry_int_def<int> RRegIntDef;
typedef registry_int<T_PLAYER> RRegPlayer;
typedef registry_int_def<T_PLAYER> RRegPlayerDef;
typedef registry_int<COLORREF> RRegColor;
typedef registry_int_def<COLORREF> RRegColorDef;
typedef registry_binary<POINT> RRegPoint;
typedef registry_binary_def<POINT> RRegPointDef;
typedef registry_int<bool> RRegBool;
typedef registry_int_def<bool> RRegBoolDef;
typedef registry_int<UINT> RRegUInt;
typedef registry_int_def<UINT> RRegUIntDef;
typedef registry_var_binary<BYTE> RRegCoverRes;

static const TCHAR c_sDealerReg[] = _T("Software\\Medea\\Kierki\\DEALER");
static const TCHAR c_sWaitTimeReg[] = _T("Software\\Medea\\Kierki\\WAITTIME");
static const TCHAR c_sWaitGetTimeReg[] = _T("Software\\Medea\\Kierki\\WAITGETTIME");
static const TCHAR c_sWaitPassTimeReg[] = _T("Software\\Medea\\Kierki\\WAITPASSTIME");
static const TCHAR c_sLastNameReg[] = _T("Software\\Medea\\Kierki\\LASTNAME");
static const TCHAR c_sPlayer2Reg[] = _T("Software\\Medea\\Kierki\\PLAYER2");
static const TCHAR c_sPlayer3Reg[] = _T("Software\\Medea\\Kierki\\PLAYER3");
static const TCHAR c_sPlayer4Reg[] = _T("Software\\Medea\\Kierki\\PLAYER4");

static const TCHAR c_sTableColorReg[] = _T("Software\\Medea\\Kierki\\TABLECOLOR");
static const TCHAR c_sResultColorReg[] = _T("Software\\Medea\\Kierki\\RESULTCOLOR");

static const TCHAR c_sStartPointReg[] = _T("Software\\Medea\\Kierki\\STARTPOINT");
static const TCHAR c_sSaveScoresReg[] = _T("Software\\Medea\\Kierki\\SAVESCORES");

// cover
static const TCHAR c_sCoverRes[] = _T("Software\\Medea\\Kierki\\Res\\COVERRES");

static const TCHAR c_sConfirmTrickReg[] = _T("Software\\Medea\\Kierki\\CONFIRMTRICK");


RRegData::RRegData(void)
{
}


RRegData::~RRegData(void)
{
}


void RRegData::Serialize(void)
{
	m_regView.Serialize();
	m_regTime.Serialize();
	m_regPlayers.Serialize();
	m_regRules.Serialize();
}


RRegData::RViewRegData::RViewRegData(void)
{
	RCoverRes_Init(&m_resCover);
	m_colorTable = (COLORREF)RRegColorDef(c_sTableColorReg, HKEY_CURRENT_USER, GetDefaultTableColor());
	m_colorResult = (COLORREF)RRegColorDef(c_sResultColorReg, HKEY_CURRENT_USER, GetDefaultResultColor());

	RRegCoverRes l_regCoverRes(c_sCoverRes, HKEY_CURRENT_USER);
	if (l_regCoverRes.exists())
	{
		DWORD l_dwSize = 0;
		l_regCoverRes.get_size(&l_dwSize);
		LPBYTE l_pBt = new BYTE[l_dwSize];
		l_regCoverRes.get_value(l_pBt, l_dwSize);
		RCoverRes_FromBuf(&m_resCover, l_pBt, l_dwSize);
		delete[] l_pBt;
	}
	else
	{
#pragma todo("only works for dll's - not for static libs") 
		// we check where the default rcards lib is
		TCHAR l_sPath[MAX_PATH];
		HMODULE l_hLib = ::LoadLibrary(_T("RCards.dll"));
		::GetModuleFileNameEx(::GetCurrentProcess(), l_hLib, l_sPath, ArraySize(l_sPath));
		::FreeLibrary(l_hLib);

		RCoverRes_Set(&m_resCover, true, l_sPath, true, reinterpret_cast<LPVOID>(57));
	}
}


RRegData::RViewRegData::~RViewRegData(void)
{
	RCoverRes_Done(&m_resCover);
}


COLORREF RRegData::RViewRegData::GetDefaultTableColor(void) const 
{
	return ::GetSysColor(COLOR_DESKTOP);
}


COLORREF RRegData::RViewRegData::GetDefaultResultColor(void) const
{
	return ::GetSysColor(COLOR_DESKTOP);
}


void RRegData::RViewRegData::Serialize(void)
{
	RRegColor l_regTableColor(c_sTableColorReg, HKEY_CURRENT_USER); 
	l_regTableColor = m_colorTable;
	RRegColor l_regResultColor(c_sResultColorReg, HKEY_CURRENT_USER); 
	l_regResultColor = m_colorResult;

	RRegCoverRes l_regCoverRes(c_sCoverRes, HKEY_CURRENT_USER);
	DWORD l_dwSize = RCoverRes_GetSize(&m_resCover);
	LPBYTE l_pBt = new BYTE[l_dwSize];
	RCoverRes_ToBuf(&m_resCover, l_pBt, l_dwSize);
	l_regCoverRes.set_value(l_pBt, l_dwSize);
	delete[] l_pBt;
}


LPCTSTR RRegData::RViewRegData::GetRes(void) const
{
	return RCoverRes_GetIntResource(&m_resCover);
}


void RRegData::RViewRegData::SetRes(LPCVOID a_res)
{
	RCoverRes_SetResource(&m_resCover, a_res);
}


RRegData::RTimeRegData::RTimeRegData(void)
{
	m_iWaitThrowTime = (int)RRegIntDef(c_sWaitTimeReg, HKEY_CURRENT_USER, 200);
	m_iWaitGetTime = (int)RRegIntDef(c_sWaitGetTimeReg, HKEY_CURRENT_USER, 500);
	m_iWaitPassTime = (int)RRegIntDef(c_sWaitPassTimeReg, HKEY_CURRENT_USER, 1500);
}


void RRegData::RTimeRegData::Serialize(void)
{
	RRegInt l_regWaitTime(c_sWaitTimeReg, HKEY_CURRENT_USER); 
	l_regWaitTime = m_iWaitThrowTime;
	RRegInt l_regWaitGet(c_sWaitGetTimeReg, HKEY_CURRENT_USER); 
	l_regWaitGet = m_iWaitGetTime;
	RRegInt l_regPassTime(c_sWaitPassTimeReg, HKEY_CURRENT_USER); 
	l_regPassTime = m_iWaitPassTime;
}


RRegData::RPlayersRegData::RPlayersRegData(void)
{
	m_arrPlayersNames[0] = (tstring)RRegTString(c_sLastNameReg, HKEY_CURRENT_USER);
	TCHAR l_sDefName[128];
	::LoadString(::GetModuleHandle(NULL), IDS_DEFNAME1, l_sDefName, ArraySize(l_sDefName));
	m_arrPlayersNames[1] = (tstring)RRegTStringDef(c_sPlayer2Reg, HKEY_CURRENT_USER, l_sDefName);
	::LoadString(::GetModuleHandle(NULL), IDS_DEFNAME2, l_sDefName, ArraySize(l_sDefName));
	m_arrPlayersNames[2] = (tstring)RRegTStringDef(c_sPlayer3Reg, HKEY_CURRENT_USER, l_sDefName);
	::LoadString(::GetModuleHandle(NULL), IDS_DEFNAME3, l_sDefName, ArraySize(l_sDefName));
	m_arrPlayersNames[3] = (tstring)RRegTStringDef(c_sPlayer4Reg, HKEY_CURRENT_USER, l_sDefName);
}


void RRegData::RPlayersRegData::Serialize(void)
{
	RRegTString l_regPlayer1(c_sLastNameReg, HKEY_CURRENT_USER); 
	l_regPlayer1 = m_arrPlayersNames[0];
	RRegTString l_regPlayer2(c_sPlayer2Reg, HKEY_CURRENT_USER); 
	l_regPlayer2 = m_arrPlayersNames[1];
	RRegTString l_regPlayer3(c_sPlayer3Reg, HKEY_CURRENT_USER); 
	l_regPlayer3 = m_arrPlayersNames[2];
	RRegTString l_regPlayer4(c_sPlayer4Reg, HKEY_CURRENT_USER); 
	l_regPlayer4 = m_arrPlayersNames[3];
}


RRegData::ROtherRegData::ROtherRegData(void)
{
	m_enFirstDealer = (T_PLAYER)RRegPlayerDef(c_sDealerReg, HKEY_CURRENT_USER, E_DL_1);
	POINT l_ptDef = {1, 1};
	m_ptStart = (POINT)RRegPointDef(c_sStartPointReg, HKEY_CURRENT_USER, l_ptDef);
	m_bSaveScores = (bool)RRegBoolDef(c_sSaveScoresReg, HKEY_CURRENT_USER, true);
}


RRegData::ROtherRegData::~ROtherRegData(void)
{
	RRegPlayer l_regDealer(c_sDealerReg, HKEY_CURRENT_USER); 
	l_regDealer = m_enFirstDealer;

	RRegPoint l_regStartPoint(c_sStartPointReg, HKEY_CURRENT_USER); 
	l_regStartPoint = m_ptStart;

	RRegBool l_regSaveScores(c_sSaveScoresReg, HKEY_CURRENT_USER); 
	l_regSaveScores = m_bSaveScores;
}

//	-----------------------------------------------------------
//	Ustawia nazwê gracza
//
void RRegData::SetPlayerName(T_PLAYER a_enPlayer, const tstring& a_sName)
{
	m_regPlayers.m_arrPlayersNames[static_cast<int>(a_enPlayer)] = a_sName;
}


RRegData::RRulesRegData::RRulesRegData(void)
{
	m_bConfirmTrick = (bool)RRegBoolDef(c_sConfirmTrickReg, HKEY_CURRENT_USER, true);
}


void RRegData::RRulesRegData::Serialize(void)
{
	RRegBool l_regConfirmTrick(c_sConfirmTrickReg, HKEY_CURRENT_USER); 
	l_regConfirmTrick = m_bConfirmTrick;
}
