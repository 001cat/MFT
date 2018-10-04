ManualPick is a matlab GUI tool to manually pick arrival time. It helps you to pick one arrival time for each component(Z,N,E). It is designed to pick P-like phase in Z, and S-like phase in N and E.

Step 1:
Run ManualPick.m in matlab

Step 2:
Click Arrival Time Dir button, choose the directory contains arrival time files.
Arrival time files should be named by eventName, and format is:
Network.Station Year(YYYY) Month(MM) Day(DD) Hour(hh) Minute(mm) Second(ss.ff) ArrivalTimeP ArrivalTimeS

Step 3:
Click Load button, choose the folder contains event SAC file. The folder should be named by eventname, SAC files be named as Network.Station.band[ENZ].SAC, i.e. CWB.CHN1.EHE.SAC.

Step 4:
Select station in the list box to the right of Load button. Three waveform will show up at the three axes, in the order Z,N,E from up to bottom. The arrival time of P(for Z) or S(for N,E) phase is set to zero time. You can change the length and filter below Save button.

Step 5:
Using Zoom In button to zoom in like SAC software (click the start and end of segment you want to zoom in), until time of phase onset is clear.

Step 6:
Select Pick button to pick the phase you want. There are three button for picking at three axes, please use corresponding button.
(If you make a wrong pick, please remember the station and component, you need to delete it manually from the output file)

Step 7:
After finish picking, click Save, it will save the time your picked in [ArrivalTimeDir]_picked/eventName, with format:
Network.Station.band[ENZ].SAC YYYY MM DD hh mm ss.ff


