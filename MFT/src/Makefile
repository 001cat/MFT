CC = g++
CFLAGS = -lfftw3 -std=c++11

SRCS = MFTdetect.c MFTfuncs.c
HEAD = crosscorrelation.h filt.h sac_ayu.h sacio_ayu.h timecal.h

all: MFT MFT_weak MFT_partweak
	mv MFT MFT_weak MFT_partweak ../bin

MFT: 			MFT.c $(SRCS) $(HEAD)
	$(CC) MFT.c $(CFLAGS) -o MFT

MFT_weak: 		MFT_weak.c $(SRCS) $(HEAD)
	$(CC) MFT_weak.c $(CFLAGS) -o MFT_weak

MFT_partweak: 	MFT_partweak.c $(SRCS) $(HEAD)
	$(CC) MFT_partweak.c $(CFLAGS) -o MFT_partweak