%杨老师要求最终输出WAngular
function fov = find_fov(pbrt_path)
fileID = fopen(pbrt_path);
tmp = textscan(fileID,'%s','Delimiter','\n','CommentStyle',{'#'});
txtLines = tmp{1};
nline = numel(txtLines);
ii=1;
while ii<=nline
    thisLine = txtLines{ii};
    if strncmp(thisLine, '"float fov"', length('"float fov"'))
        if  length(thisLine) > length('"float fov" ')
            fovLine = thisLine;
        else
            fovLine = txtLines{ii+1};
        end
    end
    ii=ii+1;
end
fclose(fileID);
num = regexp(fovLine, '-?\d*\.?\d*', 'match');
fov = str2double(num);

% 注意特意提到了pbrt与iset的f不同，而上边显然没有做转换。杨老师要求最终输出WAngular
% 分辨率pbrt里就可以拿，但我估计.m里肯定也拿过，不用担心
% (4)	Pbrt文件里面的fov表示的是垂直方向的HAngular（角度制），换算成水平方向的WAngular（角度制）的方法如下(计算时请注意角度制和弧度制是否需要转换)：
% half_HAngular = HAngular / 2
% half_WAngular = arctan(half_HAngular/HH*WW)
% WAngular = half_Wangular * 2
% 其中HH和WW分别是radiance数据的垂直分辨率和水平分辨率

