/*************************************************************************
  Name:		sac_ayu.h

  Purpose:	structure for header of a SAC (Seismic Analysis Code) 
			data file, and prototype for basic SAC I/O

  References:	O'Neill, D. (1987).  IRIS Interim Data Distribution Format
                (SAC ASCII), Version 1.0 (12 November 1987).  Incorporated
		Research Institutions for Seismology, 1616 North Fort Myer
		Drive, Suite 1440, Arlington, Virginia 22209.  11 pp.
		Tull, J. (1987).  SAC User's Manual, Version 10.2, October 7,
		1987.  Lawrence Livermore National Laboratory, L-205,
		Livermore, California 94550.  ??? pp.

  Original Author:Dennis O'Neill, Lorraine Hwang, Xiaoming Ding, Lupei Zhu
*************************************************************************/

/*********************************************************
* 2015 / 08 / 20  V1.0 by Ayu
*
* 2015 / 09 / 08  V1.1 by Ayu
* Add free_sac
*
* 2015 / 10 / 20  V1.2 by Ayu
* Add read_part_sac
* Add change_nztime_sac
*
* 2016 / 09 / 21 V1.41 by Ayu
* add new struct SACHEADDOUBLE and change head of SACDOUBLE to it
*********************************************************/

#ifndef _sac_ayu_
#define _sac_ayu_


