
// MfcProjectDlg.h: 헤더 파일
//

#pragma once
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#define WM_UPDATE_POINTS (WM_USER + 1)


struct Point {
	int x;
	int y;
};

// CMfcProjectDlg 대화 상자
class CMfcProjectDlg : public CDialogEx
{
// 생성입니다.
public:
	CMfcProjectDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCPROJECT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	CListBox* m_pListBox;
	std::vector<CPoint> m_points;  // 클릭 좌표 저장
	int m_radius;                 // 반지름
	int m_thickness;         // 두께
	int m_draggingIndex;
	bool m_isDragging;

	std::thread m_moveThread;
	bool m_threadRunning;

	CRITICAL_SECTION m_cs;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

public:
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnMove();
	afx_msg void OnEnChangeEditRadius();
	afx_msg void OnEnChangeEditThickness();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLbnSelchangeList();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUpdatePoints(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()
};