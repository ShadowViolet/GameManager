#ifndef ___MY_DUI_H__
#define ___MY_DUI_H__

#include "../stdafx.h"

//////////////////////////////////////////////////////////
//Base
#include "Base/ControlBase.h"
#include "Base/GlobalFunction.h"
#include "Base/MessageInterface.h"

//////////////////////////////////////////////////////////
//Timer
#include "Timer/Timer.h"

//////////////////////////////////////////////////////////
//Button
#include "Button/HideButton.h"
#include "Button/ImageButton.h"

//////////////////////////////////////////////////////////
//Other
#include "Other/Frame.h"
#include "Other/Line.h"
#include "Other/SelectBox.h"

//////////////////////////////////////////////////////////
//Picture
#include "Picture/Picture.h"

//////////////////////////////////////////////////////////
//Tab
#include "Tab/Tab.h"

//////////////////////////////////////////////////////////
//Dialog
#include "Dialog/DlgBase.h"
#include "Dialog/DlgPopup.h"
#include "Dialog/DlgSkin.h"
#include "Dialog/WndShadow.h"

//////////////////////////////////////////////////////////
//Menu
#include "Menu/MenuItem.h"
#include "Menu/MenuEx.h"

//////////////////////////////////////////////////////////
//Resource

//¶¨Ê±Æ÷
#define					IDI_TIME_TEST						1

//¿Ø¼þ
#define					PIC_LOGO							2000
#define					BT_SKIN								2001
#define					BT_FEEDBACK							2002
#define					BT_MENU								2003
#define					BT_VERSION							2004
#define					BT_BOX						     	2005
#define					BT_HANDLE							2006

#define					TAB									2010

#define					MENU_MAIN_SETING					2020
#define					MENU_MAIN_UPDATE					2021
#define					MENU_MAIN_HANDLE					2022
#define					MENU_MAIN_BBS						2023
#define					MENU_MAIN_HELP						2024
#define					MENU_MAIN_ABOUT						2025

#define					BT_SKIN_CUSTOMIZE					2030
#define					BT_SKIN_IMAGE						2031
#define					BT_SKIN_COLOR						2032
#define					PIC_SELECT							2033
#define					BOX_COLOR							2034
#define					BOX_IMAGE							2035

#define					WM_SKIN								(WM_USER + 1)
#define					WM_MIAN_MENU						(WM_USER + 2)	

#endif