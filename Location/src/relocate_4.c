/* this program relocate all potential events found from MFT,
the method can be found in doc/readme.pptx */
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>

#define EARTH_RADIUS 6371.0
#define PI 3.1415926535898

using namespace std;

void errfunc()
{
	cout << "Usage" << endl;
}
double calF1(double dx, double dy, double dz, double Dx, double Dy, double Dz, double C);
double calF(double dx, double dy, double dz, double* Dx, double* Dy, double* Dz, double* C, int n);
int caltheta(double dx, double dy, double dz, double* Dx, double* Dy, double* Dz, double* C, int n, double abc[3]);
double calstep(double dx, double dy, double dz, double* Dx, double* Dy, double* Dz, double* C, int n, double abc[3], double maxstep, int n_step);
double norm(double a, double b, double c);
double azimuth(double lat1, double lon1, double lat2, double lon2);
int main(int argc, char** argv)
{
	/* read input */
	double* stalon, *stalat, *staht;
	double *traveltimeZ, *offsetZ, *traveltimeN, *offsetN, *traveltimeE, *offsetE,\
		   *mag_modifyZ, *mag_modifyN, *mag_modifyE;
	double tplon, tplat, tpdepth, tpmag;
	int n_sta,i_sta;
	int n_paras_eachcomp = 3;
	for (int i = 1; i<argc; i++)
	{
		//cout << i << argc << endl;
		if (strcmp(argv[i], "-tp") == 0)
		{
			tplon = atof(argv[i+1]);
			tplat = atof(argv[i+2]);
			tpdepth = atof(argv[i+3]);
			tpmag = atof(argv[i+4]);
			n_sta = (argc - 1 - 5)/(4+3*(n_paras_eachcomp+1));
			i_sta = 0;
			stalon = (double*)malloc(sizeof(double)*n_sta);
			stalat = (double*)malloc(sizeof(double)*n_sta);
			staht = (double*)malloc(sizeof(double)*n_sta);
			traveltimeZ = (double*)malloc(sizeof(double*)*n_sta);
			offsetZ = (double*)malloc(sizeof(double)*n_sta);
			mag_modifyZ = (double*)malloc(sizeof(double)*n_sta);
			traveltimeN = (double*)malloc(sizeof(double*)*n_sta);
			offsetN = (double*)malloc(sizeof(double)*n_sta);
			mag_modifyN = (double*)malloc(sizeof(double)*n_sta);
			traveltimeE = (double*)malloc(sizeof(double*)*n_sta);
			offsetE = (double*)malloc(sizeof(double)*n_sta);
			mag_modifyE = (double*)malloc(sizeof(double)*n_sta);
			i+=4;
		}
		else if(strcmp(argv[i], "-sta") == 0)
		{
			stalon[i_sta] = atof(argv[i+1]);
			stalat[i_sta] = atof(argv[i+2]);
			staht[i_sta] = atof(argv[i+3])/1000;
			for (int j = 0; j<3; j++)
			{
				switch(argv[i+4+j*(n_paras_eachcomp+1)][0])
				{
					case 'Z':
						//cout << "Z" <<endl;
						traveltimeZ[i_sta] = atof(argv[i+4+j*(n_paras_eachcomp+1)+1]);
						offsetZ[i_sta] = atof(argv[i+4+j*(n_paras_eachcomp+1)+2]);
						mag_modifyZ[i_sta] = atof(argv[i+4+j*(n_paras_eachcomp+1)+3]);
						break;
					case 'N':
						//cout << "N" <<endl;
						traveltimeN[i_sta] = atof(argv[i+4+j*(n_paras_eachcomp+1)+1]);
						offsetN[i_sta] = atof(argv[i+4+j*(n_paras_eachcomp+1)+2]);
						mag_modifyN[i_sta] = atof(argv[i+4+j*(n_paras_eachcomp+1)+3]);
						break;
					case 'E':
						//cout << "E" <<endl;
						traveltimeE[i_sta] = atof(argv[i+4+j*(n_paras_eachcomp+1)+1]);
						offsetE[i_sta] = atof(argv[i+4+j*(n_paras_eachcomp+1)+2]);
						mag_modifyE[i_sta] = atof(argv[i+4+j*(n_paras_eachcomp+1)+3]);
						break;
				}
			}
			i+=4+3*(n_paras_eachcomp+1)-1;
			i_sta++;
		}
		else
		{
			errfunc();
			return -1;
		}
	}
	
	// cout << tplon << " " << tplat << " " << tpdepth << endl;
	// for (int i = 0; i<n_sta;i++)
	// {
	// 	cout << stalon[i] << " "
	// 		 << stalat[i] << " "
	// 		 << staht[i] << " "
	// 		 << traveltimeZ[i] << " "
	// 		 << offsetZ[i] << " "
	// 		 << traveltimeN[i] << " "
	// 		 << offsetN[i] << " "
	// 		 << traveltimeE[i] << " "
	// 		 << offsetE[i] << " "
	// 		 <<endl;
	// }
	// cout << "#####" << endl;



	double kmperlat = EARTH_RADIUS*PI/180.0;
	double kmperlon = cos(tplat/180*PI)*EARTH_RADIUS*PI/180.0;
	double* Dx = (double*)malloc(sizeof(double)*n_sta);
	double* Dy = (double*)malloc(sizeof(double)*n_sta);
	double* C = (double*)malloc(sizeof(double)*n_sta);
	double* DDT = (double*)malloc(sizeof(double)*n_sta);
	double* DT = (double*)malloc(sizeof(double)*n_sta);
	double* Dz = (double*)malloc(sizeof(double)*n_sta);
	double smag_modify = 0,az;
	double angcover[360] = {0};
	double WeightAngcover = 0;
	for (int i = 0; i<n_sta; i++)
	{
		Dx[i] = (tplon - stalon[i])*kmperlon;
		Dy[i] = (tplat - stalat[i])*kmperlat;
		Dz[i] = tpdepth + staht[i]/1000;
		DDT[i] = offsetZ[i] - (offsetE[i] + offsetN[i])/2;
		DT[i] = - traveltimeZ[i] + (traveltimeN[i] + traveltimeE[i])/2;
		C[i] = (DDT[i]/DT[i])*norm(Dx[i],Dy[i],Dz[i]);
		smag_modify += (mag_modifyE[i] + mag_modifyN[i] + mag_modifyZ[i])/3;
		az = azimuth(tplat, tplon, stalat[i], stalon[i]);
		for (int k = -60; k<=60; k++) angcover[(k+(int)round(az)+360)%360] = 1;
	}
	smag_modify /= n_sta;
	for (int i = 0; i<360; i++) WeightAngcover += angcover[i];
	WeightAngcover /= 360.;

	double dx = 0, dy = 0, dz = 0;
	double F = 0; const double maxstep = 1; const double n_step = 100;
	double abc[2] = {0,0}; double step = 0;
	const double step_minlimit = maxstep/n_step;
	const double F_minlimit = 0.;
	int n_count = 0, MAX_COUNT = 1000;
	//cout<<"step_minlimit:"<<step_minlimit<<endl;
	do 
	{
		caltheta(dx, dy, dz, Dx, Dy, Dz, C, n_sta, abc);
		if (abs(abc[0] - 10086)<1) break;
		step = calstep(dx, dy, dz, Dx, Dy, Dz, C, n_sta, abc, maxstep, n_step);
		dx += step*abc[0]; dy += step*abc[1];
		F = calF(dx, dy, dz, Dx, Dy, Dz, C, n_sta);
		n_count++;
		//printf("step:%.8f theta:%.8f F:%.8f\n",step,theta,F);
		if (n_count >= MAX_COUNT) break;
	}while( step >= step_minlimit && F > F_minlimit);





	printf("%.3f %.3f %.3f %.3f %.8f %d %.8f %.4f\n",dx,dy,dz,4/3*log10(smag_modify)+tpmag,F/n_sta,n_count,step,WeightAngcover);
	return 0;
}


