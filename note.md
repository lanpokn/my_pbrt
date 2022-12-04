1. 这个版本的pbrt居然不支持interactive？
2. 这个版本对同一个scene出warning问题 Couldn't find supported color space that matches chromaticities: r (0.7347, 0.2653) g (0, 1) b (0.0001, -0.077), w (0.32167906, 0.3376722)
3. channels name 为：B,G,R,也就是说深度信息不能从exr的输出里体现出来，这会要认真追代码了，因为结果不在image里边，metadata的概率较大
4. 目前多相机会崩掉，有空看一下
5. 虽然有warning，但是输出图片没有问题，图片花掉的问题依然存在