
// HUDDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CHUDDlg �Ի���
class CHUDDlg : public CDialogEx
{
// ����
public:
	CHUDDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HUD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_combo;
	double	m_vel;	//����
	int		m_heig;	//��ѹ�߶�
	double	m_head;	//�����
	double	m_roll;	//�����
	double	m_pitch;	//������
	int		m_spx;	//��·������
	int		m_spy;
	int		m_spz;	//��·�㺣��
	int		m_cx;	//��ǰ����
	int		m_cy;
	double	m_dehd;	//Ŀ�꺽·�㺽��
	int		m_grdt;	//��Ŀ�꺽·��ĵ������(CCRP��ΪͶ��ǰ���н�����)
	int		m_ttg;	//�����¸���·������ʱ��
	int		m_tardt;	//��Ŀ��ľ���
	double	m_uwind;	//����
	double	m_ewind;	//�����
	int		m_falltime;//ը������ʱ��
	/*���ܣ����Ǳ�׼�ĺ��������ת����[0,360)��Χ
	���룺init:�Ǳ�׼�ĺ��������
	�����ת����[0, 360)��Χ�ĺ����*/
	int valTransfer(int init);
	/*���ܣ�����Ŀ�꺽���˫����
	���룺x,y:����Ǳ����Ŀ�꺽�������� pDC:�豸������ָ��*/
	void dbVerLineDraw(double x, double y, CDC* pDC);
	/*���ܣ�����ָ������Ŀ�꺽���ͷ
	���룺x,y:��ͷ��˵����� pDC:�豸������ָ��*/
	void lArrowDraw(double x, double y, CDC* pDC);
	/*���ܣ�����ָ���Ҳ��Ŀ�꺽���ͷ
	���룺x,y:��ͷ��˵����� pDC:�豸������ָ��*/
	void rArrowDraw(double x, double y, CDC* pDC);
	/*���ܣ�ĳһʱ�̼��Ͼ���ʱ�����õ�ʱ��
	���룺time:��ʼʱ�� increment:����ʱ��
	�������������ʱ��д��ʱ���֣�����ʽ���ַ���*/
	CString timeTransfer(CTime time, int increment);
	/*���ܣ�����Ŀ�꺽·�㺽���
	���룺cux��cuy:��ǰ���� spx��spy:Ŀ�꺽·������
	�����Ŀ�꺽·�㺽���*/
	double deheadCompute(int cux, int cuy, int spx, int spy);
	/*���ܣ�����Ŀ��ָ����
	���룺startx,starty:ָ������� endx,endx:ָ��Զ�˵��յ� length:�߳��� pDC:�豸������ָ��*/
	void tarIndicateLineDraw(int startx, int starty, int endx, int endy, int length, CDC* pDC);

public:
	/*���ܣ������ٶȱ��
	���룺vel:���� pDC:�豸������ָ��*/
	void velDraw(double vel, CDC* pDC);
	/*���ܣ����Ƹ߶ȱ��
	���룺height:��ѹ�߶� pDC:�豸������ָ��*/
	void heightDraw(int height, CDC* pDC);
	/*���ܣ����ƺ���Ǳ��
	���룺path:����ǣ��Ƕȣ� depath:Ŀ�꺽·�㺽��� pDC:�豸������ָ��
	�������Ŀ�꺽·��ת���ת����Ϣ��1����ת��-1����ת��0�������Ѷ�׼*/
	int headDraw(double path, double depath, CDC* pDC);
	/*���ܣ����ƺ���Ǳ�ߵ�Բ������
	���룺x��y:ƽ�����ĵ����꣨Բ�ģ� pDC:�豸������ָ��*/
	void rollDraw(int x, int y, CDC* pDC);
	/*���ܣ����ݺ���Ǵ�С���ƺ���Ǳ���ϵ�������
	���룺roll:����ǣ��Ƕȣ� x��y:ƽ�����ĵ����꣨Բ�ģ� pDC:�豸������ָ��*/
	void trianDraw(double roll, int x, int y, CDC* pDC);
	/*���ܣ����Ƹ����Ǳ���е�ĳһ������
	���룺ra:����ǣ��Ƕȣ���Χ[0,360) pb:���������ϵĸ����ǣ��Ƕȣ���Χ[-180,0][0,180]
	x,y:ƽ�����ĵ����� l:�̶��߳� pDC:�豸������ָ�� pFont:��ȡ�ĵ�ǰĬ������ָ��*/
	void pitSinDraw(int ra, int pb, int x, int y, int l, CDC* pDC);
	/*���ܣ����Ƹ����Ǳ�ߵ����п���
	���룺roll:����ǣ��Ƕȣ� pitch:���������ϵĸ����ǣ��Ƕ�) x,y:ƽ�����ĵ�����
	pDC:�豸������ָ�� pFont:��ȡ�ĵ�ǰĬ������ָ��*/
	void pitchDraw(double roll, double pitch, int x, int y, CDC* pDC);
	/*���ܣ���ʾĿ�꺽·�������롢�߶���Ϣ
	���룺groungdt:Ŀ�꺽·��ĵ������ stpz:Ŀ�꺽·��ĸ߶� pDC:�豸������ָ��*/
	void stpInfoDraw(int grounddt, int stpz, CDC* pDC);
	/*���ܣ���ʾĿ�꺽·��TTG��TOT��ʱ����Ϣ
	���룺timetogo:�Ե�ǰ�ٶȵ���Ŀ�꺽·������ʱ�� pDC:�豸������ָ��*/
	void timeDraw(int timetogo, CDC* pDC);
	/*���ܣ�����Ŀ�꺽·�������ָ����
	���룺pDC:�豸������ָ�� turn:��Ŀ�꺽·��ת���ת����Ϣ��1����ת��-1����ת��0�������Ѷ�׼
	cx,cy:��ǰ���� stpx,stpy,stpz:��·������ grounddt:��Ŀ�꺽·��ĵ������ head:����� dehead:Ŀ�꺽·�㺽��*/
	void targetDraw(int turn, int height, int cx, int cy, int stpx, int stpy, int stpz, int grounddt, double head, double dehead, CDC* pDC);
	/*���ܣ�����CCRP��׼��
	���룺pDC:�豸������ָ�� cenx��ceny:��׼���������� radius:��׼���뾶 tarrange:��Ŀ����� minrange:��СͶ�ž��� roll:�����*/
	void reticleDraw(int cenx, int ceny, int radius, double tarrange, double minrange, double roll, CDC* pDC);
	/*���ܣ����Ʒ�λ������
	���룺pDC:�豸������ָ�� timetogo:����Ͷ�ŵ�ʱ�� relrange:����Ͷ�ŵ����*/
	void aslDraw(double vel, double roll, int timetogo, int turn, double head, double dehead, double relrange, CDC* pDC);
	/*���ܣ����ƾ�����׼����(solution cue)
	���룺pDC:�豸������ָ�� xtop��ytop:��λ�����߶�������*/
	void solutioncueDraw(double vel, double roll, int timetogo, double xtop, double ytop, double relrange, CDC* pDC);
	/*���ܣ����½���ʾģʽ��ը������ʱ��
	���룺pDC:�豸������ָ�� nsel:combobox��ǰ���к� falltime:ը������ʱ��*/
	void modeFalltimeDraw(int nsel, int falltime, CDC* pDC);
	/*���ܣ�ը�����к���˸
	���룺pDC:�豸������ָ�� falltime:ը������ʱ��*/
	void flash(int falltime, CDC* pDC);
	/*���ܣ�˫�����������ƽ�Ի���
	���룺pDC:�豸������ָ�� rect:ƽ�Ի�������� ����Ϊ�ɻ���Ŀ�꺽·����Ϣ
	�������Ŀ�꺽·��ת���ת����Ϣ��1����ת��-1����ת��0�������Ѷ�׼*/
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
