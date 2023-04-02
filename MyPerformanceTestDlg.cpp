#pragma once
// MyPerformanceTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MyPerformanceTest.h"
#include "MyPerformanceTestDlg.h"
#include "afxdialogex.h"
#include "CpuTest.hpp"
#include "MemoryTest.hpp"
#include "DistTest.hpp"
#include "Information.hpp"
extern "C" double gpuTest();

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMyPerformanceTestDlg 对话框



CMyPerformanceTestDlg::CMyPerformanceTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MYPERFORMANCETEST_DIALOG, pParent)
	, m_cpuScore(0)
	, m_gpuScore(0)
	, m_memoryScore(0)
	, m_distScore(0)
	, m_score(0)
{
	cpuType = CA2T(getCpuType().c_str());
	osInfo = CA2T(getOsInfo().c_str());
	memorySize = CA2T(getMemorySize().c_str());
	memoryUseR = CA2T(getMemoryUseR().c_str());
	networkIndo = CA2T(getNetworkInfo().c_str());
	screenInfo = CA2T(getScreen().c_str());
	distSerial = CA2T(getHardDiskSerial().c_str());
	distSize = CA2T(getDiskSpace().c_str());
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyPerformanceTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_cpuScore);
	DDX_Text(pDX, IDC_EDIT2, m_gpuScore);
	DDX_Text(pDX, IDC_EDIT3, m_memoryScore);
	DDX_Text(pDX, IDC_EDIT4, m_distScore);
	DDX_Text(pDX, IDC_EDIT5, m_score);
	DDX_Text(pDX, IDC_EDIT6, cpuType);
	DDX_Text(pDX, IDC_EDIT7, osInfo);
	DDX_Text(pDX, IDC_EDIT9, memorySize);
	DDX_Text(pDX, IDC_EDIT8, memoryUseR);
	DDX_Text(pDX, IDC_EDIT11, networkIndo);
	DDX_Text(pDX, IDC_EDIT10, screenInfo);
	DDX_Text(pDX, IDC_EDIT13, distSerial);
	DDX_Text(pDX, IDC_EDIT12, distSize);
}

BEGIN_MESSAGE_MAP(CMyPerformanceTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON, &CMyPerformanceTestDlg::OnBnClickedButton)
END_MESSAGE_MAP()


// CMyPerformanceTestDlg 消息处理程序

BOOL CMyPerformanceTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	pBtn = (CButton*)GetDlgItem(IDC_CHECK1);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMyPerformanceTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyPerformanceTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyPerformanceTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMyPerformanceTestDlg::OnBnClickedButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int state = pBtn->GetCheck();

	m_cpuScore = cpuTest();
	if (state == 1)m_gpuScore = gpuTest();
	else m_gpuScore = 0;
	m_memoryScore = memoryTest();
	m_distScore = distTest();
	if (state == 1) m_score = pow(m_cpuScore * m_gpuScore * m_memoryScore * m_distScore, 1.0 / 4);
	else m_score = pow(m_cpuScore * m_memoryScore * m_distScore, 1.0 / 3);

	//m_cpuScore = cpuTest();
	//m_gpuScore = gpuTest();
	//m_memoryScore = memoryTest();
	//m_distScore = distTest();
	//m_score = pow(m_cpuScore * m_gpuScore * m_memoryScore * m_distScore, 1.0 / 4);

	m_cpuScore = floor(m_cpuScore * 1000.000f + 0.5) / 1000.000f;
	m_gpuScore = floor(m_gpuScore * 1000.000f + 0.5) / 1000.000f;
	m_memoryScore = floor(m_memoryScore * 1000.000f + 0.5) / 1000.000f;
	m_distScore = floor(m_distScore * 1000.000f + 0.5) / 1000.000f;
	m_score = floor(m_score * 1000.000f + 0.5) / 1000.000f;

	UpdateData(false);
}