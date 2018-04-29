// Manager.h : Manager DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "sqlite3\sqlite3.h"
using namespace SqliteSpace;

#define WM_CHILDMESSAGE  WM_USER + 100 //�Զ����Ӵ�����Ϣ

// CManagerApp
// �йش���ʵ�ֵ���Ϣ������� Manager.cpp
//

class CManagerApp : public CWinApp
{
// ��Ա����
public:
	CManagerApp();
	BOOL ConfirmHost(CString HostName);  // ȷ������
	CString GetApplicationVersion();     // �õ��汾��
	

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

// ��Ա����
public:
	sqlite3 *db;
	sqlite3_stmt * stmt;
	char * errMsg;

	int ChildType;
	CString HostName, HostVersion, Class, Path, Name, Type, Fever, Parameters, User, Password;
};

extern CManagerApp theApp;