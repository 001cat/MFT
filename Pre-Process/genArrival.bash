#!/bin/bash
############################
# generate arrival time list
# taup_time is needed
############################
# catalog format
# eventName Year(YYYY) Month(MM) Day(DD) Hour(hh) Minute(mm) seconds(ss.ff) longitude latitude depth magnitude
# station list format
# network.station latitude longitude elevation band(e.g. BH,EH,...)
############################

catalog='../example/cata_lst'
stationlst='../example/sta_lst'
outputdir='../example/arrivalTime'

mkdir $outputdir
while read evt year month day hour minute second lon lat depth mag res; do
	echo $evt $year$month$day $hour $minute $second
	rm -rf $outputdir/$evt
	while read sta stalat stalon height; do
		travelp=`taup_time -mod prem -h $depth -ph p,P,Pn -evt $lat $lon -sta $stalat $stalon --time | awk '{print $1}'`
		travels=`taup_time -mod prem -h $depth -ph s,S,Sn -evt $lat $lon -sta $stalat $stalon --time | awk '{print $1}'`
		echo $sta $year $month $day $hour $minute $second $travelp $travels >> $outputdir/$evt
	done < $stationlst
done < $catalog
