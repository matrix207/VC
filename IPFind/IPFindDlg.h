// IPFindDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "ARPThread.h"
#include <string>
using namespace std;

// CIPFindDlg dialog
class CIPFindDlg : public CDialog
{
// Construction
public:
	CIPFindDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IPFIND_DIALOG };

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
	afx_msg void OnBnClickedButtonStart();
	void InitListCtrl();
	bool CheckMACAddr(CString strMAC);
	void InsetCount(CString struser,CString dest);
	void InitAdapter();
	CComboBox m_Device;
	afx_msg void OnCbnSelchangeCmbDevice();

	string m_strFindIP;
	string m_strFindMask;

	void GetCurSelIPMask(char *ip, char *mask);
	afx_msg void OnBnClickedButtonCopyToClipboard();
	vector<st_ip_mac> vtIpMac;
};
