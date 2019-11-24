#include "stdafx.h"
#include "CcrpAim.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

CcrpAim::CcrpAim() :nAim(0),nFall(0),A0(0),T(0)
{
	for (int i = 0; i < MAXCOUNT; i++)
	{
		xeBomb[i] = 0;
		yeBomb[i] = 0;
		zeBomb[i] = 0;
	}
}

CcrpAim::~CcrpAim()
{
}

void CcrpAim::reset()
{
	A0 = 0;
	T = 0;
	nAim = 0;
	nFall = 0;
	for (int i = 0; i < MAXCOUNT; i++)
	{
		xeBomb[i] = 0;
		yeBomb[i] = 0;
		zeBomb[i] = 0;
	}
}

void CcrpAim::CCRP(double height, double v, double omega, double& K, double u, double e, double deltat, double h,
	double mxeInit, double myeInit, double vxeInit, double vyeInit)
{
	double Kr = PI*K / 180;
	double er = PI*e / 180;
	double alpha = atan(u*sin(er) / (v + u*cos(er)));
	double tInit = 0;												//有风和无风情况下弹道诸元的初始值
	double usInit = v;							
	double wsInit = 0;														
	double xInit = 0;									
	double yInit = 0;		
	double zInit = 0;
	///////////////////标准弹道解算////////////////
	StdBombTraj(height, tInit, usInit, wsInit, xInit, yInit, h);	
	//////////////////CCRP瞄准阶段/////////////////
	Aim(height, v, omega, Kr, u, er, alpha, deltat, mxeInit, myeInit, vxeInit, vyeInit);
	////////////////弹丸实际下落阶段//////////////
	double uInit = v + u*cos(er);
	double pInit = u*sin(er);
	double wInit = 0;
	RealBombTraj(height, Kr, tInit, uInit, wInit, pInit, xInit, yInit, zInit, h, myeInit, vyeInit);
	K = Kr * 180 / PI;
}

double CcrpAim::output_xbomb(int index)
{
	return xeBomb[index];
}

double CcrpAim::output_ybomb(int index)
{
	return yeBomb[index];
}

double CcrpAim::output_zbomb(int index)
{
	return zeBomb[index];
}

double CcrpAim::output_stdrange()
{
	return A0;
}

double CcrpAim::output_stdtime()
{
	return T;
}

int CcrpAim::output_aim()
{
	return nAim;
}

int CcrpAim::output_index()
{
	return nAim + nFall;
}

////////////////////////弹丸和空气基本参数/////////////////
double CcrpAim::trajC(double resc, double caliber, double weight)
{
	double bulC = resc / STDRESC;
	return bulC*pow(caliber, 2)*1e3 / weight;
}

double CcrpAim::tao(double height, double y)
{
	return TON - GG*(height - y);
}

double CcrpAim::hg(double height, double y)
{
	return HON * pow(1 - GG*(height - y) / TON, 1 / (R*GG));
}

double CcrpAim::Hg(double height, double y)
{
	return 13.6 * hg(height, y) / R / tao(height, y) / RON * sqrt(tao(height, y) / TON);
}

/////////////////////////解算弹丸的标准弹道（无风）//////////////////////
double CcrpAim::funcv(double u, double w)
{
	return sqrt(pow(u, 2) + pow(w, 2));
}

double CcrpAim::Cx0(double height, double y, double u, double w)
{
	double a = 20.074*sqrt(tao(height, y));
	double Ma = funcv(u, w) / a;
	if (Ma <= 0.8)
		return 0.6;
	else
		return Ma - 0.1;
}

double CcrpAim::resG(double height, double y, double u, double w)
{
	return PI * RON / 8e3 * funcv(u, w) * Cx0(height, y, u, w)*sqrt(TON / tao(height, y));
}

double CcrpAim::funcu(double height, double u, double w, double y)
{
	return -1 * trajC(RESC, CALIBER, WEIGHT) * Hg(height, y) * resG(height, y, u, w) * u;
}

double CcrpAim::funcw(double height, double u, double w, double y)
{
	return -1 * trajC(RESC, CALIBER, WEIGHT) * Hg(height, y) * resG(height, y, u, w) * w + G;
}

double CcrpAim::funcx(double u)
{
	return u;
}

double CcrpAim::funcy(double w)
{
	return w;
}

