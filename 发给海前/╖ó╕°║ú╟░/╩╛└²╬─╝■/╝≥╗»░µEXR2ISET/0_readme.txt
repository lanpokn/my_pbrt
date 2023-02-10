主文件为Exr2Scene.m
先转移底层，自底向上即可，piEXR2Mat会复杂一些，连接pbrt部分
总结：主文件Exr2Scene.m
底层文件：Energy2Quanta（只调用vcConstants），find_fov,ieParamFormat（这最底层）
，piEXR2Mat（用了docker调用pbrt中的exr2bin）
（to_docker_path不用翻译，让小米安装docker不现实）
sceneCreate巨长，真要一点一点翻译它，那就是主要工作都在这里了
但它还用了sceneDefault，真翻译起来就没完了，
好消息，sceneCreate从始至终没出现，不用翻译它了


ISET调用EXR调用MAT
全做完再检查细节，因为代码多可能乱，wangular已经转过了，可能只需要注意角度单位的问题

%如果无法更改就用两个可执行文件，执行本地的imgtool即可
%执行已有可执行省时省力，要不就这么定了，10号开始写代码，应该可以大功告成
---注意matlab有存储函数，ieobject在c++中可能要做成和matlab兼容的---