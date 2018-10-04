function [ sac_hp ] = SAChighpass( sac, Fpass, varargin )
%% make a highpass for sac data
%   Using FIR-equiripple way to get a highpass filter. The group delay has
%   been cut off.
%--------------------------------------------------------------------------
% Created on 2015/10/13
%--------------------------------------------------------------------------
%--------------------------------------------------------------------------
% Inputs:
%   sac:    a structure containing sac header and data
%           (use readsac_ayu.m to create this structure)
%   Fpass: Low limit Frequency of highpass
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

Fstop=max(Fpass-0.5,Fpass*0.9);
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
filter_hp = design(fdesign.highpass(Fstop,Fpass,Astop,Apass,Fs),'equiripple');
grpdelay = sac.delta*(filter_hp.impzlength-1)/2;

sac_hp = sac;
sac_hp.data = filter(filter_hp.numerator,1,sac.data);

sac_hp.b = sac.b-grpdelay;
sac_hp.e = sac.e-grpdelay;
sac_hp.npts = length(sac_hp.data);

%sprintf('%s',varargin{1})
%sac_stlp=sac;
end





