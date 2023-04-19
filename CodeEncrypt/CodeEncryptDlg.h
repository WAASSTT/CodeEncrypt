
// CodeEncryptDlg.h: 头文件
//

#pragma once
#include "Cryptographic.hpp"


// CCodeEncryptDlg 对话框
class CCodeEncryptDlg : public CDialogEx
{
// 构造
public:
	CCodeEncryptDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CODEENCRYPT_DIALOG };
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
	void OnBnClickedEncryptBtn();
	void OnBnClickedDecryptBtn();
	DECLARE_MESSAGE_MAP()
private:
	FileEncryptor encryptor;
	CString m_inFile;
	CString m_outFile;
	CString m_password;
	CFileDialog m_fileDlg;
	CEdit  m_passwordCtrl;

	void InitControls();
	void OnBrowseBtnClicked();
void OnBrowseBtnClicked2();
};
