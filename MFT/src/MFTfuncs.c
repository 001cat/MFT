double getmax(double* array, unsigned int n);
double getmean(double* array, int n);
double getsigma(double* array, int n);

int char2daytime(char* day_time_char, DAYTIME* day_time_struct);
int char2daytime_nosymbol(char* day_time_char, DAYTIME* day_time_struct);

void synchronize_sac(SACHEADDOUBLE* sachead, DAYTIME* daytime);
void synchronize_daytime(DAYTIME*, DAYTIME*, double);

/////////////////////////////////////////////////////////
double getmax(double* array, unsigned int n)
{
	double tmp = array[0];
	for (int i = 1; i < n; i++)
	{
		if ( tmp < array[i] ) tmp = array[i];
	}
	return tmp;
}
double getmean(double* array, int n)
{
	double mean = 0;
	for (int i = 0; i < n; i++ ) mean+=array[i];
	mean/=n;
	return mean;
}
double getsigma(double* array, int n)
{
	double sigma = 0;
	double sum = 0;
	for (int i = 0; i < n; i++ )
	{
		sum+=array[i];
		sigma+=array[i]*array[i];
	}
	sigma/=n; sigma-=sum*sum/n/n;
	sigma = sqrt(sigma);
	return sigma;
}


int char2daytime(char* day_time_char, DAYTIME* day_time_struct)
{
	////添加错误判断
	int inlength = strlen(day_time_char);
	char temp[6][20]={'\0'},chtemp;
	int i=0,j=0;
	for (int k=0; k < inlength; k++)
	{
		chtemp = day_time_char[k];
		if (chtemp != '-' && chtemp != ':')
		{
			temp[i][j] = day_time_char[k];
			j++;
		}
		else if(chtemp == '-')
		{
			temp[2][0]=temp[i][j-2];
			temp[2][1]=temp[i][j-1];
			temp[2][2]='\0';
			temp[1][0]=temp[i][j-4];
			temp[1][1]=temp[i][j-3];
			temp[1][2]='\0';
			temp[i][j-4]='\0';
			i=i+3;
			j=0;
		}
		else if(chtemp == ':')
		{
			temp[i][j] = '\0';
			i++; j = 0;
		}
	}
	temp[i][j] = '\0';
	//printf("%s\n",temp[0]);
	day_time_struct->year = atoi(temp[0]);
	day_time_struct->month = atoi(temp[1]);
	day_time_struct->day = atoi(temp[2]);
	day_time_struct->hour = atoi(temp[3]);
	day_time_struct->minute = atoi(temp[4]);
	day_time_struct->second = atof(temp[5]);
	return 1;
}

int char2daytime_nosymbol(char* day_time_char, DAYTIME* day_time_struct)
{
	char tmp[1000];
	int i = 0, j = 0, k = 0;
	//cout<<1<<endl;
	while (day_time_char[i] != '\0')
	{
		//printf("i-j:%d\n",i-j);
		if (day_time_char[i] != ' ' ) tmp[i-j] = day_time_char[i];
		else
		{
			k++;
			switch (k)
			{
				case 1:
					j++;
					break;
				case 2:
					j++;
					break;
				case 3:
					tmp[i-j] = '-';
					break;
				case 4:
					tmp[i-j] = ':';
					break;
				case 5:
					tmp[i-j] = ':';
			}
		}
	i++;
	}
	tmp[i-j] = '\0';
//cout<<tmp<<endl;
	char2daytime(tmp,day_time_struct);
	return 1;
}

void synchronize_daytime(DAYTIME* a, DAYTIME* b, double delta)
{
	double timedif = timediff(a, b);
	memcpy(a, b, sizeof(DAYTIME));
	timeadd_second(a, round(timedif/delta)*delta);
}
void synchronize_sac(SACHEADDOUBLE* sachead, DAYTIME* daytime)
{
	DAYTIME* daytimeb = (DAYTIME*)malloc(sizeof(DAYTIME));
	DAYTIME* daytimetmp = (DAYTIME*)malloc(sizeof(DAYTIME));
	readzerotime(daytimeb ,sachead);
	timeadd_second(daytimeb, sachead->b);
	double doubletmp  = timediff(daytimeb, daytime);
	int inttmp = round(doubletmp/sachead->delta);
	memcpy(daytimetmp, daytime, sizeof(DAYTIME));
	timeadd_second(daytimetmp, sachead->delta * inttmp);
	sachead->b -= timediff(daytimeb, daytimetmp);
	//synchronize_daytime(daytimeb, daytime, sachead->delta);
}