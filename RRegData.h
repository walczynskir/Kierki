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
		COLORREF	m_colorTable;
		COLORREF	m_colorResult;
		UINT		m_idCover;
		COLORREF GetDefaultTableColor(void) const;
		COLORREF GetDefaultResultColor(void) const;
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

	RViewRegData	m_regView;
	RTimeRegData	m_regTime;
	RPlayersRegData m_regPlayers;
	ROtherRegData	m_regOther;
	RRulesRegData	m_regRules;
};
