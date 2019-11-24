#pragma once

#define MAXCOUNT 20000	//��¼���������������
#define PI 3.1415926		//Բ����
#define G 9.806			//�������ٶ�
#define RON 1.225			//����������ر�׼ֵ
#define TON 288.4			//��׼�����¶�(K)
#define HON 760			//��ƽ���׼��ѹ(mmHg)
#define R 29.27			//���峣��
#define GG 5.862e-3		//�¶��ݶ�
#define STDRESC 0.659		//��׼���������ϵ��
#define RESC 0.16			//ָ�����������ϵ��
#define WEIGHT 216.5		//���أ�kg��
#define CALIBER 0.299		//���ھ���m��

class CcrpAim
{
public:
	CcrpAim();
	~CcrpAim();

	/*���ܣ�ʵ��CCRP��ع��������д���׼�������ٵ����������ȫ���̣���¼����ȫ�̵�������͸��׶ε��������
	  ������height���߶�  v���ɻ�����  omega���ɻ�ת����ٶ�  K���ɻ������  u������  e�����ٺ����  deltat����������  
	        h������������沽��  mxeInit����ʼʱ��Ŀ���ڵ�������ϵ�е�x����  myeInit����ʼʱ��Ŀ���ڵ�������ϵ�е�y����  
	        vxeInit����ʼʱ�̷ɻ��ڵ�������ϵ�е�x����  vyeInit����ʼʱ�̷ɻ��ڵ�������ϵ�е�y����*/
	void CCRP(double height, double v, double omega, double& K, double u, double e, double deltat, double h,
		double mxeInit, double myeInit, double vxeInit, double vyeInit);

	/*���ܣ����ը����ĳһx����ֵ
	  ������index��ĳһ��������*/
	double output_xbomb(int index);

	/*���ܣ����ը����ĳһy����ֵ���߶ȣ�
	  ������index��ĳһ��������*/
	double output_ybomb(int index);

	/*���ܣ����ը����ĳһz����ֵ
	  ������index��ĳһ��������*/
	double output_zbomb(int index);

	/*���ܣ����ը���ı�׼���*/
	double output_stdrange();

	/*���ܣ����ը���ı�׼����ʱ��*/
	double output_stdtime();

	/*���ܣ������׼���̵��������*/
	int output_aim();

	/*���ܣ����ȫ�̵����������*/
	int output_index();

	void reset();

private:
	double xeBomb[MAXCOUNT];		//��������ը���˶����꣨��׼�����塢���䣩
	double yeBomb[MAXCOUNT];		//�߶ȷ���
	double zeBomb[MAXCOUNT];
	int nAim;					//��׼�׶��������
	int nFall;					//��������������
	double A0;					//�����׼��̣��޷磩
	double T;					//�����׼����ʱ�䣨�޷磩

	////////////////////////����Ϳ�����������/////////////////
	/*���ܣ����㵯��ϵ��������ֵ
	  ������resC:ָ�����������ϵ��  caliber:���ھ�  weight:����*/
	double trajC(double resc, double caliber, double weight);

	/*���ܣ����㵱ǰ�߶ȵ������¶Ȳ�����ֵ
	  ������height���߶�  y:��ǰ�������*/
	double tao(double height, double y);

	/*���ܣ����㵱ǰ�߶ȵ���ѹ������ֵ
	  ������height���߶�  y:��ǰ�������*/
	double hg(double height, double y);

	/*���ܣ����㵱ǰ�߶ȵı��غ���������ֵ�����������ܸ߶ȵ�Ӱ�죩
	  ������height���߶�  y����ǰ�������*/
	double Hg(double height, double y);

	/////////////////////////���㵯��ı�׼�������޷磩//////////////////////
	/*���ܣ����㵯���ٶȲ�����ֵ
	  ������u:�ٶȵĺ������  w:�ٶȵ��������*/
	double funcv(double u, double w);

	/*���ܣ����㵱ǰ�߶ȵĵ�������ϵ��������ֵ�����������ܸ߶ȵ�Ӱ�죩
	  ������height���߶�  y����ǰ������� u:�ٶȵĺ������  w:�ٶȵ��������*/
	double Cx0(double height, double y, double u, double w);

