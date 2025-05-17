
// MfcProjectDlg.cpp: 구현 파일


#include "pch.h"
#include "framework.h"
#include "MfcProject.h"
#include "MfcProjectDlg.h"
#include "afxdialogex.h"
//#include <vector>
//#include <random>
//#include <thread>
//#include <chrono>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };

#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

protected:
	DECLARE_MESSAGE_MAP()
};



CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}



void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()



CMfcProjectDlg::CMfcProjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCPROJECT_DIALOG, pParent), 
	  m_radius(50),
	  m_thickness(2),
	  m_points()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}



void CMfcProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CMfcProjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BTN_RESET, &CMfcProjectDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_MOVE, &CMfcProjectDlg::OnBnClickedBtnMove)
	ON_EN_CHANGE(IDC_EDIT_RADIUS, &CMfcProjectDlg::OnEnChangeEditRadius)
	ON_EN_CHANGE(IDC_EDIT_THICKNESS, &CMfcProjectDlg::OnEnChangeEditThickness)
	ON_LBN_SELCHANGE(IDC_LIST, &CMfcProjectDlg::OnLbnSelchangeList)
	ON_WM_DESTROY()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_UPDATE_POINTS, &CMfcProjectDlg::OnUpdatePoints)
END_MESSAGE_MAP()


// CMfcProjectDlg 메시지 처리기

BOOL CMfcProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_draggingIndex = -1;
	m_isDragging = false;

	InitializeCriticalSection(&m_cs);

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_pListBox = (CListBox*)GetDlgItem(IDC_LIST);
	if (m_pListBox)
	{
		m_pListBox->ResetContent();
		m_pListBox->AddString(_T("좌표 목록"));
	}

	SetDlgItemText(IDC_EDIT_RADIUS, _T("50"));
	SetDlgItemText(IDC_EDIT_THICKNESS, _T("2"));

	return TRUE;

	}



void CMfcProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}



void CMfcProjectDlg::OnPaint()
{
	CPaintDC dc(this);

	if (m_points.size() == 3)
	{
		int x1 = m_points[0].x, y1 = m_points[0].y;
		int x2 = m_points[1].x, y2 = m_points[1].y;
		int x3 = m_points[2].x, y3 = m_points[2].y;

		int d = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
		if (d != 0)
		{
			int ux = ((x1 * x1 + y1 * y1) * (y2 - y3) +
				(x2 * x2 + y2 * y2) * (y3 - y1) +
				(x3 * x3 + y3 * y3) * (y1 - y2)) / d;
			int uy = ((x1 * x1 + y1 * y1) * (x3 - x2) +
				(x2 * x2 + y2 * y2) * (x1 - x3) +
				(x3 * x3 + y3 * y3) * (x2 - x1)) / d;

			int radius = static_cast<int>(sqrt((x1 - ux) * (x1 - ux) + (y1 - uy) * (y1 - uy)));

			CPen pen(PS_SOLID, m_thickness, RGB(80, 100, 0));
			CPen* pOldPen = dc.SelectObject(&pen);

			const int segments = 360;
			for (int i = 0; i < segments; ++i)
			{
				double angle1 = 2.0 * 3.14159265 * i / segments;
				double angle2 = 2.0 * 3.14159265 * (i + 1) / segments;

				int xStart = static_cast<int>(ux + radius * cos(angle1));
				int yStart = static_cast<int>(uy + radius * sin(angle1));
				int xEnd = static_cast<int>(ux + radius * cos(angle2));
				int yEnd = static_cast<int>(uy + radius * sin(angle2));

				dc.MoveTo(xStart, yStart);
				dc.LineTo(xEnd, yEnd);
			}

			dc.SelectObject(pOldPen);
		}
	}


	
	for (const auto& p : m_points)
	{
		CBrush brush(RGB(0, 0, 255));
		CBrush* pOldBrush = dc.SelectObject(&brush);
		CPen pen(PS_SOLID, 1, RGB(0, 0, 255));
		CPen* pOldPen = dc.SelectObject(&pen);

		// 점의 크기를 m_radius 반영하여 그림
		int r = m_radius;
		dc.Ellipse(p.x - r, p.y - r, p.x + r, p.y + r);

		dc.SelectObject(pOldBrush);
		dc.SelectObject(pOldPen);
	}

	CDialogEx::OnPaint();
}



