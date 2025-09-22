#include "StdAfx.h"
#include "resource.h"
#include "RRegData.h"
#include <rcommon/RString.h>
#include <rcommon/registry_value.h>
#include <Psapi.h>
#include <RCards/resource.h>

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
typedef registry_int<LANGID> RRegLangid;
typedef registry_int_def<LANGID> RRegLangidDef;

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

static const TCHAR c_sCoverID[] = _T("Software\\Medea\\Kierki\\COVERID");

static const TCHAR c_sConfirmTrickReg[] = _T("Software\\Medea\\Kierki\\CONFIRMTRICK");
static const TCHAR c_sLanguageReg[] = _T("Software\\Medea\\Kierki\\LANGUAGE");
static const TCHAR c_sLogonDlgReg[] = _T("Software\\Medea\\Kierki\\LOGONDLG");
static const TCHAR c_sHelpVisibleReg[] = _T("Software\\Medea\\Kierki\\HELPVISIBLE");
static const TCHAR c_sHelpFontReg[] = _T("Software\\Medea\\Kierki\\HELPFONT");


#define DARK_GREEN_COLOR RGB(0, 128, 0)
#define LIME_GREEN_COLOR RGB(50, 205, 50)


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
	m_colorTable = (COLORREF)RRegColorDef(c_sTableColorReg, HKEY_CURRENT_USER, GetDefaultTableColor());
	m_colorResult = (COLORREF)RRegColorDef(c_sResultColorReg, HKEY_CURRENT_USER, GetDefaultResultColor());
	m_idCover = (UINT)RRegUIntDef(c_sCoverID, HKEY_CURRENT_USER, IDB_COVER_1);
}


COLORREF RRegData::RViewRegData::GetDefaultTableColor(void) const 
{
	return DARK_GREEN_COLOR;
}


COLORREF RRegData::RViewRegData::GetDefaultResultColor(void) const
{
	return LIME_GREEN_COLOR;
}


void RRegData::RViewRegData::Serialize(void)
{
	RRegColor l_regTableColor(c_sTableColorReg, HKEY_CURRENT_USER); 
	l_regTableColor = m_colorTable;
	RRegColor l_regResultColor(c_sResultColorReg, HKEY_CURRENT_USER); 
	l_regResultColor = m_colorResult;
	RRegUInt l_regCover(c_sCoverID, HKEY_CURRENT_USER);
	l_regCover = m_idCover;
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
	m_idLanguage = (LANGID)RRegLangidDef(c_sLanguageReg, HKEY_CURRENT_USER, ::GetThreadUILanguage());
	m_bLogonDlg = (bool)RRegBoolDef(c_sLogonDlgReg, HKEY_CURRENT_USER, true);
	m_bHelpVisible = (bool)RRegBoolDef(c_sHelpVisibleReg, HKEY_CURRENT_USER, true);	
	m_sHelpFont = (tstring)RRegTStringDef(c_sHelpFontReg, HKEY_CURRENT_USER, _T("Segoe Print"));
}


void RRegData::RRulesRegData::Serialize(void)
{
	RRegBool l_regConfirmTrick(c_sConfirmTrickReg, HKEY_CURRENT_USER); 
	l_regConfirmTrick = m_bConfirmTrick;
	RRegLangid l_regLanguage(c_sLanguageReg, HKEY_CURRENT_USER);
	l_regLanguage = m_idLanguage;
	RRegBool l_regLogonDlg(c_sLogonDlgReg, HKEY_CURRENT_USER);
	l_regLogonDlg = m_bLogonDlg;
	RRegBool l_regHelpVisible(c_sHelpVisibleReg, HKEY_CURRENT_USER);
	l_regHelpVisible = m_bHelpVisible;
	RRegTString l_regHelpFont(c_sHelpFontReg, HKEY_CURRENT_USER);
	l_regHelpFont = m_sHelpFont;
}

