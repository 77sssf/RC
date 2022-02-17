// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include "SrvSocket.h"
#include <direct.h>
#include <io.h>
#include <vector>
#include <map>
#include <list>
#include <atlimage.h>
#include "LockDialog.h"
#include "RemoteControl.h"
#include <afxdialogex.h>
#include "Tool.h"
#include "CommandHanle.h"
#include "Pkt.h"

#define STARTUP_PATH "C:\\Users\\moeby\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\RemoteControl.exe"
#define ADMIN L"Administrator"
#define SUB_KEY "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"

#endif //PCH_H
