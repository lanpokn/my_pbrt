1. main函数在src,pbrt,cmd,pbrt_test中，比较隐蔽
2. launch中给到最后的参数，带双引号是没问题的，之前出现问题是因为路径错误
3. 乱七八糟的parsearg等都是用来处理命令行参数的字符串算法，不需要特别考虑，直接看后边的option存了什么，怎么处理的
4. 这个版本的cmake，除了extern以外，没有乱七八糟的子文件了，比较舒服。
5. 抽象基类的纯虚函数如果不想实现，必须给附上等于0，否则过不了链接（编译的时候由于认为其实现可能在任意cpp中，因此不给报警），=0就是告诉它这是纯虚，不要等它的实现了
6. main函数的argv接收时，最后面的字符串反而是第0号，然后第一个是1号，第二个是二号(非常混乱)，不知道换成windows会不会有区别，神奇
7. argv要特别注意：第一参数是可执行文件的位置，没什么用，有用的是后边三个，部署乱序是顺序的，只是第一个必须为可执行文件的位置！之前理解出现了大问题
8. 怎样解决乱码？成功解决！很可能是在之前的版本中出现了数组越界问题，没有及时注意到，所以把电脑干崩了一次。删除了繁琐中间步骤，操作指针一定要尽量简单明了，尽量交给string去做
   （不过由于需要给个命令行要求的指针数组，所以也是避无可比了）
9.  要增加新的相机，似乎需要改动camera,cameras两个文件，建议通过相机的命令行参数追一下相机的匹配过程
10. writeimage的过程我没看懂。。。，至于图像传递问题，注意似乎Vscode的自动跳转是错误的，要去image.cpp里看真正的实现，外层使用了一个函数指针封装了一下？可以重新学习一下函数指针
11. 输出一张图片真的需要很长时间吗，如果不需要实时的话（但是生成中间文件是很垃圾的代码结构）
12. cpu:追到intergrators 中的render，里边出现了camera.GetFilm().WriteImage(metadata, 1.0f / waveStart);，而gpu里边也出现了这个，所以不妨耐心追一下
    注意render里哪个选项是是否将中间过程的图片输出，所以默认是不输出，但是在完成渲染之后就可以输出了，所以不是到L那里，比较神奇
    在同一cpp的L方法中才有最终输出，可以看到其输出用的也是
    camera.InitMetadata(&metadata);
    camera.GetFilm().WriteImage(metadata, 1.0f / waveStart);
    （感觉做的很烂，cpu和gpu的图片输出居然分开了，总之还得追这里）
    dispatch cpu用了很多晦涩难懂的语法。。
13. 用服务器的gpu渲染一张多长时间？
14. 结论已经有了：经过不知道为什么要进行的复杂调用之后，最终回到image::write（），对于exr的输出就是wirteexr。而接受的数据便是imagemetadata与一个name。
    中间的中转向相当于是把camera中的write转到了真正的imagewrite，不同的相机，对于同一个film上的光强，得到的数据肯定不一样，因此可以理解这样的做法。
    但是具体中间是进行的哪些跳转，哪些是必须用到的，哪些是有了第三方库文件就不需要其头文件的？如果难以判断的话，建议直接用一下代码生成图片：
   GPU中的：
    ImageMetadata metadata;
    metadata.samplesPerPixel = integrator->sampler.SamplesPerPixel();
    integrator->camera.InitMetadata(&metadata);
    metadata.renderTimeSeconds = seconds;
    metadata.samplesPerPixel = integrator->sampler.SamplesPerPixel();
    integrator->film.WriteImage(metadata);
   cpu中的：
    ImageMetadata metadata;
    metadata.renderTimeSeconds = progress.ElapsedSeconds();
    metadata.samplesPerPixel = waveStart;
    camera.InitMetadata(&metadata);
    camera.GetFilm().WriteImage(metadata, 1.0f / waveStart);
   总之需要：metadata，metadata需要renderTimeSeconds和InitMetadata，而这个需要的东西一个和integrator的成员函数耦合，一个干脆就用的integrator中的成员函数
   离谱。。。
   把一个metadata作为中间变量传递，不要随便动它的成员变量，应该可以做到返回一个图像参数？
   注意writeimage需要WriteImage(metadata, splatScale)，这个都是film的写函数，但是有时候第二个缺省了，没给默认值能缺省吗？
   原因：    void WriteImage(ImageMetadata metadata, Float splatScale = 1);参数缺省在定义和声明中只能出现一次，所以此处是在声明中定义了缺省默认值，不是没给。
   如此一来，除了不知道film是如何转到image的，其他都已经懂了，建议再追一下，逐帧debug。
   Ts... is a name!
   dispatch
        return detail::DispatchCPU<F, R, Ts...>(func, ptr(), Tag() - 1); it decides which cpu to use?
   现在有两个思路：
   1. 返回metadata，那就要求其他地方也有这个metadata的定义，但是灵活，有可能可以生成各种其他类型的图片。不过metadata本身是易用的，把头文件拿出来即可，
   以有的pbrt_render已经包括这个了
   2. 或者追到生成exr之前的最后一步，这个一定是openexr的东西，通用性更强，但是可能不够灵活，而且有个巨大的问题，就是中间的调用极度离谱，估计很难靠逐级返回的方法把这个返回出来
