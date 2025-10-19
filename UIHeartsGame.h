#pragma once

#include "Card.h"


class CUIHeartsGame
{
public:
	void OnCardPlayed(Player a_player, short a_nCard);
	void OnWaitForUser(Player a_player);
	void OnTrickTaken(bool* a_pbPlayed);

	void SetWnd(HWND a_hWnd) { m_hWnd = a_hWnd; };

private:
	HWND m_hWnd{};
};
