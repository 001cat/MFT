/*********************************************************
* 2015 / 08 / 20  V1.0 by Ayu
*
* 2015 / 09 / 08  V1.1 by Ayu
* Add free_sac
* Add SACDOUBLE struct for save sac data in double
*
* 2015 / 10 / 20  V1.2 by Ayu
* Add read_part_sac
* Repair some bugs
* Add change_nztime_sac
* Add read_event_sac
*
* 2015 / 11 / 06  V1.3 by Ayu
* Add read_event_sacs 
* Add isexactdiv
* Add readzerotime
* Add writezerotime
* Repair some bugs
*
* 2015 / 11 / 07  V1.31 by Ayu
* Add float2double
* read_event_sacs : using float2double to replace (double) for get delta
* read_event_sacs : add protect when input file is not exist
*
* 2016 / 09 / 21 V1.4 bt Ayu
* fix a bug for float2double when input is 0
* change some details
* 2016 / 12 / 27 V1.41 by Ayu
* fix a lethal bug for float2double, a missing return
* 2017 / 02 / 13 V1.42 by Ayu
* fix a lethal bug in float2double_sac, double2float_sac
* 2017 / 02 / 13 V1.43 by Ayu
* add function: rmean_sac, rtrend_sac
* 2017 / 02 / 18 V1.44 by Ayu
* correct read_event_sacs input argument list in declaration
* add function: cut_sac
* using template in change_nztime_sac writezerotime readzerotime
* 2017 / 02 / 20 V1.45 by Ayu
* correct bugs when using ceil() and floor() using isexactdiv()
* add function: readbegintime
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "timecal.h"
#include "sac_ayu.h"

#define max(a,b) ( ((a)>(b)) ? (a):(b) )
#define min(a,b) ( ((a)>(b)) ? (b):(a) )

#ifndef _sacio_ayu_
#define _sacio_ayu_

/* for normal sac r/w */
int	read_sachead(const char *, SACHEAD *);
int read_sac(const char *, SAC *);
int	write_sac(const char *, SAC *);
int free_sac(SAC *);
int is_need_swab4(SACHEAD *);
void swab4(char *, int);

/* for sac using double instead of float */
int read_sac_double(const char *, SACDOUBLE *);
int write_sac_double(const char *, SACDOUBLE *);
int float2double_sac(SAC *, SACDOUBLE *);
int double2float_sac(SACDOUBLE *, SAC *);
int free_sac_double(SACDOUBLE *);


/* for time point of sac */
template < class T > void readzerotime(DAYTIME*, T*);
template < class T > void readbegintime(DAYTIME*, T*);
template < class T > void writezerotime(T*, DAYTIME*);
template < class T > int change_nztime_sac(T*, double);

/* partly deal with sac */
int read_part_sac(const char *, SAC *, DAYTIME *, double);
int read_event_sac(const char*, SAC *, DAYTIME *, double, double);
int read_event_sacs(char**, unsigned, SAC *, DAYTIME* , double, double, char**, char*, char*, char*);
template < class T > int cut_sac(T*, DAYTIME, double, double);

/* others */
double float2double(float);
double float2double(float, int);
int isexactdiv(double, double, double);
template < class T > void rmean_sac(T*);
template < class T > void rtrend_sac(T*);


/****************************/
int	read_sachead(const char *name, SACHEAD *hd)
{
	FILE *strm;
	if((strm = fopen(name,"rb"))==NULL)
	{
		fprintf(stderr, "Unable to open %s\n",name);
		return -1;
	}
	if(fread(hd,sizeof(SACHEAD),1,strm) != 1)
	{
		fprintf(stderr, "Error in reading SAC header %s\n",name);
		fclose(strm);
		return -1;
	}
	int needswab = is_need_swab4(hd);
	if(needswab==-1)
	{
		fprintf(stderr, "Error:The revision of header not match.\n");
		return -1;
	}
	else if(needswab==1) fprintf(stderr, "Warning:Endian type between write and read device is different\n");
	fclose(strm);
	return 0;
}

