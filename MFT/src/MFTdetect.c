#ifndef _MFTdetect_
#define _MFTdetect_

typedef struct eventstruct
{
	DAYTIME* evttime_cutb;	//first point of found event, have minus travel time
	int n_comp;
	int n_point_bak;
	double CC;
	double sigma;
	double threshold;
	double* mag_modify;
	double* traveltime;
	double* offset;
	double* CClst;
	char tpname[1000];
	//------------|---------	CC
	//      ------|||||||---	Weak CC
	//              |
	//	found after weak and stack
	//  offset = 2*delta
} EVTST;

int buildevt(EVTST* evtst, DAYTIME* btime, int n_point, double delta, int n_comp,\
	double CC, double sigma, double threshold, const char* tpname);
int detect_event(SACDOUBLE* sac, double mean, double sigma, double nsigma,\
	int n_comp, DAYTIME* scntimestart, DAYTIME* scntimefinish, EVTST* evtlst,\
	const char* tpname);
int findmax(double* array, int n, int &position, double &value);
double findmaxrange(double* array, int n);
int review_event(EVTST* evtlst, int n_evt, SACDOUBLE** sacCC, SACDOUBLE** sacevt, \
	SACDOUBLE** sacscn, int n_sacCC, int weak_nMAX);
/*********************/

int review_event(EVTST* evtlst, int n_evt, SACDOUBLE** sacCC, SACDOUBLE** sacevt, \
	SACDOUBLE** sacscn, int n_sacCC, int weak_nMAX)
{

	int position = 0; double value = -2;
	for (int i = 0; i<n_evt; i++)
	{
		for (int j = 0; j<n_sacCC; j++)
		{
			position = 0; value = -2;
			findmax(sacCC[j]->sacdata+evtlst[i].n_point_bak,weak_nMAX,position,value);
			evtlst[i].offset[j] = (weak_nMAX - 1 - position)*sacCC[j]->sachead->delta;
			evtlst[i].CClst[j] = value;
			evtlst[i].mag_modify[j] = findmaxrange(sacevt[j]->sacdata, sacevt[j]->sachead->npts) / findmaxrange(sacscn[j]->sacdata+evtlst[i].n_point_bak+position, sacevt[j]->sachead->npts);
		}
	}
}

int review_event_partweak(EVTST* evtlst, int n_evt, SACDOUBLE** sacCC, SACDOUBLE** sacevt, \
	SACDOUBLE** sacscn, int n_sacCC, int weak_nMAX, double peaktop_min)
{

	int position = 0; double value = -2;
	for (int i = 0; i<n_evt; i++)
	{
		for (int j = 0; j<n_sacCC; j++)
		{
			position = 0; value = -2;
			findmax(sacCC[j]->sacdata+evtlst[i].n_point_bak,weak_nMAX,position,value);
			if (value < peaktop_min) {position = weak_nMAX-1; value = *(sacCC[j]->sacdata+evtlst[i].n_point_bak + position);}
			evtlst[i].offset[j] = (weak_nMAX - 1 - position)*sacCC[j]->sachead->delta;
			evtlst[i].CClst[j] = value;
			evtlst[i].mag_modify[j] = findmaxrange(sacevt[j]->sacdata, sacevt[j]->sachead->npts) / findmaxrange(sacscn[j]->sacdata+evtlst[i].n_point_bak+position, sacevt[j]->sachead->npts);
		}
	}
}

int review_event_noweak(EVTST* evtlst, int n_evt, SACDOUBLE** sacCC, SACDOUBLE** sacevt, \
	SACDOUBLE** sacscn, int n_sacCC, int weak_nMAX)
{

	int position = 0; double value = -2;
	for (int i = 0; i<n_evt; i++)
	{
		for (int j = 0; j<n_sacCC; j++)
		{
			position = weak_nMAX-1; value = *(sacCC[j]->sacdata+evtlst[i].n_point_bak + position);
			evtlst[i].offset[j] = (weak_nMAX - 1 - position)*sacCC[j]->sachead->delta;
			evtlst[i].CClst[j] = value;
			evtlst[i].mag_modify[j] = findmaxrange(sacevt[j]->sacdata, sacevt[j]->sachead->npts) / findmaxrange(sacscn[j]->sacdata+evtlst[i].n_point_bak+position, sacevt[j]->sachead->npts);
		}
	}
}

