#include "StdAfx.h"
#include "resource.h"
#include "RegData.h"
#include <rcommon/RString.h>
#include <rcommon/registry_value.h>
#include <Psapi.h>
#include <RCards/resource.h>

typedef registry_string<tstring> RRegTString;
typedef registry_string_def<tstring> RRegTStringDef;
typedef registry_int<int> RRegInt;
typedef registry_int_def<int> RRegIntDef;
typedef registry_int<int> RRegPlayer;
typedef registry_int_def<int> RRegPlayerDef;
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
typedef registry_int<BYTE> RRegByte;
typedef registry_int_def<BYTE> RRegByteDef;

static const TCHAR c_sDealerReg[] = _T("Software\\Medea\\Kierki\\DEALER");
static const TCHAR c_sWaitTimeReg[] = _T("Software\\Medea\\Kierki\\WAITTIME");
static const TCHAR c_sWaitGetTimeReg[] = _T("Software\\Medea\\Kierki\\WAITGETTIME");
static const TCHAR c_sWaitPassTimeReg[] = _T("Software\\Medea\\Kierki\\WAITPASSTIME");
static const TCHAR c_sLastNameReg[] = _T("Software\\Medea\\Kierki\\LASTNAME");
static const TCHAR c_sPlayer2Reg[] = _T("Software\\Medea\\Kierki\\PLAYER2");
static const TCHAR c_sPlayer3Reg[] = _T("Software\\Medea\\Kierki\\PLAYER3");
static const TCHAR c_sPlayer4Reg[] = _T("Software\\Medea\\Kierki\\PLAYER4");

static const TCHAR c_sTableFeltReg[] = _T("Software\\Medea\\Kierki\\TABLEFELT");
static const TCHAR c_sResultColorReg[] = _T("Software\\Medea\\Kierki\\RESULTCOLOR");

static const TCHAR c_sStartPointReg[] = _T("Software\\Medea\\Kierki\\STARTPOINT");
static const TCHAR c_sSaveScoresReg[] = _T("Software\\Medea\\Kierki\\SAVESCORES");

static const TCHAR c_sCoverID[] = _T("Software\\Medea\\Kierki\\COVERID");

static const TCHAR c_sConfirmTrickReg[] = _T("Software\\Medea\\Kierki\\CONFIRMTRICK");
static const TCHAR c_sLanguageReg[] = _T("Software\\Medea\\Kierki\\LANGUAGE");
static const TCHAR c_sLogonDlgReg[] = _T("Software\\Medea\\Kierki\\LOGONDLG");
static const TCHAR c_sHelpVisibleReg[] = _T("Software\\Medea\\Kierki\\HELPVISIBLE");
static const TCHAR c_sHelpFontReg[] = _T("Software\\Medea\\Kierki\\HELPFONT");

static const TCHAR c_sAlphaHelpBackground[] = _T("Software\\Medea\\Kierki\\ALPHAHELPBACKGROUND");
static const TCHAR c_sTintHelpBackground[] = _T("Software\\Medea\\Kierki\\TINTHELPBACKGROUND");

static const TCHAR c_sAlphaGameBackground[] = _T("Software\\Medea\\Kierki\\ALPHAGAMEBACKGROUND");
static const TCHAR c_sTintGameBackground[] = _T("Software\\Medea\\Kierki\\TINTGAMEBACKGROUND");

static const TCHAR c_sAlphaResultBackground[] = _T("Software\\Medea\\Kierki\\ALPHARESULTBACKGROUND");
static const TCHAR c_sTintResultBackground[] = _T("Software\\Medea\\Kierki\\TINTRESULTBACKGROUND");

static const TCHAR c_sFancyStyle[] = _T("Software\\Medea\\Kierki\\FANCYSTYLE");

static const TCHAR c_sNormalFontReg[] = _T("Software\\Medea\\Kierki\\NORMALFONT");
static const TCHAR c_sFancyFontReg[] = _T("Software\\Medea\\Kierki\\NFANCYFONT");
static const TCHAR c_sGameFontSizeReg[] = _T("Software\\Medea\\Kierki\\GAMEFONTSIZE");

