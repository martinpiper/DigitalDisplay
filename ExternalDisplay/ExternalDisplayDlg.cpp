
// ExternalDisplayDlg.cpp : implementation file
//

#include "pch.h"
#include <math.h>
#include "framework.h"
#include "ExternalDisplay.h"
#include "ExternalDisplayDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// If M_PI doesn't exist then define it now
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


// CExternalDisplayDlg dialog



CExternalDisplayDlg::CExternalDisplayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXTERNALDISPLAY_DIALOG, pParent) , mFlagErase(true)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExternalDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CExternalDisplayDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_USERRESPONSE, &CExternalDisplayDlg::OnUserresponse)
END_MESSAGE_MAP()

// Allocate some memory for the "pretend screen" data
static void* screen = 0;
static int screenWidth = 0 , screenHeight = 0;


void InitScreen(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	screen = malloc(screenWidth * screenHeight * 4);
	int x, y;
	int* pixel = (int*)screen;
#if 0
	// Pretty pattern
	for (y = 0; y < screenHeight; y++)
	{
		// Defines a repeating sin wave for each y position
		int offset = (int)(256.0f * sin(M_PI * float((y) & 255) / 256.0f));

		// Store different pixel colours for each horizontal screen position
		for (x = 0; x < screenWidth; x++)
		{
			*pixel++ = RGB(
				(x + offset) & 255,
				(y + x) & 255,
				((((x) & 255) * ((y) & 65535)) >> 8) & 255
			);
		}
	}
#else
	// Snow
	for (y = 0; y < screenHeight; y++)
	{
		for (x = 0; x < screenWidth; x++)
		{
			if (rand() & 1)
			{
				*pixel++ = RGB(0,0,0);
			}
			else
			{
				*pixel++ = RGB(255, 255, 255);
			}
		}
	}
#endif
}


// CExternalDisplayDlg message handlers

BOOL CExternalDisplayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_NORMAL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CExternalDisplayDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExternalDisplayDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC theDC(this); // device context for painting

		CRect rect;
		GetClientRect(&rect);

		CBitmap bmp;
		// Creates a bitmap with data from the "screen" we have just calculated
		bmp.CreateBitmap(screenWidth, screenHeight, 1, 32, screen);

		// Create an in-memory DC compatible with the
		// display DC we're using to paint
		CDC dcMemory;
		// This creates an in memory DC of the same size as the one we need to draw
		dcMemory.CreateCompatibleDC(&theDC);

		// Select the bitmap into the in-memory DC
		// Basically, initialise the in memory DC with the contents of the bitmap
		CBitmap* pOldBitmap = dcMemory.SelectObject(&bmp);

		// Not really needed as the bitmap is created blank
		//	dcMemory.FillSolidRect(0,0,rect.right,rect.bottom,RGB(0,0,0));

#if 0
		CPen newPen(PS_SOLID, 1, RGB(0, 255, 0));
		CPen* pOldPen = dcMemory.SelectObject(&newPen);
		dcMemory.MoveTo(0, 0);
		dcMemory.LineTo(100, 100);
		dcMemory.SelectObject(pOldPen);

		dcMemory.SetTextColor(RGB(255, 255, 255));
		dcMemory.SetBkColor(RGB(0, 0, 0));
		dcMemory.SetBkMode(TRANSPARENT);
		dcMemory.TextOut(10, 10, L"Hello");
#endif
		float scaleX = float(rect.right) / float(screenWidth);
		float scaleY = float(rect.bottom) / float(screenHeight);
		float bestScale = min(scaleX, scaleY);
		int targetWidth = int(bestScale * screenWidth);
		int targetHeight = int(bestScale * screenHeight);

		if (mFlagErase)
		{
			theDC.FillSolidRect(0, 0, rect.right, rect.bottom, 0);
			mFlagErase = false;
		}

		// Now blit the in memory DC to the window DC
		theDC.SetStretchBltMode(COLORONCOLOR);
		theDC.StretchBlt(0, 0, targetWidth, targetHeight, &dcMemory, 0, 0, screenWidth, screenHeight, SRCCOPY);

		dcMemory.SelectObject(pOldBitmap);

		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CExternalDisplayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CExternalDisplayDlg::OnEraseBkgnd(CDC* pDC)
{
//	return CDialogEx::OnEraseBkgnd(pDC);
	return TRUE;
}


void CExternalDisplayDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	mFlagErase = true;
	RedrawWindow();

	// Test...
//	::PostMessage(GetSafeHwnd(), WM_USERRESPONSE,0,0);
}


afx_msg LRESULT CExternalDisplayDlg::OnUserresponse(WPARAM wParam, LPARAM lParam)
{
	// A trigger message from receiving digital signals to cause a redraw
	RedrawWindow();
	return 0;
}
