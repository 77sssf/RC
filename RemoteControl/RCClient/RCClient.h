
// RCClient.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CRCClientApp:
// See RCClient.cpp for the implementation of this class
//

class CRCClientApp : public CWinApp
{
public:
	CRCClientApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRCClientApp theApp;