int read_sac(const char *name, SAC *sac)
{
	sac->sachead = (SACHEAD*)malloc(sizeof(SACHEAD));
	FILE *strm;
	if ((strm = fopen(name, "rb")) == NULL)
	{
		fprintf(stderr, "Unable to open %s\n", name);
		return -1;
	}
	if (fread(sac->sachead, sizeof(SACHEAD), 1, strm) != 1)
	{
		fprintf(stderr, "Error in reading SAC header %s\n", name);
		fclose(strm);
		return -1;
	}
	int needswab = is_need_swab4(sac->sachead);
	if (needswab == -1)
	{
		fprintf(stderr, "Error:The revision of header not match.\n");
		return -1;
	}
	else if (needswab == 1) fprintf(stderr, "Warning:Endian type between write and read device is different\n");
	sac->sacdata = (float*)malloc(sizeof(float)*sac->sachead->npts);
	if (fread(sac->sacdata, sizeof(float)*sac->sachead->npts, 1, strm) != 1)
	{
		fprintf(stderr, "Error in reading SAC data %s\n", name);
		fclose(strm);
		return -1;
	}
	fclose(strm);
	if (needswab == 1) swab4((char*)sac->sacdata, sac->sachead->npts);
	return 0;
}

int write_sac(const char *name, SAC *sac)
{
	FILE *strm;
	if ((strm = fopen(name, "wb")) == NULL)
	{
		fprintf(stderr, "Unable to open %s\n", name);
		return -1;
	}
	fwrite(sac->sachead, sizeof(SACHEAD), 1, strm);
	fwrite(sac->sacdata, sizeof(float)*sac->sachead->npts, 1, strm);
	fclose(strm);
	return 0;
}

int free_sac(SAC *sac)
{
	free(sac->sacdata);
	free(sac->sachead);
	free(sac);
	return 0;
}

int is_need_swab4(SACHEAD *hd)
{
	if(hd->nvhdr!=SACHEAD_VISION)
	{
		swab4((char*)hd, HD_SWAPSIZE);
		if(hd->nvhdr!=SACHEAD_VISION) return -1;
		else return 1;
	}
	else return 0;
}

void swab4(char *pt,int n)
{
  int i;
  char temp;
  for(i=0;i<n;i+=4) {
    temp = pt[i+3];
    pt[i+3] = pt[i];
    pt[i] = temp;
    temp = pt[i+2];
    pt[i+2] = pt[i+1];
    pt[i+1] = temp;
  }
}

/****************************/
int read_sac_double(const char *name, SACDOUBLE *sac)
{
	SAC* sactemp = (SAC*)malloc(sizeof(SAC));
	read_sac(name, sactemp);
	float2double_sac(sactemp, sac);
	free_sac(sactemp);
	return 0;
}

int write_sac_double(const char *name, SACDOUBLE *sac)
{
	SAC* sac_float = (SAC*)malloc(sizeof(SAC));
	double2float_sac(sac, sac_float);
	write_sac(name, sac_float);
	free_sac(sac_float);
	return 0;
}

int float2double_sac(SAC *sac_float, SACDOUBLE *sac_double)
{
	sac_double->sachead = (SACHEADDOUBLE*)malloc(sizeof(SACHEADDOUBLE));
	for (int i = 0; i<70; i++)
	{
		*((double*)(sac_double->sachead)+i) = float2double(*((float*)(sac_float->sachead)+i));
	}
	memcpy((double*)(sac_double->sachead)+70, (float*)(sac_float->sachead)+70, sizeof(SACHEAD)-sizeof(float)*70);
	sac_double->sacdata = (double*)malloc(sizeof(double)*sac_double->sachead->npts);
	for (int i = 0; i < sac_double->sachead->npts; i++) *(sac_double->sacdata + i) = *(sac_float->sacdata + i);
	return 0;
}

int double2float_sac(SACDOUBLE *sac_double, SAC *sac_float)
{
	sac_float->sachead = (SACHEAD*)malloc(sizeof(SACHEAD));
	for (int i = 0; i<70; i++)
	{
		*((float*)(sac_float->sachead)+i) = (float)(*((double*)(sac_double->sachead)+i));
	}
	memcpy((float*)(sac_float->sachead)+70, (double*)(sac_double->sachead)+70, sizeof(SACHEAD)-sizeof(float)*70);
	sac_float->sacdata = (float*)malloc(sizeof(float)*sac_float->sachead->npts);
	//printf("%d", *(sac_double->sacdata + 0));
	for (int i = 0; i < sac_float->sachead->npts; i++) *(sac_float->sacdata + i) = (float)*(sac_double->sacdata + i);
	return 0;
}