double calF1(double dx, double dy, double dz, double Dx, double Dy, double Dz, double C)
{
	double l = norm(Dx, Dy, Dz);
	double lnew = norm(Dx+dx, Dy+dy, Dz+dz);
	return (lnew - l - C);
}
double calF(double dx, double dy, double dz, double* Dx, double* Dy, double* Dz, double* C, int n)
{
	double f = 0;
	for(int i = 0; i < n; i++) f+=pow(calF1(dx,dy,dz,Dx[i],Dy[i],Dz[i],C[i]),2);
	return f;
}
//double caldF1ddx(double dx, double dy, double Dx, double Dy, double Dz, double C);
//double caldF1ddy(double dx, double dy, double Dx, double Dy, double Dz, double C);
int caltheta(double dx, double dy, double dz,double* Dx, double* Dy, double* Dz, double* C, int n, double abc[3] )
{
	double lnew = 0;
	double AA = 0, BB = 0;
	for (int i = 0; i < n; i++)
	{
		lnew = norm(Dx[i]+dx, Dy[i]+dy, Dz[i]+dz);
		AA += calF1(dx,dy,dz,Dx[i],Dy[i],Dz[i],C[i]) * ( dx + Dx[i] ) / lnew;
		BB += calF1(dx,dy,dz,Dx[i],Dy[i],Dz[i],C[i]) * ( dy + Dy[i] ) / lnew;
	}
	if (abs(norm(AA,BB,0)) == 0)
	{
        abc[0] = 10086;abc[1] = 10086;
        return -1;
    }
    abc[0] = -AA/norm(AA,BB,0);
    abc[1] = -BB/norm(AA,BB,0);
    if (abc[0] > 1.01 || abc[1] > 1.01 || abc[2] > 1.01 )
	{
        abc[0] = 10086;abc[1] = 10086;
        return -1;
    }
    return 1;
}
double calstep(double dx, double dy, double dz, double* Dx, double* Dy, double* Dz, double* C, int n, double abc[3], double maxstep, int n_step)
{
	double F = 0;
	double F_min = calF(dx, dy, dz, Dx, Dy, Dz, C, n);
	double step = 0;
	//cout << F_min << endl;
	for (int i = 1; i <= n_step; i++)
	{
		F = calF(dx+maxstep*abc[0]*i/n_step, dy+maxstep*abc[1]*i/n_step, dz, Dx, Dy, Dz, C, n);
		//cout << i << ":" << F << endl;
		if (F<F_min)
		{
			F_min = F;
			step = maxstep*i/n_step;
		}
	}
	//cout << min_step << endl;
	return step;
}
double norm(double a, double b, double c)
{
	return sqrt(a*a+b*b+c*c);
}

double azimuth(double lat1, double lon1, double lat2, double lon2)
{
	lat1 = lat1 / 180 * PI;lon1 = lon1 / 180 * PI;
	lat2 = lat2 / 180 * PI;lon2 = lon2 / 180 * PI;
	double az = atan2(cos(lat2)*sin(lon2-lon1), cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(lon2-lon1)); //from matlab distance
	if ( lat1 <= -PI/2 || lat2 >= PI/2)  az = 0;
	if ( lat1 >= PI/2 || lat2 <= -PI/2)  az = PI;
	az = az/PI*180;
	return az;
}
