#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <fftw3.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "sacio_ayu.h"
#include "timecal.h"
#include "crosscorrelation.h"
#include "filt.h"
using namespace std;

#define MAX_EVTDETECT 2000
#define path_MAX 1000
#include "MFTfuncs.c"
#include "MFTdetect.c"



void warnfunc()
{
	cout << "Usage: ./MFT [tpname] [tptime] [tpsac dir] [tp arrival time list] "
		 << "[tpcutb] [tpcute] [time of scan start] [time of scan end] [nsigma] [list of sac file for scan]"
		 <<endl;
	cout << "       ./MFT [tpname] [tptime] [tpsac dir] [tp arrival time list] "
		 << "[tpcutb] [tpcute] [time of scan start] [time of scan end] [nsigma] [list of sac file for scan] "
		 << "-plot [directory for save sac file for plot]"
		 <<endl;
}
void helpfunc()
{
	warnfunc();
	cout << "[tpname]: name of template" << endl;
	cout << "[tptime]: time when template event occured" << endl;
	cout << "[tpsac dir]: directory of sac records of template, saved as NETWORK.STATION.BAND.SAC" << endl;
	cout << "[tp arrival time list]: arrival time list of template event, each line saved as" << endl;
	cout <<	"NETWORK.STATION.BAND.SAC YYYY MM DD hh mm ss.ss" << endl;
	cout << "[tpcutb/tpcute]: begin/end time of template sac piece for crosscorrelation, based on arrival time" << endl;
	cout << "[time of scan start/end]: time segment for find new event, only new event occured in this segment will be found" << endl;
	cout << "[nsigma]: the factor to multiply sigma, we use mean+nsigma*sigma as threshold" << endl;
	cout << "[list of sac file for scan]input of sac file list for scan, each line for one file and using # to seperate different component" << endl;
}
int read_event_sacs_syn(char**name, unsigned filenum,SAC *sac, DAYTIME* eventtime, DAYTIME* syntime, double before, double after, char** datarecord, char* dealterminal, char* dealgap, char* dealoverlap);
void print_event(EVTST* evt_detect_lst, int n_evtdet, char* sacintemplate[path_MAX], int n_comp);
int writesac4plot(EVTST* evtlst, int n_evt, SACDOUBLE** sacevt, SACDOUBLE** sacscn, \
	char* sacintemplate[path_MAX], int n_comp, char* sac4plotdir,\
	int weak_nMAX);

