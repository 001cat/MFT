#!/bin/bash
########################
# cutEvent by Ayu
# cut event from raw continuous record(SAC format)
# event is cut around origin time of earthquake, from 200s before
# to 1000s after origin time
########################
# catalog format:
# eventName Year(YYYY) Month(MM) Day(DD) Hour(hh) Minute(mm) seconds(ss.ff) longitude latitude depth magnitude
# station list format:
# network.station latitude longitude elevation band(e.g. BH,EH,...)
# rawdatadir format:
# rawdatadir/YYYYMMDD/Network.Station.band[ZNE].SAC
# e.g. DataScn/20100307/CWB.CHN1.EHE.SAC
########################

rawdatadir="../example/DataScn"  # do not use ~ as home directory here
outputdir="../example/DataEvent"
catalog="../example/cata_lst"
stalst="../example/sta_lst"

dirnow=`pwd`
export PATH=$PATH:$dirnow

while read line; do
	read evtname year month day hour minute second lon lat depth mag < <(echo $line)
	echo $outputdir/$evtname
	if [[ ! -e $outputdir/$evtname ]]; then
		mkdir -p $outputdir/$evtname
	fi
	daydir0="$year$month$day"
	daydir1=`timecal -add $year $month $day 0 0 0 1 0 0 0 | awk '{print $1$2$3}'`
	daydir2=`timecal -add $year $month $day 0 0 0 -1 0 0 0 | awk '{print $1$2$3}'`
	#echo $daydir2 $daydir1 $daydir0
	while read staline; do
		read network station res < <(echo $staline | sed 's/\./ /g')
		echo "Now:" $evtname $network $station
		band=`ls $rawdatadir/$daydir0/${network}.${station}.*.SAC \
				 $rawdatadir/$daydir1/${network}.${station}.*.SAC \
				 $rawdatadir/$daydir2/${network}.${station}.*.SAC \
				 2>/dev/null | awk -F '.' '{print $(NF-1)}' | cut -c 1-2 | sort -u`
		if [[ `echo $band | awk '{print NF}'` != 1 ]]; then
			if [[ `echo $band | awk '{print NF}'` == 0 ]]; then
				continue
			else
				echo "Error: Wrong Band for event $evtname in station $station, Ignored."
				continue
			fi
		fi
		for component in Z N E; do
			relatedrawdata=`ls $rawdatadir/$daydir0/${network}.${station}.${band}${component}.SAC \
							   $rawdatadir/$daydir1/${network}.${station}.${band}${component}.SAC \
							   $rawdatadir/$daydir2/${network}.${station}.${band}${component}.SAC \
							   2>/dev/null`
			saccut -t $year$month$day"-"$hour":"$minute":"$second \
				   -b -200 -e 1000 -o $outputdir/$evtname/${network}.${station}.${band}${component}.SAC \
				   $relatedrawdata --verbose
		done
	done < $stalst
	if [[ `ls $outputdir/$evtname | wc -l` == 0 ]]; then
		rmdir $outputdir/$evtname
	fi
done < $catalog