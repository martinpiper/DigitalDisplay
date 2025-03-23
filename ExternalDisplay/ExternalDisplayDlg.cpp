
// ExternalDisplayDlg.cpp : implementation file
//

#include "pch.h"
#include <math.h>
#include "framework.h"
#include "ExternalDisplay.h"
#include "ExternalDisplayDlg.h"
#include "afxdialogex.h"
#include "RNXPSockets/Inc/XPSockets.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/StringUtils.h"
#include "Display.h"
#include "RNPlatform/Inc/SysTime.h"


#ifdef _DEBUG
#define new DEBUG_NEW
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
	ON_BN_CLICKED(IDC_BUTTON1, &CExternalDisplayDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// Allocate some memory for the "pretend screen" data
static int screenWidth = 0 , screenHeight = 0;
static int screenWidthSubPixel = 0;
static Display theDisplay;
static RNReplicaNet::SysTime theTime;

static RNReplicaNet::XPSocket listenSocket;
static HWND safeHWND = 0;
class NetworkHandler : public RNReplicaNet::ThreadClass , public RNReplicaNet::Thread
{
public:
	NetworkHandler()
	{

	}
	virtual ~NetworkHandler()
	{

	}

	virtual int ThreadEntry(void)
	{
		std::string receivedData;
		RNReplicaNet::XPSocket* currentConnection = 0;
		while (!GetTerminated())
		{
			RNReplicaNet::XPSocket* gotConnection = listenSocket.Accept();
			if (gotConnection)
			{
				if (currentConnection)
				{
					currentConnection->Close();
					delete currentConnection;
				}
				currentConnection = gotConnection;
			}

			while (!receivedData.empty())
			{
				size_t pos = receivedData.find_first_of("\r\n");
				if (std::string::npos != pos)
				{
					std::string line = receivedData.substr(0, pos);
					receivedData = receivedData.erase(0, pos+1);
					line = RNReplicaNet::TrimWhite(line);
					if (!line.empty())
					{
						if (theDisplay.parseMessage(line.c_str()))
						{
							if (theTime.FloatTime() > 0.01f)
							{
								theTime.Reset();
								::PostMessage(safeHWND, WM_USERRESPONSE, 0, 0);
							}
						}
					}
				}
				else
				{
					break; // No full data line yet
				}
			}

			if (currentConnection)
			{
				if (currentConnection->IsAlive())
				{
					char receiveBuffer[10240];
					int received = currentConnection->Recv(receiveBuffer , sizeof(receiveBuffer)-1);
					if (received > 0)
					{
						receiveBuffer[received] = '\0';
						receivedData += receiveBuffer;
						continue; // Quickly try to receive all data and any connections, without any sleeping
					}
					else if (received == XPSOCK_EERROR)
					{
						currentConnection->Close();
					}
				}
				else
				{
					delete currentConnection;
					currentConnection = 0;
				}
			}

			Sleep(1);
		}
		return 0;
	}
};
static NetworkHandler networkHandler;


void InitScreen(int width, int height, int subPixelX)
{
	screenWidthSubPixel = subPixelX;
	screenWidth = width;
	screenHeight = height;

	theDisplay.Resize(screenWidth * screenWidthSubPixel, screenHeight);

	listenSocket.Create();
	listenSocket.Listen(7654);

	networkHandler.Begin(&networkHandler);
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

	safeHWND = GetSafeHwnd();

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

		CRect fullRect , rect;
		GetClientRect(&fullRect);
		GetClientRect(&rect);
		if (rect.bottom < 48)
		{
			return;
		}
		rect.bottom -= 32;


		int* pixels = new int[sizeof(int) * theDisplay.getWidth() * theDisplay.getHeight()];
		const RGBTRIPLE* optSourcePixels = theDisplay.getPixels();
		int i = 0;
		for (int y = 0; y < theDisplay.getHeight(); y++)
		{
			for (int x = 0; x < theDisplay.getWidth(); x++)
			{
				pixels[i] = (optSourcePixels[i].rgbtRed << 16) | (optSourcePixels[i].rgbtGreen << 8) | (optSourcePixels[i].rgbtBlue << 0);
				i++;
			}
		}

		CBitmap bmp;
		// Creates a bitmap with data from the "screen" we have just calculated
		bmp.CreateBitmap(theDisplay.getWidth(), theDisplay.getHeight(), 1, 32, pixels);

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
			theDC.FillSolidRect(0, 32, fullRect.right, fullRect.bottom, 0);
			mFlagErase = false;
		}

		// Now blit the in memory DC to the window DC
		theDC.SetStretchBltMode(COLORONCOLOR);
		theDC.StretchBlt(0, 32, targetWidth, targetHeight, &dcMemory, 0, 0, screenWidth * screenWidthSubPixel, screenHeight, SRCCOPY);

		dcMemory.SelectObject(pOldBitmap);

		delete pixels;

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


void CExternalDisplayDlg::OnBnClickedButton1()
{
	theDisplay.CreateSnow();
	RedrawWindow();
}
