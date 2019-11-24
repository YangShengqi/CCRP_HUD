#pragma once

#define MAXCOUNT 20000	//记录坐标数组最大容量
#define PI 3.1415926		//圆周率
#define G 9.806			//重力加速度
#define RON 1.225			//地面空气比重标准值
#define TON 288.4			//标准虚拟温度(K)
#define HON 760			//海平面标准气压(mmHg)
#define R 29.27			//气体常数
#define GG 5.862e-3		//温度梯度
#define STDRESC 0.659		//标准弹丸的阻力系数
#define RESC 0.16			//指定弹丸的阻力系数
#define WEIGHT 216.5		//弹重（kg）
#define CALIBER 0.299		//弹口径（m）

class CcrpAim
{
public:
	CcrpAim();
	~CcrpAim();

	/*功能：实现CCRP火控工作过程中从瞄准到俯冲再到弹丸下落的全过程，记录弹丸全程地理坐标和各阶段的坐标点数
	  参数：height：高度  v：飞机空速  omega：飞机转弯角速度  K：飞机航向角  u：风速  e：风速航向角  deltat：步长周期  
	        h：龙格库塔仿真步长  mxeInit：初始时刻目标在地理坐标系中的x坐标  myeInit：初始时刻目标在地理坐标系中的y坐标  
	        vxeInit：初始时刻飞机在地理坐标系中的x坐标  vyeInit：初始时刻飞机在地理坐标系中的y坐标*/
	void CCRP(double height, double v, double omega, double& K, double u, double e, double deltat, double h,
		double mxeInit, double myeInit, double vxeInit, double vyeInit);

	/*功能：输出炸弹的某一x坐标值
	  参数：index：某一坐标索引*/
	double output_xbomb(int index);

	/*功能：输出炸弹的某一y坐标值（高度）
	  参数：index：某一坐标索引*/
	double output_ybomb(int index);

	/*功能：输出炸弹的某一z坐标值
	  参数：index：某一坐标索引*/
	double output_zbomb(int index);

	/*功能：输出炸弹的标准射程*/
	double output_stdrange();

	/*功能：输出炸弹的标准落下时间*/
	double output_stdtime();

	/*功能：输出瞄准过程的坐标点数*/
	int output_aim();

	/*功能：输出全程的坐标点总数*/
	int output_index();

	void reset();

private:
	double xeBomb[MAXCOUNT];		//整个过程炸弹运动坐标（瞄准、俯冲、下落）
	double yeBomb[MAXCOUNT];		//高度方向
	double zeBomb[MAXCOUNT];
	int nAim;					//瞄准阶段坐标点数
	int nFall;					//下落过程坐标点数
	double A0;					//弹丸标准射程（无风）
	double T;					//弹丸标准落下时间（无风）

	////////////////////////弹丸和空气基本参数/////////////////
	/*功能：计算弹道系数并返回值
	  参数：resC:指定弹丸的阻力系数  caliber:弹口径  weight:弹重*/
	double trajC(double resc, double caliber, double weight);

	/*功能：计算当前高度的虚拟温度并返回值
	  参数：height：高度  y:当前下落距离*/
	double tao(double height, double y);

	/*功能：计算当前高度的气压并返回值
	  参数：height：高度  y:当前下落距离*/
	double hg(double height, double y);

	/*功能：计算当前高度的比重函数并返回值（考虑音速受高度的影响）
	  参数：height：高度  y：当前下落距离*/
	double Hg(double height, double y);

	/////////////////////////解算弹丸的标准弹道（无风）//////////////////////
	/*功能：计算弹丸速度并返回值
	  参数：u:速度的横向分量  w:速度的纵向分量*/
	double funcv(double u, double w);

	/*功能：计算当前高度的弹丸阻力系数并返回值（考虑音速受高度的影响）
	  参数：height：高度  y：当前下落距离 u:速度的横向分量  w:速度的纵向分量*/
	double Cx0(double height, double y, double u, double w);