static const TCHAR c_sStatusBarColor[] = _T("Software\\Medea\\Kierki\\STATUSBARCOLOR");
static const TCHAR c_sHelpPanelHeight[] = _T("Software\\Medea\\Kierki\\HELPPANELHEIGHT");


constexpr TCHAR c_sHelpFromResource[] = _T("Software\\Medea\\Kierki\\HELPFROMRESOURCE");
constexpr TCHAR c_sHelpUrl[] = _T("Software\\Medea\\Kierki\\HELPURL");

constexpr TCHAR c_sShowAllCards[] = _T("Software\\Medea\\Kierki\\SHOWALLCARDS");



#define DARK_GREEN_COLOR RGB(0, 128, 0)
#define LIME_GREEN_COLOR RGB(50, 205, 50)

constexpr BYTE cc_iAlphaDefault = 160;

constexpr COLORREF cc_iWhiteTintDefault = 0xffffff;		// to enlighten
constexpr COLORREF cc_iGameTintDefault = 0x008800;		// dark green

constexpr int cc_iDefaultFontSize = 12;
constexpr int cc_dyDefaultHelpPanel = 36;

constexpr LPCTSTR cc_sFancyFontDefault = _T("Segoe Print");
constexpr LPCTSTR cc_sNormalFontDefault = _T("Segoe UI");

constexpr COLORREF cc_clrStatusBarColorDefault = 0xFF8728;

constexpr int cc_iHelpPanelHeightDefault = 36;

constexpr LPCTSTR cc_sHelpUrlDefault = _T("https://raw.githubusercontent.com/walczynskir/Kierki/master/instructions/instructions_");



static tstring GetDeafaultSystemFontName();


CRegData::CRegData(void)
{
}


CRegData::~CRegData(void)
{
}


void CRegData::Serialize(void)
{
	m_regView.Serialize();
	m_regTime.Serialize();
	m_regPlayers.Serialize();
	m_regRules.Serialize();
}


CRegData::RViewRegData::RViewRegData(void)
{
	m_bFancyStyle = RRegBoolDef(c_sFancyStyle, HKEY_CURRENT_USER, TRUE);;
	m_idCover = RRegUIntDef(c_sCoverID, HKEY_CURRENT_USER, IDB_COVER_1);
}




void CRegData::RViewRegData::Serialize(void)
{
	RRegBool l_regFancyStyle(c_sFancyStyle, HKEY_CURRENT_USER);
	l_regFancyStyle = m_bFancyStyle;

	RRegUInt l_regCover(c_sCoverID, HKEY_CURRENT_USER);
	l_regCover = m_idCover;
}


CRegData::RTimeRegData::RTimeRegData(void)
{
	m_iWaitThrowTime = (int)RRegIntDef(c_sWaitTimeReg, HKEY_CURRENT_USER, 200);
	m_iWaitGetTime = (int)RRegIntDef(c_sWaitGetTimeReg, HKEY_CURRENT_USER, 500);
	m_iWaitPassTime = (int)RRegIntDef(c_sWaitPassTimeReg, HKEY_CURRENT_USER, 1500);
}


void CRegData::RTimeRegData::Serialize(void)
{
	RRegInt l_regWaitTime(c_sWaitTimeReg, HKEY_CURRENT_USER); 
	l_regWaitTime = m_iWaitThrowTime;
	RRegInt l_regWaitGet(c_sWaitGetTimeReg, HKEY_CURRENT_USER); 
	l_regWaitGet = m_iWaitGetTime;
	RRegInt l_regPassTime(c_sWaitPassTimeReg, HKEY_CURRENT_USER); 
	l_regPassTime = m_iWaitPassTime;
}