int free_sac_double(SACDOUBLE *sac)
{
	free(sac->sacdata);
	free(sac->sachead);
	free(sac);
	return 0;
}


/****************************/
int read_part_sac(const char *name, SAC *sac, DAYTIME* starttime, double duration)
{
	sac->sachead = (SACHEAD*)malloc(sizeof(SACHEAD));
	FILE *strm;
	if((strm = fopen(name,"rb"))==NULL)
	{
		fprintf(stderr, "Unable to open %s\n",name);
		return -1;
	}
	if(fread(sac->sachead,sizeof(SACHEAD),1,strm) != 1)
	{
		fprintf(stderr, "Error in reading SAC header %s\n",name);
		fclose(strm);
		return -1;
	}
	int needswab = is_need_swab4(sac->sachead);
	if(needswab==-1)
	{
		fprintf(stderr, "Error:The revision of header not match.\n");
		return -1;
	}
	else if(needswab==1) fprintf(stderr, "Warning:Endian type between write and read device is different\n");

	/*calculate some time point*/
	DAYTIME* zerotime = (DAYTIME *)malloc(sizeof(DAYTIME));	//trans zero time point, which named by "nz*", to struct DAYTIME
	readzerotime(zerotime, sac->sachead);

	DAYTIME *btime = (DAYTIME *)malloc(sizeof(DAYTIME));	//trans begin time point to struct DAYTIME
	*btime = *zerotime;
	timeadd_second(btime, sac->sachead->b);

	double starttime_relative = timediff(starttime, btime); //get the start/end time of the read part, based on btime
	double endtime_relative = starttime_relative + duration;
	double delta = float2double(sac->sachead->delta);

	/*check times*/
	if (endtime_relative < 0 || duration <= 0)
	{
		fprintf(stderr, "Error: End time of cut is earlier than begin time! (Function read_part_sac in sacio_ayu.h )\n");
		fclose(strm);
		free(sac->sachead);
		free(zerotime); free(btime);
		return -1;
	}
	if (starttime_relative < 0)
	{
		fprintf(stderr, "Warning: Start time of cut is earlier than begin time! (Function read_part_sac in sacio_ayu.h )\n");
		starttime_relative = 0;
	}
	if (endtime_relative >= delta * (sac->sachead->npts - 0.5))
	{
		fprintf(stderr, "Warning: End time of cut is later than end time! (Function read_part_sac in sacio_ayu.h )\n");
		endtime_relative = delta * (sac->sachead->npts - 1);
	}

	/*read sac data*/  ////swab4
	fseek(strm, ceil(starttime_relative / delta)*sizeof(float), 1);
	sac->sachead->npts = floor(endtime_relative / delta) - ceil(starttime_relative / delta) + 1;
	sac->sacdata = (float*)malloc(sizeof(float)*sac->sachead->npts);
	if (fread(sac->sacdata, sizeof(float)*sac->sachead->npts, 1, strm) != 1)
	{
		fprintf(stderr, "Error in reading SAC data %s\n", name);
		free(sac);
		free(zerotime); free(btime);
		fclose(strm);
		return -1;
	}

	/*change sac head*/
	sac->sachead->b = sac->sachead->b + ceil(starttime_relative / delta - 0.01*isexactdiv(starttime_relative, delta, delta/100)) * delta;
	sac->sachead->e = sac->sachead->b + (sac->sachead->npts - 1) * delta;
	if (needswab == 1) swab4((char*)sac->sacdata, sac->sachead->npts);

	fclose(strm); free(zerotime); free(btime);
	return 0;
}

int read_event_sac(const char*name, SAC *sac, DAYTIME* eventtime, double before, double after)
{
	DAYTIME *beforetime = (DAYTIME*)malloc(sizeof(DAYTIME));
	*beforetime = *eventtime;
	timeadd_second(beforetime, before);
	read_part_sac(name, sac, beforetime, after - before);
	free(beforetime);
	return 0;
}

