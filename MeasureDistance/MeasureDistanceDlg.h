
// MeasureDistanceDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <thread>
#include <atomic>


// CMeasureDistanceDlg dialog
class CMeasureDistanceDlg : public CDialogEx
{
// Construction
public:
	CMeasureDistanceDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEASUREDISTANCE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


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
	CMenu m_menu;

private:
	CListCtrl m_image_list;
	int m_mode = 0;
	CStatic m_pic_image;
	HBITMAP m_hBitmapPic = nullptr; // currently displayed bitmap (must be freed)
	CString dstFolder, imageName;
	CDC* pImgDC = nullptr;
	CRect orgRect;
	int drawWidth = 0;
	int drawHeight = 0;
	bool imageLoaded;
	CImage img;
	CString preTitle;

	bool m_firstPointSet = false;
	bool m_secondPointSet = false;

	CPoint m_point1;
	CPoint m_point2;

public:
	afx_msg void OnBnClickedRadioAuto();
	afx_msg void OnBnClickedRadioSemiauto();
	afx_msg void OnBnClickedRadioManual();
	afx_msg void OnMenuOpenFolder();
	afx_msg void OnBnClickedBtnStop();

public:
	afx_msg void OnBnClickedButtonRecog();
	
	// posted from worker thread to update UI for each processed item:
	afx_msg LRESULT OnRecogItemProcessed(WPARAM wParam, LPARAM lParam);
	// posted when worker finished
	afx_msg LRESULT OnRecogFinished(WPARAM wParam, LPARAM lParam);
	// add declaration in the public/afx_msg section
	afx_msg void OnLvnItemchangedListImageName(NMHDR* pNMHDR, LRESULT* pResult);

private:
	int m_currentIndex = -1;
	void DoRecog(int index = -1);
	void loadImage();


	// background worker
	std::thread m_worker;
	std::atomic<bool> m_processing{ false };
	std::atomic<bool> m_cancel{ false };
	
public:
	void OnPictureLButtonDown(CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void DrawMeasurement();
	afx_msg void OnDblclkListImageName(NMHDR* pNMHDR, LRESULT* pResult);
};