CRegData::RPlayersRegData::RPlayersRegData(void)
{
	m_arrPlayersNames[0] = RRegTString(c_sLastNameReg, HKEY_CURRENT_USER);
	TCHAR l_sDefName[128];
	::LoadString(::GetModuleHandle(NULL), IDS_DEFNAME1, l_sDefName, ArraySize(l_sDefName));
	m_arrPlayersNames[1] = RRegTStringDef(c_sPlayer2Reg, HKEY_CURRENT_USER, l_sDefName);
	::LoadString(::GetModuleHandle(NULL), IDS_DEFNAME2, l_sDefName, ArraySize(l_sDefName));
	m_arrPlayersNames[2] = RRegTStringDef(c_sPlayer3Reg, HKEY_CURRENT_USER, l_sDefName);
	::LoadString(::GetModuleHandle(NULL), IDS_DEFNAME3, l_sDefName, ArraySize(l_sDefName));
	m_arrPlayersNames[3] = RRegTStringDef(c_sPlayer4Reg, HKEY_CURRENT_USER, l_sDefName);
}


void CRegData::RPlayersRegData::Serialize(void)
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


//	-----------------------------------------------------------
//	Ustawia nazwê gracza
//
void CRegData::SetPlayerName(T_PLAYER a_enPlayer, const tstring& a_sName)
{
	m_regPlayers.m_arrPlayersNames[static_cast<int>(a_enPlayer)] = a_sName;
}


CRegData::RRulesRegData::RRulesRegData(void)
{
	m_bConfirmTrick = RRegBoolDef(c_sConfirmTrickReg, HKEY_CURRENT_USER, true);
	m_idLanguage = RRegLangidDef(c_sLanguageReg, HKEY_CURRENT_USER, ::GetThreadUILanguage());
	m_bLogonDlg = RRegBoolDef(c_sLogonDlgReg, HKEY_CURRENT_USER, true);
	m_bHelpVisible = RRegBoolDef(c_sHelpVisibleReg, HKEY_CURRENT_USER, true);	
}


void CRegData::RRulesRegData::Serialize(void)
{
	RRegBool l_regConfirmTrick(c_sConfirmTrickReg, HKEY_CURRENT_USER); 
	l_regConfirmTrick = m_bConfirmTrick;
	RRegLangid l_regLanguage(c_sLanguageReg, HKEY_CURRENT_USER);
	l_regLanguage = m_idLanguage;
	RRegBool l_regLogonDlg(c_sLogonDlgReg, HKEY_CURRENT_USER);
	l_regLogonDlg = m_bLogonDlg;
	RRegBool l_regHelpVisible(c_sHelpVisibleReg, HKEY_CURRENT_USER);
	l_regHelpVisible = m_bHelpVisible;

}


CRegData::RHiddenRegData::RHiddenRegData(void)
{
	m_clrTintHelpBackground = RRegColorDef(c_sTintHelpBackground, HKEY_CURRENT_USER, cc_iWhiteTintDefault);
	m_clrTintGameBackground = RRegColorDef(c_sTintGameBackground, HKEY_CURRENT_USER, cc_iGameTintDefault);
	m_clrTintResultBackground = RRegColorDef(c_sTintResultBackground, HKEY_CURRENT_USER, cc_iWhiteTintDefault);

	m_clrStatusBarColor = RRegColorDef(c_sStatusBarColor, HKEY_CURRENT_USER, cc_clrStatusBarColorDefault);

	m_sNormalFont = RRegTStringDef(c_sNormalFontReg, HKEY_CURRENT_USER, GetDeafaultSystemFontName());
	m_sFancyFont = RRegTStringDef(c_sFancyFontReg, HKEY_CURRENT_USER, cc_sFancyFontDefault);

	m_iGameFontSize = RRegIntDef(c_sGameFontSizeReg, HKEY_CURRENT_USER, cc_iDefaultFontSize);

	m_dyHelpPanel = RRegIntDef(c_sHelpPanelHeight, HKEY_CURRENT_USER, cc_iHelpPanelHeightDefault);
	m_bHelpFromResource = RRegBoolDef(c_sHelpFromResource, HKEY_CURRENT_USER, false);
	m_sHelpUrl = RRegTStringDef(c_sHelpUrl, HKEY_CURRENT_USER, cc_sHelpUrlDefault);

	m_bShowAllCards = RRegBoolDef(c_sShowAllCards, HKEY_CURRENT_USER, false);
}