	/*功能：计算当前高度空气阻力函数并返回值（考虑音速受高度的影响）
	  参数：height：高度  y：当前下落距离 u:速度的横向分量  w:速度的纵向分量*/
	double resG(double height, double y, double u, double w);

	/*功能：弹丸质心运动方程组中关于u的微分方程，以弹丸下落时间t为自变量
	  参数：height：高度  y:当前下落距离  u:速度的横向分量  w:速度的纵向分量*/
	double funcu(double height, double u, double w, double y);

	/*功能：弹丸质心运动方程组中关于w的微分方程，以弹丸下落时间t为自变量
	  参数：height：高度  y:当前下落距离  u:速度的横向分量  w:速度的纵向分量 */
	double funcw(double height, double u, double w, double y);

	/*功能：弹丸质心运动方程组中关于x（射程）的微分方程，以弹丸下落时间t为自变量
	  参数：u:速度的横向分量*/
	double funcx(double u);

	/*功能：弹丸质心运动方程组中关于y（下落距离）的微分方程，以弹丸下落时间t为自变量
	  参数：w:速度的纵向分量*/
	double funcy(double w);

	/*功能：采用四阶龙格库塔法解轰炸弹道微分方程组,得出标准射程A0和标准落下时间T
	  参数：height：高度  tInit、uInit、pInit、xInit、yInit：弹道诸元初始值  h：时间迭代步长*/
	void StdBombTraj(double height, double tInit, double uInit, double pInit, double xInit, double yInit, double h);

	////////////////CCRP方向瞄准和距离瞄准，控制飞机抵达正确投弹点/////////////////////////

	/*功能：计算在每个步长周期中飞机向目标飞行的距离在地理坐标系x轴上的分量并返回值
	  参数：v：飞机空速  K：飞机空速航向角  u：风速  e：风速航向角  deltat：步长周期*/
	double LxeVeh(double v, double K, double u, double e, double deltat);

	/*功能：计算在每个步长周期中飞机向目标飞行的距离在地理坐标系y轴上的分量并返回值
	  参数：v：飞机空速  K：飞机空速航向角  u：风速  e：风速航向角  deltat：步长周期*/
	double LyeVeh(double v, double K, double u, double e, double deltat);

	/*功能：将地理坐标系中飞机到目标距离矢量投影到航迹坐标系中，返回x轴上的分量
	  参数：dxe：地理坐标系中飞机到目标距离矢量在x轴上的分量  dye：地理坐标系中飞机到目标距离矢量在y轴上的分量
	        K：飞机风速航向角  alpha：偏流角*/
	double Dxw(double dxe, double dye, double K, double alpha);

	/*功能：将地理坐标系中飞机到目标距离矢量投影到航迹坐标系中，返回y轴上的分量
	  参数：dxe：地理坐标系中飞机到目标距离矢量在x轴上的分量  dye：地理坐标系中飞机到目标距离矢量在y轴上的分量
	        K：飞机风速航向角  alpha：偏流角*/
	double Dyw(double dxe, double dye, double K, double alpha);

	/*功能：计算爆炸点射程向量在航迹坐标系x轴上的分量
	  参数：u：风速  e：风速航向角  alpha：偏流角*/
	double Axw(double u, double e, double alpha);

	/*功能：计算爆炸点射程向量在航迹坐标系y轴上的分量
	  参数：u：风速  e：风速航向角  alpha：偏流角*/
	double Ayw(double u, double e, double alpha);

	/*功能：计算飞机在方向瞄准过程中的航向修正量并返回值
	  参数：K：飞机航向角  alpha：偏流角  dmxe：飞机到目标距离矢量在地理坐标系x轴上的分量  dmye：飞机到目标距离矢量在地理坐标系y轴上的分量
	        ayw：爆炸点射程向量在航迹坐标系y轴上的分量  dmxw：飞机到目标距离矢量在航迹坐标系x轴上的分量*/
	double DeltaK(double K, double dmxe, double dmye, double alpha, double ayw, double dmxw);