int read_event_sacs(char**name, unsigned filenum,SAC *sac, DAYTIME* eventtime, double before, double after, char** datarecord, char* dealterminal, char* dealgap, char* dealoverlap)
{
	SAC** saclist = (SAC**)malloc(sizeof(SAC*)*filenum);
	sac->sachead = (SACHEAD*)malloc(sizeof(SACHEAD));
	
	DAYTIME *beforetime = (DAYTIME*)malloc(sizeof(DAYTIME));	//time limit before event
	DAYTIME *aftertime = (DAYTIME*)malloc(sizeof(DAYTIME));		//time limit after event
	*beforetime = *eventtime;
	*aftertime = *eventtime;
	timeadd_second(beforetime, before);
	timeadd_second(aftertime, after);

	double delta = 0;
	DAYTIME* starttime = (DAYTIME*)malloc(sizeof(DAYTIME));
	DAYTIME* endtime = (DAYTIME*)malloc(sizeof(DAYTIME));

	DAYTIME* zerotime_buffer = (DAYTIME*)malloc(sizeof(DAYTIME));	//save zerotime time for every input file
	DAYTIME* btime_buffer0 = (DAYTIME*)malloc(sizeof(DAYTIME));		//save b time for first input file
	DAYTIME* btime_buffer1 = (DAYTIME*)malloc(sizeof(DAYTIME));		//save b time for every input file
	DAYTIME* zerotime_buffer1 = (DAYTIME*)malloc(sizeof(DAYTIME));	//save b time for every input file
	DAYTIME* zerotime_buffer2 = (DAYTIME*)malloc(sizeof(DAYTIME));	//save e time for every input file
	int startnum_buffer, endnum_buffer, length_buffer, a, b, i;
	*datarecord = NULL;
	for (i = 0; i < filenum; i++)
	{
		saclist[i] = (SAC*)malloc(sizeof(SAC));
		saclist[i]->sachead = (SACHEAD*)malloc(sizeof(SACHEAD));
		if(read_sachead(name[i], saclist[i]->sachead) != 0) 
		{
			////fprintf(stderr,"Read");
			continue;
		}
		readzerotime(zerotime_buffer, saclist[i]->sachead);

		//set arguments based on first input file
		if (delta == 0)
		{
			delta = float2double( saclist[i]->sachead->delta );
			*btime_buffer0 = *zerotime_buffer; timeadd_second(btime_buffer0, saclist[i]->sachead->b); //begin time of sac file

			*starttime = *btime_buffer0;// begin time for sac cut
			*endtime = *btime_buffer0;// end time for sac cut
			timeadd_second(starttime, delta*ceil(timediff(beforetime, starttime) / delta - 0.01*isexactdiv(timediff(beforetime, starttime), delta, delta/100)));
			timeadd_second(endtime, delta*floor(timediff(aftertime, endtime) / delta - 0.99*isexactdiv(timediff(aftertime, endtime) , delta, delta/100)));
			memcpy(sac->sachead, saclist[i]->sachead, sizeof(SACHEAD));
			sac->sachead->delta = delta;
			sac->sachead->npts = round(timediff(endtime, starttime) / delta + 1);
			sac->sachead->b = timediff(starttime, eventtime);
			sac->sachead->e = timediff(endtime, eventtime);

			//showdaytime(endtime);
			//printf("%f\n",timediff(endtime, eventtime));

			writezerotime(sac->sachead, eventtime);
			sac->sacdata = (float*)malloc(sizeof(float)*(sac->sachead->npts));
			memset(sac->sacdata, 0, sizeof(float)*sac->sachead->npts);

			*datarecord = (char*)malloc(sizeof(char)*(sac->sachead->npts));
			memset(*datarecord, 0, sizeof(char)*sac->sachead->npts);
		}
		//exam the delta and time point of every input file
		if (delta != float2double(saclist[i]->sachead->delta))
		{
			fprintf(stderr, "Error: Delta in SAC files are not the same!\n");
			free(*datarecord); free(saclist); free(beforetime); free(aftertime); free(starttime); free(endtime);
			free(btime_buffer0); free(btime_buffer1); free(zerotime_buffer); free(zerotime_buffer1); free(zerotime_buffer2);
			return -1;
		}
		*btime_buffer1 = *zerotime_buffer; timeadd_second(btime_buffer1, saclist[i]->sachead->b);
		if (isexactdiv(timediff(btime_buffer1, btime_buffer0), delta, delta/10) != 1) fprintf(stderr, "Warning: Time series in SAC files are not synchronous!\n");

		//calculate a and b, a means points before cut series, b means points after cut series
		*zerotime_buffer1 = *zerotime_buffer; *zerotime_buffer2 = *zerotime_buffer;
		timeadd_second(zerotime_buffer1, saclist[i]->sachead->b); timeadd_second(zerotime_buffer2, saclist[i]->sachead->b + delta*(saclist[i]->sachead->npts - 1));
		a = round(timediff(starttime, zerotime_buffer1)/delta); b = round(timediff(zerotime_buffer2, endtime)/delta);

		//copy data
		if (sac->sachead->npts+min(b,0)+min(a,0) <= 0)
		{
			free(saclist[i]->sachead); free(saclist[i]);
			continue;
		}
		free(saclist[i]->sachead); read_sac(name[i], saclist[i]);
		if (strcmp(dealoverlap, "average") == 0)
		{
			for (int j = -min(a, 0); j <= sac->sachead->npts + min(b, 0) - 1; j++)
			{
				sac->sacdata[j] = (sac->sacdata[j] * (*datarecord)[j] + saclist[i]->sacdata[j+a]) / ((*datarecord)[j] + 1);
				(*datarecord)[j]++;
			}
		}
		else if (strcmp(dealoverlap, "overwrite") == 0)
		{
			for (int j = -min(a, 0); j <= sac->sachead->npts + min(b, 0) - 1; j++)
			{
				sac->sacdata[j] = saclist[i]->sacdata[j+a];
				(*datarecord)[j]++;
			}
		}
		free_sac(saclist[i]);
	}

	//check if all input sac file are not exist
	if (*datarecord == NULL)
	{
		fprintf(stderr, "Error: All input file are not exist!\n");
		free(saclist);
		free(beforetime);
		free(aftertime);
		free(starttime);
		free(endtime);
		free(btime_buffer0); free(btime_buffer1); free(zerotime_buffer); free(zerotime_buffer1); free(zerotime_buffer2);
		return -2;
	}

	char isallempty = 1;
	for (i = 0; i < sac->sachead->npts; i++)
	{
		if ((*datarecord)[i] > 0)
		{
			isallempty = 0;
			break;
		}
	}
	if (isallempty == 1)
	{
		fprintf(stderr, "Error: Event record you need do not exsit in given file!\n");
		return -3;
	}

	////dealterminal
	////dealgap
	if (strcmp(dealterminal,"keep") != 0 || strcmp(dealgap, "zeros") != 0)
	{
		int gap_b = -1, gap_e = -1;
		int num_terminal1 = 0, num_terminal2 = 0;
		int nnpts = sac->sachead->npts;
		for (i = 0; i < nnpts; i++)
		{
			if ((*datarecord)[i] == 0)
			{
				if (gap_b == -1) gap_b = i;
			}
			else
			{
				if (gap_b != -1)
				{
					gap_e = i-1;
					if (gap_b == 0)
					{
						num_terminal1 = gap_e - gap_b + 1;
					}
					else if(strcmp(dealgap,"l-interp") == 0)
					{
						for (int j=gap_b; j <= gap_e; j++)
						{
							sac->sacdata[j] = (sac->sacdata[gap_b - 1] * (gap_e + 1 - j) + sac->sacdata[gap_e + 1] * (j - gap_b +1))/(gap_e-gap_b+2);
						}
					}
					gap_b = -1;
				}
			}
		}
		if (gap_b != -1)
		{
			num_terminal2 = nnpts - 1 - gap_b + 1;
		}
		if (strcmp(dealterminal,"omit") == 0)
		{
			sac->sachead->b = timediff(starttime, eventtime) + num_terminal1 * delta ;
			sac->sachead->e = timediff(endtime, eventtime) - num_terminal2 * delta;
			float* sacdata_temp = (float*)malloc(sizeof(float)*(nnpts - num_terminal1 - num_terminal2));
			char* datarecord_temp = (char*)malloc(sizeof(char)*(nnpts - num_terminal1 - num_terminal2));
			memcpy(sacdata_temp, sac->sacdata + num_terminal1, sizeof(float)*(nnpts - num_terminal1 - num_terminal2));
			memcpy(datarecord_temp, (*datarecord) + num_terminal1, sizeof(char)*(nnpts - num_terminal1 - num_terminal2));
			free(sac->sacdata);
			free(*datarecord);
			sac->sacdata = sacdata_temp;
			*datarecord = datarecord_temp;
			sac->sachead->npts -= (num_terminal1 + num_terminal2);
		}
	}
	//exam completeness
	char completeness = 1;
	for (i = 0; i < sac->sachead->npts; i++)
	{
		if ((*datarecord)[i] != 1)
		{
			completeness = 0;
			break;
		}
	}

	free(saclist); 
	free(beforetime); 
	free(aftertime); 
	free(starttime); 
	free(endtime);
	free(btime_buffer0); free(btime_buffer1); free(zerotime_buffer); free(zerotime_buffer1); free(zerotime_buffer2);

	if (completeness == 0) return 2;
	return 0;
}