int main(int argc, char** argv)
{
	clock_t t_START = clock();

	/* Read Input */
	bool isplot = FALSE;
	char sac4plotdir[path_MAX];

	if (argc == 13 && strcmp(argv[11],"-plot") == 0)
	{
		strcpy(sac4plotdir,argv[12]);
		isplot = TRUE;
	}
	else if(argc == 11)
	{
		isplot = FALSE;
	}
	else if(argc == 2 && strcmp(argv[1], "-h") == 0)
	{
		helpfunc();
		return -1;
	}
	else
	{
		warnfunc();
		return -1;
	}

	const char* eventname = argv[1];
	DAYTIME* evtstarttime = (DAYTIME*)malloc(sizeof(DAYTIME));
	char2daytime(argv[2], evtstarttime);
	const char* eventdatadir = argv[3];
	const char* eventarrlst = argv[4];
	const double eventcutb = atof(argv[5]);
	const double eventcute = atof(argv[6]);
	DAYTIME* scntimestart = (DAYTIME*)malloc(sizeof(DAYTIME));
	char2daytime(argv[7], scntimestart);
	DAYTIME* scntimefinish = (DAYTIME*)malloc(sizeof(DAYTIME));
	char2daytime(argv[8], scntimefinish);
	double nsigma = atof(argv[9]);
	const char* insaclst = argv[10];
	

	/* Set Const Paras */ 
	double weak_MAX = 0.4; 
	char dealterminal[] = "keep";
	char dealgap[] = "zeros";
	char dealoverlap[] = "average";
	// use matlab to get a,b: [b,a] = butter(2,[f1/fc,f2/fc]) 
	//2-15hz / 50hz sample
	double a[5]={1.0, -1.355697485, 0.4706337422, -0.1910263999, 0.1725312505};
	double b[5]={0.3115387742, 0., -0.6230775485, 0., 0.3115387742};
	//1-8hz / 50hz sample
	//double a[5]={1.0, -2.63632988, 2.671098332, -1.319927866, 0.2946365276};
	//double b[5]={0.1173510367, 0., -0.2347020734, 0., 0.1173510367};
	
	//1-20hz / 50hz sample
	// double a[5]={1.0, -0.6874450146, -0.8157085862, 0.1939361276, 0.3476653949};
	// double b[5]={0.582517797, 0., -1.165035594, 0., 0.582517797};
	//1-10hz
	//double a[5] = {1.0, -2.299055356, 1.96749776, -0.8748055564, 0.2196539839};
	//double b[5] = {0.1750876437, 0., -0.3501752873, 0., 0.1750876437};
	//1-15hz
	//double a[5] = {1.0, -1.495321274, 0.4565602601, -0.1171063728, 0.1802320739};
	//double b[5] = {0.3503463779, 0., -0.7006927557, 0., 0.3503463779};
	//2-10hz
	// double a[5] = {1.0, -2.221934917, 2.001987831, -0.9780053579, 0.2523246263};
	// double b[5] = {0.1453238839, 0., -0.2906477678, 0., 0.1453238839};
	//2-15hz
	// double a[5] = {1.0, -1.355697485, 0.4706337422, -0.1910263999, 0.1725312505};
	// double b[5] = {0.3115387742, 0, -0.6230775485, 0, 0.3115387742};
	//2-20hz
	// double a[5] = {1.0, -0.5170037745, -0.7343184542, 0.1038433984, 0.2946365276};
	// double b[5] = {0.5299672271, 0., -1.059934454, 0., 0.5299672271};
	//1-10hz / 100hz sample
	// double a[5] = {1.0, -3.159463302, 3.792684423, -2.082573317, 0.4504454301};
	// double b[5] = {0.05644846226, 0., -0.1128969245, 0., 0.05644846226};
	//1-15hz
	// double a[5] = {1.0, -2.736180357, 2.81356985, -1.369919875, 0.2946365276};
	// double b[5] = {0.1173510367, 0., -0.2347020734, 0., 0.1173510367};
	//1-20hz
	// double a[5] = {1.0, -2.335082402, 1.95096469, -0.8192636853, 0.2066719852};
	// double b[5] = { 0.1906166001, 0., -0.3812332002, 0., 0.1906166001};
	//2-10hz
	// double a[5] = {1.0, -3.174777319, 3.886581585, -2.19912329, 0.4918122372};
	// double b[5] = {0.04613180209, 0, -0.09226360419, 0, 0.04613180209};
	//2-15hz
	// double a[5] = {1.0, -2.723053333, 2.858937479, -1.447037203, 0.3197324699};
	// double b[5] = {0.1040783568, 0., -0.2081567135, 0., 0.1040783568};
	//2-20hz
	// double a[5] = {1.0, -2.299055356, 1.96749776, -0.8748055564, 0.2196539839};
	// double b[5] = {0.1750876437, 0., -0.3501752873, 0., 0.1750876437};
	int n_filter = 5;
	int n_comp_MAX = 200;
	double redundance = 200;

	/* Initial Paras */
	DAYTIME* evttime = (DAYTIME*)malloc(sizeof(DAYTIME));
	DAYTIME* daytimetmp = (DAYTIME*)malloc(sizeof(DAYTIME));
	//DAYTIME* daytimetmp_debug1 = (DAYTIME*)malloc(sizeof(DAYTIME));
	//DAYTIME* daytimetmp_debug2 = (DAYTIME*)malloc(sizeof(DAYTIME));
	SAC* sactmp = (SAC*)malloc(sizeof(SAC));
	char chartmp1[path_MAX], chartmp2[path_MAX];
	int n_comp = 0, n_sacin1comp = 0, n_pointavailable_1sac = 0;
	double delta4syn = 0;
	double sigma = 0, mean = 0;			// Sigma and mean of CC array
	string evtarr, insac;
	char** datarecord = (char**)malloc(sizeof(char*));
	double* traveltime = (double*)malloc(sizeof(double)*n_comp_MAX);
	SACDOUBLE** sacevt = (SACDOUBLE**)malloc(sizeof(SACDOUBLE*)*n_comp_MAX);
	SACDOUBLE** sacscn = (SACDOUBLE**)malloc(sizeof(SACDOUBLE*)*n_comp_MAX);
	SACDOUBLE** sacCC = (SACDOUBLE**)malloc(sizeof(SACDOUBLE*)*n_comp_MAX);
	char* sacin1comp[n_comp_MAX];
	char* sacintemplate[n_comp_MAX];
	for (int i = 0; i<n_comp_MAX; i++)
	{
		sacin1comp[i] = (char*)malloc(sizeof(char)*path_MAX);
		sacintemplate[i] = (char*)malloc(sizeof(char)*path_MAX);
		sacCC[i] = (SACDOUBLE*)malloc(sizeof(SACDOUBLE));
		sacevt[i] = (SACDOUBLE*)malloc(sizeof(SACDOUBLE));
		sacscn[i] = (SACDOUBLE*)malloc(sizeof(SACDOUBLE));
	}
	ifstream in_evtarrlst(eventarrlst);
	ifstream in_insaclst(insaclst);

	/* Main -- get CC list */
	if(in_evtarrlst)
	{
		while (getline(in_evtarrlst, evtarr))				// for each component
		{
			if (n_comp >= n_comp_MAX) return -1;			// avoid too many component				
			evtarr.copy(chartmp1,evtarr.length(),0);
			chartmp1[evtarr.length()] = '\0';
			//cout << evtarr << ":" << endl;
			fprintf(stderr,"%s:\n",chartmp1);	
			char2daytime_nosymbol(strchr(chartmp1,' ')+1,evttime);	//get arrival time

			n_sacin1comp = 0;
			if (in_insaclst)								// gether all sacfile for merge
			{
				while( getline(in_insaclst, insac) )
				{
					if(insac == "#") break;
					insac.copy(sacin1comp[n_sacin1comp],insac.length(),0);
					*(sacin1comp[n_sacin1comp]+insac.length()) = '\0';
					n_sacin1comp++;
					//cout << insac << endl;
				}
			}

			evtarr.copy(chartmp1,evtarr.length(),0);		// get fullpath of sac event data
			*(strchr(chartmp1,' ')) = '\0';					// save in chartmp2
			strcpy(sacintemplate[n_comp],chartmp1);
			strcpy(chartmp2, eventdatadir); strcat(chartmp2,"/"); strcat(chartmp2,chartmp1);
		
			read_sac_double(chartmp2, sacevt[n_comp]);				// get sac
			change_nztime_sac(sacevt[n_comp]->sachead, sacevt[n_comp]->sachead->b);
			filt(b,a,n_filter,sacevt[n_comp]->sacdata,sacevt[n_comp]->sachead->npts);
			readbegintime(daytimetmp, sacevt[n_comp]->sachead);
			cut_sac(sacevt[n_comp],evttime,eventcutb,eventcute);
		
			if(read_event_sacs_syn(sacin1comp, n_sacin1comp, sactmp, scntimestart, daytimetmp, -2*redundance, \
				timediff(scntimefinish, scntimestart)+2*redundance, datarecord, dealterminal, \
				dealgap, dealoverlap ) < 0 ) continue;
			n_pointavailable_1sac = 0;
			for (int i  = 0; i<sactmp->sachead->npts;i++) if((*datarecord)[i]>0) n_pointavailable_1sac++;
			if(2*n_pointavailable_1sac<sactmp->sachead->npts)
			{
				fprintf(stderr,"Warning: Too little data read for MFT in %s\n",sacintemplate[n_comp]);
				continue;
			}
			free(*datarecord);
			change_nztime_sac(sactmp->sachead, sactmp->sachead->b);
			float2double_sac(sactmp, sacscn[n_comp]);
			free(sactmp->sacdata);free(sactmp->sachead);
			filt(b,a,n_filter,sacscn[n_comp]->sacdata,sacscn[n_comp]->sachead->npts);
			memcpy(daytimetmp, scntimestart, sizeof(DAYTIME));timeadd_second(daytimetmp,timediff(evttime, evtstarttime));
			delta4syn = timediff(daytimetmp, evttime)+(0-1*redundance)-eventcutb;
			if (isexactdiv(delta4syn, sacevt[n_comp]->sachead->delta, sacevt[n_comp]->sachead->delta/100) == 1) delta4syn = 0;
			else delta4syn = (delta4syn/sacevt[n_comp]->sachead->delta - floor(delta4syn/sacevt[n_comp]->sachead->delta))*sacevt[n_comp]->sachead->delta;
			//cout<<delta4syn<<endl;
			//readbegintime(daytimetmp_debug1, sacscn[n_comp]->sachead);
			//showdaytime(daytimetmp_debug1);
			cut_sac(sacscn[n_comp],daytimetmp,0-1*redundance-delta4syn,timediff(scntimefinish, scntimestart)+redundance);

		// write evt and scn for debug
		// sprintf(chartmp1, "%devt.sac", n_comp);
		// printf("%s\n",chartmp1);
		// write_sac_double(chartmp1, sacevt[n_comp]);
		// sprintf(chartmp1, "%dscn.sac", n_comp);
		// printf("%s\n",chartmp1);
		// write_sac_double(chartmp1, sacscn[n_comp]);

			//get CC
			sacCC[n_comp]->sachead = (SACHEADDOUBLE*)malloc(sizeof(SACHEADDOUBLE));
			memcpy(sacCC[n_comp]->sachead, sacscn[n_comp]->sachead, sizeof(SACHEADDOUBLE));
			sacCC[n_comp]->sachead->npts = sacscn[n_comp]->sachead->npts - sacevt[n_comp]->sachead->npts + 1;
			sacCC[n_comp]->sacdata = crosscorrelation_inside_strict_FFT(sacscn[n_comp]->sacdata, \
				sacscn[n_comp]->sachead->npts, sacevt[n_comp]->sacdata, sacevt[n_comp]->sachead->npts);
			evtarr.copy(sacintemplate[n_comp],evtarr.length()+1,0);
			*(strchr(sacintemplate[n_comp],' ')) = '\0';
			traveltime[n_comp] = timediff(evttime, evtstarttime);
			n_comp++;

		// write CC for debug
		// sprintf(chartmp1, "%s.CC", sacintemplate[n_comp-1]);
		// printf("%s\n",chartmp1);
		// write_sac_double(chartmp1, sacCC[n_comp-1]);
		// printf("n_comp:%d\n",n_comp);
		//return -1;
		
		}

	}

	/* Main -- stack */
	double peaktop = -2;
	SACDOUBLE* sacstack = (SACDOUBLE*)malloc(sizeof(SACDOUBLE));
	sacstack->sachead = (SACHEADDOUBLE*)malloc(sizeof(SACHEADDOUBLE));
	memcpy(sacstack->sachead, sacCC[n_comp-1]->sachead, sizeof(SACHEADDOUBLE));
	int weak_nMAX = round(weak_MAX / sacstack->sachead->delta);
	sacstack->sachead->npts = sacCC[n_comp-1]->sachead->npts - 2*weak_nMAX;	//weak_nMAX-1 is enough			//due to sac cut will cause different npts for each sac file
	DAYTIME* newzerotime4stack = (DAYTIME*)malloc(sizeof(DAYTIME));
	readzerotime(newzerotime4stack, sacstack->sachead);
	timeadd_second(newzerotime4stack, -1.0*timediff(evttime, evtstarttime)+(weak_nMAX-1)*sacstack->sachead->delta);
	writezerotime(sacstack->sachead, newzerotime4stack);

	SACDOUBLE* sacstackweak = (SACDOUBLE*)malloc(sizeof(SACDOUBLE));
	sacstackweak->sachead = (SACHEADDOUBLE*)malloc(sizeof(SACHEADDOUBLE));
	memcpy(sacstackweak->sachead, sacstack->sachead, sizeof(SACHEADDOUBLE));
	sacstack->sacdata = (double*)malloc(sizeof(double)*sacstack->sachead->npts);
	sacstackweak->sacdata = (double*)malloc(sizeof(double)*sacstackweak->sachead->npts);
	memset(sacstack->sacdata, 0, sizeof(double)*sacstack->sachead->npts);
	memset(sacstackweak->sacdata, 0, sizeof(double)*sacstackweak->sachead->npts);
	for (int i = 0; i < n_comp; i++) { for(int j = 0; j < sacstack->sachead->npts; j++) { sacstack->sacdata[j] += sacCC[i]->sacdata[j+weak_nMAX-1]; }}
	for (int j = 0; j < sacstack->sachead->npts; j++) sacstack->sacdata[j] /= n_comp;

	for (int i = 0; i < n_comp; i++)
	{
		peaktop = getmax(sacCC[i]->sacdata, weak_nMAX);
		sacstackweak->sacdata[0] += peaktop;
		for(int j = 1; j < sacstackweak->sachead->npts; j++)
		{
			if(peaktop < sacCC[i]->sacdata[j+weak_nMAX-1]) peaktop = sacCC[i]->sacdata[j+weak_nMAX-1];
			else if(peaktop == sacCC[i]->sacdata[j-1]) peaktop = getmax(sacCC[i]->sacdata+j, weak_nMAX);
			sacstackweak->sacdata[j] += peaktop;
		}
	}
	for (int j = 0; j < sacstackweak->sachead->npts; j++) sacstackweak->sacdata[j] /= n_comp;
// write_sac_double("stack.sac",sacstack);
// write_sac_double("stackweak.sac",sacstackweak);

	/* Main -- detect */
	EVTST* evt_detect_lst = (EVTST*)malloc(sizeof(EVTST)*MAX_EVTDETECT);
	DAYTIME* sacstackbtime = (DAYTIME*)malloc(sizeof(DAYTIME));
	readbegintime(sacstackbtime, sacstack->sachead);
	int realscnb = max(0,ceil(timediff(scntimestart, sacstackbtime)/sacstack->sachead->delta));
	int realscne = min(sacstack->sachead->npts,floor(timediff(scntimefinish, sacstackbtime)/sacstack->sachead->delta - isexactdiv(timediff(scntimefinish, sacstackbtime) , sacstack->sachead->delta, sacstack->sachead->delta/100)));
	mean = getmean(sacstack->sacdata+realscnb, realscne-realscnb);
	sigma = getsigma(sacstack->sacdata+realscnb, realscne-realscnb);
	int n_evtdet = detect_event(sacstack, mean, sigma, nsigma, n_comp, scntimestart, scntimefinish, evt_detect_lst, eventname);
	for (int i = 0; i<n_evtdet; i++)
	{
		evt_detect_lst[i].traveltime = (double*)malloc(sizeof(double)*n_comp);
		memcpy(evt_detect_lst[i].traveltime, traveltime, sizeof(double)*n_comp);
		evt_detect_lst[i].offset = (double*)malloc(sizeof(double)*n_comp);
		evt_detect_lst[i].CClst = (double*)malloc(sizeof(double)*n_comp);
		evt_detect_lst[i].mag_modify = (double*)malloc(sizeof(double)*n_comp);
	}
	review_event_noweak(evt_detect_lst, n_evtdet, sacCC, sacevt, sacscn, n_comp, weak_nMAX);
	print_event(evt_detect_lst, n_evtdet, sacintemplate, n_comp);
	if (isplot == TRUE) 
	{
		writesac4plot(evt_detect_lst, n_evtdet, sacevt, sacscn, sacintemplate, n_comp, sac4plotdir, weak_nMAX);
	}
//cout << n_evtdet << endl;
//cout << mean << endl;
//cout << sigma << endl;

	clock_t t_FINISH = clock();
	//cout << "Run time is: " << (t_FINISH - t_START)*1.0/CLOCKS_PER_SEC << endl;

	return 0;
}