	/*功能：计算飞机在距离瞄准过程中的距离修正量并返回值
	  参数：axw：爆炸点射程向量在航迹坐标系x轴上的分量  ayw：爆炸点射程向量在航迹坐标系y轴上的分量  
	        dmxw：飞机到目标距离矢量在航迹坐标系x轴上的分量*/
	double DeltaD(double axw, double ayw, double dmxw);

	double valTransfer(double init);

	int turnDirection(double dpa, double cpa);

	double deheadCompute(int cux, int cuy, int spx, int spy);

	/*功能：实现CCRP瞄准过程，使飞机到达正确投弹点，记录飞机坐标和轨迹点数nAim
	  参数：height：高度  v：飞机空速  omega：飞机转弯角速度  K：飞机航向角  u：风速  e：风速航向角  alpha：偏流角  
	        deltat：步长周期  mxeInit：初始时刻目标在地理坐标系中的x坐标  myeInit：初始时刻目标在地理坐标系中的y坐标  
	        vxeInit：初始时刻飞机在地理坐标系中的x坐标  vyeInit：初始时刻飞机在地理坐标系中的y坐标*/
	void Aim(double height, double v, double omega, double& K, double u, double& e, double alpha,double deltat,
		double mxeInit, double myeInit, double vxeInit, double vyeInit);

	///////////////////////////解算飞机投弹后弹丸的实际弹道（有风）////////////////////////
	/*功能：计算弹丸速度并返回值
	  参数：u:速度的横向分量(x)  w：速度的纵向分量(y)  p:速度的侧向分量(z)*/
	double funcv(double u, double w, double p);

	/*功能：计算当前高度的弹丸阻力系数并返回值（考虑音速受高度的影响）
	  参数：height：高度  y：当前下落距离 u:速度的横向分量(x)  w：速度的纵向分量(y)  p:速度的侧向分量(z)*/
	double Cx0(double height, double y, double u, double w, double p);

	/*功能：计算当前高度空气阻力函数并返回值（考虑音速受高度的影响）
	  参数：height：高度  u:速度的横向分量(x)  w：速度的纵向分量(y)  p:速度的侧向分量(z)*/
	double resG(double height, double y, double u, double w, double p);

	/*功能：弹丸质心运动方程组中关于u的微分方程，以弹丸下落时间t为自变量
	  参数：height：高度  y:当前下落距离  u:速度的横向分量(x)  w：速度的纵向分量(y)  p:速度的侧向分量(z)*/
	double funcu(double height, double u, double w, double p, double y);

	/*功能：弹丸质心运动方程组中关于w的微分方程，以弹丸下落时间t为自变量
	  参数：height：高度  u:速度的横向分量(x)  w：速度的纵向分量(y)  p:速度的侧向分量(z)*/
	double funcw(double height, double u, double w, double p, double y);

	/*功能：弹丸质心运动方程组中关于p的微分方程，以弹丸下落时间t为自变量
	  参数：height：高度  u:速度的横向分量(x)  w：速度的纵向分量(y)  p:速度的侧向分量(z)*/
	double funcp(double height, double u, double w, double p, double y);

	/*功能：弹丸质心运动方程组中关于z（侧向射程）的微分方程，以弹丸下落时间t为自变量
	  参数：p:速度的侧向分量(z)*/
	double funcz(double p);

	/*功能：采用四阶龙格库塔法解轰炸弹道微分方程组
	  参数：height：高度  K：飞机空速航向角  tInit、uInit、wInit、pInit、xInit、yInit、zInit：弹道诸元初始值  h：时间迭代步长*/
	void RealBombTraj(double height, double K, double tInit, double uInit, double wInit, double pInit, double xInit, double yInit,
		double zInit, double h, double myeInit, double vyeInit);
};