int cut_sac(SACDOUBLE* sac, DAYTIME* newzerotime, double b, double e)
{
	DAYTIME *starttime = (DAYTIME*)malloc(sizeof(DAYTIME));	//time limit before event
	DAYTIME *endtime = (DAYTIME*)malloc(sizeof(DAYTIME));		//time limit after event
	DAYTIME *zerotime = (DAYTIME*)malloc(sizeof(DAYTIME));

	double delta = sac->sachead->delta;
	readzerotime(zerotime, sac->sachead);
	double timedif = timediff(newzerotime,zerotime);

	*starttime = *zerotime;
	*endtime = *zerotime;
	timeadd_second(starttime, sac->sachead->b);
	timeadd_second(endtime, sac->sachead->b);
	timeadd_second(starttime, delta*ceil((timedif + b - sac->sachead->b) / delta - 0.01*isexactdiv((timedif + b - sac->sachead->b) , delta, delta/100)));
	timeadd_second(endtime, delta*floor((timedif + e - sac->sachead->b) / delta - 0.99*isexactdiv((timedif + e - sac->sachead->b) , delta, delta/100)));
	int nptsremove1 = max(0 ,round((timediff(starttime, zerotime) - sac->sachead->b) / delta));
	int nptsremove2 = max(0 ,round((timediff(zerotime, endtime) + sac->sachead->b + (sac->sachead->npts-1)*delta) / delta));
	int nptsnew = sac->sachead->npts - nptsremove1 - nptsremove2;

	double* tmp = (double*)malloc(sizeof(double)*sac->sachead->npts);
	memcpy(tmp, sac->sacdata, sizeof(double)*sac->sachead->npts);
	free(sac->sacdata);
	sac->sacdata = (double*)malloc(sizeof(double)*nptsnew);
	memcpy(sac->sacdata, tmp+nptsremove1, sizeof(double)*nptsnew);
	sac->sachead->npts = nptsnew;
	change_nztime_sac(sac->sachead, timedif);
	sac->sachead->b += delta * nptsremove1;
	sac->sachead->e = sac->sachead->b + delta * (nptsnew-1);

	free(tmp);
	free(starttime);
	free(endtime);
	free(zerotime);

	return 1;
}


