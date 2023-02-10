%这就是进行解包，全是依赖EXR2MAT的
function output = piReadEXR(filename, varargin)
%% Read multispectral data from a .exr file (Openexr format)
%
%   [imageData, imageSize, lens] = piReadEXR(filename)
%
%% Deprecated in favor of python wrapper version
%% Left in tree for anyone who has OpenEXR via .mex files working
%% But has a problem with python + pyexr -- David Cardinal 10/05/21
%% Using docker to convert exr to mat file. -- Zhenyi 10/14/21
%
% Required Input
%   filename - existing .exr file
%
% Optional parameter/val
%   dataType - specify the type of data for the output
%   NSpectrumSamples - number of spectrum samples
%
% Returns
%  
% Read .exr image data from the fiven filename.
%
%
% Zhenyi, 2020
% 
% Make sure openexr functions are compiled: 
%               (run make.m in ~/iset3d/ext/openexr)
% 
% Get exr channel information
%{
      data = exrinfo(filename);
      fprintf( '%s \n', data.channels{:} ); fprintf('\n')
%}   

%% 
parser = inputParser();
varargin = ieParamFormat(varargin);

parser.addRequired('filename', @ischar);
parser.addParameter('datatype', 'radiance', @ischar);

parser.parse(filename, varargin{:});
filename = parser.Results.filename;
dataType = parser.Results.datatype;

%%

switch dataType
    case "radiance"
        output = piEXR2Mat(filename, 'Radiance');
    case "zdepth"
        output = piEXR2Mat(filename, 'Pz');
    case "depth"
        XDepthMap = piEXR2Mat(filename, 'Px');
        YDepthMap = piEXR2Mat(filename, 'Py');
        ZDepthMap = piEXR2Mat(filename, 'Pz');
        
        output = sqrt(XDepthMap.^2+YDepthMap.^2+ZDepthMap.^2);
    case "3dcoordinates"
        output(:,:,1) = piEXR2Mat(filename, 'Px');
        output(:,:,2) = piEXR2Mat(filename, 'Py');
        output(:,:,3) = piEXR2Mat(filename, 'Pz');

    case "material" % single channel
        output = piEXR2Mat(filename, 'MaterialId');
    case "normal"
        output(:,:,1) = piEXR2Mat(filename, 'Nx');
        output(:,:,2) = piEXR2Mat(filename, 'Ny');
        output(:,:,3) = piEXR2Mat(filename, 'Nz');
    case "albedo"
        % to add; only support rgb for now, spectral albdeo needs to add;
    case "instance" % single channel
        output = piEXR2Mat(filename, 'InstanceId');
    otherwise
        error('Datatype not supported. \n%s', 'Supported datatypes are: "radiance", "zdepth", "3dcoordinates", "material", "normal";')
end


end