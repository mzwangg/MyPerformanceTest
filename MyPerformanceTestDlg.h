
// MyPerformanceTestDlg.h: 头文件
//

#pragma once


// CMyPerformanceTestDlg 对话框
class CMyPerformanceTestDlg : public CDialogEx
{
// 构造
public:
	CMyPerformanceTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYPERFORMANCETEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton* pBtn;
	double m_cpuScore;
	double m_gpuScore;
	double m_memoryScore;
	double m_distScore;
	double m_score;
	afx_msg void OnBnClickedButton();
	CString cpuType;
	CString osInfo;
	CString memorySize;
	CString memoryUseR;
	CString networkIndo;
	CString screenInfo;
	CString distSerial;
	CString distSize;
};