	/*���ܣ����㵱ǰ�߶ȿ�����������������ֵ�����������ܸ߶ȵ�Ӱ�죩
	  ������height���߶�  y����ǰ������� u:�ٶȵĺ������  w:�ٶȵ��������*/
	double resG(double height, double y, double u, double w);

	/*���ܣ����������˶��������й���u��΢�ַ��̣��Ե�������ʱ��tΪ�Ա���
	  ������height���߶�  y:��ǰ�������  u:�ٶȵĺ������  w:�ٶȵ��������*/
	double funcu(double height, double u, double w, double y);

	/*���ܣ����������˶��������й���w��΢�ַ��̣��Ե�������ʱ��tΪ�Ա���
	  ������height���߶�  y:��ǰ�������  u:�ٶȵĺ������  w:�ٶȵ�������� */
	double funcw(double height, double u, double w, double y);

	/*���ܣ����������˶��������й���x����̣���΢�ַ��̣��Ե�������ʱ��tΪ�Ա���
	  ������u:�ٶȵĺ������*/
	double funcx(double u);

	/*���ܣ����������˶��������й���y��������룩��΢�ַ��̣��Ե�������ʱ��tΪ�Ա���
	  ������w:�ٶȵ��������*/
	double funcy(double w);

	/*���ܣ������Ľ�������������ը����΢�ַ�����,�ó���׼���A0�ͱ�׼����ʱ��T
	  ������height���߶�  tInit��uInit��pInit��xInit��yInit��������Ԫ��ʼֵ  h��ʱ���������*/
	void StdBombTraj(double height, double tInit, double uInit, double pInit, double xInit, double yInit, double h);

	////////////////CCRP������׼�;�����׼�����Ʒɻ��ִ���ȷͶ����/////////////////////////

	/*���ܣ�������ÿ�����������зɻ���Ŀ����еľ����ڵ�������ϵx���ϵķ���������ֵ
	  ������v���ɻ�����  K���ɻ����ٺ����  u������  e�����ٺ����  deltat����������*/
	double LxeVeh(double v, double K, double u, double e, double deltat);

	/*���ܣ�������ÿ�����������зɻ���Ŀ����еľ����ڵ�������ϵy���ϵķ���������ֵ
	  ������v���ɻ�����  K���ɻ����ٺ����  u������  e�����ٺ����  deltat����������*/
	double LyeVeh(double v, double K, double u, double e, double deltat);

	/*���ܣ�����������ϵ�зɻ���Ŀ�����ʸ��ͶӰ����������ϵ�У�����x���ϵķ���
	  ������dxe����������ϵ�зɻ���Ŀ�����ʸ����x���ϵķ���  dye����������ϵ�зɻ���Ŀ�����ʸ����y���ϵķ���
	        K���ɻ����ٺ����  alpha��ƫ����*/
	double Dxw(double dxe, double dye, double K, double alpha);

	/*���ܣ�����������ϵ�зɻ���Ŀ�����ʸ��ͶӰ����������ϵ�У�����y���ϵķ���
	  ������dxe����������ϵ�зɻ���Ŀ�����ʸ����x���ϵķ���  dye����������ϵ�зɻ���Ŀ�����ʸ����y���ϵķ���
	        K���ɻ����ٺ����  alpha��ƫ����*/
	double Dyw(double dxe, double dye, double K, double alpha);

	/*���ܣ����㱬ը����������ں�������ϵx���ϵķ���
	  ������u������  e�����ٺ����  alpha��ƫ����*/
	double Axw(double u, double e, double alpha);

	/*���ܣ����㱬ը����������ں�������ϵy���ϵķ���
	  ������u������  e�����ٺ����  alpha��ƫ����*/
	double Ayw(double u, double e, double alpha);

	/*���ܣ�����ɻ��ڷ�����׼�����еĺ���������������ֵ
	  ������K���ɻ������  alpha��ƫ����  dmxe���ɻ���Ŀ�����ʸ���ڵ�������ϵx���ϵķ���  dmye���ɻ���Ŀ�����ʸ���ڵ�������ϵy���ϵķ���
	        ayw����ը����������ں�������ϵy���ϵķ���  dmxw���ɻ���Ŀ�����ʸ���ں�������ϵx���ϵķ���*/
	double DeltaK(double K, double dmxe, double dmye, double alpha, double ayw, double dmxw);

