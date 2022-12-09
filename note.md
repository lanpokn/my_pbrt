1. 这个版本的pbrt居然不支持interactive？
2. 这个版本对同一个scene出warning问题 Couldn't find supported color space that matches chromaticities: r (0.7347, 0.2653) g (0, 1) b (0.0001, -0.077), w (0.32167906, 0.3376722)
3. channels name 为：B,G,R,也就是说深度信息不能从exr的输出里体现出来，这会要认真追代码了，因为结果不在image里边，metadata的概率较大
4. 目前多相机会崩掉，有空看一下
5. 虽然有warning，但是输出图片没有问题，图片花掉的问题依然存在
6. 目前跑通了，只要它不改ext代码就没问题（说实话一般会在ext加代码就离谱）
经过git diff之后， 比较值得注意的参数：
cameras.cpp中可以找omni的参数
+    else if (name == "omni")
+        camera = OmniCamera::Create(parameters, cameraTransform, film, medium, loc,
perpective多了distortionPolynomials,很多相机都多了这个
omni也是镜片，可能要看参数才能知道区别，microlens?
interactive确实删除了
multipleFrames是新增了，难道已经支持了可移动相机？

7. 在film查看新加的东西，\\zhenyi应该都是，比如我看到了instanceId  = si.instanceId; // zhenyi，直接diff这个文件
8. +    
9. // zhenyi: Add spectrum values into pixel
+    for (int i = 0; i < NSpectrumSamples; ++i) {
+        p.L[i] += L[i];
+    }
  追踪这个来检查光子还是光谱的问题
10. +    // zhenyi
+    //---------------------Provide options to select output type---------------------------------
+    // create channel names
追踪这个来看通道问题。
总之 全局搜索zhenyi一定没错，writeDz比较值得注意
11. For multispectral texture-,增加了多光谱纹理的支持
总结，大部分内容都是原作者加的，想找我们组的最新内容，只需要搜索zhenyi即可
12. 终于找到衔接点了
13. void CameraBase::InitMetadata(ImageMetadata *metadata) const {
    metadata->cameraFromWorld = cameraTransform.CameraFromWorld(shutterOpen).GetMatrix();
}没做什么阿，找不到metadata的赋值点,这里比较乱，在GetImage也有赋值
：    metadata->pixelBounds = pixelBounds;
    metadata->fullResolution = fullResolution;
    metadata->colorSpace = colorSpace;
没有写入深度的功能，所以我追不到？

14. 深度的寻找：Printf("Distance from camera: %f\n", Distance(intr.p(), cr->ray.o));然而这只是一个简单的两点间距离的寻找
      string DzChannelNames[2] = {"Dzx", "Dzy"};是什么？恐怕和深度无关，是微分的
      Desc：description

15. channelNames is just a string

16. 这意味着光谱的通道数只支持16或31
        if (writeRadiance)
        {
            ImageChannelDesc radianceDesc = image.GetChannelDesc(RadianceChannelNames);
            if (NSpectrumSamples==31)
            {
                image.SetChannels(pOffset, radianceDesc, {L[0], L[1], L[2], L[3], L[4], L[5],
                                L[6], L[7], L[8], L[9], L[10], L[11],
                                L[12], L[13], L[14], L[15],L[16], L[17], L[18], L[19], L[20],
                                L[21], L[22], L[23], L[24], L[25], L[26],
                                L[27], L[28], L[29], L[30]});
            }
            if (NSpectrumSamples==16)
            {
                image.SetChannels(pOffset, radianceDesc, {L[0], L[1], L[2], L[3], L[4], L[5],
                                L[6], L[7], L[8], L[9], L[10], L[11],
                                L[12], L[13], L[14], L[15]});
            }

        }
另外那些额外的write选项怎么打开？该代码存在于GbufferFilm里，而该film的设置也在配置文件中
solved!!!!
improtant:

{R,G,B}:Pixel color (as is also output by the regular "rgb" film).
Albedo.{R,G,B}: red, green, and blue albedo of the first visible surface
P{x,y,z}: x, y, and z components of the position.
u, v: u and v coordinates of the surface parameterization.
dzd{x,y}: partial derivatives of camera-space depth z with respect to raster-space x and y.
N{x,y,z}: x, y, and z components of the geometric surface normal.
Ns{x,y,z}: x, y, and z components of the shading normal, including the effect of interpolated per-vertex normals and bump/or normal mapping, if present.
Variance.{R,G,B}: sample variance of the red, green, and blue pixel color.
RelativeVariance.{R,G,B}: relative sample variance of red, green, and blue.

    this->film = Film::Create(film.name, film.parameters, exposureTime,
                              camera.cameraTransform, filt, &film.loc, alloc);
BasicScene::SetOptions to define film and so on

Radiance.C01

总结：
1. 光谱的通道数要么是31，要么是16，不是非常可变，看看能不能改动
2. zhenyi
3. yizhi

            if (NSpectrumSamples==31)
            {
                image.SetChannels(pOffset, radianceDesc, {L[0], L[1], L[2], L[3], L[4], L[5],
                                L[6], L[7], L[8], L[9], L[10], L[11],
                                L[12], L[13], L[14], L[15],L[16], L[17], L[18], L[19], L[20],
                                L[21], L[22], L[23], L[24], L[25], L[26],
                                L[27], L[28], L[29], L[30]});
结果是0.32这种的，不知道是不是光子数
iter.name @? bgppprrrrrrr
initpbrt 会出大问题，可能这条路行不通，要学习如何动手改变参数了。
InitBufferCaches会崩掉，还是老实做不重复读取场景的多相机吧
似乎读取的大头都在integrator->Render();所以cpu和gpu还要分开做。
总之多相机不是一个简单的事情。
总之先把框架整理好，保证一个初版是正确的，并且避开initpbrt()