int findmax(double* array, int n, int &position, double &value)
{
	double MAX = array[0];
	int k = 0;
	for (int i = 1; i < n; i++)
	{
		if (array[i] > MAX)
		{
			MAX = array[i];
			k = i;
		}
	}
	position = k;
	value = MAX;
	return 1;
}

double findmaxrange(double* array, int n)
{
	double max = array[0];
	double min = array[0];
	for (int i = 1; i < n; i++)
	{
		if (array[i] > max) max = array[i];
		else if (array[i] < min) min = array[i];
	}
	return max-min;
}

int detect_event(SACDOUBLE* sac, double mean, double sigma, double nsigma, \
	int n_comp, DAYTIME* scntimestart, DAYTIME* scntimefinish, EVTST* evtlst,\
	const char* tpname)
{
	double EVT_TIMEDIF_MIN = 5;
	double threshold = mean+sigma*nsigma;
	int count_evt = 0, potential_event_time = 0;
	double potential_event_CC = -2;
	DAYTIME* sacbtime = (DAYTIME*)malloc(sizeof(DAYTIME));
	readbegintime(sacbtime, sac->sachead);
	int a = max(0,ceil(timediff(scntimestart, sacbtime)/sac->sachead->delta));
	int b = min(sac->sachead->npts,floor(timediff(scntimefinish, sacbtime)/sac->sachead->delta - isexactdiv(timediff(scntimefinish, sacbtime) , sac->sachead->delta, sac->sachead->delta/100)));
//timeadd_second(sacbtime, b*sac->sachead->delta);
//timeadd_second(sacbtime,a*sac->sachead->delta);
//showdaytime(sacbtime);
	for (int i = a; i < b; i++)
	{
		if(sac->sacdata[i] >= threshold)
		{
			//cout << i << " " << sac->sacdata[i] << endl;
			if (potential_event_CC == -2)
			{
				potential_event_time = i;
				potential_event_CC = sac->sacdata[i];
				continue;
			}
			if( i - potential_event_time > EVT_TIMEDIF_MIN/sac->sachead->delta )
			{
				if (count_evt >= MAX_EVTDETECT)
				{
					fprintf(stderr,"Error: events detected exceed max limit!");
					free(sacbtime);
					return -1;
				}
				buildevt(evtlst+count_evt, sacbtime, potential_event_time, sac->sachead->delta,\
			n_comp, potential_event_CC, sigma, threshold, tpname);
				potential_event_time = i;
				potential_event_CC = sac->sacdata[i];
				count_evt++;
				continue;
			}
			if( potential_event_CC < sac->sacdata[i] )
			{
				potential_event_time = i;
				potential_event_CC = sac->sacdata[i];
			}
		}
	}
	if ( b - potential_event_time > EVT_TIMEDIF_MIN/sac->sachead->delta && potential_event_CC != -2 )
	{
		if (count_evt >= MAX_EVTDETECT)
		{
			free(sacbtime);
			fprintf(stderr,"Error: events detected exceed max limit!");
			return -1;
		}
		buildevt(evtlst+count_evt, sacbtime, potential_event_time, sac->sachead->delta,\
			n_comp, potential_event_CC, sigma, threshold, tpname);
		count_evt++;
	}
	free(sacbtime);
	return count_evt;
}

int buildevt(EVTST* evtst, DAYTIME* btime, int n_point, double delta, int n_comp,\
	double CC, double sigma, double threshold, const char* tpname)
{
	evtst->evttime_cutb = (DAYTIME*)malloc(sizeof(DAYTIME));
	memcpy(evtst->evttime_cutb, btime, sizeof(DAYTIME));
	timeadd_second(evtst->evttime_cutb, delta*n_point);
	evtst->n_comp = n_comp;
	evtst->CC = CC;
	evtst->sigma = sigma;
	evtst->threshold = threshold;
	evtst->n_point_bak = n_point;
	strcpy(evtst->tpname, tpname);
	return 1;
}




#endif