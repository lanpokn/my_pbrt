function newpath = to_docker_path(path)
path = [lower(path(1)) path(2:end)];
path = strrep(path,':','');
path = strrep(path,'\','/');
newpath = insertBefore(path, 1, "//");