void CcrpAim::StdBombTraj(double height, double tInit, double uInit, double wInit, double xInit, double yInit, double h)
{
	double uK1, uK2, uK3, uK4;
	double wK1, wK2, wK3, wK4;
	double xK1, xK2, xK3, xK4;
	double yK1, yK2, yK3, yK4;
	double yRes = yInit;					//记录弹道诸元的数组
	double uRes = uInit;
	double wRes = wInit;
	double xRes = xInit;
	double tRes = tInit;
	int i = 0;
	while (yRes < height)
	{
		uK1 = funcu(height, uRes, wRes, yRes);
		wK1 = funcw(height, uRes, wRes, yRes);
		xK1 = funcx(uRes);
		yK1 = funcy(wRes);

		uK2 = funcu(height, uRes + h*uK1 / 2.0, wRes + h*wK1 / 2, yRes + h*yK1 / 2.0);
		wK2 = funcw(height, uRes + h*uK1 / 2.0, wRes + h*wK1 / 2, yRes + h*yK1 / 2.0);
		xK2 = funcx(uRes + h*uK1 / 2.0);
		yK2 = funcy(wRes + h*wK1 / 2.0);

		uK3 = funcu(height, uRes + h*uK2 / 2.0, wRes + h*wK2 / 2.0, yRes + h*yK2 / 2.0);
		wK3 = funcw(height, uRes + h*uK2 / 2.0, wRes + h*wK2 / 2.0, yRes + h*yK2 / 2.0);
		xK3 = funcx(uRes + h*uK2 / 2.0);
		yK3 = funcy(wRes + h*wK2 / 2.0);

		uK4 = funcu(height, uRes + h*uK3, wRes + h*wK3, yRes + h*yK3);
		wK4 = funcw(height, uRes + h*uK3, wRes + h*wK3, yRes + h*yK3);
		xK4 = funcx(uRes + h*uK3);
		yK4 = funcy(wRes + h*wK3);

		uRes = uRes + h*(uK1 + 2 * uK2 + 2 * uK3 + uK4) / 6.0;
		wRes = wRes + h*(wK1 + 2 * wK2 + 2 * wK3 + wK4) / 6.0;
		xRes = xRes + h*(xK1 + 2 * xK2 + 2 * xK3 + xK4) / 6.0;
		yRes = yRes + h*(yK1 + 2 * yK2 + 2 * yK3 + yK4) / 6.0;
		tRes = tRes + h;

		i++;
	}
	i--;
	A0 = xRes;		//弹丸标准射程
	T = tRes;		//弹丸标准落下时间
}

////////////////CCRP方向瞄准和距离瞄准，控制飞机抵达正确投弹点/////////////////////////
double CcrpAim::LxeVeh(double v, double K, double u, double e, double deltat)
{
	double xhVeh = (v + u*cos(e))*deltat;
	double yhVeh = u*sin(e)*deltat;
	return xhVeh*cos(K) - yhVeh*sin(K);
}

double CcrpAim::LyeVeh(double v, double K, double u, double e, double deltat)
{
	double xhVeh = (v + u*cos(e))*deltat;
	double yhVeh = u*sin(e)*deltat;
	return xhVeh*sin(K) + yhVeh*cos(K);
}

double CcrpAim::Dxw(double dxe, double dye, double K, double alpha)
{
	return dxe*cos(K + alpha) + dye*sin(K + alpha);
}

double CcrpAim::Dyw(double dxe, double dye, double K, double alpha)
{
	return -1 * dxe*sin(K + alpha) + dye*cos(K + alpha);
}

double CcrpAim::Axw(double u, double e, double alpha)
{
	return A0*cos(alpha) + u*T*cos(e - alpha);
}

double CcrpAim::Ayw(double u, double e, double alpha)
{
	return -1 * A0*sin(alpha) + u*T*sin(e - alpha);
}

double CcrpAim::DeltaK(double K, double dmxe, double dmye, double alpha, double ayw, double dmxw)
{
	double Km = atan(dmye / dmxe) - K;//目标与飞机空速矢量间的水平方位角
	/*if (fabs(Km) > PI)
		Km = atan(dmye / dmxe) - (K + 2 * PI);*/
	double alphaS = atan(ayw / dmxw);	   //命中点偏侧角
	return Km - alpha - alphaS;
}

double CcrpAim::DeltaD(double axw, double ayw, double dmxw) 
{    
	return sqrt(pow(dmxw, 2) + pow(ayw, 2)) - sqrt(pow(axw, 2) + pow(ayw, 2));
}

double CcrpAim::valTransfer(double init)
{
	double result;
	if (init >= 0 && init < 360)  //航向角转化为[0，360)范围
		result = init;
	else if (init >= 360)
		result = init - 360;
	else if (init < 0)
		result = 360 - fabs(init);
	return result;
}

int CcrpAim::turnDirection(double dpa, double cpa)
{
	double difpa;
	if (dpa > cpa)
	{
		difpa = dpa - cpa;
		if (difpa <= 180)
			return 1;
		else
			return -1;
	}
	else if (dpa < cpa)
	{
		difpa = cpa - dpa;
		if (difpa >= 180)
			return 1;
		else
			return -1;
	}
}