int writesac4plot(EVTST* evtlst, int n_evt, SACDOUBLE** sacevt, SACDOUBLE** sacscn, \
	char* sacintemplate[path_MAX], int n_comp, char* sac4plotdir,\
	int weak_nMAX)
{
	double before_weakmax_sacscn_cut = 200;
	double after_weakmax_sacscn_cut = 200;
	int n_b_sacscn_cut = 0 ;
	char dirpath[path_MAX],evtname[path_MAX],sacfullpath[path_MAX];
	SACDOUBLE* sac4write = (SACDOUBLE*)malloc(sizeof(SACDOUBLE));
	sac4write->sachead = (SACHEADDOUBLE*)malloc(sizeof(SACHEADDOUBLE));
	DAYTIME* daytime4write = (DAYTIME*)malloc(sizeof(DAYTIME));
	if (access(sac4plotdir, 0) == -1) mkdir(sac4plotdir,0755);
	for (int i = 0; i<n_evt; i++)
	{
		//cout<<i<<endl;
		sprintf(evtname,"NEWEVENT%02d%02d%02d%02d%02d%02d",\
				evtlst[i].evttime_cutb->year%100,\
				evtlst[i].evttime_cutb->month,\
				evtlst[i].evttime_cutb->day,\
				evtlst[i].evttime_cutb->hour,\
				evtlst[i].evttime_cutb->minute,\
				int(evtlst[i].evttime_cutb->second));
		strcpy(dirpath,sac4plotdir);strcat(dirpath,"/");strcat(dirpath,evtname);
	//cout<<1<<endl;
		if (access(dirpath, 0) == -1) mkdir(dirpath,0755);
		for (int j = 0; j<n_comp; j++)
		{
			//cout<<j<<endl;
			memcpy(sac4write->sachead, sacevt[j]->sachead, sizeof(SACHEADDOUBLE));
			sac4write->sacdata = (double*)malloc(sizeof(double)*sac4write->sachead->npts);
		//cout <<2<<endl;
			memcpy(sac4write->sacdata, sacevt[j]->sacdata, sizeof(double)*sac4write->sachead->npts);
			strcpy(sacfullpath,dirpath);strcat(sacfullpath,"/");
			strcat(sacfullpath,sacintemplate[j]);strcat(sacfullpath,".tp");
			change_nztime_sac(sac4write->sachead,sac4write->sachead->b);
			*daytime4write = *(evtlst[i].evttime_cutb);
			timeadd_second(daytime4write, evtlst[i].traveltime[j] - evtlst[i].offset[j]);
			writezerotime(sac4write->sachead, daytime4write);
			write_sac_double(sacfullpath,sac4write);
			free(sac4write->sacdata);
		//cout <<3<<endl;
			memcpy(sac4write->sachead, sacscn[j]->sachead, sizeof(SACHEADDOUBLE));
			n_b_sacscn_cut = evtlst[i].n_point_bak + weak_nMAX -1 - before_weakmax_sacscn_cut/sac4write->sachead->delta;
		//cout << n_b_sacscn_cut << endl;
			sac4write->sachead->npts = (before_weakmax_sacscn_cut+after_weakmax_sacscn_cut)/sac4write->sachead->delta;
			sac4write->sachead->b += n_b_sacscn_cut*sac4write->sachead->delta;
			sac4write->sacdata = (double*)malloc(sizeof(double)*sac4write->sachead->npts);
			memcpy(sac4write->sacdata, sacscn[j]->sacdata+n_b_sacscn_cut, sizeof(double)*sac4write->sachead->npts);
			strcpy(sacfullpath,dirpath);strcat(sacfullpath,"/");
			strcat(sacfullpath,sacintemplate[j]);
			change_nztime_sac(sac4write->sachead,sac4write->sachead->b);
			write_sac_double(sacfullpath,sac4write);
			free(sac4write->sacdata);
		}
	}
	free(sac4write->sachead);free(sac4write);free(daytime4write);
	return 0;
}


