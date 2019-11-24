
// HUDDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CHUDDlg 对话框
class CHUDDlg : public CDialogEx
{
// 构造
public:
	CHUDDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HUD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_combo;
	double	m_vel;	//空速
	int		m_heig;	//气压高度
	double	m_head;	//航向角
	double	m_roll;	//横滚角
	double	m_pitch;	//俯仰角
	int		m_spx;	//航路点坐标
	int		m_spy;
	int		m_spz;	//航路点海拔
	int		m_cx;	//当前坐标
	int		m_cy;
	double	m_dehd;	//目标航路点航向
	int		m_grdt;	//与目标航路点的地面距离(CCRP中为投放前的行进距离)
	int		m_ttg;	//到达下个航路点所需时间
	int		m_tardt;	//与目标的距离
	double	m_uwind;	//风速
	double	m_ewind;	//风向角
	int		m_falltime;//炸弹下落时间
	/*功能：将非标准的航向角输入转化成[0,360)范围
	输入：init:非标准的航向角输入
	输出：转化成[0, 360)范围的航向角*/
	int valTransfer(int init);
	/*功能：绘制目标航向的双竖线
	输入：x,y:航向角标尺上目标航向点的坐标 pDC:设备上下文指针*/
	void dbVerLineDraw(double x, double y, CDC* pDC);
	/*功能：绘制指向左侧的目标航向箭头
	输入：x,y:箭头尖端的坐标 pDC:设备上下文指针*/
	void lArrowDraw(double x, double y, CDC* pDC);
	/*功能：绘制指向右侧的目标航向箭头
	输入：x,y:箭头尖端的坐标 pDC:设备上下文指针*/
	void rArrowDraw(double x, double y, CDC* pDC);
	/*功能：某一时刻加上经过时间所得的时刻
	输入：time:初始时刻 increment:经过时间
	输出：返回所得时间写成时：分：秒形式的字符串*/
	CString timeTransfer(CTime time, int increment);
	/*功能：计算目标航路点航向角
	输入：cux、cuy:当前坐标 spx、spy:目标航路点坐标
	输出：目标航路点航向角*/
	double deheadCompute(int cux, int cuy, int spx, int spy);
	/*功能：绘制目标指向线
	输入：startx,starty:指向线起点 endx,endx:指向远端的终点 length:线长度 pDC:设备上下文指针*/
	void tarIndicateLineDraw(int startx, int starty, int endx, int endy, int length, CDC* pDC);

public:
	/*功能：绘制速度标尺
	输入：vel:空速 pDC:设备上下文指针*/
	void velDraw(double vel, CDC* pDC);
	/*功能：绘制高度标尺
	输入：height:气压高度 pDC:设备上下文指针*/
	void heightDraw(int height, CDC* pDC);
	/*功能：绘制航向角标尺
	输入：path:航向角（角度） depath:目标航路点航向角 pDC:设备上下文指针
	输出：向目标航路点转向的转向信息，1：右转，-1：左转，0：航向已对准*/
	int headDraw(double path, double depath, CDC* pDC);
	/*功能：绘制横滚角标尺的圆弧刻线
	输入：x，y:平显中心点坐标（圆心） pDC:设备上下文指针*/
	void rollDraw(int x, int y, CDC* pDC);
	/*功能：根据横滚角大小绘制横滚角标尺上的三角形
	输入：roll:横滚角（角度） x，y:平显中心点坐标（圆心） pDC:设备上下文指针*/
	void trianDraw(double roll, int x, int y, CDC* pDC);
	/*功能：绘制俯仰角标尺中的某一条刻线
	输入：ra:横滚角（角度）范围[0,360) pb:本条刻线上的俯仰角（角度）范围[-180,0][0,180]
	x,y:平显中心点坐标 l:刻度线长 pDC:设备上下文指针 pFont:获取的当前默认字体指针*/
	void pitSinDraw(int ra, int pb, int x, int y, int l, CDC* pDC);
	/*功能：绘制俯仰角标尺的所有刻线
	输入：roll:横滚角（角度） pitch:本条刻线上的俯仰角（角度) x,y:平显中心点坐标
	pDC:设备上下文指针 pFont:获取的当前默认字体指针*/
	void pitchDraw(double roll, double pitch, int x, int y, CDC* pDC);
	/*功能：显示目标航路点地面距离、高度信息
	输入：groungdt:目标航路点的地面距离 stpz:目标航路点的高度 pDC:设备上下文指针*/
	void stpInfoDraw(int grounddt, int stpz, CDC* pDC);
	/*功能：显示目标航路点TTG、TOT等时间信息
	输入：timetogo:以当前速度到达目标航路点所需时间 pDC:设备上下文指针*/
	void timeDraw(int timetogo, CDC* pDC);
	/*功能：绘制目标航路点符号与指向线
	输入：pDC:设备上下文指针 turn:向目标航路点转向的转向信息，1：右转，-1：左转，0：航向已对准
	cx,cy:当前坐标 stpx,stpy,stpz:航路点坐标 grounddt:与目标航路点的地面距离 head:航向角 dehead:目标航路点航向*/
	void targetDraw(int turn, int height, int cx, int cy, int stpx, int stpy, int stpz, int grounddt, double head, double dehead, CDC* pDC);
	/*功能：绘制CCRP瞄准环
	输入：pDC:设备上下文指针 cenx、ceny:瞄准环中心坐标 radius:瞄准环半径 tarrange:与目标距离 minrange:最小投放距离 roll:横滚角*/
	void reticleDraw(int cenx, int ceny, int radius, double tarrange, double minrange, double roll, CDC* pDC);
	/*功能：绘制方位操纵线
	输入：pDC:设备上下文指针 timetogo:到达投放点时间 relrange:到达投放点距离*/
	void aslDraw(double vel, double roll, int timetogo, int turn, double head, double dehead, double relrange, CDC* pDC);
	/*功能：绘制距离瞄准符号(solution cue)
	输入：pDC:设备上下文指针 xtop、ytop:方位操纵线顶端坐标*/
	void solutioncueDraw(double vel, double roll, int timetogo, double xtop, double ytop, double relrange, CDC* pDC);
	/*功能：左下角显示模式和炸弹下落时间
	输入：pDC:设备上下文指针 nsel:combobox当前序列号 falltime:炸弹下落时间*/
	void modeFalltimeDraw(int nsel, int falltime, CDC* pDC);
	/*功能：炸弹命中后闪烁
	输入：pDC:设备上下文指针 falltime:炸弹下落时间*/
	void flash(int falltime, CDC* pDC);
	/*功能：双缓冲绘制整个平显画面
	输入：pDC:设备上下文指针 rect:平显画面的区域 其余为飞机、目标航路点信息
	输出：向目标航路点转向的转向信息，1：右转，-1：左转，0：航向已对准*/
	int dynDraw(CDC* pDC, CRect rect, double vel, int height, double head, double dehead, double roll, double pitch, double tardt,
		int grounddt, int falltime, int timetogo, int cx, int cy, int spx, int spy, int spz);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedDefaultnav();
	afx_msg void OnBnClickedDefaultccrp();
};
