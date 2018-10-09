#!/bin/bash

MFTevtlst=../example/Result/MFTevt.lst
MFTcatalog=../example/Result/MFTcatalog.txt  # Output file
catalog=../example/cata_lst
stalst=../example/sta_lst
relocate_exe=bin/relocate_4

rm -rf $MFTcatalog

count=0
compin_count=0
while read line; do
	if [[ $line == "##########" ]]; then
		#$relocate_exe $inputbuffer #for test
		read offsetx offsety offsetz newmag Fpersta n_round finalstep WeightAngle < <($relocate_exe $inputbuffer)
		echo $evtname $year $month $day $hour $minute $second $tplon $tplat $tpdepth $tpmag $offsetx $offsety $offsetz $newmagm $Fpersta $n_round $finalstep $WeightAngle >> $MFTcatalog
		count=0
	elif [[ $count -eq 0 ]]; then
		read evtname year month day hour minute second < <(echo $line)
		((count++))
	elif [[ $count -eq 1 ]]; then
		read tpname CC sigma threshold < <(echo $line)
		read tplon tplat tpdepth tpmag < <(cat "$catalog" | grep "^$tpname" | awk '{print $8,$9,$10,$11}')
		inputbuffer="-tp $tplon $tplat $tpdepth $tpmag"
		((count++))
	else
		((compin_count++))
		read network station band < <(echo $line | awk -F '.' '{print $1,$2,$3}')
		read stalat stalon staht < <(cat "$stalst" | grep "^$network.$station" | awk '{print $2,$3,$4}')
		read traveltime offset mag_modify res < <(echo $line | awk '{print $2,$3,$4}')
		compname=`echo $band | cut -c 3`
		if [[ $compin_count -eq 1 ]]; then
			inputbuffer="$inputbuffer -sta $stalon $stalat $staht"
		elif [[ $compin_count -eq 3 ]]; then
			compin_count=0
		fi
		inputbuffer="$inputbuffer $compname $traveltime $offset $mag_modify"
	fi
done < $MFTevtlst
