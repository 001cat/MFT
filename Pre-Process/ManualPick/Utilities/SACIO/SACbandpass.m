function [ sac_bp ] = SACbandpass( sac, Fpass1, Fpass2, varargin )
%% make a bandpass for sac data
%   Using FIR-equiripple way to get a bandpass filter. The group delay has
%   been cut off.
%--------------------------------------------------------------------------
% Created on 2015/10/13
%--------------------------------------------------------------------------
% Usage: [ sac_hp ] = bandpasssac_ayu( sac, Fpass1, Fpass2, varargin )
%--------------------------------------------------------------------------
% Inputs:
%   sac:    a structure containing sac header and data
%           (use readsac_ayu.m to create this structure)
%   Fpass1: Low limit Frequency of bandpass
%   Fpass2: High limit Frequency of bandpass
%   varargin: Contains other optional parameters: Fstop, Apass, Astop.
%
%                      Fpass
%                      /----------------
%                     /
%                    /
%                   /
%   ---------------/
%                 Fstop  
%   (see the matlab help of fdesign.lowpass for the following parameters)
%--------------------------------------------------------------------------
%%
optargin = numel(varargin);

Fstop1=max(Fpass1-0.5,Fpass1*0.9);
Fstop2=min(Fpass2+0.5,Fpass2*1.1);
Apass=1;
Astop1=60;
Astop2=60;
while optargin>0
    if optargin == 1
        Fstop1 = varargin{1};
    elseif optargin == 2
        Fstop2 = varargin{2};
    elseif optargin == 3
        Apass = varargin{3};
    elseif optargin == 4
        Astop1 = varargin{4};
    elseif optargin == 5
        Astop2 = varargin{5};
    else
        error('Too many input arguments!');
    end
    optargin =optargin - 1;
end


Fs = 1/sac.delta;
filter_hp = design(fdesign.bandpass(Fstop1,Fpass1,Fpass2,Fstop2,Astop1,Apass,Astop2,Fs),'equiripple');
grpdelay = sac.delta*(filter_hp.impzlength-1)/2;

sac_bp = sac;
sac_bp.data = filter(filter_hp.numerator,1,sac.data);

sac_bp.b = sac.b-grpdelay;
sac_bp.e = sac.e-grpdelay;
sac_bp.npts = length(sac_bp.data);
end