void CRegData::RHiddenRegData::Serialize(void)
{
	RRegColor l_regTintHelpBackground(c_sTintHelpBackground, HKEY_CURRENT_USER);
	l_regTintHelpBackground = m_clrTintHelpBackground;

	RRegColor l_regTintGameBackground(c_sTintGameBackground, HKEY_CURRENT_USER);
	l_regTintGameBackground = m_clrTintGameBackground;

	RRegColor l_regTintResultBackground(c_sTintResultBackground, HKEY_CURRENT_USER);
	l_regTintResultBackground = m_clrTintResultBackground;

	RRegColor l_regStatusBarColor(c_sStatusBarColor, HKEY_CURRENT_USER);
	l_regStatusBarColor = m_clrStatusBarColor;

	RRegTString l_regNormalFont(c_sNormalFontReg, HKEY_CURRENT_USER);
	l_regNormalFont = m_sNormalFont;

	RRegTString l_regFancyFont(c_sFancyFontReg, HKEY_CURRENT_USER);
	l_regFancyFont = m_sFancyFont;

	RRegInt l_regGameFontSize(c_sGameFontSizeReg, HKEY_CURRENT_USER);
	l_regGameFontSize = m_iGameFontSize;

	RRegInt l_regHelpPanelHeight(c_sGameFontSizeReg, HKEY_CURRENT_USER);
	l_regHelpPanelHeight = m_dyHelpPanel;

	RRegBool l_regHelpFromResource(c_sHelpFromResource, HKEY_CURRENT_USER);
	l_regHelpFromResource = m_bHelpFromResource;

	RRegTString l_regHelpUrl(c_sHelpUrl, HKEY_CURRENT_USER);
	l_regHelpUrl = m_sHelpUrl;

	RRegBool l_regShowAllCards(c_sShowAllCards, HKEY_CURRENT_USER);
	l_regShowAllCards = m_bShowAllCards;
}


CRegData::RAutoPersistent::RAutoPersistent(void)
{
	RRegPlayerDef l_regPlayer(c_sDealerReg, HKEY_CURRENT_USER, static_cast<int>(Player::South));
	m_enFirstDealer = static_cast<Player>(static_cast<int>(l_regPlayer));

	m_btAlphaHelpBackground = (BYTE)RRegByteDef(c_sAlphaHelpBackground, HKEY_CURRENT_USER, cc_iAlphaDefault);;
	m_btAlphaGameBackground = (BYTE)RRegByteDef(c_sAlphaGameBackground, HKEY_CURRENT_USER, cc_iAlphaDefault);;
	m_btAlphaResultBackground = (BYTE)RRegByteDef(c_sAlphaResultBackground, HKEY_CURRENT_USER, cc_iAlphaDefault);;
}


CRegData::RAutoPersistent::~RAutoPersistent(void)
{
	RRegPlayer l_regDealer(c_sDealerReg, HKEY_CURRENT_USER);
	l_regDealer = static_cast<int>(m_enFirstDealer);

	RRegByte l_regAlphaHelpBackground(c_sAlphaHelpBackground, HKEY_CURRENT_USER);
	l_regAlphaHelpBackground = m_btAlphaHelpBackground;
	RRegByte l_regAlphaGameBackground(c_sAlphaGameBackground, HKEY_CURRENT_USER);
	l_regAlphaGameBackground = m_btAlphaGameBackground;
	RRegByte l_regAlphaResultBackground(c_sAlphaResultBackground, HKEY_CURRENT_USER);
	l_regAlphaResultBackground = m_btAlphaResultBackground;
}


tstring GetDeafaultSystemFontName()
{
	NONCLIENTMETRICS l_metrics = {};
	l_metrics.cbSize = sizeof(NONCLIENTMETRICS);

#if (_WIN32_WINNT < 0x0600)
	l_metrics.cbSize -= sizeof(l_metrics.iPaddedBorderWidth);
#endif

	if (::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, l_metrics.cbSize, &l_metrics, 0))
		return l_metrics.lfMessageFont.lfFaceName;

	return cc_sNormalFontDefault;
}