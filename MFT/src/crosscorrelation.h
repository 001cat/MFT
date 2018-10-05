// Remember to free array C !
// Remember to rmean first !

#ifndef _crosscorrelation_ayu_
#define _crosscorrelation_ayu_

#include <math.h>
//#include "complex_ayu.h"
//#include "fft_ayu.h"

#include <fftw3.h>

double rmean(double* in, double* out, int n)
{
	double tmp = 0;
	for (int i = 0; i<n; i++) tmp += in[i];
	tmp = tmp/n;
	for (int i = 0; i<n; i++) out[i] = in[i] - tmp;
	return tmp;
}

double* crosscorrelation_inside(double *A, int lenA, double *B, int lenB)
// do crosscorrelation inside array A, len(A) > len(B), normalized with 2-norm
// remember to free array C!
{
	int lenC = lenA-lenB+1;
	double *C = (double*)malloc(sizeof(double)*lenC);
	//double *Brmean = (double*)malloc(sizeof(double)*lenB);
	double normA = 0, normB = 0, normAB = 0;
	//rmean(B, Brmean, lenB);
	for (int i = 0; i<lenB; i++)
	{
		normB += B[i]*B[i];
	}

	for (int i=0; i<lenC; i++)
	{
		C[i] = 0;
		for (int j=0; j<lenB; j++)
		{	
			C[i]+=A[i+j]*B[j];
			if (i==0) normA += A[i+j]*A[i+j];
		}
		if (i > 0) normA += A[i+lenB-1]*A[i+lenB-1]-A[i-1]*A[i-1];
		normAB = sqrt(normB*normA);

		if (normAB > 0) C[i] = C[i]/normAB;
		else C[i] = 0;

		if (C[i] > 1.1)
		{
			for (int k = max(0,i-200); k<=i; k++)
				C[k] = 0;
		}
	}
	return C;
}

double* crosscorrelation_inside_strict(double *A, int lenA, double *B, int lenB)
// do crosscorrelation inside array A, len(A) > len(B), normalized with 2-norm
// remember to free array C!
{
	int lenC = lenA-lenB+1;
	double *C = (double*)malloc(sizeof(double)*lenC);
	double normA = 0, normB = 0, normAB = 0, normA_debug = 0;
	double *Abuffer = (double*)malloc(sizeof(double)*lenC);
	double *Bbuffer = (double*)malloc(sizeof(double)*lenB);
//double* Abuffer_debug = (double*)malloc(sizeof(double)*lenB);
	rmean(B, Bbuffer, lenB);
	for (int i = 0; i<lenB; i++)
	{
		normB += Bbuffer[i]*Bbuffer[i];
	}
	for (int i = 0; i<lenB; i++)
	{
		Abuffer[0] += A[i];
	}
	for (int i = 1; i<lenC; i++)
	{
		Abuffer[i] = Abuffer[i-1] - A[i-1] + A[i+lenB-1];
	}

	//printf("%d\n",lenA-lenB+1);

	for (int i=0; i<lenC; i++)
	{
		C[i] = 0;
		//if (i >1045000) printf("i = %d\n",i);
		for (int j=0; j<lenB; j++)
		{	
			C[i]+=A[i+j]*Bbuffer[j];
			//printf("C[%d] = %f,Abuffer[%d] = %f, Bbuffer[%d] = %f\n",i, C[i], j, Abuffer[j], j, Bbuffer[j]);
			//printf("A[%d] = %f, B[%d] = %f\n",j, A[j], j, B[j]);
			if (i==0) normA += A[i+j]*A[i+j];
		}
		if (i > 0) normA += A[i+lenB-1]*A[i+lenB-1] - A[i-1]*A[i-1];
	// rmean(A+i, Abuffer_debug, lenB);
	// normA_debug = 0;
	// for (int j = 0; j<lenB; j++)
	// {
	// 	normA_debug += Abuffer_debug[j]*Abuffer_debug[j];
	// }
		normAB = sqrt(normB)*sqrt(normA - Abuffer[i]/lenB*Abuffer[i]);
		if ( normAB > 0) C[i] /= normAB;//sqrt(normB*normA_debug); //normAB;
		else C[i] = 0;
		
		if (C[i] > 1.1)
		{
			for (int k = max(0,i-200); k<=i; k++)
				C[k] = 0;
		}
		//printf("i = %d,C[i] = %f\n",i,C[i]);
		//if (i%1000 == 0) printf("i = %d\n",i);
	}
	free(Abuffer);
	free(Bbuffer);
	return C;
}








