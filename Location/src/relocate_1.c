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
	double DDT = 0, DT = 0, Dist2 = 0;
	double sDxy = 0, sDx2 = 0, sDy2 = 0, sDxC = 0, sDyC = 0;
	double smag_modify;
	for (int i = 0; i<n_sta; i++)
	{
		Dx[i] = (tplon - stalon[i])*kmperlon;
		Dy[i] = (tplat - stalat[i])*kmperlat;
		DDT = offsetZ[i] - (offsetE[i] + offsetN[i])/2;
		DT = traveltimeZ[i] - (traveltimeN[i] + traveltimeE[i])/2;
		Dist2 = Dx[i]*Dx[i] + Dy[i]*Dy[i] + (tpdepth+staht[i])*(tpdepth+staht[i]);
		C[i] = DDT/DT*Dist2;
printf("%f %f %f %f %f %f\n",Dx[i],Dy[i],C[i],DDT,DT,Dist2);
		sDxy += Dx[i]*Dy[i];
		sDx2 += Dx[i]*Dx[i];
		sDy2 += Dy[i]*Dy[i];
		sDxC += Dx[i]*C[i];
		sDyC += Dy[i]*C[i];
		smag_modify += (mag_modifyE[i] + mag_modifyN[i] + mag_modifyZ[i])/3;
	}
	double dx = (sDy2*sDxC - sDxy*sDyC)/(sDx2*sDy2 - sDxy*sDxy);
	double dy = (sDx2*sDyC - sDxy*sDxC)/(sDx2*sDy2 - sDxy*sDxy);
	smag_modify /= n_sta;
	printf("%.2f %.2f %.2f\n",dx,dy,4/3*log10(smag_modify)+tpmag);
	return 0;
}