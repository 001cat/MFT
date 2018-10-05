/*****************day-time add or minus****************
calculate the day and time after some time elapsed.
******************************************************
******************************************************
*	2015/10/20 V1.21 by Ayu
*	Add _timecal_ variable, for include this header more than once
******************************************************
*	2015/10/20 V1.22 by Ayu
*	Fix a bug in timediff(), timeday aa or bb will change if the day of them are different
******************************************************
*	2015/11/09 V1.23 by Ayu
*	Fix a bug in day2jday and jday2day; these function was wrong.
******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _timecal_
#define _timecal_
typedef struct daytime
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	double second;
}DAYTIME;
//typedef daytime DAYTIME;
typedef struct daysec
{
	int year;
	int month;
	int day;
	double second;
}DAYSEC;
//typedef daysec DAYSEC;

int runnianpanduan(int year);				//whether this year is leap year
//int day2jday(int month, int day);			//transfor month day to jdays

void timeadd_second(DAYTIME* a, double sec);
void timeadd_minute(DAYTIME* a, int min);
void timeadd_hour(DAYTIME* a, int hour);
void timeadd_day(DAYTIME* a, int day);
void timeadd_month(DAYTIME* a, int month);
void timeadd_year(DAYTIME* a, int year);

void daysec2daytime(DAYTIME* a, DAYSEC* b);
void daytime2daysec(DAYTIME* a, DAYSEC* b);

void timeadd(DAYTIME* a, DAYTIME* b);
double timediff(DAYTIME *a, DAYTIME* b);
void normdaytime(DAYTIME* a);

void timeadd(DAYTIME* a, DAYTIME* b)/*add days, hours, minutes and seconds to daytime*/
{
	timeadd_second(a, b->second);
	timeadd_minute(a, b->minute);
	timeadd_hour(a, b->hour);
	timeadd_day(a, b->day);
	timeadd_month(a, b->month);
	timeadd_year(a, b->year);
}
double timediff(DAYTIME *aa, DAYTIME* bb)/* calculate the daytime difference, a-b */
{
	double sum_second = 0;
	int sum_day = 0;
	DAYTIME* a = (DAYTIME*)malloc(sizeof(DAYTIME)); *a = *aa;
	DAYTIME* b = (DAYTIME*)malloc(sizeof(DAYTIME)); *b = *bb;
	normdaytime(a); normdaytime(b);
	while (a->year > b->year || (a->year == b->year &&  a->month > b->month) || (a->year == b->year &&  a->month == b->month && a->day > b->day) )
	{
		timeadd_day(a, -1);
		sum_day++;
	}
	while (a->year < b->year || (a->year == b->year &&  a->month < b->month) || (a->year == b->year &&  a->month == b->month && a->day < b->day))
	{
		timeadd_day(a, 1);
		sum_day--;
	}
	sum_second = sum_day * 86400 + (a->hour - b->hour) * 3600 + (a->minute - b->minute) * 60 + (a->second - b->second);
	return sum_second;
}

void timeadd_second(DAYTIME* a, double sec)
{
	a->second += sec;
	int aminute = (int)(a->second / 60);
	a->second -= aminute * 60;
	if (a->second < 0) { a->second += 60; aminute -= 1; }
	if (aminute != 0) timeadd_minute(a, aminute);
}
void timeadd_minute(DAYTIME* a, int min)
{
	a->minute += min;
	min = a->minute / 60;
	a->minute = a->minute % 60;
	if (a->minute < 0) { a->minute += 60; min -= 1; }
	if (min !=0 ) timeadd_hour(a, min);
}
void timeadd_hour(DAYTIME* a, int hour)
{
	a->hour += hour;
	hour = a->hour / 24;
	a->hour = a->hour % 24;
	if (a->hour < 0) { a->hour += 24; hour -= 1; }
	if (hour != 0) timeadd_day(a, hour);
}
void timeadd_day(DAYTIME* a, int day)
{
	a->day += day;
	int monthdaychart[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int runnianchart[12] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (a->day>0)
	{
		while (a->day > (monthdaychart[(a->month - 1)%12] + runnianpanduan(a->year) * runnianchart[(a->month - 1)%12]))
		{
			a->day -= (monthdaychart[(a->month - 1)%12] + runnianpanduan(a->year) * runnianchart[(a->month - 1)%12]);
			timeadd_month(a,1);
		}
	}
	else
	{
		while (a->day <= 0)
		{
			timeadd_month(a, -1);
			a->day += (monthdaychart[(a->month - 1)%12] + runnianpanduan(a->year) * runnianchart[(a->month - 1)%12]);
		}
	}
}
void timeadd_month(DAYTIME* a, int month)
{
	a->month += month;
	month = (a->month-1) / 12;
	a->month = (a->month-1) % 12+1;
	if (a->month <= 0) { a->month += 12; month -= 1; }
	if (month != 0) timeadd_year(a, month);
}
void timeadd_year(DAYTIME* a, int year)
{
	year += a->year;
	if (year >= 0 && a->year < 0) a->year = year + 1;
	else if (year <= 0 && a->year > 0) a->year = year - 1;
	else a->year = year;
}

void daysec2daytime(DAYTIME* a, DAYSEC* b)
{
	*a = { b->year, b->month, b->day, 0, 0, b->second };
	normdaytime(a);
}
void daytime2daysec(DAYTIME* a, DAYSEC* b)
{
	normdaytime(a);
	*b = { a->year, a->month, a->day, (a->hour) * 3600 + (a->minute) * 60 + (a->second) };
}

void normdaytime(DAYTIME* a)
/*for daytime such as, 2015 01 32 06 00 00, trans to a correct format: 2015 02 01 06 00 00*/
/*you can also use this way to get a */
{
	DAYTIME* a0 = (DAYTIME*)malloc(sizeof(DAYTIME));
	*a0 = { 0, 0, 0, 0, 0, 0 };
	timeadd(a, a0);
	free(a0);
}

int day2jday(int year, int month, int day)
{
	int daynum = day;
	int i = 1;
	int monthdaychart[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (runnianpanduan(year)==1) monthdaychart[1]++;
	while (i<month)
	{
		daynum += monthdaychart[i - 1];
		i++;
	}
	return daynum;
}
int jday2day(int year, int jday)
{
	int monthday = 100;
	int i = 1;
	int monthdaychart[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (runnianpanduan(year)==1) monthdaychart[1]++;
	while (jday > monthdaychart[i-1])
	{
		jday -= monthdaychart[i - 1];
		monthday += 100;
		i++;
	}
	monthday += jday;
	return monthday;
}
int runnianpanduan(int year)
{
	if (year % 4 == 0 && year % 100 != 0)
		return 1;
	else if (year % 400 == 0)
		return 1;
	else
		return 0;
}

void showdaytime(DAYTIME* daytime)
{
	printf("Year: %d\nMonth: %d\nDay: %d\nHour: %d\nMinute: %d\nSecond: %f\n",daytime->year,daytime->month,daytime->day,daytime->hour,daytime->minute,daytime->second);
}

int isdaytimecorrect(DAYTIME* daytime)
{
	if (daytime->year == 0) return -1;
	if (daytime->month > 12 || daytime->month < 1) return -1;
	int monthdaychart[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (runnianpanduan(daytime->year)==1) monthdaychart[1]++;
	if (daytime->day < 1 || daytime->day > monthdaychart[daytime->month - 1]) return -1;
	if (daytime->hour < 0 || daytime->hour > 24) return -1;
	if (daytime->minute < 0 || daytime->minute > 60) return -1;
	if (daytime->second < 0 || daytime->second > 60) return -1;
	return 1;
}
#endif