HCURSOR CMfcProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMfcProjectDlg::OnBnClickedBtnReset()
{
	m_points.clear();

	CListBox* m_pListBox = (CListBox*)GetDlgItem(IDC_LIST);
	if (m_pListBox)
	{
		m_pListBox->ResetContent();
		m_pListBox->AddString(_T("좌표 목록"));
	}

	Invalidate();

}



void CMfcProjectDlg::OnBnClickedBtnMove()
{
	if (m_points.size() != 3 || m_threadRunning)
		return; 

	m_threadRunning = true;

	m_moveThread = std::thread([this]()
		{
			CRect rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = 800; 
			rect.bottom = 600;

			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> distX(rect.left + m_radius, rect.right - m_radius);
			std::uniform_int_distribution<> distY(rect.top + m_radius, rect.bottom - m_radius);

			for (int i = 0; i < 10; ++i)
			{
				EnterCriticalSection(&m_cs);
				for (int j = 0; j < 3; ++j)
				{
					m_points[j].x = distX(gen);
					m_points[j].y = distY(gen);
				}
				LeaveCriticalSection(&m_cs);

				PostMessage(WM_UPDATE_POINTS);

				std::this_thread::sleep_for(std::chrono::milliseconds(700)); 
			}

			m_threadRunning = false;
		});

	m_moveThread.detach();
}



void CMfcProjectDlg::OnEnChangeEditRadius()
{
	CString str;
	GetDlgItemText(IDC_EDIT_RADIUS, str);
	m_radius = _ttoi(str);
	Invalidate();
}



void CMfcProjectDlg::OnEnChangeEditThickness()
{
	CString str;
	GetDlgItemText(IDC_EDIT_THICKNESS, str);
	m_thickness = _ttoi(str);
	Invalidate();
}



void CMfcProjectDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (size_t i = 0; i < m_points.size(); ++i)
	{
		int dx = m_points[i].x - point.x;
		int dy = m_points[i].y - point.y;
		if (dx * dx + dy * dy <= m_radius * m_radius)
		{
			m_draggingIndex = i;
			SetCapture();
			break;
		}
	}

	if (m_points.size() < 3 && m_draggingIndex == -1)
	{
		POINT pt = { point.x, point.y };
		m_points.push_back(pt); // 먼저 벡터에 추가

		if (m_pListBox) // m_pListBox가 nullptr인지 확인
		{
			CString str;
			str.Format(_T("(%d, %d)"), point.x, point.y);
			m_pListBox->AddString(str);
		}
	}

	Invalidate();
}



void CMfcProjectDlg::OnLbnSelchangeList()
{
}



void CMfcProjectDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_draggingIndex != -1)
	{
		m_points[m_draggingIndex] = point;

		if (m_pListBox && m_draggingIndex + 1 < (UINT)m_pListBox->GetCount())
		{
			CString str;
			str.Format(_T("(%d, %d)"), point.x, point.y);
			m_pListBox->DeleteString(m_draggingIndex + 1); 
			m_pListBox->InsertString(m_draggingIndex + 1, str);
			m_pListBox->SetCurSel(m_draggingIndex + 1);
		}

		Invalidate();
	}
}



void CMfcProjectDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_draggingIndex != -1)
	{
		m_draggingIndex = -1;
		ReleaseCapture();
		Invalidate();
	}
}



void CMfcProjectDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	DeleteCriticalSection(&m_cs);
}



LRESULT CMfcProjectDlg::OnUpdatePoints(WPARAM wParam, LPARAM lParam)
{
	std::vector<CPoint> pointsCopy;
	EnterCriticalSection(&m_cs);
	pointsCopy = m_points;
	LeaveCriticalSection(&m_cs);

	if (m_pListBox)
	{
		m_pListBox->ResetContent();
		m_pListBox->AddString(_T("좌표 목록"));

		for (const auto& pt : pointsCopy)
		{
			CString str;
			str.Format(_T("(%d, %d)"), pt.x, pt.y);
			m_pListBox->AddString(str);
		}
	}

	Invalidate();

	return 0;
}
