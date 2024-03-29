function [ sac_st ] = SACcreate
% SAC_create: creates an empty sac structure
%--------------------------------------------------------------------------
% Usage: [ sac_st ] = SAC_create
%--------------------------------------------------------------------------
% Output
%   sac_st: a structure containing sac header and data
%
% sac header is consisted of the following elements
%--------------------------------------------------------------------------
%delta	stla	evla	data(10) iftype   dist     xminimum      trcLen	
%b      stlo	evlo	label(3) idep     az       xmaximum      scale
%e      stel	evel             iztype   baz      yminimum
%o      stdp	evdp             iinst    gcarc    ymaximum
%a      cmpaz	nzyear           istreg	  norid
%t0     cmpinc	nzjday           ievreg	  nevid
%t1     kstnm	nzhour           ievtyp	  nwfid
%t2     kcmpnm	nzmin            iqual	  nxsize
%t3     knetwk	nzsec            isynth	  nysize
%t4		nzmsec
%t5		kevnm
%t6		mag
%t7		imagtyp
%t8		imagsrc
%t9
%f
%k0
%ka
%kt1
%kt2
%kt3
%kt4
%kt5
%kt6
%kt7
%kt8
%kt9
%kf
%response is a 10-element array, and trcLen is a scalar. 
%--------------------------------------------------------------------------
% Notes: 
%   2011-12-16: modified
%   2011-12-23: modified, default gmt time is set to
%   (nzyear=2000,nzjday=1,nzhour=0,nzsec=0,nzmsec=0). This is the reference
%   zero time used in SAC_gmt2sec;
%--------------------------------------------------------------------------

%% define empty sac head
h = zeros(302,1);
h(1:97) = -12345*ones(97,1);
h(106) = 1; % leven is set to true(equidistant sampling)
h(108) = 1; % lovrok is set to true(overwirte is permitted, added on 3/6/2010)
h(109) = 1; % lcalda is set to true(calculate az,backaz,dist from evla/lo and stla/lo, added on 3/6/2010)

%% set sac structure

% real header variables
%--------------------------------------------------------------------------
sac_st.delta = h(1);
sac_st.depmin = h(2);
sac_st.depmax = h(3);
sac_st.scale = h(4);
sac_st.odelta = h(5);
sac_st.b = h(6);
sac_st.e = h(7);
sac_st.o = h(8);
sac_st.a = h(9);
sac_st.t0 = h(11);
sac_st.t1 = h(12);
sac_st.t2 = h(13);
sac_st.t3 = h(14);
sac_st.t4 = h(15);
sac_st.t5 = h(16);
sac_st.t6 = h(17);
sac_st.t7 = h(18);
sac_st.t8 = h(19);
sac_st.t9 = h(20);
sac_st.f = h(21);
sac_st.resp0 = h(22);
sac_st.resp1 = h(23);
sac_st.resp2 = h(24);
sac_st.resp3 = h(25);
sac_st.resp4 = h(26);
sac_st.resp5 = h(27);
sac_st.resp6 = h(28);
sac_st.resp7 = h(29);
sac_st.resp8 = h(30);
sac_st.resp9 = h(31);
sac_st.stla = h(32);
sac_st.stlo = h(33);
sac_st.stel = h(34);
sac_st.stdp = h(35);
sac_st.evla = h(36);
sac_st.evlo = h(37);
sac_st.evel = h(38);
sac_st.evdp = h(39);
sac_st.mag = h(40);
sac_st.user0 = h(41);
sac_st.user1 = h(42);
sac_st.user2 = h(43);
sac_st.user3 = h(44);
sac_st.user4 = h(45);
sac_st.user5 = h(46);
sac_st.user6 = h(47);
sac_st.user7 = h(48);
sac_st.user8 = h(49);
sac_st.user9 = h(50);
sac_st.dist = h(51);
sac_st.az = h(52);
sac_st.baz = h(53);
sac_st.gcarc = h(54);
sac_st.depmen = h(57);
sac_st.cmpaz = h(58);
sac_st.cmpinc = h(59);
sac_st.xminimum = h(60);
sac_st.xmaximum = h(61);
sac_st.yminimum = h(62);
sac_st.ymaximum = h(63);

% integer header variables
%--------------------------------------------------------------------------
% sac_st.nzyear = h(71);
sac_st.nzyear = 2000;
% sac_st.nzjday = h(72);
sac_st.nzjday = 1;
% sac_st.nzhour = h(73);
sac_st.nzhour = 0;
% sac_st.nzmin = h(74);
sac_st.nzmin = 0;
% sac_st.nzsec = h(75);
sac_st.nzsec = 0;
% sac_st.nzmsec = h(76);
sac_st.nzmsec = 0;
sac_st.nvhdr = h(77);
sac_st.norid = h(78);
sac_st.nevid = h(79);
sac_st.npts = h(80);
sac_st.nwfid = h(82);
sac_st.nxsize = h(83);
sac_st.nysize = h(84);
sac_st.iftype = h(86);
sac_st.idep = h(87);
sac_st.iztype = h(88);
sac_st.iinst = h(90);
sac_st.istreg = h(91);
sac_st.ievreg = h(92);
sac_st.ievtyp = h(93);
sac_st.iqual = h(94);
sac_st.isynth = h(95);
sac_st.imagtyp = h(96);
sac_st.imagsrc = h(97);

% logical header variables
%--------------------------------------------------------------------------
sac_st.leven = h(106);
sac_st.lpspol = h(107);
sac_st.lovrok = h(108);
sac_st.lcalda = h(109);

% character header variables
%---------------------------------------------------------------------------
sac_st.kstnm = char(h(111:118));
sac_st.kevnm = char(h(119:134));
sac_st.khole = char(h(135:142));
sac_st.ko = char(h(143:150));
sac_st.ka = char(h(151:158));
sac_st.kt0 = char(h(159:166));
sac_st.kt1 = char(h(167:174));
sac_st.kt2 = char(h(175:182));
sac_st.kt3 = char(h(183:190));
sac_st.kt4 = char(h(191:198));
sac_st.kt5 = char(h(199:206));
sac_st.kt6 = char(h(207:214));
sac_st.kt7 = char(h(215:222));
sac_st.kt8 = char(h(223:230));
sac_st.kt9 = char(h(231:238));
sac_st.kf = char(h(239:246));
sac_st.kuser0 = char(h(247:254));
sac_st.kuser1 = char(h(255:262));
sac_st.kuser2 = char(h(263:270));
sac_st.kcmpnm = char(h(271:278));
sac_st.knetwk = char(h(279:286));
sac_st.kdatrd = char(h(287:294));
sac_st.kinst = char(h(295:302));

% sac data
%--------------------------------------------------------------------------
sac_st.data = ones(0,1);

end
