%C++中要自己定义这么一种ieObject出来，
%建议弄个matlabStruct.h,让所有文件include一下
function scene = piSceneCreate(photons,varargin)
% Create a scene from radiance data
%
%    scene = piSceneCreate(photons,varargin)
%
% Required
%    photons - row x col x nwave data, computed by PBRT usually
%
% Key/values
%    fov         - horizontal field of view (deg)
%
% Return
%  An ISET scene structure
%
% Example:
%{
   sceneFile = '/home/wandell/pbrt-v2-spectral/pbrt-scenes/bunny.dat';
   photons = piReadDAT(outFile, 'maxPlanes', 31);
   scene = piSceneCreate(photons);
   ieAddObject(scene); sceneWindow;
%}
% BW, SCIENTSTANFORD, 2017

%% When the PBRT uses a pinhole, we treat the radiance data as a scene
%input parser用c++的默认值系统代替掉即可
p = inputParser;
p.KeepUnmatched = true;
p.addRequired('photons',@isnumeric);
p.addParameter('fov',40,@isscalar)               % Horizontal fov, degrees
p.addParameter('meanluminance',100,@isscalar);
p.addParameter('wavelength', 400:10:700, @isvector);

if length(varargin) > 1
    for i = 1:length(varargin)
        if ~(isnumeric(varargin{i}) || ...
                islogical(varargin{i}) || ...
                isobject(varargin{i}))
            varargin{i} = ieParamFormat(varargin{i});
        end
    end
else
    varargin =ieParamFormat(varargin);
end

p.parse(photons,varargin{:});

%% Sometimes ISET is not initiated. We need at least this

global vcSESSION
if ~isfield(vcSESSION,'SCENE')
    vcSESSION.SCENE = {};
end

%% Set the photons into the scene
scene.type = 'scene';
scene.metadata = [];
scene.spectrum.wave = p.Results.wavelength;
scene.distance = 1.0;
scene.magnification = 1;
scene.data.photons = photons;
scene.data.luminance = [];
scene.illuminant.name = 'D65';
scene.illuminant.type = 'illuminant';
scene.illuminant.spectrum.wave = [400,410,420,430,440,450,460,470,480,490,500,510,520,530,540,550,560,570,580,590,600,610,620,630,640,650,660,670,680,690,700];
scene.illuminant.data.photons = [9.0457595e+15;1.0250447e+16;1.0724296e+16;1.0186997e+16;1.2611661e+16;1.4392664e+16;1.4814279e+16;1.4757830e+16;1.5211861e+16;1.4576585e+16;1.4949163e+16;1.5032161e+16;1.4899500e+16;1.5606805e+16;1.5416749e+16;1.5648686e+16;1.5313995e+16;1.5016408e+16;1.5193658e+16;1.4309865e+16;1.4769715e+16;1.4948381e+16;1.4871616e+16;1.4351952e+16;1.4652141e+16;1.4228330e+16;1.4481596e+16;1.5079676e+16;1.4562064e+16;1.3159828e+16;1.3712425e+16];
scene.wAngular = 40;
[r,c] = size(photons(:,:,1)); depthMap = ones(r,c);
scene.depthMap = depthMap;
end
