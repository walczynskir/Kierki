#pragma once
#include <rcommon/RString.h>
#include "typedefs.h"


class CRegData
{
public:
	CRegData(void);
	~CRegData(void);
	void Serialize(void);

	const tstring& GetPlayerName(T_PLAYER a_enPlayer) const { return m_regPlayers.m_arrPlayersNames[(int)a_enPlayer]; };
	void SetPlayerName(T_PLAYER a_enPlayer,	const tstring& a_sName);

	class RBaseRegData
	{
	public:
		virtual void Serialize(void) = 0;
	};

	class RViewRegData : public RBaseRegData
	{
	public:
		RViewRegData(void);
		bool		m_bFancyStyle;  // fancy style of UI
		UINT		m_idCover;
		virtual  void Serialize(void);
	};

	class RTimeRegData : public RBaseRegData
	{
	public:
		RTimeRegData(void);
		int			m_iWaitPassTime;
		int			m_iWaitThrowTime;
		int			m_iWaitGetTime;
		virtual void Serialize(void);
	};

	class RPlayersRegData : public RBaseRegData
	{
	public:
		RPlayersRegData(void);
		tstring	m_arrPlayersNames[4];
		virtual void Serialize(void);
	};
	
	class RRulesRegData : public RBaseRegData
	{
	public:
		RRulesRegData(void);
		bool m_bConfirmTrick;
		LANGID m_idLanguage;	// LANGID
		bool m_bLogonDlg;		// show the LogonDlg?
		bool m_bHelpVisible;	// show help at startup?
		virtual void Serialize(void);
	};
	

	class RHiddenRegData
	{
	public:
		RHiddenRegData();
		virtual void Serialize(void);

		COLORREF m_clrTintHelpBackground;
		COLORREF m_clrTintGameBackground;
		COLORREF m_clrTintResultBackground;

		COLORREF m_clrStatusBarColor;

		tstring m_sNormalFont;
		tstring m_sFancyFont;
		int m_iGameFontSize;
		int m_dyHelpPanel;
		bool m_bHelpFromResource;	// help not taken from URL, only default one, compiled into program
		tstring m_sHelpUrl;


	};

	class RAutoPersistent
	{
	public:
		RAutoPersistent();
		~RAutoPersistent();

		BYTE		m_btAlphaHelpBackground;
		BYTE		m_btAlphaResultBackground;
		BYTE		m_btAlphaGameBackground;
		T_PLAYER	m_enFirstDealer;	// dealer
	};

	RViewRegData	m_regView;
	RTimeRegData	m_regTime;
	RPlayersRegData m_regPlayers;
	RRulesRegData	m_regRules;
	RHiddenRegData  m_regHidden;
	RAutoPersistent m_regAuto;
};
