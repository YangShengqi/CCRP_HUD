
// HUDDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HUD.h"
#include "HUDDlg.h"
#include "afxdialogex.h"
#include <cstdlib>
#include <algorithm>
#include "CcrpAim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI 3.14159265358979323846
int ttg;
double released;//到达投放点距离
int minrelrange = 2000;//最小投放距离
CTime ct, ct2;
BOOL flag4 = TRUE, flag7 = TRUE, flag9 = TRUE;//4、7、9号定时器的启动标志（TRUE：启动 FALSE：关闭）
double omega = 0.1;		//飞机转弯角速度
double deltat = 0.01;		//瞄准过程步长
double h = 0.01;			//龙格库塔仿真时间步长
int nSel;//combobox当前序列号
int nFlash=1;//闪烁间隔
// CHUDDlg 对话框


CHUDDlg::CHUDDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HUD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHUDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODE, m_combo);
}

BEGIN_MESSAGE_MAP(CHUDDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_START, &CHUDDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CHUDDlg::OnBnClickedStop)
    ON_BN_CLICKED(IDC_UPDATE, &CHUDDlg::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_LOAD, &CHUDDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_DEFAULTNAV, &CHUDDlg::OnBnClickedDefaultnav)
	ON_BN_CLICKED(IDC_DEFAULTCCRP, &CHUDDlg::OnBnClickedDefaultccrp)
END_MESSAGE_MAP()


// CHUDDlg 消息处理程序

