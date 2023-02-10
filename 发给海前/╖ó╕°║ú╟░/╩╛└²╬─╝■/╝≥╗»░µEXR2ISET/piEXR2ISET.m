function piEXR2ISET(inputFile, varargin)
% Read an exr-file rendered by PBRT, and return an ieObject or a
% metadataMap
%       ieObject = piExr2ISET(inputFile, varagin)
% 
% Brief description:
%   We take a exr-file from pbrt as input and return an ISET object.
%
% Inputs
%   inputFile - Multi-spectral exr-file rendered by pbrt.
% 
% Optional key/value pairs
%   label            -  Specify the type of data: radiance, mesh, depth.
%                       Default is radiance
%   pbrt_path        -  path of pbrt file
%
% Output
%   ieObject: if label is radiance: optical image;
%             else, a metadatMap
%
%
% Zhenyi, 2021
%
% ## python test
% python installation: https://docs.conda.io/en/latest/miniconda.html
% Install python 3.8 for matlab 2020 and above
% check version in matlab command window:
%          pvcConstantse = pyenv; 
% Install python library for reading exr files, run this in terminal: 
%          sudo apt install libopenexr-dev # (ubuntu)
%          brew install openexr # (mac)
%          pip install git+https://github.com/jamesbowman/openexrpython.git
%          pip install pyexr
%%
%{
 opticalImage = piEXR2ISET('radiance.exr','label','radiance','pbrt_path',pbrt_path);
%}

%%
varargin =ieParamFormat(varargin);

p = inputParser;
p.addRequired('inputFile',@(x)(exist(x,'file')));
%p.addParameter('label','radiance',@(x)(ischar(x)||iscell(x)));
p.addParameter('pbrt_path',[],@(x)(ischar(x)));

p.parse(inputFile,varargin{:});
%label       = p.Results.label;
pbrt_path       = p.Results.pbrt_path;
%%

energy = piReadEXR(inputFile, 'data type','radiance');

%确定通道数，我记得上级也确定过一次，可能多次确定保证其稳定性？
if isempty(find(energy(:,:,17),1))
    energy = energy(:,:,1:16);
    data_wave = 400:20:700;
else
    data_wave = 400:10:700;
end            
photons  = Energy2Quanta(data_wave,energy);
%x,y一定已知？这快有点奇怪
try
    depthImage = piReadEXR(inputFile, 'data type','depth');

catch
    warning('Can not find "Pz" channel, ignore reading depth');
end
            


%%
% Create a name for the ISET object
[indir, fname,~] = fileparts(inputFile);
fname = strrep(fname,'.exr','');

ieObjName  = sprintf(fname,datestr(now,'mmm-dd,HH:MM'));
cameraType = 'perspective';
%对ieObject的一堆东西进行设计，这肯定是需要在c++里定义出来的，和sceneCreate里应该是一个
switch lower(cameraType)
    case {'pinhole','spherical','perspective'}
        ieObject = piSceneCreate(photons,'wavelength', data_wave);
        ieObject.name = ieObjName;
        if numel(data_wave)<31
            % interpolate data for gpu rendering
            ieObject = sceneInterpolateW(ieObject,wave);
        end
        
        if ~isempty(pbrt_path)
            
            % PBRT may have assigned a field of view
            %这里已经做了转成wANGULar
            [hh,ww,cc]=size(photons);
            fov = find_fov(pbrt_path);
            wAngular = atan(tan(fov*pi/(2*180))*ww/hh)/pi*180*2;
            ieObject.wAngular = wAngular;
        end
        
    case {'realistic'}
        % todo
    otherwise
        error('Unknown optics type %s\n',cameraType);
end
if exist('ieObject','var') && ~isempty(ieObject) && exist('depthImage','var')
    ieObject.depthMap = depthImage;
end
mat_name = [fname, '.mat'];
save_path = fullfile(indir,mat_name);
save(save_path, 'ieObject')
end





