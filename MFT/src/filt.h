#ifndef _filt_ayu_
#define _filt_ayu_

#include "time.h"

int filt(double *b, double *a, int n_filter, double* data, int n_data)
{
	//clock_t timer0 = clock();
	double* databak = (double*)malloc(sizeof(double)*n_data);
	memcpy(databak, data, sizeof(double)*n_data);
	//double* buffer = (double*)malloc(sizeof(double)*n_filter);
	//memset(buffer, 0, sizeof(double)*n_filter);
	//clock_t timer1 = clock();
	for(int i=0; i<n_data; i++)
	{
		//printf("%d %d",i, n_data);
		//buffer[i%n_filter] = data[i];
		//data[i] = buffer[i]*b[0];
		data[i] = databak[i]*b[0];
		for (int j=1; (j<n_filter && j<=i); j++)
		{
			//data[i] += buffer[(i+n_filter-j)%n_filter]*b[j] - data[i-j]*a[j];
			data[i] += databak[i-j]*b[j] - data[i-j]*a[j];
		}
		//printf(" %d\n",i);
	}
	//clock_t timer2 = clock();
	//printf("T1:%f,T2:%f\n",(timer1-timer0)*1.0/CLOCKS_PER_SEC, (timer2-timer1)*1.0/CLOCKS_PER_SEC);
	free(databak);
	//free(buffer);
}

#endif