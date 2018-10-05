#!/bin/bash
############################
# doMFT by Ayu
############################


# Sub Function #############
getmodifyday()
{
	year=`echo $1 | cut -c 1-4`
	month=`echo $1 | cut -c 5-6`
	day=`echo $1 | cut -c 7-8`
	hh=`echo $1 | cut -c 10-11`
	mm=`echo $1 | cut -c 13-14`
	ss=`echo $1 | cut -c 16-17`
	read year month day hh mm ss < <(timecal -add $year $month $day $hh $mm $ss 0 0 0 $2)

}
# Set paras #################

evtname=15071631P10
evtdatadir=../example/DataEvent/$evtname
evtarrlst=../example/arrivaltime_picked/$evtname
catalog=../example/cata_lst

scndatadir=../example/DataScn
scntimestart="20100307-01:00:00"
scntimefinish="20100307-23:00:00"

MFTexe=bin/MFT
resultdir=../example/Result
MFTevtlst=$resultdir/MFTevt.lst
sac4plotdir=$resultdir/sac4plot

evtcutb="-2"
evtcute="2"
sigma=6

if [[ ! -e $resultdir ]]; then
	mkdir $resultdir
fi
evtstarttime=`cat "$catalog" | grep "^$evtname" | awk '{print $2$3$4"-"$5":"$6":"$7}'`

# Main ###################
CUR_DIR=$(cd "$(dirname "${BASH_SOURCE-$0}")"; pwd)
CUR_BASH=${BASH_SOURCE-$0}

insaclst=$CUR_DIR/insaclst.${CUR_BASH}.tmp
rm -rf $insaclst

redundance=87000 #600+86400

getmodifyday $scntimestart -$redundance
date1="$year$month$day"
getmodifyday $scntimefinish $redundance
date2="$year$month$day"

while read sacnm res; do
	read nt sta comp res < <(echo $sacnm | awk -F '.' '{print $1,$2,$3,$4}')
	date0=$date1
	#datafirst used for syn
	#datafirst=`cat $evtname | cut -c 4-9`
	#datafirst=
	#ls $sacndatadir/$datafirst/$sta.*.$nt.*.$comp.sac >> $insaclst
	while [[ $date0 -le $date2 ]]; do
		ls $scndatadir/$date0/$nt.$sta.$comp.SAC >> $insaclst
		getmodifyday ${date0}-00:00:00 86400
		date0="$year$month$day"
	done
	echo '#' >> $insaclst
done < "$evtarrlst"

$MFTexe $evtname $evtstarttime $evtdatadir "$evtarrlst" $evtcutb $evtcute $scntimestart $scntimefinish $sigma $insaclst "-plot" $sac4plotdir >> $MFTevtlst
mv *.CC *.sac  $resultdir
rm -rf $insaclst


