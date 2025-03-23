
// ExternalDisplayDlg.h : header file
//

#pragma once

#define WM_USERRESPONSE WM_APP

void InitScreen(int width, int height, int subPixelX);

// CExternalDisplayDlg dialog
class CExternalDisplayDlg : public CDialogEx
{
// Construction
public:
	CExternalDisplayDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXTERNALDISPLAY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	bool mFlagErase;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnUserresponse(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton1();
};