double CcrpAim::deheadCompute(int cux, int cuy, int spx, int spy)
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
double delta[5000];
void CcrpAim:: Aim(double height, double v, double omega, double& K, double u, double& e, double alpha, double deltat,
	double mxeInit, double myeInit, double vxeInit, double vyeInit)
{
	int i = 0;
	double kInit = K;
	kInit = kInit * 180 / PI;
	xeBomb[0] = vxeInit;
	zeBomb[0] = vyeInit;
	yeBomb[0] = height;
	double dmxe = fabs(mxeInit - vxeInit);
	double dmye = fabs(myeInit - vyeInit);
	double dmxw = Dxw(dmxe, dmye, K, alpha);
	double dmyw = Dyw(dmxe, dmye, K, alpha);
	double axw = Axw(u, e, alpha);
	double ayw = Ayw(u, e, alpha);
	double lxe = 0;
	double lye = 0;
	double dpa = deheadCompute(vxeInit, vyeInit, mxeInit, myeInit);
	double deltak = fabs(DeltaK(K, dmxe, dmye, alpha, ayw, dmxw));
 	int turn = turnDirection(dpa, kInit);
	int k = 0;
	while (deltak > 0.001)      //方向瞄准
	{
		K = K * 180 / PI;
		if (turn > 0)
		{
			K = K + (omega * 180 / PI) * deltat;
			K = valTransfer(K);
		}
		else if (turn < 0)
		{
			K = K - (omega * 180 / PI) * deltat;
			K = valTransfer(K);
		}
		K = K*PI / 180;
		lxe = LxeVeh(v, K, u, e, deltat);
		lye = LyeVeh(v, K, u, e, deltat);
 		xeBomb[i + 1] = xeBomb[i] + lxe;
 		if (myeInit >= vyeInit)
			zeBomb[i + 1] = zeBomb[i] + lye;
		else
			zeBomb[i + 1] = zeBomb[i] - lye;
		yeBomb[i + 1] = height;
		dmxe = dmxe-lxe;
		dmye = dmye-lye;
		dmxw = Dxw(dmxe, dmye, K, alpha);
		dmyw = Dyw(dmxe, dmye, K, alpha);
		deltak = fabs(DeltaK(K, dmxe, dmye, alpha, ayw, dmxw));
		i++;
		delta[k] = deltak;
		k++;
	}
	double deltad = DeltaD(axw, ayw, dmxw);
	while (deltad > 0.001)      //距离瞄准
	{
		lxe = LxeVeh(v, K, u, e, deltat);
		lye = LyeVeh(v, K, u, e, deltat);
		xeBomb[i + 1] = xeBomb[i] + lxe;
		if (myeInit >= vyeInit)
			zeBomb[i + 1] = zeBomb[i] + lye;
		else
			zeBomb[i + 1] = zeBomb[i] - lye;
		yeBomb[i + 1] = height;
		dmxe = dmxe - lxe;
		dmye = dmye - lye;
		dmxw = Dxw(dmxe, dmye, K, alpha);
		dmyw = Dyw(dmxe, dmye, K, alpha);
		deltad = DeltaD(axw, ayw, dmxw);
		i++;
	}
	nAim = i;				//瞄准阶段坐标点数
}

///////////////////////////解算飞机投弹后弹丸的实际弹道（有风）////////////////////////
double CcrpAim::funcv(double u, double w, double p)
{
	return sqrt(pow(u, 2) + pow(w, 2) + pow(p, 2));
}

double CcrpAim::Cx0(double height, double y, double u, double w, double p)
{
	double a = 20.074*sqrt(tao(height, y));
	double Ma = funcv(u, w, p) / a;
	if (Ma <= 0.8)
		return 0.6;
	else
		return Ma - 0.1;
}

double CcrpAim::resG(double height, double y, double u, double w, double p)
{
	return PI * RON / 8e3 * funcv(u, w, p) * Cx0(height, y, u, w, p)*sqrt(TON / tao(height, y));
}

double CcrpAim::funcu(double height, double u, double w, double p, double y)
{
	return -1 * trajC(RESC, CALIBER, WEIGHT) * Hg(height, y) * resG(height, y, u, w, p) * u;
}

double CcrpAim::funcw(double height, double u, double w, double p, double y)
{
	return -1 * trajC(RESC, CALIBER, WEIGHT) * Hg(height, y) * resG(height, y, u, w, p) * w + G;
}

double CcrpAim::funcp(double height, double u, double w, double p, double y)
{
	return -1 * trajC(RESC, CALIBER, WEIGHT) * Hg(height, y) * resG(height, y, u, w, p) * p;
}

double CcrpAim::funcz(double p)
{
	return p;
}