typedef struct sac_head
{
  float	delta;			/* RF time increment, sec    */
  float	depmin;			/*    minimum amplitude      */
  float	depmax;			/*    maximum amplitude      */
  float	scale;			/*    amplitude scale factor */
  float	odelta;			/*    observed time inc      */
  float	b;			/* RD initial time - wrt nz* */
  float	e;			/* RD end time               */
  float	o;			/*    event start            */
  float	a;			/*    1st arrival time       */
  float	internal1;		/*    internal use           */
  float	t0;			/*    user-defined time pick */
  float	t1;			/*    user-defined time pick */
  float	t2;			/*    user-defined time pick */
  float	t3;			/*    user-defined time pick */
  float	t4;			/*    user-defined time pick */
  float	t5;			/*    user-defined time pick */
  float	t6;			/*    user-defined time pick */
  float	t7;			/*    user-defined time pick */
  float	t8;			/*    user-defined time pick */
  float	t9;			/*    user-defined time pick */
  float	f;			/*    event end, sec > 0     */
  float	resp0;			/*    instrument respnse parm*/
  float	resp1;			/*    instrument respnse parm*/
  float	resp2;			/*    instrument respnse parm*/
  float	resp3;			/*    instrument respnse parm*/
  float	resp4;			/*    instrument respnse parm*/
  float	resp5;			/*    instrument respnse parm*/
  float	resp6;			/*    instrument respnse parm*/
  float	resp7;			/*    instrument respnse parm*/
  float	resp8;			/*    instrument respnse parm*/
  float	resp9;			/*    instrument respnse parm*/
  float	stla;			/*  T station latititude     */
  float	stlo;			/*  T station longitude      */
  float	stel;			/*  T station elevation, m   */
  float	stdp;			/*  T station depth, m      */
  float	evla;			/*    event latitude         */
  float	evlo;			/*    event longitude        */
  float	evel;			/*    event elevation        */
  float	evdp;			/*    event depth            */
  float	unused1;		/*    reserved for future use*/
  float	user0;			/*    available to user      */
  float	user1;			/*    available to user      */
  float	user2;			/*    available to user      */
  float	user3;			/*    available to user      */
  float	user4;			/*    available to user      */
  float	user5;			/*    available to user      */
  float	user6;			/*    available to user      */
  float	user7;			/*    available to user      */
  float	user8;			/*    available to user      */
  float	user9;			/*    available to user      */
  float	dist;			/*    stn-event distance, km */
  float	az;			/*    event-stn azimuth      */
  float	baz;			/*    stn-event azimuth      */
  float	gcarc;			/*    stn-event dist, degrees*/
  float	internal2;		/*    internal use           */
  float	internal3;		/*    internal use           */
  float	depmen;			/*    mean value, amplitude  */
  float	cmpaz;			/*  T component azimuth     */
  float	cmpinc;			/*  T component inclination */
  float	unused2;		/*    reserved for future use*/
  float	unused3;		/*    reserved for future use*/
  float	unused4;		/*    reserved for future use*/
  float	unused5;		/*    reserved for future use*/
  float	unused6;		/*    reserved for future use*/
  float	unused7;		/*    reserved for future use*/
  float	unused8;		/*    reserved for future use*/
  float	unused9;		/*    reserved for future use*/
  float	unused10;		/*    reserved for future use*/
  float	unused11;		/*    reserved for future use*/
  float	unused12;		/*    reserved for future use*/
  int	nzyear;			/*  F zero time of file, yr  */
  int	nzjday;			/*  F zero time of file, day */
  int	nzhour;			/*  F zero time of file, hr  */
  int	nzmin;			/*  F zero time of file, min */
  int	nzsec;			/*  F zero time of file, sec */
  int	nzmsec;			/*  F zero time of file, msec*/
  int	nvhdr;			/*  R header version number  */
  int	internal5;		/*    internal use           */
  int	internal6;		/*    internal use           */
  int	npts;			/* RF number of samples      */
  int	internal7;		/*    internal use           */
  int	internal8;		/*    internal use           */
  int	unused13;		/*    reserved for future use*/
  int	unused14;		/*    reserved for future use*/
  int	unused15;		/*    reserved for future use*/
  int	iftype;			/* RA type of file          */
  int	idep;			/*    type of amplitude      */
  int	iztype;			/*    zero time equivalence  */
  int	unused16;		/*    reserved for future use*/
  int	iinst;			/*    recording instrument   */
  int	istreg;			/*    stn geographic region  */
  int	ievreg;			/*    event geographic region*/
  int	ievtyp;			/*    event type             */
  int	iqual;			/*    quality of data        */
  int	isynth;			/*    synthetic data flag    */
  int	unused17;		/*    reserved for future use*/
  int	unused18;		/*    reserved for future use*/
  int	unused19;		/*    reserved for future use*/
  int	unused20;		/*    reserved for future use*/
  int	unused21;		/*    reserved for future use*/
  int	unused22;		/*    reserved for future use*/
  int	unused23;		/*    reserved for future use*/
  int	unused24;		/*    reserved for future use*/
  int	unused25;		/*    reserved for future use*/
  int	unused26;		/*    reserved for future use*/
  int	leven;			/* RA data-evenly-spaced flag*/
  int	lpspol;			/*    station polarity flag  */
  int	lovrok;			/*    overwrite permission   */
  int	lcalda;			/*    calc distance, azimuth */
  int	unused27;		/*    reserved for future use*/
  char	kstnm[8];		/*  F station name           */
  char	kevnm[16];		/*    event name             */
  char	khole[8];		/*    man-made event name    */
  char	ko[8];			/*    event origin time id   */
  char	ka[8];			/*    1st arrival time ident */
  char	kt0[8];			/*    time pick 0 ident      */
  char	kt1[8];			/*    time pick 1 ident      */
  char	kt2[8];			/*    time pick 2 ident      */
  char	kt3[8];			/*    time pick 3 ident      */
  char	kt4[8];			/*    time pick 4 ident      */
  char	kt5[8];			/*    time pick 5 ident      */
  char	kt6[8];			/*    time pick 6 ident      */
  char	kt7[8];			/*    time pick 7 ident      */
  char	kt8[8];			/*    time pick 8 ident      */
  char	kt9[8];			/*    time pick 9 ident      */
  char	kf[8];			/*    end of event ident     */
  char	kuser0[8];		/*    available to user      */
  char	kuser1[8];		/*    available to user      */
  char	kuser2[8];		/*    available to user      */
  char	kcmpnm[8];		/*  F component name         */
  char	knetwk[8];		/*    network name           */
  char	kdatrd[8];		/*    date data read         */
  char	kinst[8];		/*    instrument name        */
} SACHEAD;
typedef struct sac_head_double
{
  double delta;      /* RF time increment, sec    */
  double depmin;     /*    minimum amplitude      */
  double depmax;     /*    maximum amplitude      */
  double scale;      /*    amplitude scale factor */
  double odelta;     /*    observed time inc      */
  double b;      /* RD initial time - wrt nz* */
  double e;      /* RD end time               */
  double o;      /*    event start            */
  double a;      /*    1st arrival time       */
  double internal1;    /*    internal use           */
  double t0;     /*    user-defined time pick */
  double t1;     /*    user-defined time pick */
  double t2;     /*    user-defined time pick */
  double t3;     /*    user-defined time pick */
  double t4;     /*    user-defined time pick */
  double t5;     /*    user-defined time pick */
  double t6;     /*    user-defined time pick */
  double t7;     /*    user-defined time pick */
  double t8;     /*    user-defined time pick */
  double t9;     /*    user-defined time pick */
  double f;      /*    event end, sec > 0     */
  double resp0;      /*    instrument respnse parm*/
  double resp1;      /*    instrument respnse parm*/
  double resp2;      /*    instrument respnse parm*/
  double resp3;      /*    instrument respnse parm*/
  double resp4;      /*    instrument respnse parm*/
  double resp5;      /*    instrument respnse parm*/
  double resp6;      /*    instrument respnse parm*/
  double resp7;      /*    instrument respnse parm*/
  double resp8;      /*    instrument respnse parm*/
  double resp9;      /*    instrument respnse parm*/
  double stla;     /*  T station latititude     */
  double stlo;     /*  T station longitude      */
  double stel;     /*  T station elevation, m   */
  double stdp;     /*  T station depth, m      */
  double evla;     /*    event latitude         */
  double evlo;     /*    event longitude        */
  double evel;     /*    event elevation        */
  double evdp;     /*    event depth            */
  double unused1;    /*    reserved for future use*/
  double user0;      /*    available to user      */
  double user1;      /*    available to user      */
  double user2;      /*    available to user      */
  double user3;      /*    available to user      */
  double user4;      /*    available to user      */
  double user5;      /*    available to user      */
  double user6;      /*    available to user      */
  double user7;      /*    available to user      */
  double user8;      /*    available to user      */
  double user9;      /*    available to user      */
  double dist;     /*    stn-event distance, km */
  double az;     /*    event-stn azimuth      */
  double baz;      /*    stn-event azimuth      */
  double gcarc;      /*    stn-event dist, degrees*/
  double internal2;    /*    internal use           */
  double internal3;    /*    internal use           */
  double depmen;     /*    mean value, amplitude  */
  double cmpaz;      /*  T component azimuth     */
  double cmpinc;     /*  T component inclination */
  double unused2;    /*    reserved for future use*/
  double unused3;    /*    reserved for future use*/
  double unused4;    /*    reserved for future use*/
  double unused5;    /*    reserved for future use*/
  double unused6;    /*    reserved for future use*/
  double unused7;    /*    reserved for future use*/
  double unused8;    /*    reserved for future use*/
  double unused9;    /*    reserved for future use*/
  double unused10;   /*    reserved for future use*/
  double unused11;   /*    reserved for future use*/
  double unused12;   /*    reserved for future use*/
  int nzyear;     /*  F zero time of file, yr  */
  int nzjday;     /*  F zero time of file, day */
  int nzhour;     /*  F zero time of file, hr  */
  int nzmin;      /*  F zero time of file, min */
  int nzsec;      /*  F zero time of file, sec */
  int nzmsec;     /*  F zero time of file, msec*/
  int nvhdr;      /*  R header version number  */
  int internal5;    /*    internal use           */
  int internal6;    /*    internal use           */
  int npts;     /* RF number of samples      */
  int internal7;    /*    internal use           */
  int internal8;    /*    internal use           */
  int unused13;   /*    reserved for future use*/
  int unused14;   /*    reserved for future use*/
  int unused15;   /*    reserved for future use*/
  int iftype;     /* RA type of file          */
  int idep;     /*    type of amplitude      */
  int iztype;     /*    zero time equivalence  */
  int unused16;   /*    reserved for future use*/
  int iinst;      /*    recording instrument   */
  int istreg;     /*    stn geographic region  */
  int ievreg;     /*    event geographic region*/
  int ievtyp;     /*    event type             */
  int iqual;      /*    quality of data        */
  int isynth;     /*    synthetic data flag    */
  int unused17;   /*    reserved for future use*/
  int unused18;   /*    reserved for future use*/
  int unused19;   /*    reserved for future use*/
  int unused20;   /*    reserved for future use*/
  int unused21;   /*    reserved for future use*/
  int unused22;   /*    reserved for future use*/
  int unused23;   /*    reserved for future use*/
  int unused24;   /*    reserved for future use*/
  int unused25;   /*    reserved for future use*/
  int unused26;   /*    reserved for future use*/
  int leven;      /* RA data-evenly-spaced flag*/
  int lpspol;     /*    station polarity flag  */
  int lovrok;     /*    overwrite permission   */
  int lcalda;     /*    calc distance, azimuth */
  int unused27;   /*    reserved for future use*/
  char  kstnm[8];   /*  F station name           */
  char  kevnm[16];    /*    event name             */
  char  khole[8];   /*    man-made event name    */
  char  ko[8];      /*    event origin time id   */
  char  ka[8];      /*    1st arrival time ident */
  char  kt0[8];     /*    time pick 0 ident      */
  char  kt1[8];     /*    time pick 1 ident      */
  char  kt2[8];     /*    time pick 2 ident      */
  char  kt3[8];     /*    time pick 3 ident      */
  char  kt4[8];     /*    time pick 4 ident      */
  char  kt5[8];     /*    time pick 5 ident      */
  char  kt6[8];     /*    time pick 6 ident      */
  char  kt7[8];     /*    time pick 7 ident      */
  char  kt8[8];     /*    time pick 8 ident      */
  char  kt9[8];     /*    time pick 9 ident      */
  char  kf[8];      /*    end of event ident     */
  char  kuser0[8];    /*    available to user      */
  char  kuser1[8];    /*    available to user      */
  char  kuser2[8];    /*    available to user      */
  char  kcmpnm[8];    /*  F component name         */
  char  knetwk[8];    /*    network name           */
  char  kdatrd[8];    /*    date data read         */
  char  kinst[8];   /*    instrument name        */
} SACHEADDOUBLE;