	/*���ܣ�����ɻ��ھ�����׼�����еľ���������������ֵ
	  ������axw����ը����������ں�������ϵx���ϵķ���  ayw����ը����������ں�������ϵy���ϵķ���  
	        dmxw���ɻ���Ŀ�����ʸ���ں�������ϵx���ϵķ���*/
	double DeltaD(double axw, double ayw, double dmxw);

	double valTransfer(double init);

	int turnDirection(double dpa, double cpa);

	double deheadCompute(int cux, int cuy, int spx, int spy);

	/*���ܣ�ʵ��CCRP��׼���̣�ʹ�ɻ�������ȷͶ���㣬��¼�ɻ�����͹켣����nAim
	  ������height���߶�  v���ɻ�����  omega���ɻ�ת����ٶ�  K���ɻ������  u������  e�����ٺ����  alpha��ƫ����  
	        deltat����������  mxeInit����ʼʱ��Ŀ���ڵ�������ϵ�е�x����  myeInit����ʼʱ��Ŀ���ڵ�������ϵ�е�y����  
	        vxeInit����ʼʱ�̷ɻ��ڵ�������ϵ�е�x����  vyeInit����ʼʱ�̷ɻ��ڵ�������ϵ�е�y����*/
	void Aim(double height, double v, double omega, double& K, double u, double& e, double alpha,double deltat,
		double mxeInit, double myeInit, double vxeInit, double vyeInit);

	///////////////////////////����ɻ�Ͷ�������ʵ�ʵ������з磩////////////////////////
	/*���ܣ����㵯���ٶȲ�����ֵ
	  ������u:�ٶȵĺ������(x)  w���ٶȵ��������(y)  p:�ٶȵĲ������(z)*/
	double funcv(double u, double w, double p);

	/*���ܣ����㵱ǰ�߶ȵĵ�������ϵ��������ֵ�����������ܸ߶ȵ�Ӱ�죩
	  ������height���߶�  y����ǰ������� u:�ٶȵĺ������(x)  w���ٶȵ��������(y)  p:�ٶȵĲ������(z)*/
	double Cx0(double height, double y, double u, double w, double p);

	/*���ܣ����㵱ǰ�߶ȿ�����������������ֵ�����������ܸ߶ȵ�Ӱ�죩
	  ������height���߶�  u:�ٶȵĺ������(x)  w���ٶȵ��������(y)  p:�ٶȵĲ������(z)*/
	double resG(double height, double y, double u, double w, double p);

	/*���ܣ����������˶��������й���u��΢�ַ��̣��Ե�������ʱ��tΪ�Ա���
	  ������height���߶�  y:��ǰ�������  u:�ٶȵĺ������(x)  w���ٶȵ��������(y)  p:�ٶȵĲ������(z)*/
	double funcu(double height, double u, double w, double p, double y);

	/*���ܣ����������˶��������й���w��΢�ַ��̣��Ե�������ʱ��tΪ�Ա���
	  ������height���߶�  u:�ٶȵĺ������(x)  w���ٶȵ��������(y)  p:�ٶȵĲ������(z)*/
	double funcw(double height, double u, double w, double p, double y);

	/*���ܣ����������˶��������й���p��΢�ַ��̣��Ե�������ʱ��tΪ�Ա���
	  ������height���߶�  u:�ٶȵĺ������(x)  w���ٶȵ��������(y)  p:�ٶȵĲ������(z)*/
	double funcp(double height, double u, double w, double p, double y);

	/*���ܣ����������˶��������й���z��������̣���΢�ַ��̣��Ե�������ʱ��tΪ�Ա���
	  ������p:�ٶȵĲ������(z)*/
	double funcz(double p);

	/*���ܣ������Ľ�������������ը����΢�ַ�����
	  ������height���߶�  K���ɻ����ٺ����  tInit��uInit��wInit��pInit��xInit��yInit��zInit��������Ԫ��ʼֵ  h��ʱ���������*/
	void RealBombTraj(double height, double K, double tInit, double uInit, double wInit, double pInit, double xInit, double yInit,
		double zInit, double h, double myeInit, double vyeInit);
};