void CcrpAim::RealBombTraj(double height, double K, double tInit, double uInit, double wInit, double pInit, double xInit, double yInit, double zInit, double h, double myeInit, double vyeInit)
{
	double uK1, uK2, uK3, uK4;
	double wK1, wK2, wK3, wK4;
	double pK1, pK2, pK3, pK4;
	double xK1, xK2, xK3, xK4;
	double yK1, yK2, yK3, yK4;
	double zK1, zK2, zK3, zK4;
	double uRes = uInit;			//记录弹道诸元的数组
	double wRes = wInit;
	double pRes = pInit;
	double xRes = xInit;
	double yRes = yInit;
	double zRes = zInit;
	double tRes = tInit;
	double lxeFall, lyeFall, lzeFall;
	int i = 1;
	while (height - yRes > 1)
	{
		uK1 = funcu(height, uRes, wRes, pRes, yRes);
		wK1 = funcw(height, uRes, wRes, pRes, yRes);
		pK1 = funcp(height, uRes, wRes, pRes, yRes);
		xK1 = funcx(uRes);
		yK1 = funcy(wRes);
		zK1 = funcz(pRes);

		uK2 = funcu(height, uRes + h*uK1 / 2.0, wRes + h*wK1 / 2.0, pRes + h*pK1 / 2, yRes + h*yK1 / 2.0);
		wK2 = funcw(height, uRes + h*uK1 / 2.0, wRes + h*wK1 / 2.0, pRes + h*pK1 / 2, yRes + h*yK1 / 2.0);
		pK2 = funcp(height, uRes + h*uK1 / 2.0, wRes + h*wK1 / 2.0, pRes + h*pK1 / 2, yRes + h*yK1 / 2.0);
		xK2 = funcx(uRes + h*uK1 / 2.0);
		yK2 = funcy(wRes + h*wK1 / 2.0);
		zK2 = funcz(pRes + h*pK1 / 2.0);

		uK3 = funcu(height, uRes + h*uK2 / 2.0, wRes + h*wK2 / 2.0, pRes + h*pK2 / 2, yRes + h*yK2 / 2.0);
		wK3 = funcw(height, uRes + h*uK2 / 2.0, wRes + h*wK2 / 2.0, pRes + h*pK2 / 2, yRes + h*yK2 / 2.0);
		pK3 = funcp(height, uRes + h*uK2 / 2.0, wRes + h*wK2 / 2.0, pRes + h*pK2 / 2, yRes + h*yK2 / 2.0);
		xK3 = funcx(uRes + h*uK2 / 2.0);
		yK3 = funcy(wRes + h*wK2 / 2.0);
		zK3 = funcz(pRes + h*pK2 / 2.0);

		uK4 = funcu(height, uRes + h*uK3, wRes + h*wK3, pRes + h*pK3, yRes + h*yK3);
		wK4 = funcw(height, uRes + h*uK3, wRes + h*wK3, pRes + h*pK3, yRes + h*yK3);
		pK4 = funcp(height, uRes + h*uK3, wRes + h*wK3, pRes + h*pK3, yRes + h*yK3);
		xK4 = funcx(uRes + h*uK3);
		yK4 = funcy(wRes + h*wK3);
		zK4 = funcz(pRes + h*pK3);

		uRes = uRes + h*(uK1 + 2 * uK2 + 2 * uK3 + uK4) / 6.0;
		wRes = wRes + h*(wK1 + 2 * wK2 + 2 * wK3 + wK4) / 6.0;
		pRes = pRes + h*(pK1 + 2 * pK2 + 2 * pK3 + pK4) / 6.0;
		xRes = xRes + h*(xK1 + 2 * xK2 + 2 * xK3 + xK4) / 6.0;
		yRes = yRes + h*(yK1 + 2 * yK2 + 2 * yK3 + yK4) / 6.0;
		zRes = zRes + h*(zK1 + 2 * zK2 + 2 * zK3 + zK4) / 6.0;
		tRes = tRes + h;

		lxeFall = xRes * cos(K) - zRes * sin(K);  //炸弹坐标从飞机航向系转到地理系
		lyeFall = yRes;
		lzeFall = xRes * sin(K) + zRes * cos(K);

		xeBomb[nAim + i] = xeBomb[nAim] + lxeFall;		//俯冲结束时弹丸坐标加上下落段运动的长度
		yeBomb[nAim + i] = yeBomb[nAim] - lyeFall;
		if (myeInit >= vyeInit)
			zeBomb[nAim + i] = zeBomb[nAim] + lzeFall;
		else
			zeBomb[nAim + i] = zeBomb[nAim] - lzeFall;

		i++;
	}
	nFall = i - 1;     //弹丸下落阶段的坐标数
}