/****************************/
template < class T > int change_nztime_sac(T *sachead, double timeshift)
{
	sachead->b -= timeshift;
	sachead->e -= timeshift;

	if (sachead->o != -12345.0) sachead->o -= timeshift;
	if (sachead->a != -12345.0) sachead->a -= timeshift;
	if (sachead->f != -12345.0) sachead->f -= timeshift;
	if (sachead->t0 != -12345.0) sachead->t0 -= timeshift;
	if (sachead->t1 != -12345.0) sachead->t1 -= timeshift;
	if (sachead->t2 != -12345.0) sachead->t2 -= timeshift;
	if (sachead->t3 != -12345.0) sachead->t3 -= timeshift;
	if (sachead->t4 != -12345.0) sachead->t4 -= timeshift;
	if (sachead->t5 != -12345.0) sachead->t5 -= timeshift;
	if (sachead->t6 != -12345.0) sachead->t6 -= timeshift;
	if (sachead->t7 != -12345.0) sachead->t7 -= timeshift;
	if (sachead->t8 != -12345.0) sachead->t8 -= timeshift;
	if (sachead->t9 != -12345.0) sachead->t9 -= timeshift;

	DAYTIME* zerotime = (DAYTIME *)malloc(sizeof(DAYTIME));	//trans zero time point, which named by "nz*", to struct DAYTIME
	readzerotime(zerotime, sachead);

	timeadd_second(zerotime, timeshift);

	writezerotime(sachead,zerotime);

	free(zerotime);
	return 1;
}
template < class T > void readbegintime(DAYTIME* begintime, T* sachead)
{
	readzerotime(begintime, sachead);
	timeadd_second(begintime, sachead->b);
}
template < class T > void readzerotime(DAYTIME* zerotime, T* sachead)
{
	zerotime->year = sachead->nzyear;
	int monthday = jday2day(sachead->nzyear, sachead->nzjday);
	zerotime->month = monthday / 100;
	zerotime->day = monthday % 100;
	zerotime->hour = sachead->nzhour;
	zerotime->minute = sachead->nzmin;
	zerotime->second = sachead->nzsec + sachead->nzmsec / 1000.0;
}
template < class T > void writezerotime(T* sachead, DAYTIME* zerotime)
{
	sachead->nzyear = zerotime->year;
	sachead->nzjday = day2jday(zerotime->year, zerotime->month, zerotime->day);
	sachead->nzhour = zerotime->hour;
	sachead->nzmin = zerotime->minute;
	sachead->nzsec = (int)(zerotime->second);
	sachead->nzmsec = round((zerotime->second - sachead->nzsec) * 1000);
}

