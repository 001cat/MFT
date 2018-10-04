function [ sac_lp ] = SAClowpass( sac, Fpass, varargin )
%% make a lowpass for sac data
%   Using FIR-equiripple way to get a lowpass filter. The group delay has
%   been cut off.
%--------------------------------------------------------------------------
% Usage: [ sac_lp ] = lowpasssac_ayu( sac, Fpass, Fstop, varargin )
%--------------------------------------------------------------------------
% Created on 2015/10/08
%--------------------------------------------------------------------------
% Inputs:
%   sac_st: a structure containing sac header and data
%           (use readsac_ayu.m to create this structure)
%   Fpass: Higt limit Frequency of lowpass
%   varargin: Contains other optional parameters: Fstop, Apass, Astop.
%
%                Fpass
%     ------------\
%                  \
%                   \
%                    \
%                     \-----------------
%                    Fstop
%   (see the matlab help of fdesign.lowpass for the following parameters)
%--------------------------------------------------------------------------
%%
optargin = numel(varargin);

Fstop=min(Fpass+0.5,Fpass*1.1);
Apass=1;
Astop=60;
while optargin>0
    if optargin == 1
        Fstop = varargin{1};
    elseif optargin == 2
        Apass = varargin{2};
    elseif optargin == 3
        Astop = varargin{3};
    else
        error('Too many input arguments!');
    end
    optargin =optargin - 1;
end

Fs = 1/sac.delta;
filter_lp = design(fdesign.lowpass(Fpass,Fstop,Apass,Astop,Fs),'equiripple');
%filter_lp.impzlength
grpdelay = sac.delta*(filter_lp.impzlength-1)/2;

sac_lp = sac;
sac_lp.data = filter(filter_lp.numerator,1,sac.data);

sac_lp.b = sac.b-grpdelay;
sac_lp.e = sac.e-grpdelay;
sac_lp.npts = length(sac_lp.data);

%sprintf('%s',varargin{1})
%sac_stlp=sac;

end