void Film::WriteImage(ImageMetadata metadata, Float splatScale) {
    auto write = [&](auto ptr) { return ptr->WriteImage(metadata, splatScale); };
    return DispatchCPU(write);
}
为什么，void可以有返回值？
DispatchCPU是一个非常非常频率调用的函数，可能是并发的考虑，可能很多函数都会传一个函数指针进去，让他决定哪个被调用，而且
template <typename F, typename R, typename T0, typename T1, typename T2, typename T3,
          typename T4, typename T5, typename T6, typename T7, typename... Ts,
          typename = typename std::enable_if_t<(sizeof...(Ts) > 0)>>
auto DispatchCPU(F &&func, void *ptr, int index) {
    DCHECK_GE(index, 0);

    switch (index) {
    case 0:
        return func((T0 *)ptr);
    case 1:
        return func((T1 *)ptr);
    case 2:
        return func((T2 *)ptr);
    case 3:
        return func((T3 *)ptr);
    case 4:
        return func((T4 *)ptr);
    case 5:
        return func((T5 *)ptr);
    case 6:
        return func((T6 *)ptr);
    case 7:
        return func((T7 *)ptr);
    default:
        return DispatchCPU<F, R, Ts...>(func, ptr, index - 8);
    }
}
从形式上，就是同一个函数func，不用的指针类型（ptr由void强转为其他类型的指针变量），问题是这么多typename是在哪里？
上述没有被调用。。。。，因为它有一堆重载，重载的是模板T1，T2的数量，write用的那个直接没有index什么事了。
name comes from cmd
hou to give back an image without output and without return.
can I add a parameter to edit imagewrite to update a paramter in pbrt(rather than output an image) and let main include it, so that we can give back an image
        Imf::FrameBuffer fb = imageToFrameBuffer(*this, AllChannelsDesc(), dataWindow);
        Imf::OutputFile file(name.c_str(), header);
        file.setFrameBuffer(fb);
        file.writePixels(resolution.y);
      fb,resolution.y is all we need to create an exr file, this is method2
      method1: return metadata, and try to create a file by image.write()(need to be test!!)


  
  15. 难以理解的问题what():  Failed to write pixel data to image file "done". No frame buffer specified as pixel data source.
   将文件输出成exr居然还涉及不同线程的问题？,已经确定了，fb在image.cpp中是好的，传到我的cpp中就变成空了。注意到之前有重定义的问题，我加了个static解决掉的，可能这样根本就
   没有解决，首先查一查全局static的作用，然后看看会不会哪里重新初始化了。再找找有没有其他全局变量，看看为什么会重定义，重定义发生在哪一步骤，然后上网找答案。
16. 
头文件中应使用extern关键字声明全局变量（不定义），如果这个变量有多个文件用到，可以新建一个cpp，在其中定义，把这个cpp加入工程即可。头文件请不要定义任何变量，那是非常业余的行为……

一般在头文件中申明，用extern,在cpp中定义。 如果在头文件中定义，如果这个头文件被多个cpp引用，会造成重复定义的链接错误。

头文件只能申明全局变量（extern），不可定义（不推荐使用）    .cpp里，在最外层定义即可（int gi），直接引用

注意定义和声明都不是初始化，定义的时候也要写int a = 1这样的关键字，不能缺省int!我建议复习声明与定义的关系，已经理解的不太好了。
17. 新的问题： explosion.exr: error writing EXR: Cannot open image file "explosion.exr". Permission denied.以前没碰到过，这里为什么一定要打开这个文件？
    为什么生成的图片花了？数据应该没问题阿？另外，为什么image里做一次，外面做一次就有permision deny？怪问题，得看openexr
    why the file will output to random location"though it's not my problem" and is it neccessary to have an exr suffix?
    because: you have forgotten to make!
    好像不是简单的int，建议还是传递resolution吧
    does the name must be the same with the file .pbrt? not this problem
    
18. change resolution in .pbrt file
    add fog:may be in jiezhi?media，explosion is a medium? it's better to change another scene
    pbrt do not have original depth msg?
    give depth image is a hard problem, original pbrt has not depth msg at all(need to rewrite a lot of code)
19. camera等信息位于.pbrt文件中，如果想把接口留出来还需要改。具体留什么接口？使用何种相机？能否对.pbrt进行文件上的更改？
    现在用的是投影相机，需要换成使用realistic的相机.由于cpu和gpu的不同，还不可以进入之后再做，必须保证scene中的camera信息是用户自己给定的。
    所有事情都要自己试过再确认，之前就误以为camera一定在命令行里，结果不在，这下巨尴尬。
20.     
    //it will be used when parsefile, if it is not null, we will create a new pbrt configfile(with a different name), and then delete it
    //because original file is very conplex and only use a filename as input, do so can avoid change code
    //为什么要生成新的文件：原本的源代码涉及到多个文件，而且最后应该是用的重定向输入流的方式进行读取，要改动就要涉及其底层逻辑
    //而且要么改动一堆源文件，要么该文件会在其他地方出现难以预料的问题
    //因此，我决定生成一个临时配置文件，至于是否需要运行后删除就看需求了
    //所以接下来的任务：读取源文件，在保证源文件,修改其中特定的camera内容(可以考虑先把camera后边所有开头是空格的行，以及camera所在行“”的内容删除，再加），再生成到新的配置文件里，
    //然后在parsefile前根据需求更改filevector（有理由认为原作者考虑过多相机，不然为什么要用vector?)
    //记得跑一下有多个相机定义的文件，看看是怎么回事,run done,but no more image output, still need to be investigate