#pragma once
#include <rcommon/RString.h>
#include "typedefs.h"


class RRegData
{
public:
	RRegData(void);
	~RRegData(void);
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
		bool m_bHandWrittenResult;  // fancy style of results (handwritten)
		virtual void Serialize(void);
	};
	
	class ROtherRegData
	{
	public:
		ROtherRegData();
		virtual ~ROtherRegData();

		T_PLAYER	m_enFirstDealer ;	// rozdaj¹cy
		POINT		m_ptStart;
		bool		m_bSaveScores;
	};

	class RHiddenRegData
	{
	public:
		RHiddenRegData();
		virtual void Serialize(void);

		BYTE	 m_btAlphaHelpBackground;	
		COLORREF m_clrTintHelpBackground;
		BYTE	 m_btAlphaResultBackground;
		COLORREF m_clrTintResultBackground;
		tstring	 m_sResultFont;	// result font name
		tstring  m_sHelpFont;	// help font name


	};

	RViewRegData	m_regView;
	RTimeRegData	m_regTime;
	RPlayersRegData m_regPlayers;
	ROtherRegData	m_regOther;
	RRulesRegData	m_regRules;
	RHiddenRegData  m_regHidden;
};
