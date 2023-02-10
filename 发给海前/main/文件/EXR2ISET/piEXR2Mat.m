%docker部分
function data = piEXR2Mat(infile, channelname)

[indir, fname,~] = fileparts(infile);

dockerimage = 'camerasimulation/pbrt-v4-cpu:latest';
%这里，把system（dockercmd）改成一个c++的可执行函数大概就完美了
%如果无法更改就用两个可执行文件，执行本地的imgtool即可
%执行已有可执行省时省力，别翻译了
if ispc
    basecmd = 'docker run --volume="%s":"%s" %s %s';
    indir_cmd = to_docker_path(indir);
    infile_cmd = to_docker_path(infile);
    cmd = ['imgtool convert --exr2bin ',channelname, ' ', infile_cmd];
    dockercmd = sprintf(basecmd, indir_cmd, indir_cmd, dockerimage, cmd);
else
    basecmd = 'docker run -ti --volume="%s":"%s" %s %s';
    cmd = ['imgtool convert --exr2bin ',channelname, ' ', infile];
    dockercmd = sprintf(basecmd, indir, indir, dockerimage, cmd);
end    


[status,result] = system(dockercmd);



if status
    disp(result);
    error('EXR to Binary conversion failed.')
end
filelist = dir([indir,sprintf('/%s_*',fname)]);
nameparts = strsplit(filelist(1).name,'_');
Nparts = numel(nameparts);
height = str2double(nameparts{Nparts-2});
width= str2double(nameparts{Nparts-1});

if strcmp(channelname,'Radiance')
    baseName = strsplit(filelist(1).name,'.');
    for ii = 1:31
        filename = fullfile(indir, [baseName{1},sprintf('.C%02d',ii)]);
        [fid, message] = fopen(filename, 'r');
        serializedImage = fread(fid, inf, 'float');
        data(:,:,ii) = reshape(serializedImage, height, width, 1);
        fclose(fid);
        delete(filename);
    end
else
        filename = fullfile(indir, filelist(1).name);
        [fid, message] = fopen(filename, 'r');
        serializedImage = fread(fid, inf, 'float');
        data = reshape(serializedImage, height, width, 1);
        fclose(fid);
        delete(filename);
end
    
end