/****************************/
double float2double(float a)
{
	/*int i = 0;
	while (a < 0.1 && a > -0.1)
	{
		a = a * 10;
		i++;
	}
	char s[100];
	sprintf(s, "%.6f", a);
	double b = atof(s);
	while (i > 0)
	{
		b = b / 10;
		i--;
	}
	return b;*/
	return float2double(a,6);
}

double float2double(float a, int N)
{
	if (a == 0) return 0;
	int i = 0;
	while (a < 0.1 && a > -0.1)
	{
		a = a * 10;
		i++;
	}
	if (N > 9999)
	{
		fprintf(stderr, "Error: Illegal Input Argu in 'float2double'.\n");
	}
	char s[100];
	char temp[10], format[12] = "%.";
	sprintf(temp, "%df",N);
	strcat(format,temp);
		//printf("%s\n", format);
	sprintf(s, format, a);
	double b = atof(s);
	while (i > 0)
	{
		b = b / 10;
		i--;
	}
	return b;
}

int isexactdiv(double i, double j, double delta) // is i can be exact division by j, the precision is delta
{
	double k = i / j;
	if (fabs(k - round(k)) < delta) return 1;
	else return 0;
}

template < class T > void rmean_sac(T* sac)
{
	double tmp = 0;
	for (int i = 0; i<sac->sachead->npts; i++) tmp+=sac->sacdata[i];
	tmp/=sac->sachead->npts;
	for (int i = 0; i<sac->sachead->npts; i++) sac->sacdata[i]-=tmp;
}

template < class T > void rtrend_sac(T* sac)
{
	double X = 0;
	double A = 0;
	double B = 0;
	int k = 0, n  = sac->sachead->npts;
	for (int i = 0; i<n; i++) X += sac->sacdata[i];
	X/=n;
	for (int i = 0; i<n; i++)
	{
		sac->sacdata[i] -= X;
		B += sac->sacdata[i] * (1.0*i/(n-1)-1/2.);
		A += (1.0*i/(n-1)-1/2.) * (1.0*i/(n-1)-1/2.);
	}
	double slope = B/A;
	//printf("Slope:%f, A:%f, B%f\n",slope,A,B);
	for (int i = 0; i<n; i++) sac->sacdata[i] -= (1.0*i/(n-1)-1./2)*slope;
}

#endif