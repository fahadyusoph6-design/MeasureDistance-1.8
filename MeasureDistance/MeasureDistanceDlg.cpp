// MeasureDistanceDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MeasureDistance.h"
#include "MeasureDistanceDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm>
#include <future>
#include <chrono>
#include <cmath>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};



void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMeasureDistanceDlg dialog



CMeasureDistanceDlg::CMeasureDistanceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEASUREDISTANCE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMeasureDistanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMAGE_NAME, m_image_list);
	DDX_Control(pDX, IDC_PIC_IMAGE, m_pic_image);
}

BEGIN_MESSAGE_MAP(CMeasureDistanceDlg, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_AUTO, &CMeasureDistanceDlg::OnBnClickedRadioAuto)
	ON_BN_CLICKED(IDC_RADIO_SEMIAUTO, &CMeasureDistanceDlg::OnBnClickedRadioSemiauto)
	ON_BN_CLICKED(IDC_RADIO_MANUAL, &CMeasureDistanceDlg::OnBnClickedRadioManual)
	ON_COMMAND(ID_MENU_OPEN_FOLDER, &CMeasureDistanceDlg::OnMenuOpenFolder)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_IMAGE_NAME, &CMeasureDistanceDlg::OnLvnItemchangedListImageName)
	ON_BN_CLICKED(IDC_BUTTON_RECOG, &CMeasureDistanceDlg::OnBnClickedButtonRecog)
	ON_MESSAGE(WM_APP + 1, &CMeasureDistanceDlg::OnRecogItemProcessed)
	ON_MESSAGE(WM_APP + 2, &CMeasureDistanceDlg::OnRecogFinished)
	ON_BN_CLICKED(IDC_BTN_STOP, &CMeasureDistanceDlg::OnBnClickedBtnStop)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_IMAGE_NAME, &CMeasureDistanceDlg::OnDblclkListImageName)
END_MESSAGE_MAP()


// CMeasureDistanceDlg message handlers

BOOL CMeasureDistanceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

	// TODO: ÔÚ´ËÌí¼Ó¶îÍâµÄ³õÊ¼»¯´úÂë
	m_menu.LoadMenuW(IDR_MENU);
	SetMenu(&m_menu);
	// radio
	((CButton*)GetDlgItem(IDC_RADIO_AUTO))->SetCheck(TRUE);
	CWnd* pWnd = GetDlgItem(IDC_PIC_IMAGE);
	pImgDC = pWnd->GetDC();
	pImgDC->SetStretchBltMode(COLORONCOLOR);
	pWnd->GetWindowRect(orgRect);
	drawWidth = orgRect.Width();
	drawHeight = orgRect.Height();


	//HMODULE module = LoadLibrary(_T("DistanceDll.dll"));
	//if (module == NULL)
	//{
	//	AfxMessageBox(_T("failed"));
	//}

	//JiaoShuiNihe Recognition = (JiaoShuiNihe)GetProcAddress(module, "Recognition");
	m_image_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_image_list.InsertColumn(0, _T("Filename"), LVCFMT_LEFT, 220);
	m_image_list.InsertColumn(1, _T("Pixels"), LVCFMT_LEFT, 60);
	m_image_list.InsertColumn(2, _T("Distance"), LVCFMT_LEFT, 60);


	return TRUE; 
}

void CMeasureDistanceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMeasureDistanceDlg::OnPaint()
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

		if (m_firstPointSet)
		{
			CBrush brush(RGB(255, 0, 0));
			CBrush* pOldBrush = dc.SelectObject(&brush);

			int radius = 6;

			dc.Ellipse(
				m_point1.x - radius,
				m_point1.y - radius,
				m_point1.x + radius,
				m_point1.y + radius
			);

			dc.SelectObject(pOldBrush);
		}


		if (m_secondPointSet)
		{
			CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
			CPen* pOldPen = dc.SelectObject(&pen);

			// Vertical line
			dc.MoveTo(m_point1.x, m_point1.y);
			dc.LineTo(m_point2.x, m_point2.y);

			dc.SelectObject(pOldPen);


			// Second dot
			CBrush brush(RGB(255, 0, 0));
			CBrush* pOldBrush = dc.SelectObject(&brush);

			int radius = 6;

			dc.Ellipse(
				m_point2.x - radius,
				m_point2.y - radius,
				m_point2.x + radius,
				m_point2.y + radius
			);

			dc.SelectObject(pOldBrush);
		}
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMeasureDistanceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMeasureDistanceDlg::OnLvnItemchangedListImageName(NMHDR* pNMHDR, LRESULT* pResult)
{
	// get selected item index
	POSITION pos = m_image_list.GetFirstSelectedItemPosition();
	
	if (!pos) {
		*pResult = 0;
		return;
	}
	int idx = (pos) ? m_image_list.GetNextSelectedItem(pos) : -1;
	m_currentIndex = idx;
	//int idx = m_image_list.GetNextSelectedItem(pos);

	// read filename from column 0
	CString name = m_image_list.GetItemText(idx, 0);
	if (name.IsEmpty()) {
		*pResult = 0;
		return;
	}

	SetWindowText(preTitle = _T("识别 [") + name + _T("]"));
	imageName = dstFolder + name;
	imageLoaded = true;
	
	*pResult = 0;

	if (m_mode == 0)
		DoRecog(idx);
	else {
		if (!imageLoaded) return;
		if (!img.IsNull())
			img.Destroy();
		HRESULT hr = img.Load(imageName);

		m_firstPointSet = false;
		m_secondPointSet = false;

		m_point1 = CPoint(0, 0);
		m_point2 = CPoint(0, 0);

		// Draw image
		img.Draw(
			pImgDC->GetSafeHdc(),
			0,
			0,
			drawWidth,
			drawHeight
		);
	}
}
void CMeasureDistanceDlg::OnBnClickedRadioAuto()
{
	// TODO: Add your control notification handler code here
	m_mode = 0;
	//DoRecog(idx);
}

void CMeasureDistanceDlg::OnBnClickedRadioSemiauto()
{
	// TODO: Add your control notification handler code here
	m_mode = 1;
	//DoRecog();
}

void CMeasureDistanceDlg::OnBnClickedRadioManual()
{
	// TODO: Add your control notification handler code here
	m_mode = 2;
	//DoRecog();
}

void CMeasureDistanceDlg::OnMenuOpenFolder()
{
	// TODO: ÔÚ´ËÌí¼ÓÃüÁî´¦Àí³ÌÐò´úÂë
	CFolderPickerDialog dlg;
	if (dlg.DoModal() == IDOK) {
		int nMaxExtent = 0;
		m_image_list.DeleteAllItems();
    	dstFolder = dlg.GetFolderPath() + _T("\\");
		CFileFind filefind;
		BOOL bworking = filefind.FindFile(dstFolder + _T("*.bmp"));
		int index = 0;
		while (bworking) {
			bworking = filefind.FindNextFile();
			if (!filefind.IsDots() && !filefind.IsDirectory()) {
				CString filename = filefind.GetFileName();
				int pos = m_image_list.InsertItem(index, filename);   // column 0 = filename
				m_image_list.SetItemData(pos, static_cast<DWORD_PTR>(index)); // stable id
				m_image_list.SetItemText(pos, 1, _T("")); // Pixels
				m_image_list.SetItemText(pos, 2, _T("")); // Distance
				++index;
				CSize cs = pImgDC->GetTextExtent(filename);
				if (cs.cx > nMaxExtent) nMaxExtent = cs.cx;
			}
		}
		if (m_image_list.GetItemCount() > 0) {
			m_image_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		}
		
		imageLoaded = false;
		Invalidate();
		SetWindowText(_T("识别"));
	} 
}
 


typedef int(*JiaoShuiNihe)(char* imageName, int mode, double* dimensions);