double* correlate_FFT(double *Fix, unsigned M, double *Slip, unsigned L, int K);
double* correlate_FFTW(double *Fix, unsigned M, double *Slip, unsigned L, int K);

double* crosscorrelation_inside_strict_FFT(double *A, int lenA, double *B, int lenB)
// do crosscorrelation inside array A, len(A) > len(B), normalized with 2-norm
// remember to free array C!
{
	int lenC = lenA-lenB+1;
	double *C = (double*)malloc(sizeof(double)*lenC);
	double normA = 0, normB = 0, normAB = 0;
	double *Abuffer = (double*)malloc(sizeof(double)*lenC);
	double *Bbuffer = (double*)malloc(sizeof(double)*lenB);
	rmean(B, Bbuffer, lenB);
	for (int i = 0; i<lenB; ++i)
	{
		normB += Bbuffer[i]*Bbuffer[i];
	}
	for (int i = 0; i<lenB; i++)
	{
		Abuffer[0] += A[i];
	}
	for (int i = 1; i<lenC; i++)
	{
		Abuffer[i] = Abuffer[i-1] - A[i-1] + A[i+lenB-1];
	}
	C = correlate_FFTW(A, lenA, Bbuffer, lenB, lenC);
	//C = correlate_FFT(A, lenA, B, lenB, lenC);
	for (int i = 0; i<lenC; i++)
	{
		for (int j=0; j<lenB; j++)
		{
			if (i==0) normA += A[i+j]*A[i+j];
			else break;
		}
		if (i > 0) normA += A[i+lenB-1]*A[i+lenB-1]-A[i-1]*A[i-1];
		normAB = sqrt(normB*(normA - Abuffer[i]/lenB*Abuffer[i]));
		if ( normAB > 0) C[i] = C[i]/normAB;
		else C[i] = 0;
		
		if (C[i] > 1.1)
		{
			for (int k = max(0,i-200); k<=i; k++)
				C[k] = 0;
		}
		
	}
	free(Abuffer);
	free(Bbuffer);
	return C;
}

