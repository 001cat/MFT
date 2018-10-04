function SACplot( sac_st, varargin )
%% Plot SAC
% Plot waveform saved in sac structure.
% Usage:  plotsac_ayu( sac_st )
%         plotsac_ayu( sac_st, color )
%% Deal with optional parameters
% read optional parameters about color
optargin = numel(varargin);
color = 'k';
while optargin>0
    if optargin == 1
        color = varargin{1};
    else
        error('Too many input arguments!');
    end
    optargin =optargin - 1;
end
%% Plot SAC
time = sac_st.b:sac_st.delta:sac_st.b+sac_st.delta*(sac_st.npts-1);
plot(time,sac_st.data,color);
end