BOOL CHUDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);		// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_combo.AddString(_T("导航"));
	m_combo.AddString(_T("CCRP瞄准"));
	m_combo.SetCurSel(0);
	nSel = m_combo.GetCurSel();
	switch (nSel)
	{
	case 0:
	{
		SetDlgItemText(IDC_CX, _T("0"));
		SetDlgItemText(IDC_CY, _T("0"));
		SetDlgItemText(IDC_HEIGHT, _T("1000"));
		SetDlgItemText(IDC_SPX, _T("1000"));
		SetDlgItemText(IDC_SPY, _T("2000"));
		SetDlgItemText(IDC_SPZ, _T("2000"));
		SetDlgItemText(IDC_AIRSPEED, _T("200"));
		SetDlgItemText(IDC_HEAD, _T("0"));
		SetDlgItemText(IDC_PITCH, _T("0"));
		SetDlgItemText(IDC_ROLL, _T("0"));
		SetDlgItemText(IDC_WINDSPEED, _T("10"));
		SetDlgItemText(IDC_WINDHEAD, _T("20"));
		SetDlgItemText(IDC_GHEIGHT, _T("200"));
		break;
	}
	case 1:
	{
		SetDlgItemText(IDC_CX, _T("0"));
		SetDlgItemText(IDC_CY, _T("0"));
		SetDlgItemText(IDC_HEIGHT, _T("1000"));
		SetDlgItemText(IDC_SPX, _T("5000"));
		SetDlgItemText(IDC_SPY, _T("5000"));
		SetDlgItemText(IDC_SPZ, _T("1000"));
		SetDlgItemText(IDC_AIRSPEED, _T("200"));
		SetDlgItemText(IDC_HEAD, _T("0"));
		SetDlgItemText(IDC_PITCH, _T("0"));
		SetDlgItemText(IDC_ROLL, _T("0"));
		SetDlgItemText(IDC_WINDSPEED, _T("10"));
		SetDlgItemText(IDC_WINDHEAD, _T("20"));
		SetDlgItemText(IDC_GHEIGHT, _T("200"));
		break;
	}
	default:
		break;
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHUDDlg::OnPaint()
{
	CDC *pDC = GetDlgItem(IDC_DRAW)->GetDC();
	CBrush newBrush;              //黑色背景
	newBrush.CreateSolidBrush(RGB(0, 0, 0));
	CBrush *pOldBrush = pDC->SelectObject(&newBrush);
	pDC->FillRect(CRect(0, 0, 700, 700), &newBrush);
	newBrush.DeleteObject();

	CPen newPen;   //白框
	newPen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	CPen *pOldPen = pDC->SelectObject(&newPen);
	pDC->Arc(55, 50, 135, 130, 95, 50, 55, 90);
	pDC->MoveTo(55, 90);
	pDC->LineTo(55, 610);
	pDC->Arc(55, 570, 135, 650, 55, 610, 95, 650);
	pDC->Arc(575, 50, 655, 130, 655, 90, 615, 50);
	pDC->MoveTo(655, 90);
	pDC->LineTo(655, 610);
	pDC->Arc(575, 570, 655, 650, 615, 650, 655, 610);
	newPen.DeleteObject();

	newPen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));    //空速矢量
	pDC->SelectObject(&newPen);
	pDC->Ellipse(345, 340, 365, 360);
	pDC->MoveTo(355, 340);
	pDC->LineTo(355, 330);
	pDC->MoveTo(345, 350);
	pDC->LineTo(335, 350);
	pDC->MoveTo(365, 350);
	pDC->LineTo(375, 350);

	pDC->MoveTo(355, 205);      //飞机纵轴  
	pDC->LineTo(347, 220);
	pDC->MoveTo(355, 205);
	pDC->LineTo(363, 220);
	pDC->MoveTo(347, 220);
	pDC->LineTo(339, 205);
	pDC->MoveTo(363, 220);
	pDC->LineTo(371, 205);
	pDC->MoveTo(339, 205);
	pDC->LineTo(325, 205);
	pDC->MoveTo(371, 205);
	pDC->LineTo(385, 205);

	switch (nSel)
	{
	case 0:
	{
		m_vel = 200;
		m_head = 0;
		m_roll = 0;
		m_pitch = 0;
		m_cx = 0;
		m_cy = 0;
		m_heig = 1000;
		m_spx = 1000;
		m_spy = 2000;
		m_spz = 2000;
		m_tardt = sqrt(pow((m_spy - m_cy), 2) + pow((m_spx - m_cx), 2));
		m_dehd = deheadCompute(m_cx, m_cy, m_spx, m_spy);
		m_grdt = sqrt(pow((m_spy - m_cy), 2) + pow((m_spx - m_cx), 2));
		m_ttg = max(m_grdt / m_vel, ceil(abs(m_heig - m_spz) / 100));
		ttg = m_ttg;
		ct = CTime::GetCurrentTime();

		velDraw(m_vel, pDC);//速度标尺
		heightDraw(m_heig, pDC);//高度标尺  
		int turn = headDraw(m_head, m_dehd, pDC);//航向角标尺
		pitchDraw(m_roll, m_pitch, 355, 350, pDC);//俯仰角标尺
		rollDraw(355, 350, pDC);  //横滚角标尺
		trianDraw(m_roll, 355, 350, pDC);
		stpInfoDraw(m_grdt, m_spz, pDC);
		timeDraw(m_ttg, pDC);
		targetDraw(turn, m_heig, m_cx, m_cy, m_spx, m_spy, m_spz, m_grdt, m_head, m_dehd, pDC);
		modeFalltimeDraw(nSel, m_falltime, pDC);
		break;
	}
	case 1:
	{
		m_vel = 200;
		m_head = 0;
		m_roll = 0;
		m_pitch = 0;
		m_cx = 0;
		m_cy = 0;
		m_heig = 1000;
		m_spx = 5000;
		m_spy = 5000;
		m_spz = 1000;
		m_uwind = 10;
		m_ewind = 20;
		m_tardt = sqrt(pow((m_spy - m_cy), 2) + pow((m_spx - m_cx), 2));
		ct = CTime::GetCurrentTime();
		double K = m_head;
		CcrpAim ccrp;
		ccrp.CCRP(m_heig, m_vel, omega, K, m_uwind, m_ewind, deltat, h, m_spx, m_spy, m_cx, m_cy);
		int nAimidx = ccrp.output_aim();
		int nCount = ccrp.output_index();
		double xrelease = ccrp.output_xbomb(nAimidx);
		double yrelease = ccrp.output_zbomb(nAimidx);
		m_dehd = K;
		m_falltime = ccrp.output_stdtime();
		m_grdt = sqrt(pow((yrelease - m_cy), 2) + pow((xrelease - m_cx), 2));
		released = m_grdt;
		m_ttg = max(m_grdt / m_vel, ceil(abs(m_heig - m_spz) / 100));
		ttg = m_ttg;
		
		velDraw(m_vel, pDC);//速度标尺
		heightDraw(m_heig, pDC);//高度标尺  
		int turn = headDraw(m_head, m_dehd, pDC);//航向角标尺
		pitchDraw(m_roll, m_pitch, 355, 350, pDC);//俯仰角标尺
		stpInfoDraw(m_grdt, m_spz, pDC);
		timeDraw(m_ttg, pDC);
		reticleDraw(355, 550, 60, m_tardt - released + m_grdt, minrelrange, m_roll, pDC);
		aslDraw(m_vel, m_roll, m_ttg, turn, m_head, m_dehd, m_grdt, pDC);
		modeFalltimeDraw(nSel, m_falltime, pDC);
		break;
	}
	}
	
	CDialogEx::OnPaint();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHUDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHUDDlg::dbVerLineDraw(double x, double y, CDC* pDC)
{
	pDC->MoveTo(x - 2, y + 5);
	pDC->LineTo(x - 2, y + 17);
	pDC->MoveTo(x + 2, y + 5);
	pDC->LineTo(x + 2, y + 17);
}
void CHUDDlg::lArrowDraw(double x, double y, CDC* pDC)
{
	pDC->MoveTo(x, y);
	pDC->LineTo(x + 5, y - 5);
	pDC->MoveTo(x, y);
	pDC->LineTo(x + 5, y + 5);
	pDC->MoveTo(x, y);
	pDC->LineTo(x + 15, y);
}
void CHUDDlg::rArrowDraw(double x, double y, CDC* pDC)
{
	pDC->MoveTo(x, y);
	pDC->LineTo(x - 4.5, y - 4.5);
	pDC->MoveTo(x, y);
	pDC->LineTo(x - 4.5, y + 4.5);
	pDC->MoveTo(x, y);
	pDC->LineTo(x - 13, y);
}
int CHUDDlg::valTransfer(int init)
{
	int result = init;
	if (abs(init) % 360 == 0)  //航向角转化为[0，360)范围
		result = 0;
	else if (init >= 360)
		result = init % 360;
	else if (init < 0)
		result = 360 - abs(init) % 360;
	return result;
}
CString CHUDDlg::timeTransfer(CTime time, int increment)
{
	int second = time.GetSecond();
	int minute = time.GetMinute(), carrym = 0;
	int hour = time.GetHour(), carryh = 0;
	second = second + increment;
	if (second >= 60)
	{
		second = second - 60;
		carrym++;
	}
	minute = minute + carrym;
	if (minute >= 60)
	{
		minute = minute - 60;
		carryh++;
	}
	hour = hour + carryh;
	if (hour >= 24)
		hour = hour - 24;
	CTime trantime(2018, 1, 1, hour, minute, second);
	CString str = trantime.Format(_T("%H:%M:%S"));
	return str;
}
double CHUDDlg::deheadCompute(int cux, int cuy, int spx, int spy)
{
	double dehead;
	int xdif = abs(spx - cux), ydif = abs(spy - cuy);
	if ((spx > cux) && (spy >= cuy))
		dehead = atan(static_cast<double>(ydif) / xdif) * 180 / PI;
	else if ((spx < cux) && (spy >= cuy))
		dehead = 180 - atan(static_cast<double>(ydif) / xdif) * 180 / PI;
	else if ((spx < cux) && (spy < cuy))
		dehead = 180 + atan(static_cast<double>(ydif) / xdif) * 180 / PI;
	else if ((spx > cux) && (spy < cuy))
		dehead = 270 + atan(static_cast<double>(ydif) / xdif) * 180 / PI;
	else if ((spx == cux) && (spy > cuy))
		dehead = 90;
	else if ((spx == cux) && (spy < cuy))
		dehead = 270;
	return dehead;
}
void CHUDDlg::tarIndicateLineDraw(int startx, int starty, int endx, int endy, int length, CDC* pDC)
{
	double theta;
	int arrowx, arrowy;
	if (startx != endx)
		theta = atan(fabs(static_cast<double>(endy - starty) / (endx - startx)));
	if ((endx > startx) && (endy <= starty))
	{
		arrowx = startx + length*cos(theta);
		arrowy = starty - length*sin(theta);
	}
	else if ((endx < startx) && (endy <= starty))
	{
		arrowx = startx - length*cos(theta);
		arrowy = starty - length*sin(theta);
	}
	else if ((endx < startx) && (endy > starty))
	{
		arrowx = startx - length*cos(theta);
		arrowy = starty + length*sin(theta);
	}
	else if ((endx > startx) && (endy > starty))
	{
		arrowx = startx + length*cos(theta);
		arrowy = starty + length*sin(theta);
	}
	else if ((endx == startx) && (endy < starty))
	{
		arrowx = startx;
		arrowy = starty - length;
	}
	else if ((endx == startx) && (endy > starty))
	{
		arrowx = startx;
		arrowy = starty + length;
	}
	else if ((endx == startx) && (endy == starty))
	{
		arrowx = startx;
		arrowy = starty;
	}
	pDC->MoveTo(startx, starty);
	pDC->LineTo(arrowx, arrowy);
}
void CHUDDlg::velDraw(double vel, CDC* pDC)
{
	int v = static_cast<int>(vel), va = v - 75, vb = v + 75;
	CString str;
	CFont font, *pFont = pDC->GetCurrentFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	wcscpy_s(lf.lfFaceName, _T("宋体"));
	lf.lfHeight *= 1.3;
	lf.lfWidth *= 1.3;
	font.CreateFontIndirect(&lf);
	pFont = pDC->SelectObject(&font);
	pDC->Rectangle(65, 320 - 20, 125, 320 + 20);
	str.Format(_T("%d"), v);
	pDC->SetTextColor(RGB(0, 255, 0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(str, CRect(65, 320 - 20, 125, 320 + 20), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	//pDC->DrawText(_T("V"), CRect(125, 330, 145, 370), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(pFont);

	pDC->SetTextAlign(TA_RIGHT);
	for (int i = 320 + 150; i >= 320 + 30; i = i - 2)
	{
		if (va % 25 == 0)//25m/s一大刻线
		{
			str.Format(_T("%d"), va);
			pDC->MoveTo(125, i);
			pDC->LineTo(105, i);
			pDC->TextOut(100, i - 8, str);
		}
		else if (va % 5 == 0)//5m/s一小刻线
		{
			pDC->MoveTo(125, i);
			pDC->LineTo(115, i);
		}
		va++;
	}
	for (int i = 320 - 150; i <= 320 - 30; i = i + 2)
	{
		if (vb % 25 == 0)//25m/s一大刻线
		{
			str.Format(_T("%d"), vb);
			pDC->MoveTo(125, i);
			pDC->LineTo(105, i);
			pDC->TextOut(100, i - 8, str);
		}
		else if (vb % 5 == 0)//5m/s一小刻线
		{
			pDC->MoveTo(125, i);
			pDC->LineTo(115, i);
		}
		vb--;
	}
	pDC->SetTextAlign(TA_LEFT);//将对齐方式恢复为系统默认的左对齐，否则将影响以下DrawText文本的对齐格式
}
void CHUDDlg::heightDraw(int height, CDC* pDC)
{
	////////////气压高度标尺//////////////
	int h = height, ha = h - 1500, hb = h + 1500;
	CString str;
	pDC->SetTextColor(RGB(0, 255, 0));
	pDC->SetBkMode(TRANSPARENT);
	CFont font, *pFont = pDC->GetCurrentFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	wcscpy_s(lf.lfFaceName, _T("宋体"));
	lf.lfHeight *= 1.3;
	lf.lfWidth *= 1.3;
	font.CreateFontIndirect(&lf);
	pFont = pDC->SelectObject(&font);
	pDC->Rectangle(585, 320 - 20, 645, 320 + 20);
	str.Format(_T("%d"), h);
	pDC->DrawText(str, CRect(585, 320 - 20, 645, 320 + 20), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	//pDC->DrawText(_T("H"), CRect(565, 330, 585, 370), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(pFont);

	pDC->SetTextAlign(TA_LEFT);
	for (double j = 320 + 150; j >= 320 + 30 - 0.1; j = j - 0.1)
	{
		if (ha % 500 == 0)  //500m一大刻线
		{
			str.Format(_T("%d"), ha);
			pDC->MoveTo(585, j);
			pDC->LineTo(605, j);
			pDC->TextOut(610, j - 8, str);
		}
		else if (ha % 100 == 0)  ////100m一小刻线
		{
			pDC->MoveTo(585, j);
			pDC->LineTo(595, j);
		}
		ha++;
	}
	for (double j = 320 - 150; j <= 320 - 30 + 0.1; j = j + 0.1)
	{
		if (hb % 500 == 0)  //500m一大刻线
		{
			str.Format(_T("%d"), hb);
			pDC->MoveTo(585, j);
			pDC->LineTo(605, j);
			pDC->TextOut(610, j - 8, str);
		}
		else if (hb % 100 == 0)  //100m一小刻线
		{
			pDC->MoveTo(585, j);
			pDC->LineTo(595, j);
		}
		hb--;
	}
	////////////////无线电高度显示///////////////
	int landheight = 200;//当前地面距离海平面高度
	int rheight = h - landheight;//飞机无线电高度
	if (rheight > 5000)
		str.Format(_T("XXXR"));
	else if (0 == rheight % 10)
		str.Format(_T("%dR"), rheight);
	else
		str.Format(_T("%dR"), rheight - rheight % 10);
	pDC->TextOut(540, 490, str);
}
int CHUDDlg::headDraw(double path, double depath, CDC* pDC)
{
	int pa = static_cast<int>(path) - 35;
	pa = valTransfer(pa);
	CString str;
	for (int i = 355 - 140; i <= 355 + 140; i = i + 4)
	{
		if (pa % 20 == 0)  //20度一大刻线
		{
			str.Format(_T("%d"), pa);
			pDC->SetTextColor(RGB(0, 255, 0));
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
			pDC->MoveTo(i, 110);
			pDC->LineTo(i, 90);
			pDC->TextOut(i, 90, str);
		}
		else if (pa % 5 == 0)  //5度一小刻线
		{
			pDC->MoveTo(i, 110);
			pDC->LineTo(i, 100);
		}
		pa++;
		if (pa >= 360)
			pa = pa - 360;
	}
	pDC->MoveTo(355, 115);
	pDC->LineTo(346, 131);
	pDC->MoveTo(355, 115);
	pDC->LineTo(364, 131);
	pDC->MoveTo(346, 131);
	pDC->LineTo(364, 131);
	/////////////目标航向标记///////////
	/*CFont font, *pFont = pDC->GetCurrentFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	wcscpy_s(lf.lfFaceName, _T("宋体"));
	lf.lfHeight *= 1.1;
	lf.lfWidth *= 1.1;
	font.CreateFontIndirect(&lf);
	pFont = pDC->SelectObject(&font);*/
	int cpa = static_cast<int>(path), dpa = static_cast<int>(depath), difpa;
	cpa = valTransfer(cpa);
	dpa = valTransfer(dpa);
	int paminus = cpa - 35, paplus = cpa + 35;
	paminus = valTransfer(paminus);
	paplus = valTransfer(paplus);

	if (dpa > cpa && (dpa <= paplus || cpa > paplus)) //绘制目标航向标记
	{
		dbVerLineDraw(355 + 4 * (dpa - cpa), 110, pDC);
		return 1;
	}
	else if (dpa <= paplus && cpa > paplus)
	{
		dbVerLineDraw(355 + 4 * (360 - cpa + dpa), 110, pDC);
		return 1;
	}
	else if (dpa < cpa && (dpa >= paminus || cpa < paminus))
	{
		dbVerLineDraw(355 - 4 * (cpa - dpa), 110, pDC);
		return -1;
	}
	else if (dpa >= paminus && cpa < paminus)
	{
		dbVerLineDraw(355 - 4 * (360 - dpa + cpa), 110, pDC);
		return -1;
	}
	else
	{
		if (dpa > cpa)
		{
			difpa = dpa - cpa;
			if (difpa <= 180)
			{
				rArrowDraw(495, 123, pDC);
				str.Format(_T("%d"), dpa);
				pDC->SetTextAlign(TA_LEFT);
				pDC->TextOut(498, 115, str);
				return 1;
			}
			else
			{
				lArrowDraw(215, 123, pDC);
				str.Format(_T("%d"), dpa);
				pDC->SetTextAlign(TA_RIGHT);
				pDC->TextOut(212, 115, str);
				return -1;
			}
		}
		else if (dpa < cpa)
		{
			difpa = cpa - dpa;
			if (difpa >= 180)
			{
				rArrowDraw(495, 123, pDC);
				str.Format(_T("%d"), dpa);
				pDC->SetTextAlign(TA_LEFT);
				pDC->TextOut(498, 115, str);
				return 1;
			}
			else
			{
				lArrowDraw(215, 123, pDC);
				str.Format(_T("%d"), dpa);
				pDC->SetTextAlign(TA_RIGHT);
				pDC->TextOut(212, 115, str);
				return -1;
			}
		}
	}
	return 0;
	//pDC->SelectObject(pFont);
}
void CHUDDlg::rollDraw(int x, int y, CDC* pDC)
{
	int r = 270;//标尺底侧圆弧的半径
	int ang = 35;//标尺底侧圆弧的圆心角
	int xs = x, ys = y + r; //中轴刻线（0度）下端点坐标
	int x_lu, x_ld; //中轴刻线左侧某一刻线上下端点x坐标
	int x_rd, x_ru; //中轴刻线右侧某一刻线上下端点x坐标
	int y_u, y_d;  //中轴刻线两侧某一刻线上下端点y坐标
	pDC->MoveTo(xs, ys);
	pDC->LineTo(xs, ys - 25);
	double theta = ang / 6.0, theta_rad;
	for (int i = 1; i <= 6; i++)
	{
		theta_rad = i*theta*PI / 180;
		x_ld = xs - r*sin(theta_rad);
		x_rd = xs + r*sin(theta_rad);
		y_d = ys - r*(1 - cos(theta_rad));
		if (i == 3 || i == 6)  //0、90、180、270度时长刻线
		{
			x_lu = x_ld + 25 * sin(theta_rad);
			x_ru = x_rd - 25 * sin(theta_rad);
			y_u = y_d - 25 * cos(theta_rad);
		}
		else //其余能被30整除的短刻线
		{
			x_lu = x_ld + 15 * sin(theta_rad);
			x_ru = x_rd - 15 * sin(theta_rad);
			y_u = y_d - 15 * cos(theta_rad);
		}
		pDC->MoveTo(x_ld, y_d);
		pDC->LineTo(x_lu, y_u);
		pDC->MoveTo(x_rd, y_d);
		pDC->LineTo(x_ru, y_u);
	}
}
void CHUDDlg::trianDraw(double roll, int x, int y, CDC* pDC)
{
	int r = 275;//三角形顶点移动所划圆弧的半径
	int ang = 35;//上述圆弧的圆心角
	int l = 18;//绘制的等边三角形边长
	int xs = x, ys = y + r;//中轴刻线（0度）下端点坐标
	double x_t, y_t; //三角形顶点坐标
	double x_l, y_l; //三角形左端点坐标
	double x_r, y_r; //三角形右端点坐标
	double theta_rad;
	int ra = static_cast<int>(roll);
	if (abs(ra) % 360 == 0)   //角度变换到[0,360)范围
		ra = 0;
	else if (ra >= 360)
		ra = ra % 360;
	else if (ra < 0)
		ra = 360 - abs(ra) % 360;
	if (ra >= 0 && ra <= 180)  //中轴线右侧
	{
		theta_rad = PI*(ra * ang / 180.0) / 180;
		x_t = xs + r*sin(theta_rad);
		y_t = ys - r*(1 - cos(theta_rad));
		x_l = xs + r*sin(theta_rad) - l*cos(theta_rad + PI / 3);
		y_l = ys - r*(1 - cos(theta_rad)) + l*sin(theta_rad + PI / 3);
		x_r = xs + r*sin(theta_rad) + l*cos(PI / 3 - theta_rad);
		y_r = ys - r*(1 - cos(theta_rad)) + l*sin(PI / 3 - theta_rad);
	}
	else if (ra > 180 && ra <= 360)  //中轴线左侧
	{
		ra = 360 - ra;
		theta_rad = PI*(ra * ang / 180.0) / 180;
		x_t = xs - r*sin(theta_rad);
		y_t = ys - r*(1 - cos(theta_rad));
		x_l = xs - r*sin(theta_rad) - l*cos(PI / 3 - theta_rad);
		y_l = ys - r*(1 - cos(theta_rad)) + l*sin(PI / 3 - theta_rad);
		x_r = xs - r*sin(theta_rad) + l*cos(PI / 3 + theta_rad);
		y_r = ys - r*(1 - cos(theta_rad)) + l*sin(PI / 3 + theta_rad);
	}
	pDC->MoveTo(x_t, y_t);
	pDC->LineTo(x_l, y_l);
	pDC->MoveTo(x_l, y_l);
	pDC->LineTo(x_r, y_r);
	pDC->MoveTo(x_r, y_r);
	pDC->LineTo(x_t, y_t);
}
void CHUDDlg::pitSinDraw(int ra, int pb, int x, int y, int l, CDC* pDC)
{
	CString str;
	CPen pn, *pOldP;
	CFont font, *pFont = pDC->GetCurrentFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	wcscpy_s(lf.lfFaceName, _T("宋体"));
	lf.lfEscapement = ra * 10;
	font.CreateFontIndirect(&lf);
	pFont = pDC->SelectObject(&font);
	str.Format(_T("%d"), pb);
	double x1, y1, x2, y2, x3, y3; //对称轴右侧刻度线的三个顶点的坐标
	double x1s, y1s, x2s, y2s, x3s, y3s; //与上述点对称的三个顶点的坐标
	int rb;//将[0,360)的横滚角转化为过平显中心的纵向轴线与此刻线的夹角
	double rollA_rad; //rb的弧度表示
	int dd = 25;//俯仰角刻线第一个点距离中心的距离

	if (0 == pb)//俯仰角为0时的水平线（实线）
	{
		int len = 160;//单侧水平线长度
		if (ra >= 0 && ra <= 90)
		{
			rb = 90 - ra;
			rollA_rad = PI*rb / 180;
			x1 = x + dd * sin(rollA_rad);
			y1 = y - dd * cos(rollA_rad);
			x2 = x1 + len * sin(rollA_rad);
			y2 = y1 - len * cos(rollA_rad);
			x3 = x2;
			y3 = y2;
			x1s = x - dd * sin(rollA_rad);
			y1s = y + dd * cos(rollA_rad);
			x2s = x1s - len * sin(rollA_rad);
			y2s = y1s + len * cos(rollA_rad);
			x3s = x2s;
			y3s = y2s;
		}
		else if (ra > 90 && ra <= 180)
		{
			rb = ra - 90;
			rollA_rad = PI*rb / 180;
			x1 = x - dd * sin(rollA_rad);
			y1 = y - dd * cos(rollA_rad);
			x2 = x1 - len * sin(rollA_rad);
			y2 = y1 - len * cos(rollA_rad);
			x3 = x2;
			y3 = y2;
			x1s = x + dd * sin(rollA_rad);
			y1s = y + dd * cos(rollA_rad);
			x2s = x1s + len * sin(rollA_rad);
			y2s = y1s + len * cos(rollA_rad);
			x3s = x2s;
			y3s = y2s;
		}
		else if (ra > 180 && ra <= 270)
		{
			rb = 270 - ra;
			rollA_rad = PI*rb / 180;
			x1 = x - dd * sin(rollA_rad);
			y1 = y + dd * cos(rollA_rad);
			x2 = x1 - len * sin(rollA_rad);
			y2 = y1 + len * cos(rollA_rad);
			x3 = x2;
			y3 = y2;
			x1s = x + dd * sin(rollA_rad);
			y1s = y - dd * cos(rollA_rad);
			x2s = x1s + len * sin(rollA_rad);
			y2s = y1s - len * cos(rollA_rad);
			x3s = x2s;
			y3s = y2s;
		}
		else if (ra > 270 && ra < 360)
		{
			rb = ra - 270;
			rollA_rad = PI*rb / 180;
			x1 = x + dd * sin(rollA_rad);
			y1 = y + dd * cos(rollA_rad);
			x2 = x1 + len * sin(rollA_rad);
			y2 = y1 + len * cos(rollA_rad);
			x3 = x2;
			y3 = y2;
			x1s = x - dd * sin(rollA_rad);
			y1s = y - dd * cos(rollA_rad);
			x2s = x1s - len * sin(rollA_rad);
			y2s = y1s - len * cos(rollA_rad);
			x3s = x2s;
			y3s = y2s;
		}
		pn.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	}
	else if (pb > 0 && pb <= 180)//俯仰角（0,180]时的刻度线（实线）
	{
		if (ra >= 0 && ra <= 90)
		{
			rb = 90 - ra;
			rollA_rad = PI*rb / 180;
			x1 = x + dd * sin(rollA_rad);
			y1 = y - dd * cos(rollA_rad);
			x2 = x1 + l *sin(rollA_rad);
			y2 = y1 - l *cos(rollA_rad);
			x3 = x2 + 15 * cos(rollA_rad);
			y3 = y2 + 15 * sin(rollA_rad);
			x1s = x - dd * sin(rollA_rad);
			y1s = y + dd * cos(rollA_rad);
			x2s = x1s - l*sin(rollA_rad);
			y2s = y1s + l*cos(rollA_rad);
			x3s = x2s + 15 * cos(rollA_rad);
			y3s = y2s + 15 * sin(rollA_rad);
			pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			pDC->TextOut(x + 110 * sin(rollA_rad), y - 110 * cos(rollA_rad), str);
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			pDC->TextOut(x - 110 * sin(rollA_rad), y + 110 * cos(rollA_rad), str);
		}
		else if (ra > 90 && ra <= 180)
		{
			rb = ra - 90;
			rollA_rad = PI*rb / 180;
			x1 = x - dd * sin(rollA_rad);
			y1 = y - dd * cos(rollA_rad);
			x2 = x1 - l*sin(rollA_rad);
			y2 = y1 - l*cos(rollA_rad);
			x3 = x2 + 15 * cos(rollA_rad);
			y3 = y2 - 15 * sin(rollA_rad);
			x1s = x + dd * sin(rollA_rad);
			y1s = y + dd * cos(rollA_rad);
			x2s = x1s + l*sin(rollA_rad);
			y2s = y1s + l*cos(rollA_rad);
			x3s = x2s + 15 * cos(rollA_rad);
			y3s = y2s - 15 * sin(rollA_rad);
			pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			pDC->TextOut(x - 110 * sin(rollA_rad), y - 110 * cos(rollA_rad), str);
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			pDC->TextOut(x + 110 * sin(rollA_rad), y + 110 * cos(rollA_rad), str);
		}
		else if (ra > 180 && ra <= 270)
		{
			rb = 270 - ra;
			rollA_rad = PI*rb / 180;
			x1 = x - dd * sin(rollA_rad);
			y1 = y + dd * cos(rollA_rad);
			x2 = x1 - l*sin(rollA_rad);
			y2 = y1 + l*cos(rollA_rad);
			x3 = x2 - 15 * cos(rollA_rad);
			y3 = y2 - 15 * sin(rollA_rad);
			x1s = x + dd * sin(rollA_rad);
			y1s = y - dd * cos(rollA_rad);
			x2s = x1s + l*sin(rollA_rad);
			y2s = y1s - l*cos(rollA_rad);
			x3s = x2s - 15 * cos(rollA_rad);
			y3s = y2s - 15 * sin(rollA_rad);
			pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			pDC->TextOut(x - 110 * sin(rollA_rad), y + 110 * cos(rollA_rad), str);
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			pDC->TextOut(x + 110 * sin(rollA_rad), y - 110 * cos(rollA_rad), str);
		}
		else if (ra > 270 && ra < 360)
		{
			rb = ra - 270;
			rollA_rad = PI*rb / 180;
			x1 = x + dd * sin(rollA_rad);
			y1 = y + dd * cos(rollA_rad);
			x2 = x1 + l*sin(rollA_rad);
			y2 = y1 + l*cos(rollA_rad);
			x3 = x2 - 15 * cos(rollA_rad);
			y3 = y2 + 15 * sin(rollA_rad);
			x1s = x - dd * sin(rollA_rad);
			y1s = y - dd * cos(rollA_rad);
			x2s = x1s - l*sin(rollA_rad);
			y2s = y1s - l*cos(rollA_rad);
			x3s = x2s - 15 * cos(rollA_rad);
			y3s = y2s + 15 * sin(rollA_rad);
			pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			pDC->TextOut(x + 110 * sin(rollA_rad), y + 110 * cos(rollA_rad), str);
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			pDC->TextOut(x - 110 * sin(rollA_rad), y - 110 * cos(rollA_rad), str);
		}
		pn.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	}
	else//俯仰角[-180,0）时的刻度线（虚线）
	{
		if (ra >= 0 && ra <= 90)
		{
			rb = 90 - ra;
			rollA_rad = PI*rb / 180;
			x2 = x + dd * sin(rollA_rad);
			y2 = y - dd * cos(rollA_rad);
			x3 = x2 + l *sin(rollA_rad);
			y3 = y2 - l *cos(rollA_rad);
			x1 = x2 - 15 * cos(rollA_rad);
			y1 = y2 - 15 * sin(rollA_rad);
			x2s = x - dd * sin(rollA_rad);
			y2s = y + dd * cos(rollA_rad);
			x3s = x2s - l*sin(rollA_rad);
			y3s = y2s + l*cos(rollA_rad);
			x1s = x2s - 15 * cos(rollA_rad);
			y1s = y2s - 15 * sin(rollA_rad);
			pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			pDC->TextOut(x + 110 * sin(rollA_rad), y - 110 * cos(rollA_rad), str);
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			pDC->TextOut(x - 110 * sin(rollA_rad), y + 110 * cos(rollA_rad), str);
		}
		else if (ra > 90 && ra <= 180)
		{
			rb = ra - 90;
			rollA_rad = PI*rb / 180;
			x2 = x - dd * sin(rollA_rad);
			y2 = y - dd * cos(rollA_rad);
			x3 = x2 - l*sin(rollA_rad);
			y3 = y2 - l*cos(rollA_rad);
			x1 = x2 - 15 * cos(rollA_rad);
			y1 = y2 + 15 * sin(rollA_rad);
			x2s = x + dd * sin(rollA_rad);
			y2s = y + dd * cos(rollA_rad);
			x3s = x2s + l*sin(rollA_rad);
			y3s = y2s + l*cos(rollA_rad);
			x1s = x2s - 15 * cos(rollA_rad);
			y1s = y2s + 15 * sin(rollA_rad);
			pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			pDC->TextOut(x - 110 * sin(rollA_rad), y - 110 * cos(rollA_rad), str);
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			pDC->TextOut(x + 110 * sin(rollA_rad), y + 110 * cos(rollA_rad), str);
		}
		else if (ra > 180 && ra <= 270)
		{
			rb = 270 - ra;
			rollA_rad = PI*rb / 180;
			x2 = x - dd * sin(rollA_rad);
			y2 = y + dd * cos(rollA_rad);
			x3 = x2 - l*sin(rollA_rad);
			y3 = y2 + l*cos(rollA_rad);
			x1 = x2 + 15 * cos(rollA_rad);
			y1 = y2 + 15 * sin(rollA_rad);
			x2s = x + dd * sin(rollA_rad);
			y2s = y - dd * cos(rollA_rad);
			x3s = x2s + l*sin(rollA_rad);
			y3s = y2s - l*cos(rollA_rad);
			x1s = x2s + 15 * cos(rollA_rad);
			y1s = y2s + 15 * sin(rollA_rad);
			pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			pDC->TextOut(x - 110 * sin(rollA_rad), y + 110 * cos(rollA_rad), str);
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			pDC->TextOut(x + 110 * sin(rollA_rad), y - 110 * cos(rollA_rad), str);
		}
		else if (ra > 270 && ra < 360)
		{
			rb = ra - 270;
			rollA_rad = PI*rb / 180;
			x2 = x + dd * sin(rollA_rad);
			y2 = y + dd * cos(rollA_rad);
			x3 = x2 + l*sin(rollA_rad);
			y3 = y2 + l*cos(rollA_rad);
			x1 = x2 + 15 * cos(rollA_rad);
			y1 = y2 - 15 * sin(rollA_rad);
			x2s = x - dd * sin(rollA_rad);
			y2s = y - dd * cos(rollA_rad);
			x3s = x2s - l*sin(rollA_rad);
			y3s = y2s - l*cos(rollA_rad);
			x1s = x2s + 15 * cos(rollA_rad);
			y1s = y2s - 15 * sin(rollA_rad);
			pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			pDC->TextOut(x + 110 * sin(rollA_rad), y + 110 * cos(rollA_rad), str);
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			pDC->TextOut(x - 110 * sin(rollA_rad), y - 110 * cos(rollA_rad), str);
		}
		pn.CreatePen(PS_DOT, 1, RGB(0, 255, 0));
	}

	pOldP = pDC->SelectObject(&pn);
	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y2);
	pDC->MoveTo(x2, y2);
	pDC->LineTo(x3, y3);
	pDC->MoveTo(x1s, y1s);
	pDC->LineTo(x2s, y2s);
	pDC->MoveTo(x2s, y2s);
	pDC->LineTo(x3s, y3s);
	pDC->SelectObject(pFont);
	pDC->SelectObject(pOldP);
}
void CHUDDlg::pitchDraw(double roll, double pitch, int x, int y, CDC* pDC)
{
	////////////俯仰角标尺///////////////
	int ra = static_cast<int>(roll);
	int pa = static_cast<int>(pitch), pb;
	double raa_rad;
	double xu, yu, xd, yd;
	if (abs(ra) % 360 == 0)   //横滚角度变换到[0,360)范围
		ra = 0;
	else if (ra >= 360)
		ra = ra % 360;
	else if (ra < 0)
		ra = 360 - abs(ra) % 360;
	if (abs(pa) % 360 == 0)   //俯仰角度变换到[0,360)范围
		pa = 0;
	else if (pa >= 360)
		pa = pa % 360;
	else if (pa < 0)
		pa = 360 - abs(pa) % 360;
	if (pa >= 0 && pa <= 180)  //将[0,360)的俯仰角度变换到(-180,0][0,180]范围
		pb = pa;
	else
		pb = pa - 360;
	int pbb = pb, pitchA = pb;
	for (int d = 0; d <= 192; d = d + 24)  //中心点向俯仰角减小方向绘制
	{
		if (abs(pb) % 5 == 0)   //5度一条刻线
		{
			if (ra >= 0 && ra <= 90)
			{
				raa_rad = PI*ra / 180;
				xu = x + d*sin(raa_rad);
				yu = y + d*cos(raa_rad);
			}
			else if (ra > 90 && ra <= 180)
			{
				raa_rad = PI*(180 - ra) / 180;
				xu = x + d*sin(raa_rad);
				yu = y - d*cos(raa_rad);
			}
			else if (ra > 180 && ra <= 270)
			{
				raa_rad = PI*(ra - 180) / 180;
				xu = x - d*sin(raa_rad);
				yu = y - d*cos(raa_rad);
			}
			else if (ra > 270 && ra < 360)
			{
				raa_rad = PI*(360 - ra) / 180;
				xu = x - d*sin(raa_rad);
				yu = y + d*cos(raa_rad);
			}
			pitSinDraw(ra, pb, xu, yu, 80, pDC);
		}
		pb--;
		if (pb < -180)
			pb = 360 + pb;
	}
	for (int d = 0; d <= 192; d = d + 24)  //中心点向俯仰角增大方向绘制
	{
		if (abs(pbb) % 5 == 0)
		{
			if (ra >= 0 && ra <= 90)
			{
				raa_rad = PI*ra / 180;
				xd = x - d*sin(raa_rad);
				yd = y - d*cos(raa_rad);
			}
			else if (ra > 90 && ra <= 180)
			{
				raa_rad = PI*(180 - ra) / 180;
				xd = x - d*sin(raa_rad);
				yd = y + d*cos(raa_rad);
			}
			else if (ra > 180 && ra <= 270)
			{
				raa_rad = PI*(ra - 180) / 180;
				xd = x + d*sin(raa_rad);
				yd = y + d*cos(raa_rad);
			}
			else if (ra > 270 && ra < 360)
			{
				raa_rad = PI*(360 - ra) / 180;
				xd = x + d*sin(raa_rad);
				yd = y - d*cos(raa_rad);
			}
			pitSinDraw(ra, pbb, xd, yd, 80, pDC);
		}
		pbb++;
		if (pbb > 180)
			pbb = pbb - 360;
	}
	////////////////俯仰角数值显示///////////////
	CString str;
	str.Format(_T("%d"), pitchA);
	pDC->SetTextAlign(TA_LEFT);
	if (pitchA)
		pDC->TextOut(365, 330, str);
}
void CHUDDlg::stpInfoDraw(int grounddt, int stpz, CDC* pDC)
{
	/*CFont font, *pFont = pDC->GetCurrentFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	wcscpy_s(lf.lfFaceName, _T("黑体"));
	lf.lfHeight *= 1.1;
	lf.lfWidth *= 1.1;
	font.CreateFontIndirect(&lf);
	pFont = pDC->SelectObject(&font);*/
	CString str;
	if (grounddt <50)
	{
		KillTimer(7);
		flag7 = FALSE;
		grounddt = 0;
	}
	str.Format(_T("%dM  /  %d"), grounddt, stpz);//航路点地面距离和海拔
	pDC->TextOut(540, 515, str);
	//pDC->SelectObject(pFont);
}
void CHUDDlg::timeDraw(int timetogo, CDC* pDC)
{
	/*CFont font, *pFont = pDC->GetCurrentFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	wcscpy_s(lf.lfFaceName, _T("黑体"));
	lf.lfHeight *= 1.1;
	lf.lfWidth *= 1.1;
	font.CreateFontIndirect(&lf);
	pFont = pDC->SelectObject(&font);*/
	CString str, str2;
	CTime curtime;//current time
	if (timetogo)
	{
		curtime = CTime::GetCurrentTime();
		ct2 = curtime;
	}
	str = ct2.Format(_T("%H:%M:%S"));
	pDC->TextOut(540, 565, str);
	if (timetogo >= 10)//time to go
		str.Format(_T(":%d"), timetogo);
	else
		str.Format(_T(":0%d"), timetogo);
	str2 = timeTransfer(ct, ttg);//time on target
	str = str + _T("  /  -") + str2;
	pDC->TextOut(540, 540, str);
	//pDC->SelectObject(pFont);
}
void CHUDDlg::targetDraw(int turn, int height, int cx, int cy, int stpx, int stpy, int stpz, int grounddt, double head, double dehead, CDC* pDC)
{
	head = valTransfer(head);
	int eyedistance = 6;
	double pitchIFOV = 45, headIFOV = 45;
	double pitchag;
	int headag;
	int targpointx, targpointy;
	int border = 0;
	///////////目标航路点在平显中的纵坐标////////
	double gdistance = sqrt(pow((stpy - cy), 2) + pow((stpx - cx), 2));
	pitchag = atan(abs(stpz - height) / gdistance) * 180 / PI;
	if (height == stpz)
		targpointy = 350;
	else if (height < stpz)
	{
		if (pitchag >= pitchIFOV)
		{
			targpointy = 155;
			border = 1;
		}
		else
		{
			targpointy = 350 - eyedistance*tan(pitchag*PI / 180) * 350 / 6.0;
			if (targpointy < 155)
			{
				targpointy = 155;
				border = 1;
			}
		}
	}
	else if (height > stpz)
	{
		if (pitchag >= pitchIFOV)
		{
			targpointy = 545;
			border = 1;
		}
		else
		{
			targpointy = 350 + eyedistance*tan(pitchag*PI / 180) * 350 / 6.0;
			if (targpointy > 545)
			{
				targpointy = 545;
				border = 1;
			}
		}
	}
	///////////目标航路点在平显中的横坐标////////
	headag = abs(static_cast<int>(head - dehead));//飞机与目标航路点的最小偏角
	if (headag > 180)
		headag = 360 - headag;
	if (0 == turn)
		targpointx = 355;
	else if (turn > 0)
	{
		if (headag >= headIFOV)
		{
			targpointx = 570;
			border = 1;
		}
		else
		{
			targpointx = 355 + eyedistance*tan(headag*PI / 180) * 350 / 6.0;
			if (targpointx > 570)
			{
				targpointx = 570;
				border = 1;
			}
		}
	}
	else if (turn < 0)
	{
		if (headag >= headIFOV)
		{
			targpointx = 140;
			border = 1;
		}
		else
		{
			targpointx = 355 - eyedistance*tan(headag*PI / 180) * 350 / 6.0;
			if (targpointx < 140)
			{
				targpointx = 140;
				border = 1;
			}
		}
	}
	///////////////目标航路点距离、角度等信息////////////
	int len = 8;
	double distance;//目标与飞机直线距离
	CString str;
	double theta;
	int l = 30, d = 30;
	int arrowx, arrowy;
	if (!flag7)
		grounddt = 0;
	if (flag7 || flag4)//未到达航路点时绘制（水平距离、高度）
	{
		pDC->SetTextAlign(TA_LEFT);
		distance = sqrt(pow(grounddt, 2) + pow(abs(static_cast<int>(height - stpz)), 2)) / 1000.0;
		if (headag)//航向对准后不再显示角度信息
		{
			str.Format(_T("%d"), headag);
			pDC->DrawText(str, CRect(targpointx - 10, targpointy - 25, targpointx + 10, targpointy - 10), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		if (static_cast<int>(10 * distance))//距离为0时不再显示
		{
			str.Format(_T("%.1f"), distance);
			pDC->DrawText(str, CRect(targpointx - 10, targpointy + 10, targpointx + 10, targpointy + 25), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		pDC->MoveTo(targpointx - len, targpointy - len);
		pDC->LineTo(targpointx + len, targpointy - len);
		pDC->MoveTo(targpointx + len, targpointy - len);
		pDC->LineTo(targpointx + len, targpointy + len);
		pDC->MoveTo(targpointx + len, targpointy + len);
		pDC->LineTo(targpointx - len, targpointy + len);
		pDC->MoveTo(targpointx - len, targpointy + len);
		pDC->LineTo(targpointx - len, targpointy - len);
		//pDC->MoveTo(targpointx - len, targpointy - len);
		//pDC->LineTo(targpointx + len, targpointy + len);
		//pDC->MoveTo(targpointx + len, targpointy - len);
		//pDC->LineTo(targpointx - len, targpointy + len);
		if (border)
			tarIndicateLineDraw(targpointx, targpointy, 355, 350, l, pDC);
		else
			tarIndicateLineDraw(355, 350, targpointx, targpointy, d, pDC);
	}
}
void CHUDDlg::reticleDraw(int cenx, int ceny, int radius, double tarrange, double minrange, double roll, CDC* pDC)
{
	int ra = static_cast<int>(roll);//reticle
	if (ra < 0)//角度变换到[0，360)
		ra = 360 - abs(ra) % 360;
	double ratheta;
	int r = radius - 10;//内环半径
	if (ra < 90)
	{
		ratheta = ra*PI / 180;
		cenx = cenx + (ceny - 350)*sin(ratheta);
		ceny = ceny - (ceny - 350)*(1 - cos(ratheta));
	}
	else if (ra > 270)
	{
		ratheta = 2 * PI - ra*PI / 180;
		cenx = cenx - (ceny - 350)*sin(ratheta);
		ceny = ceny - (ceny - 350)*(1 - cos(ratheta));
	}
	double sweepang = 360 * tarrange / 12000;
	double startang = 360 - sweepang + 90;
	double theta = startang*PI / 180;
	double thetamin = (360 - 360 * minrange / 12000 + 90)*PI / 180;
	double startx = cenx + r*cos(theta);
	double starty = ceny - r*sin(theta);
	double trianx = cenx + (r - 3)*cos(thetamin);
	double triany = ceny - (r - 3)*sin(thetamin);
	pDC->Ellipse(cenx - radius, ceny - radius, cenx + radius, ceny + radius);
	pDC->MoveTo(cenx, ceny);
	pDC->LineTo(cenx, ceny);
	pDC->MoveTo(cenx, ceny - radius);
	pDC->LineTo(cenx, ceny - radius - 15);
	pDC->MoveTo(cenx, ceny + radius);
	pDC->LineTo(cenx, ceny + radius + 15);
	pDC->MoveTo(cenx - radius, ceny);
	pDC->LineTo(cenx - radius - 15, ceny);
	pDC->MoveTo(cenx + radius, ceny);
	pDC->LineTo(cenx + radius + 15, ceny);
	pDC->MoveTo(startx, starty);
	pDC->LineTo(startx - 10 * cos(theta), starty + 10 * sin(theta));
	pDC->AngleArc(cenx, ceny, r, startang, sweepang);
	double xl = trianx - 10 * cos(thetamin - PI / 6);//三角形
	double yl = triany + 10 * sin(thetamin - PI / 6);
	double xr = xl + 10 * cos(PI / 2 - thetamin);
	double yr = yl + 10 * sin(PI / 2 - thetamin);
	pDC->MoveTo(trianx, triany);
	pDC->LineTo(xl, yl);
	pDC->MoveTo(xl, yl);
	pDC->LineTo(xr, yr);
	pDC->MoveTo(trianx, triany);
	pDC->LineTo(xr, yr);
	double sxline, syline, exline, eyline;//PBIL
	double sxline2, syline2;
	double mrx, mry, mrx2, mry2, mrx3, mry3;
	int l = 350;
	if (ra < 90)
	{
		ratheta = ra*PI / 180;
		sxline = cenx + (r - 10)*sin(ratheta);
		syline = ceny - (r - 10)*cos(ratheta);
		exline = sxline + l*sin(ratheta);
		eyline = syline - l*cos(ratheta);
		sxline2 = cenx + radius*sin(ratheta);
		syline2 = ceny - radius*cos(ratheta);
		pDC->MoveTo(sxline, syline);
		pDC->LineTo(exline, eyline);
		if ((tarrange - minrange) > 0)//MRS
		{
			if ((tarrange - minrange) > 3000)
			{
				mrx = sxline2 + 300 * sin(ratheta);
				mry = syline2 - 300 * cos(ratheta);
			}
			else
			{
				mrx = sxline2 + (tarrange - minrange) * 300 / 3000 * sin(ratheta);
				mry = syline2 - (tarrange - minrange) * 300 / 3000 * cos(ratheta);
			}
			mrx2 = mrx - 20;
			mry2 = mry;
			mrx3 = mrx2;
			mry3 = mry2 + 5;
			pDC->MoveTo(mrx, mry);
			pDC->LineTo(mrx2, mry2);
			pDC->MoveTo(mrx2, mry2);
			pDC->LineTo(mrx3, mry3);
			pDC->MoveTo(mrx, mry);
			pDC->LineTo(mrx2 + 40, mry2);
			pDC->MoveTo(mrx2 + 40, mry2);
			pDC->LineTo(mrx3 + 40, mry3);
		}
	}
	else if (ra > 270)
	{
		ratheta = 2 * PI - ra*PI / 180;
		sxline = cenx - (r - 10)*sin(ratheta);
		syline = ceny - (r - 10)*cos(ratheta);
		sxline2 = cenx - radius*sin(ratheta);
		syline2 = ceny - radius*cos(ratheta);
		exline = sxline - l*sin(ratheta);
		eyline = syline - l*cos(ratheta);
		pDC->MoveTo(sxline, syline);
		pDC->LineTo(exline, eyline);
		if ((tarrange - minrange) > 0)//MRS
		{
			if ((tarrange - minrange) > 3000)
			{
				mrx = sxline2 - 300 * sin(ratheta);
				mry = syline2 - 300 * cos(ratheta);
			}
			else
			{
				mrx = sxline2 - (tarrange - minrange) * 300 / 3000 * sin(ratheta);
				mry = syline2 - (tarrange - minrange) * 300 / 3000 * cos(ratheta);
			}
			mrx2 = mrx - 20;
			mry2 = mry;
			mrx3 = mrx2;
			mry3 = mry2 + 5;
			pDC->MoveTo(mrx, mry);
			pDC->LineTo(mrx2, mry2);
			pDC->MoveTo(mrx2, mry2);
			pDC->LineTo(mrx3, mry3);
			pDC->MoveTo(mrx, mry);
			pDC->LineTo(mrx2 + 40, mry2);
			pDC->MoveTo(mrx2 + 40, mry2);
			pDC->LineTo(mrx3 + 40, mry3);
		}
	}

}
void CHUDDlg::aslDraw(double vel, double roll, int timetogo, int turn, double head, double dehead, double relrange, CDC* pDC)
{
	int ra = static_cast<int>(roll);
	if (ra < 0)//角度变换到[0，360)
		ra = 360 - abs(ra) % 360;
	double ratheta;
	int headag = abs(static_cast<int>(dehead - head));
	if (headag > 180)
		headag = 360 - headag;
	int headIFOV = 45;
	double xbottom, ybottom, xtop, ytop;
	int l = 200;
	double d;
	double x;
	if ((headag - headIFOV) >= 0)
	{
		if (0 == ra)
		{
			if (turn > 0)
			{
				xbottom = 570;
				ybottom = 350 + l;
				xtop = 570;
				ytop = 350 - l;
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
			else if (turn < 0)
			{
				xbottom = 140;
				ybottom = 350 + l;
				xtop = 140;
				ytop = 350 - l;
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
		}
		else if (ra < 90)
		{
			ratheta = ra*PI / 180;
			x = 355 + headag*(570 - 355) / static_cast<double>(headIFOV) - l*sin(ratheta);
			if (x < 570)
			{
				d = (headag*(570 - 355) / static_cast<double>(headIFOV) - (570 - 355)) / sin(ratheta);
				xbottom = 570;
				ybottom = 350 - d*cos(ratheta);
				xtop = xbottom - (l - d)*sin(ratheta);
				ytop = ybottom - (l - d)*cos(ratheta);
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
		}
		else if (ra > 270)
		{
			ratheta = 2 * PI - ra*PI / 180;
			x = 355 - headag*(570 - 355) / static_cast<double>(headIFOV) + l*sin(ratheta);
			if (x > 140)
			{
				d = (headag*(570 - 355) / static_cast<double>(headIFOV) - (570 - 355)) / sin(ratheta);
				xbottom = 140;
				ybottom = 350 - d*cos(ratheta);
				xtop = xbottom + (l - d)*sin(ratheta);
				ytop = ybottom - (l - d)*cos(ratheta);
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
		}
	}
	else
	{
		if (0 == ra)
		{
			if (turn > 0)
			{
				xbottom = 355 + headag*(570 - 355) / static_cast<double>(headIFOV);
				ybottom = 350 + l;
				xtop = xbottom;
				ytop = 350 - l;
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
			else if (turn < 0)
			{
				xbottom = 355 - headag*(570 - 355) / static_cast<double>(headIFOV);
				ybottom = 350 + l;
				xtop = xbottom;
				ytop = 350 - l;
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
			else
			{
				xbottom = 355;
				ybottom = 350 + l;
				xtop = xbottom;
				ytop = 350 - l;
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
		}
		else if (ra < 90)
		{
			ratheta = ra*PI / 180;
			x = 355 + headag*(570 - 355) / static_cast<double>(headIFOV) + l*sin(ratheta);
			if (x > 570)
			{
				d = ((570 - 355) - headag*(570 - 355) / static_cast<double>(headIFOV)) / sin(ratheta);
				xbottom = 570;
				ybottom = 350 + d*cos(ratheta);
				xtop = xbottom - (l + d)*sin(ratheta);
				ytop = ybottom - (l + d)*cos(ratheta);
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
			else
			{
				xbottom = 355 + headag*(570 - 355) / static_cast<double>(headIFOV) + l*sin(ratheta);
				ybottom = 350 + l*cos(ratheta);
				xtop = 355 + headag*(570 - 355) / static_cast<double>(headIFOV) - l*sin(ratheta);
				ytop = 350 - l*cos(ratheta);
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
		}
		else if (ra > 270)
		{
			ratheta = 2 * PI - ra*PI / 180;
			x = 355 - headag*(570 - 355) / static_cast<double>(headIFOV) + l*sin(ratheta);
			if (x < 140)
			{
				d = ((355 - 140) - headag*(570 - 355) / static_cast<double>(headIFOV)) / sin(ratheta);
				xbottom = 140;
				ybottom = 350 + d*cos(ratheta);
				xtop = xbottom + (l + d)*sin(ratheta);
				ytop = ybottom - (l + d)*cos(ratheta);
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
			else
			{
				xbottom = 355 - headag*(570 - 355) / static_cast<double>(headIFOV) - l*sin(ratheta);
				ybottom = 350 + l*cos(ratheta);
				xtop = 355 - headag*(570 - 355) / static_cast<double>(headIFOV) + l*sin(ratheta);
				ytop = 350 - l*cos(ratheta);
				pDC->MoveTo(xbottom, ybottom);
				pDC->LineTo(xtop, ytop);
				solutioncueDraw(vel, roll, timetogo, xtop, ytop, relrange, pDC);
			}
		}
	}
}
void CHUDDlg::solutioncueDraw(double vel, double roll, int timetogo, double xtop, double ytop, double relrange, CDC* pDC)
{
	int ra = static_cast<int>(roll);
	if (ra < 0)//角度变换到[0，360)
		ra = 360 - abs(ra) % 360;
	double ratheta;
	double cuex, cuey;
	int maxrange = 2000;
	int d = 30;//solution cue距顶端距离
	int r = 7;//solution cue半径
	int l = 400;//ASL长度
	CString str;
	if (0 == ra)
	{
		if (relrange >= maxrange)
		{
			cuex = xtop;
			cuey = ytop + d;
			pDC->Ellipse(cuex - r, cuey - r, cuex + r, cuey + r);
			pDC->MoveTo(cuex, cuey);
			pDC->LineTo(cuex, cuey);
			if (timetogo <= 10 && timetogo != 0)
			{
				str.Format(_T("%d"), timetogo);
				pDC->SetTextAlign(TA_RIGHT);
				pDC->TextOut(cuex - 10, cuey - 10, str);
			}
		}
		else
		{
			cuex = xtop;
			cuey = ytop + d + (l - d - (l - d)*relrange / maxrange);
			pDC->Ellipse(cuex - r, cuey - r, cuex + r, cuey + r);
			pDC->MoveTo(cuex, cuey);
			pDC->LineTo(cuex, cuey);
			if (timetogo <= 10 && timetogo > 0)
			{
				str.Format(_T("%d"), timetogo);
				pDC->SetTextAlign(TA_RIGHT);
				pDC->TextOut(cuex - 10, cuey - 10, str);
			}
		}
	}
	else if (ra < 90)
	{
		ratheta = ra*PI / 180;
		if (relrange >= maxrange)
		{
			cuex = xtop + d*sin(ratheta);
			cuey = ytop + d*cos(ratheta);
			if (cuex <= 570)
			{
				pDC->Ellipse(cuex - r, cuey - r, cuex + r, cuey + r);
				pDC->MoveTo(cuex, cuey);
				pDC->LineTo(cuex, cuey);
				if (timetogo <= 10 && timetogo != 0)
				{
					str.Format(_T("%d"), timetogo);
					pDC->SetTextAlign(TA_RIGHT);
					pDC->TextOut(cuex - 10, cuey - 10, str);
				}
			}
		}
		else
		{
			cuex = xtop + (d + (l - d - (l - d)*relrange / maxrange))*sin(ratheta);
			cuey = ytop + (d + (l - d - (l - d)*relrange / maxrange))*cos(ratheta);
			if (cuex <= 570)
			{
				pDC->Ellipse(cuex - r, cuey - r, cuex + r, cuey + r);
				pDC->MoveTo(cuex, cuey);
				pDC->LineTo(cuex, cuey);
				if (timetogo <= 10 && timetogo != 0)
				{
					str.Format(_T("%d"), timetogo);
					pDC->SetTextAlign(TA_RIGHT);
					pDC->TextOut(cuex - 10, cuey - 10, str);
				}
			}
		}
	}
	else if (ra > 270)
	{
		ratheta = 2 * PI - ra*PI / 180;
		if (relrange >= maxrange)
		{
			cuex = xtop - d*sin(ratheta);
			cuey = ytop + d*cos(ratheta);
			if (cuex >= 140)
			{
				pDC->Ellipse(cuex - r, cuey - r, cuex + r, cuey + r);
				pDC->MoveTo(cuex, cuey);
				pDC->LineTo(cuex, cuey);
				if (timetogo <= 10 && timetogo != 0)
				{
					str.Format(_T("%d"), timetogo);
					pDC->SetTextAlign(TA_RIGHT);
					pDC->TextOut(cuex - 10, cuey - 10, str);
				}
			}
		}
		else
		{
			cuex = xtop - (d + (l - d - (l - d)*relrange / maxrange))*sin(ratheta);
			cuey = ytop + (d + (l - d - (l - d)*relrange / maxrange))*cos(ratheta);
			if (cuex >= 140)
			{
				pDC->Ellipse(cuex - r, cuey - r, cuex + r, cuey + r);
				pDC->MoveTo(cuex, cuey);
				pDC->LineTo(cuex, cuey);
				if (timetogo <= 10 && timetogo != 0)
				{
					str.Format(_T("%d"), timetogo);
					pDC->SetTextAlign(TA_RIGHT);
					pDC->TextOut(cuex - 10, cuey - 10, str);
				}
			}
		}
	}
	pDC->SetTextAlign(TA_LEFT);
}\
void CHUDDlg::flash(int falltime, CDC* pDC)
{
	CString str;
	if (!falltime && (nFlash % 10))
	{
		str.Format(_T("%d"), falltime);
		pDC->SetTextAlign(TA_RIGHT);
		pDC->TextOut(170, 515, str);
		str.Format(_T("IMPACT"));
		pDC->TextOut(380, 560, str);
	}
	pDC->SetTextAlign(TA_LEFT);
	nFlash++;
}
void CHUDDlg::modeFalltimeDraw(int nsel, int falltime, CDC* pDC)
{
	CString str;
	switch (nsel)
	{
	case 0:
	{
		str.Format(_T("MODE NAV"));
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 255, 0));
		pDC->SetTextAlign(TA_RIGHT);
		pDC->TextOut(170, 490, str);
		pDC->SetTextAlign(TA_LEFT);
		break;
	}
	case 1:
	{
		str.Format(_T("MODE CCRP"));
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 255, 0));
		pDC->SetTextAlign(TA_RIGHT);
		pDC->TextOut(170, 490, str);
		if (falltime > 0 && !flag9)
		{
			str.Format(_T("%d"), falltime);
			pDC->TextOut(170, 515, str);
		}
		pDC->SetTextAlign(TA_LEFT);
		break;
	}
	default:
		break;
	}
}
int CHUDDlg::dynDraw(CDC* pDC, CRect rect, double vel, int height, double head, double dehead, double roll, double pitch, double tardt, int grounddt, int falltime, int timetogo, int cx, int cy, int spx, int spy, int spz)
{
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap memBm;
	memBm.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	memDC.SelectObject(&memBm);

	CBrush newBrush;              //黑色背景
	newBrush.CreateSolidBrush(RGB(0, 0, 0));
	CBrush *pOldBrush = memDC.SelectObject(&newBrush);
	memDC.FillRect(CRect(0, 0, 700, 700), &newBrush);
	newBrush.DeleteObject();

	CPen newPen;   //白框
	newPen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	CPen *pOldPen = memDC.SelectObject(&newPen);
	memDC.Arc(55, 50, 135, 130, 95, 50, 55, 90);
	memDC.MoveTo(55, 90);
	memDC.LineTo(55, 610);
	memDC.Arc(55, 570, 135, 650, 55, 610, 95, 650);
	memDC.Arc(575, 50, 655, 130, 655, 90, 615, 50);
	memDC.MoveTo(655, 90);
	memDC.LineTo(655, 610);
	memDC.Arc(575, 570, 655, 650, 615, 650, 655, 610);
	newPen.DeleteObject();

	newPen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));    //瞄准环
	memDC.SelectObject(&newPen);
	memDC.Ellipse(345, 340, 365, 360);
	memDC.MoveTo(355, 340);
	memDC.LineTo(355, 330);
	memDC.MoveTo(345, 350);
	memDC.LineTo(335, 350);
	memDC.MoveTo(365, 350);
	memDC.LineTo(375, 350);

	memDC.MoveTo(355, 205);      //飞机纵轴  
	memDC.LineTo(347, 220);
	memDC.MoveTo(355, 205);
	memDC.LineTo(363, 220);
	memDC.MoveTo(347, 220);
	memDC.LineTo(339, 205);
	memDC.MoveTo(363, 220);
	memDC.LineTo(371, 205);
	memDC.MoveTo(339, 205);
	memDC.LineTo(325, 205);
	memDC.MoveTo(371, 205);
	memDC.LineTo(385, 205);

	CString str;
	switch (nSel)
	{
	case 0:
	{
		velDraw(vel, &memDC);
		heightDraw(height, &memDC);
		int turn = headDraw(head, dehead, &memDC);
		rollDraw(355, 350, &memDC);
		trianDraw(roll, 355, 350, &memDC);
		pitchDraw(roll, pitch, 355, 350, &memDC);
		stpInfoDraw(grounddt, spz, &memDC);
		timeDraw(timetogo, &memDC);
		targetDraw(turn, height, cx, cy, spx, spy, spz, grounddt, head, dehead, &memDC);
		modeFalltimeDraw(nSel, falltime, &memDC);

		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
		memBm.DeleteObject();
		memDC.DeleteDC();
		return turn;
		break;
	}
	case 1:
	{
		velDraw(vel, &memDC);
		heightDraw(height, &memDC);
		int turn = headDraw(head, dehead, &memDC);
		pitchDraw(roll, pitch, 355, 350, &memDC);
		stpInfoDraw(grounddt, spz, &memDC);
		timeDraw(timetogo, &memDC);
		reticleDraw(355, 550, 60, tardt - released + grounddt, minrelrange, roll, &memDC);
		aslDraw(vel, roll, timetogo, turn, head, dehead, grounddt, &memDC);
		modeFalltimeDraw(nSel, falltime, &memDC);
		flash(falltime, &memDC);

		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
		memBm.DeleteObject();
		memDC.DeleteDC();
		return turn;
		break;
	}
	}	
}
void CHUDDlg::OnTimer(UINT_PTR nIDEvent)
{
	CRect rect;
	CWnd *pWnd = (CWnd*)GetDlgItem(IDC_DRAW);
	pWnd->GetClientRect(&rect);
	CDC* pDC = GetDlgItem(IDC_DRAW)->GetDC();
	int turn;
	switch (nIDEvent)
	{
	case 1://1号定时器：航向角标尺向航路点航向移动
		turn = dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		if (!turn)
		{
			KillTimer(1);
			SetTimer(3, 80, NULL);
			break;
		}
		if (turn>0)
			m_head++;
		else if (turn<0)
			m_head--;
		break;
	case 2://2号定时器：向一侧转弯时横滚角变化
		if (20 == abs(static_cast<int>(m_roll)))
		{
			KillTimer(2);
			break;
		}
		turn = dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		if (turn>0)
			m_roll++;
		else if (turn<0)
			m_roll--;
		break;
	case 3://3号定时器：航向对准后横滚角回正
		if (0 == static_cast<int>(m_roll))
		{
			KillTimer(3);
			break;
		}
		dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		if (m_roll>0)
			m_roll--;
		else if (m_roll<0)
			m_roll++;
		break;
	case 4://4号定时器：向航路点高度爬升或下降
		if (m_spz == m_heig)
		{
			KillTimer(4);
			flag4 = FALSE;
			SetTimer(6, 40, NULL);
			break;
		}
		dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		if ((m_heig < m_spz) && ((m_spz - m_heig) >= 10))
			m_heig += 10;
		else if ((m_heig < m_spz) && ((m_spz - m_heig) < 10))
			m_heig++;
		else if ((m_heig > m_spz) && ((m_heig - m_spz) >= 10))
			m_heig -= 10;
		else if ((m_heig > m_spz) && ((m_heig - m_spz) < 10))
			m_heig--;
		break;
	case 5://5号定时器：爬升或下降时俯仰角变化
		if (30 == abs(static_cast<int>(m_pitch)))
		{
			KillTimer(5);
			break;
		}
		dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		if (m_heig<m_spz)
			m_pitch++;
		else if (m_heig>m_spz)
			m_pitch--;
		break;
	case 6://6号定时器：达到航路点高度时俯仰角回正
		dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		if (0 == static_cast<int>(m_pitch))
		{
			KillTimer(6);
			break;
		}
		if (m_pitch > 0)
			m_pitch--;
		else if (m_pitch < 0)
			m_pitch++;
		break;
	case 7://7号定时器：和目标的地面距离变化
		dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		m_grdt -= 50;
		break;
	case 8://8号定时器：航路点的current time、time to go、time on target
		dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		m_ttg--;
		if (!m_ttg)
			KillTimer(8);
		break;
	case 9://距离瞄准、MRS
		if (m_grdt < 10)
		{
			KillTimer(9);
			flag9 = FALSE;
			SetTimer(10, 1000, NULL);
			break;
		}
		dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		m_grdt -= 10;
		break;
	case 10://炸弹下落时间倒计时
		dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		m_falltime--;
		if (!m_falltime)
		{
			KillTimer(10);
			SetTimer(11, 1000, NULL);
			break;
		}
		break;
	case 11://炸弹命中后闪烁
		dynDraw(pDC, rect, m_vel, m_heig, m_head, m_dehd, m_roll, m_pitch, m_tardt, m_grdt, m_falltime, m_ttg, m_cx, m_cy, m_spx, m_spy, m_spz);
		if (nFlash > 100)
		{
			KillTimer(11);
			break;
		}
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CHUDDlg::OnBnClickedDefaultnav()
{
	m_vel = 200;
	m_head = 0;
	m_roll = 0;
	m_pitch = 0;
	m_cx = 0;
	m_cy = 0;
	m_heig = 1000;
	m_spx = 1000;
	m_spy = 2000;
	m_spz = 2000;
	m_dehd = deheadCompute(m_cx, m_cy, m_spx, m_spy);
	m_grdt = sqrt(pow((m_spy - m_cy), 2) + pow((m_spx - m_cx), 2));
	m_tardt = sqrt(pow((m_spy - m_cy), 2) + pow((m_spx - m_cx), 2));
	m_ttg = max(m_grdt / m_vel, ceil(abs(m_heig - m_spz) / 100));
	ttg = m_ttg;
	ct = CTime::GetCurrentTime();
	flag4 = TRUE;
	flag7 = TRUE;

	SetDlgItemText(IDC_CX, _T("0"));
	SetDlgItemText(IDC_CY, _T("0"));
	SetDlgItemText(IDC_HEIGHT, _T("1000"));
	SetDlgItemText(IDC_SPX, _T("1000"));
	SetDlgItemText(IDC_SPY, _T("2000"));
	SetDlgItemText(IDC_SPZ, _T("2000"));
	SetDlgItemText(IDC_AIRSPEED, _T("200"));
	SetDlgItemText(IDC_HEAD, _T("0"));
	SetDlgItemText(IDC_PITCH, _T("0"));
	SetDlgItemText(IDC_ROLL, _T("0"));
	SetDlgItemText(IDC_WINDSPEED, _T("10"));
	SetDlgItemText(IDC_WINDHEAD, _T("20"));
	SetDlgItemText(IDC_GHEIGHT, _T("200"));
}

void CHUDDlg::OnBnClickedDefaultccrp()
{
	m_vel = 200;
	m_head = 0;
	m_roll = 0;
	m_pitch = 0;
	m_cx = 0;
	m_cy = 0;
	m_heig = 1000;
	m_spx = 5000;
	m_spy = 5000;
	m_spz = 1000;
	m_uwind = 10;
	m_ewind = 20;
	m_tardt = sqrt(pow((m_spy - m_cy), 2) + pow((m_spx - m_cx), 2));
	double K = m_head;
	CcrpAim ccrp;
	ccrp.CCRP(m_heig, m_vel, omega, K, m_uwind, m_ewind, deltat, h, m_spx, m_spy, m_cx, m_cy);
	int nAimidx = ccrp.output_aim();
	int nCount = ccrp.output_index();
	double xrelease = ccrp.output_xbomb(nAimidx);
	double yrelease = ccrp.output_zbomb(nAimidx);
	m_dehd = K;
	m_falltime = ccrp.output_stdtime();
	m_grdt = sqrt(pow((yrelease - m_cy), 2) + pow((xrelease - m_cx), 2));
	released = m_grdt;
	m_ttg = max(m_grdt / m_vel, ceil(abs(m_heig - m_spz) / 100));
	ttg = m_ttg;
	ct = CTime::GetCurrentTime();

	SetDlgItemText(IDC_CX, _T("0"));
	SetDlgItemText(IDC_CY, _T("0"));
	SetDlgItemText(IDC_HEIGHT, _T("1000"));
	SetDlgItemText(IDC_SPX, _T("5000"));
	SetDlgItemText(IDC_SPY, _T("5000"));
	SetDlgItemText(IDC_SPZ, _T("1000"));
	SetDlgItemText(IDC_AIRSPEED, _T("200"));
	SetDlgItemText(IDC_HEAD, _T("0"));
	SetDlgItemText(IDC_PITCH, _T("0"));
	SetDlgItemText(IDC_ROLL, _T("0"));
	SetDlgItemText(IDC_WINDSPEED, _T("10"));
	SetDlgItemText(IDC_WINDHEAD, _T("20"));
	SetDlgItemText(IDC_GHEIGHT, _T("200"));
}

void CHUDDlg::OnBnClickedStart()
{
	nSel = m_combo.GetCurSel();
	switch (nSel)
	{
	case 0:
	{
		ct = CTime::GetCurrentTime();
		SetTimer(1, 80, NULL);
		SetTimer(2, 80, NULL);
		SetTimer(4, 80, NULL);
		SetTimer(5, 50, NULL);
		SetTimer(7, static_cast<int>(1000 / (m_vel / 50)), NULL);
		SetTimer(8, 1000, NULL);
		break;
	}
	case 1:
	{
		CString str;
		CcrpAim ccrp;
		double K = m_head;
		ccrp.CCRP(m_heig, m_vel, omega, K, m_uwind, m_ewind, deltat, h, m_spx, m_spy, m_cx, m_cy);
		int nAimidx = ccrp.output_aim();
		int nCount = ccrp.output_index();
		double xrelease = ccrp.output_xbomb(nAimidx);
		double yrelease = ccrp.output_zbomb(nAimidx);
		ct = CTime::GetCurrentTime();

		str.Format(_T("%d"), m_falltime);
		SetDlgItemText(IDC_STDTIME, str);
		str.Format(_T("%.2lf"), m_dehd);
		SetDlgItemText(IDC_KRELEASE, str);
		str.Format(_T("%.2lf"), xrelease);
		SetDlgItemText(IDC_XRELEASE, str);
		str.Format(_T("%.2lf"), yrelease);
		SetDlgItemText(IDC_YRELEASE, str);
		str.Format(_T("%d"), m_heig);
		SetDlgItemText(IDC_ZRELEASE, str);
		str.Format(_T("%.2lf"), ccrp.output_xbomb(nCount));
		SetDlgItemText(IDC_XCOMPACT, str);
		str.Format(_T("%.2lf"), ccrp.output_zbomb(nCount));
		SetDlgItemText(IDC_YCOMPACT, str);

		SetTimer(1, 80, NULL);
		SetTimer(2, 80, NULL);
		SetTimer(4, 80, NULL);
		SetTimer(5, 50, NULL);
		SetTimer(8, 1000, NULL);
		SetTimer(9, static_cast<int>(1000 / (m_vel / 9)), NULL);
		break;
	}
	}
}

void CHUDDlg::OnBnClickedStop()
{
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);
	KillTimer(4);
	KillTimer(5);
	KillTimer(6);
	KillTimer(7);
	KillTimer(8);
	KillTimer(9);
	KillTimer(10);
	KillTimer(11);
}

void CHUDDlg::OnBnClickedUpdate()
{
	CDC *pDC = GetDlgItem(IDC_DRAW)->GetDC();
	CBrush newBrush;              //黑色背景
	newBrush.CreateSolidBrush(RGB(0, 0, 0));
	CBrush *pOldBrush = pDC->SelectObject(&newBrush);
	pDC->FillRect(CRect(0, 0, 700, 700), &newBrush);
	newBrush.DeleteObject();

	CPen newPen;   //白框
	newPen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	CPen *pOldPen = pDC->SelectObject(&newPen);
	pDC->Arc(55, 50, 135, 130, 95, 50, 55, 90);
	pDC->MoveTo(55, 90);
	pDC->LineTo(55, 610);
	pDC->Arc(55, 570, 135, 650, 55, 610, 95, 650);
	pDC->Arc(575, 50, 655, 130, 655, 90, 615, 50);
	pDC->MoveTo(655, 90);
	pDC->LineTo(655, 610);
	pDC->Arc(575, 570, 655, 650, 615, 650, 655, 610);
	newPen.DeleteObject();

	newPen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));    //瞄准环
	pDC->SelectObject(&newPen);
	pDC->Ellipse(345, 340, 365, 360);
	pDC->MoveTo(355, 340);
	pDC->LineTo(355, 330);
	pDC->MoveTo(345, 350);
	pDC->LineTo(335, 350);
	pDC->MoveTo(365, 350);
	pDC->LineTo(375, 350);

	pDC->MoveTo(355, 205);      //飞机纵轴  
	pDC->LineTo(347, 220);
	pDC->MoveTo(355, 205);
	pDC->LineTo(363, 220);
	pDC->MoveTo(347, 220);
	pDC->LineTo(339, 205);
	pDC->MoveTo(363, 220);
	pDC->LineTo(371, 205);
	pDC->MoveTo(339, 205);
	pDC->LineTo(325, 205);
	pDC->MoveTo(371, 205);
	pDC->LineTo(385, 205);

	nSel = m_combo.GetCurSel();
	switch (nSel)
	{
	case 0:
	{
		velDraw(m_vel, pDC);//速度标尺
		heightDraw(m_heig, pDC);//高度标尺  
		int turn = headDraw(m_head, m_dehd, pDC);//航向角标尺
		pitchDraw(m_roll, m_pitch, 355, 350, pDC);//俯仰角标尺
		rollDraw(355, 350, pDC);  //横滚角标尺
		trianDraw(m_roll, 355, 350, pDC);
		stpInfoDraw(m_grdt, m_spz, pDC);
		timeDraw(m_ttg, pDC);
		targetDraw(turn, m_heig, m_cx, m_cy, m_spx, m_spy, m_spz, m_grdt, m_head, m_dehd, pDC);
		modeFalltimeDraw(nSel, m_falltime, pDC);
		break;
	}
	case 1:
	{
		velDraw(m_vel, pDC);//速度标尺
		heightDraw(m_heig, pDC);//高度标尺  
		int turn = headDraw(m_head, m_dehd, pDC);//航向角标尺
		pitchDraw(m_roll, m_pitch, 355, 350, pDC);//俯仰角标尺
		stpInfoDraw(m_grdt, m_spz, pDC);
		timeDraw(m_ttg, pDC);
		reticleDraw(355, 550, 60, m_tardt - released + m_grdt, minrelrange, m_roll, pDC);
		aslDraw(m_vel, m_roll, m_ttg, turn, m_head, m_dehd, m_grdt, pDC);
		modeFalltimeDraw(nSel, m_falltime, pDC);
		break;
	}
	}
}

void CHUDDlg::OnBnClickedLoad()
{
	CString str;
	GetDlgItemText(IDC_CX, str);
	m_cx = (int)_ttoi(str);
	GetDlgItemText(IDC_CY, str);
	m_cy = (int)_ttoi(str);
	GetDlgItemText(IDC_HEIGHT, str);
	m_heig = (int)_ttoi(str);
	GetDlgItemText(IDC_SPX, str);
	m_spx = (int)_ttoi(str);
	GetDlgItemText(IDC_SPY, str);
	m_spy = (int)_ttoi(str);
	GetDlgItemText(IDC_SPZ, str);
	m_spz = (int)_ttoi(str);
	GetDlgItemText(IDC_AIRSPEED, str);
	m_vel = (double)_ttof(str);
	GetDlgItemText(IDC_HEAD, str);
	m_head = (double)_ttof(str);
	GetDlgItemText(IDC_PITCH, str);
	m_pitch = (double)_ttof(str);
	GetDlgItemText(IDC_ROLL, str);
	m_roll = (double)_ttof(str);

	nSel = m_combo.GetCurSel();
	switch (nSel)
	{
	case 0:
	{
		m_tardt = sqrt(pow((m_spy - m_cy), 2) + pow((m_spx - m_cx), 2));
		m_dehd = deheadCompute(m_cx, m_cy, m_spx, m_spy);
		m_grdt = sqrt(pow((m_spy - m_cy), 2) + pow((m_spx - m_cx), 2));
		m_ttg = max(m_grdt / m_vel, ceil(abs(m_heig - m_spz) / 100));
		ttg = m_ttg;
		ct = CTime::GetCurrentTime();
		flag4 = TRUE;
		flag7 = TRUE;
		break;
	}
	case 1:
	{
		m_tardt = sqrt(pow((m_spy - m_cy), 2) + pow((m_spx - m_cx), 2));
		double K = m_head;
		CcrpAim ccrp;
		ccrp.CCRP(m_heig, m_vel, omega, K, m_uwind, m_ewind, deltat, h, m_spx, m_spy, m_cx, m_cy);
		int nAimidx = ccrp.output_aim();
		int nCount = ccrp.output_index();
		double xrelease = ccrp.output_xbomb(nAimidx);
		double yrelease = ccrp.output_zbomb(nAimidx);
		m_dehd = K;
		m_falltime = ccrp.output_stdtime();
		m_grdt = sqrt(pow((yrelease - m_cy), 2) + pow((xrelease - m_cx), 2));
		released = m_grdt;
		m_ttg = max(m_grdt / m_vel, ceil(abs(m_heig - m_spz) / 100));
		ttg = m_ttg;
		ct = CTime::GetCurrentTime();
		flag9 = TRUE;
		nFlash = 1;
		break;
	}
	default:
		break;
	}
}