HMODULE module = LoadLibrary(_T("DistanceDll.dll"));
JiaoShuiNihe Recognition = (JiaoShuiNihe)GetProcAddress(module, "Recognition");

void CMeasureDistanceDlg::DoRecog(int index)
{
	if (m_mode == 0) {
		if (!imageLoaded) return;
		bool recRes = true;

		// Determine originalIndex for UI mapping (use -1 if not applicable)
		int originalIndex = -1;
		// If using CListCtrl and a selected item:
		POSITION pos = m_image_list.GetFirstSelectedItemPosition();
		if (pos) {
			originalIndex = m_image_list.GetNextSelectedItem(pos); // row index
		}

		CT2A szConverted(imageName, CP_UTF8);
		char* filename = const_cast<char*>(static_cast<LPCSTR>(szConverted));

		double dimensions[5] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
		int result_code = Recognition(filename, m_mode, dimensions);

		// copy results to heap and post to UI
		double* pDims = new double[5];
		for (int k = 0; k < 5; ++k) pDims[k] = dimensions[k];

		PostMessage(WM_APP + 1,
			static_cast<WPARAM>(originalIndex),
			reinterpret_cast<LPARAM>(pDims));

		CString pixelText, distanceText;
		if (recRes) {
			pixelText.Format(_T("%.3f"), dimensions[3]);
			//m_edtLeftAngle.SetWindowText(pixelText);
			m_image_list.SetItemText(index, 1, pixelText);

			distanceText.Format(_T("%.3f"), dimensions[4]);
			//m_edtRightAngle.SetWindowText(angleText);
			m_image_list.SetItemText(index, 2, distanceText);
		}
		loadImage();
	}
	else {

		POSITION pos = m_image_list.GetFirstSelectedItemPosition();
		int idx = (pos) ? m_image_list.GetNextSelectedItem(pos) : -1;
		CString pixelText, distanceText;

		// Calculate horizontal pixel distance.
		double distancePx = abs(m_point2.y - m_point1.y);
		double PixToMm = 0.008951113022; //pixel per MM
		double kOffsetMm = 0.004808987656; // the offset of the camera in MM
		double distanceMm = PixToMm * distancePx + kOffsetMm;
		/*CString str;
		str.Format(
			_T("First point  : (%d, %d)\n")
			_T("Second point : (%d, %d)\n\n")
			_T("Pixel distance = %d pixels"),
			m_point1.x,
			m_point1.y,
			m_point2.x,
			m_point2.y,
			distancePx
		);*/
		pixelText.Format(_T("%.3f"), distancePx);
		//m_edtLeftAngle.SetWindowText(pixelText);
		m_image_list.SetItemText(idx, 1, pixelText);

		distanceText.Format(_T("%.3f"), distanceMm);
		//m_edtRightAngle.SetWindowText(angleText);
		m_image_list.SetItemText(idx, 2, distanceText);
	}
	
}

