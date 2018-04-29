#pragma once

#include "SortListCtrl.h"

// CListControl 对话框

class CListControl : public CDialogEx
{
	DECLARE_DYNAMIC(CListControl)

public:
	CListControl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CListControl();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIST_CONTROL };
#endif

public: //成员函数

	// 该方法用于向资源列表添加图片
	void OnSetImg(CString ImgPath);

	// 该方法用于向游戏数据库添加游戏数据
	BOOL OnAddData(CString Name, CString Path);

	// 启动工作者线程并刷新游戏列表
	void OnRefresh();

	// 搜索本地游戏数据库中的游戏数据
	void OnSearch();

	// 向本地游戏数据库中添加游戏数据
	void OnAddGame();

	// 过滤非游戏文件
	BOOL OnGameFilter(CString GamePath);

	// 从本地游戏数据库中修改游戏数据
	void OnModifyGame();

	// 从本地游戏数据库中删除游戏数据
	void OnDeleteGame();

	// 创建线程并开始游戏
	void OnStartGame();

	//用于计算拖放文件夹内的游戏数量
	int CountFile(CString Path);

	//拖放文件夹则执行此代码，用于添加文件夹内游戏
	void OnAddFloder(CString Path);

	//工作者线程，执行所有耗时操作
	static UINT Operate(LPVOID lpParameter);

	//启动游戏线程，读取参数并开始游戏
	static UINT StartGame(LPVOID pParam);


public: // 成员变量

	// 图像列表
	CImageList m_Imagelist;

	// 工作类型变量
	int ParameterType;

	// 查找标记与运行标记
	BOOL IsFind, IsRunning;

	// 拖放变量
	HDROP hDropInfo;

	// 工作者线程变量
	CWinThread *m_hOperate;
	
	// 游戏列表
	CSortListCtrl m_Game_List;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