void print_event(EVTST* evt_detect_lst, int n_evtdet, char* sacintemplate[path_MAX], int n_comp)
{
	for (int i = 0; i<n_evtdet; i++)
	{
		printf("NEWEVENT%02d%02d%02d%02d%02d%02d",\
				evt_detect_lst[i].evttime_cutb->year%100,\
				evt_detect_lst[i].evttime_cutb->month,\
				evt_detect_lst[i].evttime_cutb->day,\
				evt_detect_lst[i].evttime_cutb->hour,\
				evt_detect_lst[i].evttime_cutb->minute,\
				int(evt_detect_lst[i].evttime_cutb->second));
		printf(" %04d %02d %02d %02d %02d %05.2f\n",\
				evt_detect_lst[i].evttime_cutb->year,\
				evt_detect_lst[i].evttime_cutb->month,\
				evt_detect_lst[i].evttime_cutb->day,\
				evt_detect_lst[i].evttime_cutb->hour,\
				evt_detect_lst[i].evttime_cutb->minute,\
				evt_detect_lst[i].evttime_cutb->second);
		printf("%s %.4f %.4f %.4f\n",evt_detect_lst[i].tpname,evt_detect_lst[i].CC,evt_detect_lst[i].sigma,evt_detect_lst[i].threshold);
		for (int j = 0; j<n_comp; j++)
		{
			printf("%s %.2f %.2f %.4f %.4f\n",\
				sacintemplate[j],\
				evt_detect_lst[i].traveltime[j],\
				evt_detect_lst[i].offset[j],\
				evt_detect_lst[i].mag_modify[j],\
				evt_detect_lst[i].CClst[j]);
		}
		cout<<"##########"<<endl;
	}
}


