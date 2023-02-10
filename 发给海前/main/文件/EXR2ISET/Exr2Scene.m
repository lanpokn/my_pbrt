function Exr2Scene(inputFile, varargin)
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
%          pe = pyenv; 
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
%iexx都是iset的函数，这个是把一个字符串转成ie要求的格式，即lower case, no spaces
%varargin就是除了inputfile以外其他所有的输入，是Variable length input argument list的缩写
varargin =ieParamFormat(varargin);

p = inputParser;%p是用来解析函数输入的，用这样来保证灵活输入
%对函数输入提出一堆要求
p.addRequired('inputFile',@(x)(exist(x,'file')));
%p.addParameter('label','radiance',@(x)(ischar(x)||iscell(x)));
p.addParameter('pbrt_path',[],@(x)(ischar(x)));
p.addParameter('wave', 400:10:700, @isnumeric);

%进行解析，结果存储在p.results中
p.parse(inputFile,varargin{:});
%label       = p.Results.label;
pbrt_path    = p.Results.pbrt_path;
wave         = p.Results.wave;
%%

%piReadExr专门用于读取多光谱数据
energy = piReadEXR(inputFile, 'data type','radiance');

%这里可能是和pbrt那种要么15要么16通道，要么32通道进行配合，将来改通道数必炸
if isempty(find(energy(:,:,17),1))
    energy = energy(:,:,1:16);
    data_wave = 400:20:700;
else
    data_wave = 400:10:700;
end
%Convert energy (watts) to number of photons.
%注意 data_wave是对波长信息的一种采样，sample wavelengths in units of nanometers
%data_wave信息在函数前后不会变化，所以不作为返回值
%要和energy/photons配套使用
photons  = Energy2Quanta(data_wave,energy);

%简简单单读深度
try
    depthImage = piReadEXR(inputFile, 'data type','depth');

catch
    warning('Can not find "Pz" channel, ignore reading depth');
end
            


%%
% Create a name for the ISET object
%fileparts: 此 MATLAB 函数 返回指定文件的路径名称、文件名和扩展名
[indir, fname,~] = fileparts(inputFile);
fname = strrep(fname,'.exr','');

% 此 MATLAB 函数 使用 formatSpec 指定的格式化操作符格式化数组 A1,...,An 中的数
% 据，并在 str 中返回结果文,datestr - 将日期和时间转换为字符串格式
ieObjName  = sprintf(fname,datestr(now,'mmm-dd,HH:MM'));
cameraType = 'perspective';

switch lower(cameraType)
    case {'pinhole','spherical','perspective'}
        %Create a scene from radiance data
        ieObject = piSceneCreate(photons,'wavelength', data_wave);
        ieObject.name = ieObjName;
        %if numel(data_wave)<31
            % interpolate data for gpu rendering
            %ieObject = sceneInterpolateW(ieObject,wave);
        %end
        
        if ~isempty(pbrt_path)
            
            % PBRT may have assigned a field of view
            %fov直接在文件中读取出来，pbrt应该有办法，或者老老实实复现即可
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
%存在深度就拿一下
% (5)	在exr中，depthmap的通道为‘Px’，‘Py’和‘Pz’
% depthMap = sqrt(Px.^2+Py.^2+Pz.^2)
% 上边那些大概率不用改动，只需要注意下边三个，在image.cpp中一定要加个if
% 注：若pbrt文件中设置了不渲染depthmap，那么exr文件中将没有depthmap的三个通道，
% 此时depthmap给全1的单通道矩阵即可，分辨率与radiance数据的空间分辨率相同。

if exist('ieObject','var') && ~isempty(ieObject) && exist('depthImage','var')
    ieObject.depthMap = depthImage;
end
scene = ieObject;
mat_name = [fname, '.mat'];
%此 MATLAB 函数 根据指定的文件夹和文件名构建完整的文件设定,不同系统的运作不同
save_path = fullfile(indir,mat_name);
save(save_path, 'scene', '-v7.3')
end