typedef struct sac
{
	SACHEAD *sachead;
	float *sacdata;
}SAC;
typedef struct sac_double
{
	SACHEADDOUBLE *sachead;
	double *sacdata;
}SACDOUBLE;



#define HD_SWAPSIZE 440/* number of bytes in header that need to be swapped on PC (int+float+long)*/
#define HD_FLOATNUM 70/* number of float parameter*/
#define SACHEAD_VISION 6
#define SIZE_OF_SAMPLE 32//remain modified
/* definitions of constants for SAC enumerated data values */
/* undocumented == couldn't find a definition for it (denio, 07/15/88) */
#define IREAL   0		/* undocumented              */
#define ITIME   1		/* file: time series data    */
#define IRLIM   2		/* file: real&imag spectrum  */
#define IAMPH   3		/* file: ampl&phas spectrum  */
#define IXY     4		/* file: gen'l x vs y data   */
#define IUNKN   5		/* x data: unknown type      */
				/* zero time: unknown        */
				/* event type: unknown       */
#define IDISP   6		/* x data: displacement (nm) */
#define IVEL    7		/* x data: velocity (nm/sec) */
#define IACC    8		/* x data: accel (cm/sec/sec)*/
#define IB      9		/* zero time: start of file  */
#define IDAY   10		/* zero time: 0000 of GMT day*/
#define IO     11		/* zero time: event origin   */
#define IA     12		/* zero time: 1st arrival    */
#define IT0    13		/* zero time: user timepick 0*/
#define IT1    14		/* zero time: user timepick 1*/
#define IT2    15		/* zero time: user timepick 2*/
#define IT3    16		/* zero time: user timepick 3*/
#define IT4    17		/* zero time: user timepick 4*/
#define IT5    18		/* zero time: user timepick 5*/
#define IT6    19		/* zero time: user timepick 6*/
#define IT7    20		/* zero time: user timepick 7*/
#define IT8    21		/* zero time: user timepick 8*/
#define IT9    22		/* zero time: user timepick 9*/
#define IRADNV 23		/* undocumented              */
#define ITANNV 24		/* undocumented              */
#define IRADEV 25		/* undocumented              */
#define ITANEV 26		/* undocumented              */
#define INORTH 27		/* undocumented              */
#define IEAST  28		/* undocumented              */
#define IHORZA 29		/* undocumented              */
#define IDOWN  30		/* undocumented              */
#define IUP    31		/* undocumented              */
#define ILLLBB 32		/* undocumented              */
#define IWWSN1 33		/* undocumented              */
#define IWWSN2 34		/* undocumented              */
#define IHGLP  35		/* undocumented              */
#define ISRO   36		/* undocumented              */
#define INUCL  37		/* event type: nuclear shot  */
#define IPREN  38		/* event type: nuke pre-shot */
#define IPOSTN 39		/* event type: nuke post-shot*/
#define IQUAKE 40		/* event type: earthquake    */
#define IPREQ  41		/* event type: foreshock     */
#define IPOSTQ 42		/* event type: aftershock    */
#define ICHEM  43		/* event type: chemical expl */
#define IOTHER 44		/* event type: other source  */
				/* data quality: other problm*/
