Main body of MFT by Ayu

This is main body of MFT.
|
|--src/			srouce code
|--bin/			binary executable file, made from src/
|--doMFT.bash	an example about how to run binary file
|--readme.txt	this doc

There are three types of MFT: normal MFT, MFT_weak, MFT_partweak. The suffix weak means we extend CC peak to a flat peak with same amplitude. The duration time of flat peak is defined inside code, now it is 0.4s, which means the previous CC value there are all replaced with peak value. Partweak means weak only works when peak value higher then a threshold. You can run with -h to check usage.

The filter is also defined inside code, now it is Butterworth bandpass filter from 2hz to 15hz.

The output text file contains all events detected, each one with format:

	[detected event name] YYYY MM DD hh mm ss.ff
	[template name] [peak value of stacked CC] [sigma of CC serial] [CC threshold]
	Network.station.band[ZNE].SAC [travel time] [travel time difference from template] [amplitude ratio compared with template] [CC for single channel]
	##########

And sac4plot will exist if you want to plot figure to compare new detected event with template. All the output will NOT be removed if you run it again, please remove them yourself if you need to run it again.

P.S. The following warning is ok when you run example:
	ls: ../example/DataScn/20100306/CWB.CHN1.EHE.SAC: No such file or directory
	ls: ../example/DataScn/20100308/CWB.CHN1.EHE.SAC: No such file or directory
	...(similar)
and
	mv: rename *.CC to ../example/Result/*.CC: No such file or directory
	mv: rename *.sac to ../example/Result/*.sac: No such file or directory