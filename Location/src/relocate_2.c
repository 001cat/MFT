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
double calF1(double dx, double dy, double Dx, double Dy, double Dz, double C);
double calF(double dx, double dy, double* Dx, double* Dy, double* Dz, double* C, int n);
double caltheta(double dx, double dy, double* Dx, double* Dy, double* Dz, double* C, int n);
double calstep(double dx, double dy, double* Dx, double* Dy, double* Dz, double* C, int n, double theta, double step, int n_step);

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
	double* Dx = (double*)malloc(sizeof(double*)*n_sta);
	double* Dy = (double*)malloc(sizeof(double*)*n_sta);
	double* C = (double*)malloc(sizeof(double*)*n_sta);
	double* DDT = (double*)malloc(sizeof(double*)*n_sta);
	double* DT = (double*)malloc(sizeof(double*)*n_sta);
	double* Dz = (double*)malloc(sizeof(double*)*n_sta);
	double* l2 = (double*)malloc(sizeof(double*)*n_sta);
	double smag_modify;
	for (int i = 0; i<n_sta; i++)
	{
		Dx[i] = (tplon - stalon[i])*kmperlon;
		Dy[i] = (tplat - stalat[i])*kmperlat;
		DDT[i] = offsetZ[i] - (offsetE[i] + offsetN[i])/2;
		DT[i] = - traveltimeZ[i] + (traveltimeN[i] + traveltimeE[i])/2;
		Dz[i] = tpdepth+staht[i];
		l2[i] = Dx[i]*Dx[i]+Dy[i]*Dy[i]+Dz[i]*Dz[i];
		C[i] = (DDT[i]/DT[i])*2*l2[i];
		smag_modify += (mag_modifyE[i] + mag_modifyN[i] + mag_modifyZ[i])/3;
	}
	
	double dx = 0, dy = 0;
	double F = 0; const double maxstep = 0.1; const double n_step = 1000;
	double theta = 0; double step = 0;
	const double step_minlimit = maxstep/n_step*1.;
	const double F_minlimit = 0.;
	int n_count = 0, MAX_COUNT = 1000;
	//cout<<"step_minlimit:"<<step_minlimit<<endl;
	do 
	{
		theta = caltheta(dx, dy, Dx, Dy, Dz, C, n_sta);
		if (abs(theta - 10086)<1) break;
		step = calstep(dx, dy, Dx, Dy, Dz, C, n_sta, theta, maxstep, n_step);
		dx += step*cos(theta); dy += step*sin(theta);
		F = calF(dx, dy, Dx, Dy, Dz, C, n_sta);
		n_count++;
		//printf("step:%.8f theta:%.8f F:%.8f\n",step,theta,F);
		if (n_count >= MAX_COUNT) break;
	}while( step >= step_minlimit && F > F_minlimit);

	double l2new = 0;
	for (int i = 0; i<n_sta; i++)
	{
		l2new = (Dx[i]+dx)*(Dx[i]+dx)+(Dy[i]+dy)*(Dy[i]+dy)+(Dz[i])*(Dz[i]);
		//printf("DiffDist: %.8f %.8f %.8f %.8f\n",sqrt(l2new),sqrt(l2[i]), DDT[i],C[i]); 
	}




	smag_modify /= n_sta;
	//printf("%.8f\n",theta);
	printf("%.8f %.8f %d %.2f %.2f %.2f\n",F,step,n_count,dx,dy,4/3*log10(smag_modify)+tpmag);
	return 0;
}


double calF1(double dx, double dy, double Dx, double Dy, double Dz, double C)
{
	double l = sqrt(Dx*Dx + Dy*Dy + Dz*Dz);
	return ((dx*dx+dy*dy+2*dx*Dx+2*dy*Dy) - C)/l;
}
double calF(double dx, double dy, double* Dx, double* Dy, double* Dz, double* C, int n)
{
	double f = 0;
	for(int i = 0; i < n; i++) f+=pow(calF1(dx,dy,Dx[i],Dy[i],Dz[i],C[i]),2);
	return f;
}
//double caldF1ddx(double dx, double dy, double Dx, double Dy, double Dz, double C);
//double caldF1ddy(double dx, double dy, double Dx, double Dy, double Dz, double C);
double caltheta(double dx, double dy, double* Dx, double* Dy, double* Dz, double* C, int n)
{
	double theta = 0, sin_phi = 0, cos_phi = 0;
	//double l2 = 0;
	for (int i = 0; i < n; i++)
	{
		//l2 = Dx[i]*Dx[i] + Dy[i]*Dy[i] + Dz[i]*Dz[i];
		sin_phi += calF1(dx,dy,Dx[i],Dy[i],Dz[i],C[i]) * ( dx + Dx[i] );
		cos_phi += calF1(dx,dy,Dx[i],Dy[i],Dz[i],C[i]) * ( dy + Dy[i] );
	}
	//printf("%f %f\n",sin_phi,cos_phi);
	if (abs(cos_phi) < 0.000001 && abs(sin_phi) < 0.000001) return 10086;
	double sin_theta = -1.*cos_phi/sqrt(cos_phi*cos_phi+sin_phi*sin_phi);
	double cos_theta = -1.*sin_phi/sqrt(cos_phi*cos_phi+sin_phi*sin_phi);
	if (cos_theta < 0) theta = PI-asin(sin_theta);
	else theta = asin(sin_theta);
	return theta;

}
double calstep(double dx, double dy, double* Dx, double* Dy, double* Dz, double* C, int n, double theta, double step, int n_step)
{
	double F = 0;
	double F_min = calF(dx, dy, Dx, Dy, Dz, C, n);
	double min_step = 0;
	//cout << F_min << endl;
	for (int i = 1; i <= n_step; i++)
	{
		F = calF(dx+step*cos(theta)*i/n_step, dy+step*sin(theta)*i/n_step, Dx, Dy, Dz, C, n);
		//cout << i << ":" << F << endl;
		if (F<F_min)
		{
			F_min = F;
			min_step = step*i/n_step;
		}
	}
	//cout << min_step << endl;
	return min_step;
}

