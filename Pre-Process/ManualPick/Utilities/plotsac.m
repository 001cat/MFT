function p1 = plotsac( plotaxes, sacfile, datadir, arrivalt, timestart, timestop, flow, fhigh, traveltime )
%PLOTSAC 此处显示有关此函数的摘要
%   此处显示详细说明
sac_st=SACread([datadir,filesep,sacfile]);
time = sac_st.b:sac_st.delta:sac_st.b+sac_st.delta*(sac_st.npts-1);
time = time-traveltime;

if flow == 0
    flow = fhigh;
    fhigh = 'low';
end

sac_st.data = filtering_1pass(sac_st.data, sac_st.delta, flow, fhigh);
p1=plot(time,sac_st.data,'Parent', plotaxes,'Color','k');
set(plotaxes,'Xlim',[timestart, timestop]);

ylim=get(plotaxes,'YLim');
if arrivalt~=inf
    set(plotaxes,'NextPlot','add');
    plot([arrivalt, arrivalt],[ylim(1), ylim(2)],'Parent', plotaxes,'Color','g','LineWidth',2);
    set(plotaxes,'NextPlot','replace');
end

end