double* crosscorrelation_inside_FFT(double *A, int lenA, double *B, int lenB)
// do crosscorrelation inside array A, len(A) > len(B), normalized with 2-norm
// remember to free array C!
{
	int lenC = lenA-lenB+1;
	double *C;
	//double *Bbuffer = (double*)malloc(sizeof(double)*lenB);
	double normA = 0, normB = 0, normAB = 0;
	//rmean(B,Bbuffer,lenB);
	for (int i = 0; i<lenB; ++i)
	{
		normB += B[i]*B[i];
	}
	C = correlate_FFTW(A, lenA, B, lenB, lenC);
	//C = correlate_FFT(A, lenA, B, lenB, lenC);
	for (int i = 0; i<lenC; i++)
	{
		for (int j=0; j<lenB; j++)
		{
			if (i==0) normA += A[i+j]*A[i+j];
			else break;
		}
		if (i > 0) normA += A[i+lenB-1]*A[i+lenB-1]-A[i-1]*A[i-1];
		normAB = sqrt(normB*normA);
		if (normAB > 0) C[i] = C[i]/normAB;
		else C[i] = 0;

		if (C[i] > 1.1)
		{
			for (int k = max(0,i-200); k<=i; k++)
				C[k] = 0;
		}
	}
	return C;
}
/*
double* correlate_FFT(double *Fix, unsigned M, double *Slip, unsigned L, int K)
{
	if(K<0)
	{
		double* temp1=Fix;
		Fix=Slip;
		Slip=temp1;
		unsigned temp2 = M;
		M=L;
		L=temp2;
		K=-K;
	}
	unsigned N;
	for(N=1;N<L+K;N<<=1)
		;
	complex *Fix_modify = (complex*)malloc(sizeof(complex)*N);
	complex *Slip_modify = (complex*)malloc(sizeof(complex)*N);
	for(int i=0;i<N;i++)
	{
		if(i<M)Fix_modify[i].r=Fix[i];else Fix_modify[i].r=0;
		Fix_modify[i].i=0;
		if(i<L)Slip_modify[i].r=Slip[i];else Slip_modify[i].r=0;
		Slip_modify[i].i=0;
	}
	printf("First FFT Start!\n");
	fft(Fix_modify,N);
	printf("Second FFT Start!\n");
	fft(Slip_modify,N);
	printf("FFT finished!\n");
	for(int i=0;i<N;i++)
	{
		Slip_modify[i].i=-Slip_modify[i].i;
		Fix_modify[i]=ccmltp(Fix_modify[i],Slip_modify[i]);
	}
	ifft(Fix_modify,N);
	double *CC = (double*)malloc(sizeof(double)*K);
	for(int i=0;i<K;i++)CC[i]=Fix_modify[i].r;
	free(Fix_modify);
	free(Slip_modify);
	return CC;
}
*/
double* correlate_FFTW(double *Fix, unsigned M, double *Slip, unsigned L, int K)
{
	if(K<0)
	{
		double* temp1=Fix;
		Fix=Slip;
		Slip=temp1;
		unsigned temp2 = M;
		M=L;
		L=temp2;
		K=-K;
	}
	unsigned N;
	//for(N=1;N<L+K;N<<=1)
	//	;
	N=L+K;
	fftw_plan p;
	fftw_complex *Fix_modify, *Slip_modify, *Fix_modify_out, *Slip_modify_out, *CC_modify, *CC_modify_out;
	Fix_modify = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
	Fix_modify_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
	Slip_modify = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
	Slip_modify_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
	for(int i=0;i<N;i++)
	{
		if(i<M)Fix_modify[i][0]=Fix[i];else Fix_modify[i][0]=0;
		Fix_modify[i][1]=0;
		if(i<L)Slip_modify[i][0]=Slip[i];else Slip_modify[i][0]=0;
		Slip_modify[i][1]=0;
	}
	//printf("First FFT Start!\n");
	p=fftw_plan_dft_1d(N, Fix_modify, Fix_modify_out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
	//printf("Second FFT Start!\n");
	p=fftw_plan_dft_1d(N, Slip_modify, Slip_modify_out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
	//printf("FFT finished!\n");

	CC_modify = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
	CC_modify_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
	for(int i=0;i<N;i++)
	{
		Slip_modify_out[i][1]=-Slip_modify_out[i][1];
		CC_modify[i][0]=Fix_modify_out[i][0]*Slip_modify_out[i][0] - Fix_modify_out[i][1]*Slip_modify_out[i][1];
		CC_modify[i][1]=Fix_modify_out[i][1]*Slip_modify_out[i][0] + Fix_modify_out[i][0]*Slip_modify_out[i][1];	
	}
	p=fftw_plan_dft_1d(N, CC_modify, CC_modify_out, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
	double *CC = (double*)malloc(sizeof(double)*K);
	for(int i=0;i<K;i++)CC[i]=CC_modify_out[i][0]/N;
	fftw_free(Fix_modify);
	fftw_free(Slip_modify);
	fftw_free(Fix_modify_out);
	fftw_free(Slip_modify_out);
	fftw_free(CC_modify);
	fftw_free(CC_modify_out);
	return CC;
}

#endif