#pragma once
#include <UIRibbon.h>

// class to store references to the Ribbon framework and application object for RIBBON_UI
class RibbonData
{
public:
	IUIFramework* m_pFramework = NULL;  // Reference to the Ribbon framework.
	IUIApplication* m_pApplication = NULL;  // Reference to the Application object.
};