int read_event_sacs_syn(char**name, unsigned filenum,SAC *sac, DAYTIME* eventtime, DAYTIME* syntime, double before, double after, char** datarecord, char* dealterminal, char* dealgap, char* dealoverlap)
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
		//cout<<3<<endl;
			delta = float2double( saclist[i]->sachead->delta );
			*btime_buffer0 = *zerotime_buffer; timeadd_second(btime_buffer0, saclist[i]->sachead->b); //begin time of sac file
		//showdaytime(beforetime);
		//cout<<4<<endl;
			*starttime = *syntime;// begin time for sac cut
			*endtime = *syntime;// end time for sac cut
		//printf("%.10f, %.10f\n",delta*ceil(timediff(beforetime, starttime) / delta - 0.0001*isexactdiv(timediff(beforetime, starttime) , delta, delta/100)), timediff(beforetime, starttime));
			timeadd_second(starttime, delta*ceil(timediff(beforetime, starttime) / delta - 0.0001*isexactdiv(timediff(beforetime, starttime) , delta, delta/100)));
			timeadd_second(endtime, delta*floor(timediff(aftertime, endtime) / delta - isexactdiv(timediff(aftertime, endtime) , delta, delta/100)));
			memcpy(sac->sachead, saclist[i]->sachead, sizeof(SACHEAD));
			sac->sachead->delta = delta;
			sac->sachead->npts = round(timediff(endtime, starttime) / delta + 1);
			sac->sachead->b = timediff(starttime, eventtime);
		//printf("In read_event_sacs:%10f %10f\n",sac->sachead->b,timediff(starttime, eventtime));
		//showdaytime(starttime);
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
		free(*datarecord);
		free(saclist);
		free(beforetime);
		free(aftertime);
		free(starttime);
		free(endtime);
		free(btime_buffer0); free(btime_buffer1); free(zerotime_buffer); free(zerotime_buffer1); free(zerotime_buffer2);
		
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
