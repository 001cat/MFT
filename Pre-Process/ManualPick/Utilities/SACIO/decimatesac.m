function [ sac_dec ] = decimatesac_ayu( sac, r, varargin )
%   Decimate sac data
%   r: decimate factor
%   n: number of points of fir filter
%   type: only 'fir' can be used now
optargin = numel(varargin);
n = 30;
type = 'fir';
while optargin>0
    if optargin == 1
        n = varargin{1};
    elseif optargin == 2
        type = varargin{2};
    else
        error('Too many input arguments!');
    end
    optargin =optargin - 1;
end

sac_dec = sac;
sac_dec.data = decimate(sac.data,r,n,type);

sac_dec.delta = sac.delta * r;
sac_dec.npts = length(sac_dec.data);
sac_dec.e = sac_dec.b + (sac_dec.npts-1) * sac_dec.delta;
end

