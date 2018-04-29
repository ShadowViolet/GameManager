#pragma once

#include "SortListCtrl.h"

// CListControl �Ի���

class CListControl : public CDialogEx
{
	DECLARE_DYNAMIC(CListControl)

public:
	CListControl(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CListControl();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIST_CONTROL };
#endif

public: //��Ա����

	// �÷�����������Դ�б����ͼƬ
	void OnSetImg(CString ImgPath);

	// �÷�����������Ϸ���ݿ������Ϸ����
	BOOL OnAddData(CString Name, CString Path);

	// �����������̲߳�ˢ����Ϸ�б�
	void OnRefresh();

	// ����������Ϸ���ݿ��е���Ϸ����
	void OnSearch();

	// �򱾵���Ϸ���ݿ��������Ϸ����
	void OnAddGame();

	// ���˷���Ϸ�ļ�
	BOOL OnGameFilter(CString GamePath);

	// �ӱ�����Ϸ���ݿ����޸���Ϸ����
	void OnModifyGame();

	// �ӱ�����Ϸ���ݿ���ɾ����Ϸ����
	void OnDeleteGame();

	// �����̲߳���ʼ��Ϸ
	void OnStartGame();

	//���ڼ����Ϸ��ļ����ڵ���Ϸ����
	int CountFile(CString Path);

	//�Ϸ��ļ�����ִ�д˴��룬��������ļ�������Ϸ
	void OnAddFloder(CString Path);

	//�������̣߳�ִ�����к�ʱ����
	static UINT Operate(LPVOID lpParameter);

	//������Ϸ�̣߳���ȡ��������ʼ��Ϸ
	static UINT StartGame(LPVOID pParam);


public: // ��Ա����

	// ͼ���б�
	CImageList m_Imagelist;

	// �������ͱ���
	int ParameterType;

	// ���ұ�������б��
	BOOL IsFind, IsRunning;

	// �Ϸű���
	HDROP hDropInfo;

	// �������̱߳���
	CWinThread *m_hOperate;
	
	// ��Ϸ�б�
	CSortListCtrl m_Game_List;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNMClickGameList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkGameList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickGameList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedGameList(NMHDR *pNMHDR, LRESULT *pResult);
};