void CMeasureDistanceDlg::loadImage()
{
	if (!imageLoaded) return;
	img.Destroy();
	img.Load(_T("output1.png"));
	img.Draw(*pImgDC, 0, 0, drawWidth, drawHeight);

	//const CString outPath = _T("output1.png"); // or construct path if per-file outputs used
	//CImage tmp;
	//HRESULT hr = tmp.Load(outPath);
	//if (FAILED(hr)) {
	//	// failed to load — nothing to show
	//	return;

	//}

	//// detach HBITMAP that CImage created
	//HBITMAP hBmp = (HBITMAP)tmp.Detach();
	//if (!hBmp) return;

	//// Replace previous bitmap (free it)
	//if (m_hBitmapPic) {
	//	::DeleteObject(m_hBitmapPic);
	//	m_hBitmapPic = nullptr;

	//}

	//// Set the new bitmap on the static control
	//m_pic_image.SetBitmap(hBmp);
	//m_hBitmapPic = hBmp;
	//m_pic_image.Invalidate();
}
void CMeasureDistanceDlg::OnBnClickedButtonRecog()
{

	if (m_mode != 0) {
		// single image flow: call existing DoRecog
		DoRecog();
		return;
	}
	int count = m_image_list.GetItemCount();
	if (count <= 0) return;

	std::vector<CString> files;
	files.reserve(count);
	for (int i = 0; i < count; ++i) {
		CString name = m_image_list.GetItemText(i, 0); // column 0 = filename
		files.push_back(name);
	}

	// ensure Recognition function is present
	/*if (Recognition == nullptr) {
		AfxMessageBox(_T("Recognition function not available (DLL not loaded or export missing)."));
		return;
	}*/

	

	// disable/enable UI
	GetDlgItem(IDC_BUTTON_RECOG)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	m_cancel.store(false);
	m_processing.store(true);


	// capture folder and local copy of function pointer
	CString folder = dstFolder;
	auto localRecog = Recognition;
	int localMode = m_mode;

				
	// detach worker thread so UI remains responsive
	//m_worker = std::thread([this, folder, files = std::move(files), localRecog, localMode]() mutable {
	//	int count = m_image_list.GetItemCount();
	//	CString dbg;
	//	dbg.Format(_T("Starting worker: count=%d, first=%s"), count,
	//		(count > 0) ? m_image_list.GetItemText(0, 0) : _T("(none)"));
	//	OutputDebugStringW(dbg);
	//	for (size_t i = 0; i < files.size(); ++i) {
	//		if (m_cancel.load()) break;

	//		// originalIndex = row position in snapshot (stable id)
	//		int originalIndex = static_cast<int>(i);

	//		CString fullPath = folder + files[i];
	//		CT2A szConverted(fullPath, CP_UTF8);
	//		char* filename = const_cast<char*>((LPCSTR)szConverted);

	//		double dimensions[5] = { 0.0 };
	//		int rc = localRecog(filename, localMode, dimensions);

	//		// copy results to heap for PostMessage
	//		double* pDims = new double[5];
	//		for (int k = 0; k < 5; ++k) pDims[k] = dimensions[k];

	//		PostMessage(WM_APP + 1, static_cast<WPARAM>(originalIndex), reinterpret_cast<LPARAM>(pDims));

	//		if (m_cancel.load()) break;
	//		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//	}

	//	PostMessage(WM_APP + 2, 0, 0);
	// });
	// 
	// 
	m_worker = std::thread([this, folder, files = std::move(files), localRecog, localMode]() mutable {
		OutputDebugStringW(L"[Worker] started\n");
		for (size_t i = 0; i < files.size(); ++i) {
			if (m_cancel.load()) {
				OutputDebugStringW(L"[Worker] cancelled\n");
				break;
			}

			int originalIndex = static_cast<int>(i); // stable id for this file
			CString fullPath = folder + files[i];
			CT2A szConverted(fullPath, CP_UTF8);
			char* filename = const_cast<char*>((LPCSTR)szConverted);

			double dimensions[5] = { 0.0 };
			int rc = -999;
			try {
				rc = localRecog(filename, localMode, dimensions);
			}
			catch (...) {
				OutputDebugStringW(L"[Worker] Exception from Recognition\n");
				rc = -999;
			}

			// Debug log: iteration, rc and dims
			{
				char buf[512];
				sprintf_s(buf, "[Worker] i=%zu origIdx=%d rc=%d dims=%f,%f,%f,%f,%f\n",
					i, originalIndex, rc,
					dimensions[0], dimensions[1], dimensions[2], dimensions[3], dimensions[4]);
				OutputDebugStringA(buf);
			}

			// copy results and post to UI (even on error) so UI can inspect
			double* pDims = new double[5];
			for (int k = 0; k < 5; ++k) pDims[k] = dimensions[k];

			// Post the stable id = originalIndex
			this->PostMessage(WM_APP + 1, static_cast<WPARAM>(originalIndex), reinterpret_cast<LPARAM>(pDims));

			// give UI a moment
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		OutputDebugStringW(L"[Worker] finished - posting finished message\n");
		this->PostMessage(WM_APP + 2, 0, 0); // finished
		});
	// detach so it runs independently; we track m_processing flag to know state
	m_worker.detach();
	// Start batch recognition when in Auto mode
	/*	if (m_processing.load()) {
		AfxMessageBox(_T("Recognition already in progress."));
		return;
	}*/

}

// handler signature already declared and mapped with ON_MESSAGE
LRESULT CMeasureDistanceDlg::OnRecogItemProcessed(WPARAM wParam, LPARAM lParam)
{
	int stableId = static_cast<int>(wParam);
	double* p = reinterpret_cast<double*>(lParam);
	if (!p) return 0;

	// Find the row that has item data == origIndex
	int rowCount = m_image_list.GetItemCount();
	int row = -1;
	for (int r = 0; r < rowCount; ++r) {
		if (static_cast<int>(m_image_list.GetItemData(r)) == stableId) {
			row = r;
			break;
		}
	}
	if (row >= 0) {
		// Format values as strings — change indices to match your protocol
		CString pixelsStr, distStr;
		// example: p[2] = pixel count, p[3] = distance in mm
		pixelsStr.Format(_T("%d"), static_cast<int>(std::round(p[3])));
		distStr.Format(_T("%.2f"), p[4]);

		m_image_list.SetItemText(row, 1, pixelsStr); // Pixels column
		m_image_list.SetItemText(row, 2, distStr);   // Distance column

		// Optional: autosize columns so values are visible
		m_image_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		m_image_list.SetColumnWidth(1, LVSCW_AUTOSIZE);
		m_image_list.SetColumnWidth(2, LVSCW_AUTOSIZE);

		CString filename = m_image_list.GetItemText(row, 0);
		// example: the DLL currently writes "output1.png" — just load it
		// example: the DLL currently writes "output1.png" — just load it
		imageName = dstFolder + filename; // keep logical mapping if needed
		imageLoaded = true;
		loadImage();
	}
	else {
		// fallback: if you didn't call SetItemData, try using stableId as row
		if (stableId >= 0 && stableId < rowCount) {
			CString pixelsStr, distStr;
			pixelsStr.Format(_T("%d"), static_cast<int>(std::lround(p[3])));
			distStr.Format(_T("%.2f"), p[4]);
			m_image_list.SetItemText(stableId, 1, pixelsStr);
			m_image_list.SetItemText(stableId, 2, distStr);
		}
	}

	delete[] p; // free the heap memory the worker allocated
	return 0;
}
// OnRecogFinished: join worker and re-enable UI (excerpt)
LRESULT CMeasureDistanceDlg::OnRecogFinished(WPARAM, LPARAM)
{
	m_processing.store(false);
	GetDlgItem(IDC_BUTTON_RECOG)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);

	if (m_worker.joinable()) {
		try { m_worker.join(); }
		catch (...) {}
	}

	if (m_cancel.load()) {
		AfxMessageBox(_T("Batch recognition stopped by user."));
	}
	else {
		AfxMessageBox(_T("Batch recognition finished."));
	}
	return 0;
}
void CMeasureDistanceDlg::OnBnClickedBtnStop()
{
	// TODO: Add your control notification handler code here
	//AfxMessageBox(_T("Stop clicked")); // quick debug
	if (!m_processing.load()) return;
	// signal cancel; worker will exit soon and post finished
	m_cancel.store(true);
	// disable stop button to show request accepted
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
}



