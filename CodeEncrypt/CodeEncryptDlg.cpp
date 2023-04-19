
// CodeEncryptDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CodeEncrypt.h"
#include "CodeEncryptDlg.h"
#include "afxdialogex.h"
#include <afx.h>

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


// CCodeEncryptDlg 对话框



CCodeEncryptDlg::CCodeEncryptDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CODEENCRYPT_DIALOG, pParent)
	, m_fileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"), pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_fileDlg.m_ofn.lpstrTitle = _T("选择文件");
}

void CCodeEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUT_EDIT, m_inFile);
	DDX_Text(pDX, IDC_OUTPUT_EDIT, m_outFile);
	DDX_Control(pDX, IDC_PASSWORD_EDIT, m_passwordCtrl);
}

BEGIN_MESSAGE_MAP(CCodeEncryptDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE_BTN, &CCodeEncryptDlg::OnBrowseBtnClicked)
	ON_BN_CLICKED(IDC_BROWSE_BTN2, &CCodeEncryptDlg::OnBrowseBtnClicked2)
	ON_BN_CLICKED(IDC_ENCRYPT_BTN, &CCodeEncryptDlg::OnBnClickedEncryptBtn)
	ON_BN_CLICKED(IDC_DECRYPT_BTN, &CCodeEncryptDlg::OnBnClickedDecryptBtn)
END_MESSAGE_MAP()


// CCodeEncryptDlg 消息处理程序

BOOL CCodeEncryptDlg::OnInitDialog()
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
	InitControls();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCodeEncryptDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCodeEncryptDlg::InitControls()
{
	// 设置密码框为密码输入模式
	m_passwordCtrl.SetPasswordChar('*');

	// 设置默认输出文件名
	CString ext;
	ext = m_fileDlg.GetFileExt();
	m_outFile.Format(_T("Encrypted%s"), ext);

	UpdateData(FALSE);
}

void CCodeEncryptDlg::OnPaint()
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
HCURSOR CCodeEncryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCodeEncryptDlg::OnBrowseBtnClicked()
{
	// 显示文件选择对话框
	if (m_fileDlg.DoModal() == IDOK) {
		// 将选择的文件路径显示在输入文件名编辑框中
		m_inFile = m_fileDlg.GetPathName();
		UpdateData(FALSE);

		// 获取文件所在文件夹路径
		CString folderPath = m_inFile.Left(m_inFile.ReverseFind('\\') + 1);
		// 根据选择的文件名生成默认的输出文件名
		CString ext;
		ext = m_fileDlg.GetFileExt();
		m_outFile.Format(_T("%s\Encrypted.%s"), folderPath, ext);
		UpdateData(FALSE);
	}
}

void CCodeEncryptDlg::OnBrowseBtnClicked2()
{
	// 显示文件选择对话框
	if (m_fileDlg.DoModal() == IDOK) {
		// 将选择的文件路径显示在输入文件名编辑框中
		m_outFile = m_fileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

bool FileExists(const CString& fileName) {
	CFileStatus status;
	return CFile::GetStatus(fileName, status);
}

void CCodeEncryptDlg::OnBnClickedEncryptBtn()
{
	// 从界面上获取输入文件名、输出文件名和密码
	UpdateData(TRUE);
	m_passwordCtrl.GetWindowText(m_password);
	std::string inFile = CT2A(m_inFile);
	std::string outFile = CT2A(m_outFile);
	std::string password = CT2A(m_password, CP_UTF8);

	if(!FileExists((CString)inFile.c_str()))
	{
		AfxMessageBox(_T("输入文件不存在。"));
		return;
	}
	if (inFile == outFile)
	{
		const int isSelect = AfxMessageBox(_T("输入和输出文件名一致，是否继续加密"), MB_OKCANCEL);
		if (isSelect == IDCANCEL)
			return;
	}

	// 检查密码是否为空
	if (password.empty()) {
		const int isSelect = AfxMessageBox(_T("密码当前为空，是否继续加密"),MB_OKCANCEL);
		if(isSelect == IDCANCEL)
			return;
	}
	// 执行加密操作
	if (encryptor.EncryptFile(inFile, outFile, password)) {
		AfxMessageBox(_T("加密已成功完成。"));
	}
	else {
		AfxMessageBox(_T("加密失败。"));
	}
}

void CCodeEncryptDlg::OnBnClickedDecryptBtn()
{
	// 从界面上获取输入文件名、输出文件名和密码
	UpdateData(TRUE);
	m_passwordCtrl.GetWindowText(m_password);
	std::string inFile = CT2A(m_inFile);
	std::string outFile = CT2A(m_outFile);
	std::string password = CT2A(m_password, CP_UTF8);

	if (!FileExists((CString)inFile.c_str()))
	{
		AfxMessageBox(_T("输入文件不存在。"));
		return;
	}

	if(inFile == outFile)
	{
		const int isSelect = AfxMessageBox(_T("输入和输出文件名一致，是否继续解密"), MB_OKCANCEL);
		if (isSelect == IDCANCEL)
			return;
	}
	//// 检查密码是否为空
	//if (password.empty()) {
	//	AfxMessageBox(_T("请输入密码。"));
	//	return;
	//}
	// 执行解密操作
	if (encryptor.DecryptFile(inFile, outFile, password)) {
		AfxMessageBox(_T("解密成功完成。"));
	}
	else {
		AfxMessageBox(_T("解密失败。"));
	}
}