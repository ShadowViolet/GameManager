#pragma once
#include <vector>
using namespace std;

class CTimer
{
public:
	CTimer(void);
	~CTimer(void);

	//����ʾ�����
	vector<UINT>			m_vecTimer;				// ��ʱ��ID
	static UINT				m_uAccuracy;			// ����ֱ��� 
	static BOOL				m_bIsTmierOK;			// �Ƿ���Զ�ʱ 

	BOOL GetTiemrIsRun(UINT uTimerID);
	int SetTimer(UINT nTimerResolution);
	int KillTimer(UINT uTimerID);
	void KillTimer();
	static void CALLBACK TimerCallProc(UINT uTimerID, UINT msg, DWORD dwData, DWORD dwa, DWORD dwb);
	virtual void OnTimer(UINT uTimerID) = 0;
};
