function varargout = ManualPick(varargin)
path(path,genpath(pwd));
% MANUALPICK MATLAB code for ManualPick.fig
%      MANUALPICK, by itself, creates a new MANUALPICK or raises the existing
%      singleton*.
%
%      H = MANUALPICK returns the handle to a new MANUALPICK or the handle to
%      the existing singleton*.
%
%      MANUALPICK('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in MANUALPICK.M with the given input arguments.
%
%      MANUALPICK('Property','Value',...) creates a new MANUALPICK or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before ManualPick_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to ManualPick_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help ManualPick

% Last Modified by GUIDE v2.5 16-Jan-2017 20:50:24

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @ManualPick_OpeningFcn, ...
                   'gui_OutputFcn',  @ManualPick_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before ManualPick is made visible.
function ManualPick_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to ManualPick (see VARARGIN)

% Choose default command line output for ManualPick
handles.output = hObject;

% handles.data.dir = pwd;
% handles.data.list = {'SAC files','Please Click Load'};


% Update handles structure
guidata(hObject, handles);

% UIWAIT makes ManualPick wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = ManualPick_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in save.
function save_Callback(hObject, eventdata, handles)
% hObject    handle to save (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%disp(handles.data.arrivalt);
temp = find(handles.data.dir == filesep);
eventname = handles.data.dir(temp(end)+1:end);
outputdir = [get(handles.arrivaltimedirtext,'String'),'_picked'];
if ~exist(outputdir)
    mkdir(outputdir);
end
fp=fopen([outputdir,filesep,eventname],'wt');
for i = 1:size(handles.data.list)
    if handles.data.arrivalt(i,:) ~= inf
        for j = 1:3
            timevec = handles.data.eventt;
            timevec(6) = timevec(6) + handles.data.calculated(i,j) + handles.data.arrivalt(i,j);
            timevec = datevec(datestr(timevec,'yyyy-mm-dd HH:MM:SS.FFF'),'yyyy-mm-dd HH:MM:SS.FFF');
            fprintf(fp,'%s %04d %02d %02d %02d %02d %05.2f\n',handles.data.list{i,j},timevec(1),timevec(2),timevec(3),timevec(4),timevec(5),timevec(6));
        end
    end
end


% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in loadbutton.
function loadbutton_Callback(hObject, eventdata, handles)
% hObject    handle to loadbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.data.dir = uigetdir(pwd,'Please Select Data Directory');
temp = find(handles.data.dir == filesep);
eventname = handles.data.dir(temp(end)+1:end);
listmp = dir(handles.data.dir);
infiles = cell(length(listmp)-2,1);
infileflag = zeros(length(listmp)-2,1);
for i=1:length(listmp)-2
    infiles{i} = listmp(i+2).name;
end
infiles=sort(infiles);

flagtemp=0;
for i=1:length(infiles)
    infilesep=strsplit(infiles{i},'.');
    %infilesep
    if infilesep{3}(end) == 'E' && flagtemp == 0
        flagtemp=flagtemp+1;
    elseif infilesep{3}(end) == 'N' && flagtemp == 1
        flagtemp=flagtemp+1;
    elseif infilesep{3}(end) == 'Z' && flagtemp == 2
        flagtemp=0;
        infileflag(i)=1;infileflag(i-1)=1;infileflag(i-2)=1;
    else
        flagtemp=0;
    end
end

handles.data.list = cell(round(sum(infileflag)/3),3);
ntstalist = cell(round(sum(infileflag)/3),1);
flagtemp=0;
for i=1:length(infiles)
    infilesep=strsplit(infiles{i},'.');
    if infileflag(i) == 1
        handles.data.list{floor(flagtemp/3)+1,mod(flagtemp,3)+1}=infiles{i};
        flagtemp=flagtemp+1;
        if mod(flagtemp,3) == 0;
            ntstalist{flagtemp/3} = infiles{i}(1:length(infilesep{1})+length(infilesep{2})+1);
        end
    end
end
sum(infileflag)/3
set(handles.sacfilelist, 'string', ntstalist);
handles.data.arrivalt=inf*ones(size(handles.data.list));

[f1,f2,f3,f4,f5,f6,f7,f8,f9]=textread([get(handles.arrivaltimedirtext,'String'),filesep,eventname],'%s %f %f %f %f %f %f %f %f');
handles.data.calculated=inf*ones(size(handles.data.list));
for i=1:length(ntstalist)
    j=find(strcmp(f1,ntstalist{i}));
    handles.data.calculated(i,3) = f8(j);
    handles.data.calculated(i,1) = f9(j);
    handles.data.calculated(i,2) = f9(j);
end
handles.data.eventt=[f2(1),f3(1),f4(1),f5(1),f6(1),f7(1)];

guidata(hObject, handles);


% --- Executes on selection change in sacfilelist.
function sacfilelist_Callback(hObject, eventdata, handles)
% hObject    handle to sacfilelist (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns sacfilelist contents as cell array
%        contents{get(hObject,'Value')} returns selected item from sacfilelist
timestart = str2double(get(handles.startttext,'String'));
timestop = str2double(get(handles.stopttext,'String'));
flow = str2double(get(handles.lowFtext,'String'));
fhigh = str2double(get(handles.highFtext,'String'));
ninlist = get(handles.sacfilelist, 'value');
handles.plot.pe=plotsac(handles.axes4E, handles.data.list{get(handles.sacfilelist, 'value'),1}, ...
                        handles.data.dir, handles.data.arrivalt(ninlist,1), timestart, timestop, flow, fhigh, handles.data.calculated(ninlist,1));
handles.plot.pn=plotsac(handles.axes4N, handles.data.list{get(handles.sacfilelist, 'value'),2}, ...
                        handles.data.dir, handles.data.arrivalt(ninlist,2), timestart, timestop, flow, fhigh, handles.data.calculated(ninlist,2));
handles.plot.pz=plotsac(handles.axes4Z, handles.data.list{get(handles.sacfilelist, 'value'),3}, ...
                        handles.data.dir, handles.data.arrivalt(ninlist,3), timestart, timestop, flow, fhigh, handles.data.calculated(ninlist,3));
handles.zoom.i=0;
handles.zoom.l=zeros(1,100);
handles.zoom.h=zeros(1,100);
set(handles.eventttext,'String',datestr(handles.data.eventt,'HH:MM:SS.FFF'));
set(handles.caltravelttext,'String',[num2str(handles.data.calculated(ninlist,3)),'/',...
    num2str(handles.data.calculated(ninlist,1))]);
set(handles.ettext,'String',handles.data.arrivalt(ninlist,1));
set(handles.nttext,'String',handles.data.arrivalt(ninlist,2));
set(handles.zttext,'String',handles.data.arrivalt(ninlist,3));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function sacfilelist_CreateFcn(hObject, eventdata, handles)
% hObject    handle to sacfilelist (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function lowFtext_Callback(hObject, eventdata, handles)
% hObject    handle to lowFtext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of lowFtext as text
%        str2double(get(hObject,'String')) returns contents of lowFtext as a double


% --- Executes during object creation, after setting all properties.
function lowFtext_CreateFcn(hObject, eventdata, handles)
% hObject    handle to lowFtext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit2_Callback(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit2 as text
%        str2double(get(hObject,'String')) returns contents of edit2 as a double


% --- Executes during object creation, after setting all properties.
function edit2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit3_Callback(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit3 as text
%        str2double(get(hObject,'String')) returns contents of edit3 as a double


% --- Executes during object creation, after setting all properties.
function edit3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit5_Callback(hObject, eventdata, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit5 as text
%        str2double(get(hObject,'String')) returns contents of edit5 as a double


% --- Executes during object creation, after setting all properties.
function edit5_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on mouse press over axes background.
function axes4Z_ButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to axes4Z (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
a=get(handles.axes4Z, 'CurrentPoint');
disp(a);


% --- Executes on button press in pickbuttonZ.
function pickbuttonZ_Callback(hObject, eventdata, handles)
% hObject    handle to pickbuttonZ (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[x,y] = ginput(1);
position = get(handles.axes4Z, 'CurrentPoint');
if (x~=position(1,1) || y ~= position(1,2))
    error('You Selected in Wrong Axes!');
end
ylim=get(handles.axes4Z,'YLim');
try
    delete(handles.plot.lineZ);
catch
end
set(handles.axes4Z,'NextPlot','add');
handles.plot.lineZ = plot([x,x],[ylim(1),ylim(2)],'Color','r','Linewidth',3);
set(handles.axes4Z,'NextPlot','replace');

handles.data.arrivalt(get(handles.sacfilelist,'Value'),3) = x;
guidata(hObject, handles);

% --- Executes on button press in pickbuttonN.
function pickbuttonN_Callback(hObject, eventdata, handles)
% hObject    handle to pickbuttonN (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[x,y] = ginput(1);
position = get(handles.axes4N, 'CurrentPoint');
if (x~=position(1,1) || y ~= position(1,2))
    error('You Selected in Wrong Axes!');
end
ylim=get(handles.axes4N,'YLim');
try
    delete(handles.plot.lineN);
catch
end
set(handles.axes4N,'NextPlot','add');
handles.plot.lineN = plot([x,x],[ylim(1),ylim(2)],'Color','r','Linewidth',3);
set(handles.axes4N,'NextPlot','replace');

handles.data.arrivalt(get(handles.sacfilelist,'Value'),2) = x;
guidata(hObject, handles);


% --- Executes on button press in pickbuttonE.
function pickbuttonE_Callback(hObject, eventdata, handles)
% hObject    handle to pickbuttonE (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[x,y] = ginput(1);
position = get(handles.axes4E, 'CurrentPoint');
if (x~=position(1,1) || y ~= position(1,2))
    error('You Selected in Wrong Axes!');
end
ylim=get(handles.axes4E,'YLim');
try
    delete(handles.plot.lineE);
catch
end
set(handles.axes4E,'NextPlot','add');
handles.plot.lineE = plot([x,x],[ylim(1),ylim(2)],'Color','r','Linewidth',3);
set(handles.axes4E,'NextPlot','replace');

handles.data.arrivalt(get(handles.sacfilelist,'Value'),1) = x;
guidata(hObject, handles);


% --- Executes on button press in zoominbutton.
function zoominbutton_Callback(hObject, eventdata, handles)
% hObject    handle to zoominbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[x1,y1] = ginput(1);
positionZ = get(handles.axes4Z, 'CurrentPoint');
positionE = get(handles.axes4E, 'CurrentPoint');
positionN = get(handles.axes4N, 'CurrentPoint');
if (x1==positionZ(1,1) && y1 == positionZ(1,2))
    axesnow=handles.axes4Z;
elseif (x1==positionE(1,1) && y1 == positionE(1,2))
    axesnow=handles.axes4E;
elseif (x1==positionN(1,1) && y1 == positionN(1,2))
    axesnow=handles.axes4N;
else
    error('You Selected in Wrong Axes!');
end
ylim=get(axesnow,'YLim');
set(axesnow,'NextPlot','add');
ptemp = plot([x1,x1],[ylim(1),ylim(2)],'Color','b','Linewidth',1.5);
set(axesnow,'NextPlot','replace');

[x2,y2] = ginput(1);
position = get(axesnow, 'CurrentPoint');
if (x2~=position(1,1) || y2 ~= position(1,2))
    error('You Selected in Wrong Axes!');
end
if handles.zoom.i == 0
    handles.zoom.raw = get(axesnow,'XLim');
end
handles.zoom.i=handles.zoom.i+1;
handles.zoom.l(handles.zoom.i)=min(x1,x2);
handles.zoom.h(handles.zoom.i)=max(x1,x2);
set(handles.axes4Z, 'XLim',[handles.zoom.l(handles.zoom.i), handles.zoom.h(handles.zoom.i)]);
set(handles.axes4N, 'XLim',[handles.zoom.l(handles.zoom.i), handles.zoom.h(handles.zoom.i)]);
set(handles.axes4E, 'XLim',[handles.zoom.l(handles.zoom.i), handles.zoom.h(handles.zoom.i)]);
delete(ptemp);
guidata(hObject, handles);


% --- Executes on button press in zoomoutbutton.
function zoomoutbutton_Callback(hObject, eventdata, handles)
% hObject    handle to zoomoutbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if handles.zoom.i == 1
    handles.zoom.i = handles.zoom.i-1;
    xlim = handles.zoom.raw;
elseif handles.zoom.i >=2
    handles.zoom.i = handles.zoom.i-1;
    xlim = [handles.zoom.l(handles.zoom.i), handles.zoom.h(handles.zoom.i)];
end
set(handles.axes4Z, 'XLim', xlim);
set(handles.axes4N, 'XLim', xlim);
set(handles.axes4E, 'XLim', xlim);
guidata(hObject, handles);


% --- Executes on button press in pushbutton10.
function pushbutton10_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in pushbutton11.
function pushbutton11_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in pushbutton12.
function pushbutton12_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in pushbutton13.
function pushbutton13_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton13 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)



function highFtext_Callback(hObject, eventdata, handles)
% hObject    handle to highFtext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of highFtext as text
%        str2double(get(hObject,'String')) returns contents of highFtext as a double


% --- Executes during object creation, after setting all properties.
function highFtext_CreateFcn(hObject, eventdata, handles)
% hObject    handle to highFtext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function startttext_Callback(hObject, eventdata, handles)
% hObject    handle to startttext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of startttext as text
%        str2double(get(hObject,'String')) returns contents of startttext as a double


% --- Executes during object creation, after setting all properties.
function startttext_CreateFcn(hObject, eventdata, handles)
% hObject    handle to startttext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function stopttext_Callback(hObject, eventdata, handles)
% hObject    handle to stopttext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of stopttext as text
%        str2double(get(hObject,'String')) returns contents of stopttext as a double


% --- Executes during object creation, after setting all properties.
function stopttext_CreateFcn(hObject, eventdata, handles)
% hObject    handle to stopttext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in setarrivaltimedirbutton.
function setarrivaltimedirbutton_Callback(hObject, eventdata, handles)
% hObject    handle to setarrivaltimedirbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
arrivaltimedirctory = uigetdir(pwd,'Set Arrival Time Directory');
set(handles.arrivaltimedirtext,'String',arrivaltimedirctory);
% [a,b,c] = uigetfile('*.*','Select Event List File',pwd);
% if c~=0
%     set(handles.stapathtext,'String',[b,a]);
%     [f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11]=textread([b,a],'%s %f %f %f %f %f %f %f %f %f %f');
%     handles.evtinfo.evtname = f1;
%     handles.evtinfo.evttime = [f2,f3,f4,f5,f6,f7];
%     handles.evtinfo.lon = f8;
%     handles.evtinfo.lat = f9;
%     handles.evtinfo.depth = f10;
%     handles.evtinfo.mag = f11;
% else
%     error('Event List not select!');
% end
% handles.evtinfo.evtname
% [a,b,c] = uigetfile('*.*','Select Station List File',pwd);
% if c~=0
%     set(handles.arrivaltimedirtext,'String',[b,a]);
%     [f1,f2,f3,f4,f5]=textread([b,a],'%s %f %f %f %s');
%     handles.stainfo.staname = f1;
%     handles.stainfo.stalon = f2;
%     handles.stainfo.stalat = f3;
%     handles.stainfo.height = f4;
% else
%     error('Event List not select!');
% end
guidata(hObject, handles);