#define IGOOD  45		/* data quality: good        */
#define IGLCH  46		/* data quality: has glitches*/
#define IDROP  47		/* data quality: has dropouts*/
#define ILOWSN 48		/* data quality: low s/n     */
#define IRLDTA 49		/* data is real data         */
#define IVOLTS 50		/* file: velocity (volts)    */
#define INIV51 51		/* undocumented              */
#define INIV52 52		/* undocumented              */
#define INIV53 53		/* undocumented              */
#define INIV54 54		/* undocumented              */
#define INIV55 55		/* undocumented              */
#define INIV56 56		/* undocumented              */
#define INIV57 57		/* undocumented              */
#define INIV58 58		/* undocumented              */
#define INIV59 59		/* undocumented              */
#define INIV60 60		/* undocumented              */

/* True/false definitions */
#ifndef TRUE
#define FALSE	0
#define TRUE	1
#endif

/* Format strings for writing headers for SAC ASCII files */
#define FCS "%15.7f%15.7f%15.7f%15.7f%15.7f\n"	/* for floats */
#define ICS "%10d%10d%10d%10d%10d\n"		/* for integers */
#define CCS1 "%-8.8s%-8.8s%-8.8s\n"		/* for strings */
#define CCS2 "%-8.8s%-16.16s\n"			/* for strings */



/* a SAC structure containing all null values */
static SACHEAD sac_null = {
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345., -12345., -12345., -12345., -12345.,
  -12345, -12345, -12345, -12345, -12345,
  -12345,      6, -12345, -12345, -12345,
  -12345, -12345, -12345, -12345, -12345,
  -12345, -12345, -12345, -12345, -12345,
  -12345, -12345, -12345, -12345, -12345,
  -12345, -12345, -12345, -12345, -12345,
  -12345, -12345, -12345, -12345, -12345,
  -12345, -12345, -12345, -12345, -12345,
  { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }, { '-','1','2','3','4','5',' ',' ' },
  { '-','1','2','3','4','5',' ',' ' }
};



//#define TMARK	10
//#define USERN	40

#endif