BOOL CMeasureDistanceDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_LBUTTONDOWN)
	{
			CPoint screenPoint;

			::GetCursorPos(&screenPoint);

			CRect picRect;
			m_pic_image.GetWindowRect(&picRect);

			if (picRect.PtInRect(screenPoint))
			{
				CPoint point = screenPoint;

				m_pic_image.ScreenToClient(&point);

				OnPictureLButtonDown(point);

				return TRUE;
			}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
void CMeasureDistanceDlg::OnPictureLButtonDown(CPoint point)
{
	if (!m_firstPointSet)
	{
		m_point1 = point;

		m_firstPointSet = true;
		m_secondPointSet = false;

		// The line will initially have zero length.
		m_point2 = m_point1;

		DrawMeasurement();

		return;
	}
	if (!m_secondPointSet)
	{
		m_point2 = point;
		m_secondPointSet = true;

		// Calculate horizontal pixel distance.
		int distancePx =
			abs(m_point2.y - m_point1.y);

		/*CString str;
		str.Format(
			_T("First point  : (%d, %d)\n")
			_T("Second point : (%d, %d)\n\n")
			_T("Pixel distance = %d pixels"),
			m_point1.x,
			m_point1.y,
			m_point2.x,
			m_point2.y,
			distancePx
		);*/

		//AfxMessageBox(str);

		DrawMeasurement();

		return;
	}
}
void CMeasureDistanceDlg::DrawMeasurement()
{
	if (pImgDC == nullptr)
		return;

	if (img.IsNull())
		return;


	// =========================================
	// Redraw original image
	// =========================================

	img.Draw(
		pImgDC->GetSafeHdc(),
		0,
		0,
		drawWidth,
		drawHeight
	);
	if (m_firstPointSet)
	{
		CBrush redBrush(
			RGB(255, 0, 0)
		);

		CBrush* pOldBrush =
			pImgDC->SelectObject(&redBrush);

		const int radius = 5;

		pImgDC->Ellipse(
			m_point1.x - radius,
			m_point1.y - radius,
			m_point1.x + radius,
			m_point1.y + radius
		);

		pImgDC->SelectObject(pOldBrush);
	}
	if (m_firstPointSet)
	{
		CPen redPen(
			PS_DOT,
			1,
			RGB(255, 0, 0)
		);

		CPen* pOldPen =
			pImgDC->SelectObject(&redPen);


		// Start at the TOP of the Picture Control
		// at the X coordinate of the first point.
		pImgDC->MoveTo(
			m_point1.x,
			m_point1.y
		);


		// Draw to the BOTTOM of the Picture Control
		// at the same X coordinate.
		pImgDC->LineTo(
			m_point1.x,
			drawHeight
		);


		pImgDC->SelectObject(pOldPen);
	}
	if (m_secondPointSet)
	{
		CBrush redBrush(
			RGB(255, 0, 0)
		);

		CBrush* pOldBrush =
			pImgDC->SelectObject(&redBrush);

		const int radius = 5;

		pImgDC->Ellipse(
			m_point2.x - radius,
			m_point2.y - radius,
			m_point2.x + radius,
			m_point2.y + radius
		);

		pImgDC->SelectObject(pOldBrush);
	}
}

void CMeasureDistanceDlg::OnDblclkListImageName(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	POSITION pos = m_image_list.GetFirstSelectedItemPosition();

	if (!pos) {
		*pResult = 0;
		return;
	}
	int idx = (pos) ? m_image_list.GetNextSelectedItem(pos) : -1;
	m_currentIndex = idx;
	//int idx = m_image_list.GetNextSelectedItem(pos);

	// read filename from column 0
	CString name = m_image_list.GetItemText(idx, 0);
	if (name.IsEmpty()) {
		*pResult = 0;
		return;
	}

	SetWindowText(preTitle = _T("识别 [") + name + _T("]"));
	imageName = dstFolder + name;
	imageLoaded = true;

	*pResult = 0;

	if (m_mode == 0)
		DoRecog(idx);
	else {
		if (!imageLoaded) return;
		if (!img.IsNull())
			img.Destroy();
		HRESULT hr = img.Load(imageName);

		m_firstPointSet = false;
		m_secondPointSet = false;

		m_point1 = CPoint(0, 0);
		m_point2 = CPoint(0, 0);

		// Draw image
		img.Draw(
			pImgDC->GetSafeHdc(),
			0,
			0,
			drawWidth,
			drawHeight
		);
	}

	*pResult = 0;
}
