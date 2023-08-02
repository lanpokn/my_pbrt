import numpy as np

a = np.zeros((2,2,2,2))
b = a[:,1]
print(b.size())

"""_summary_
    num = int((width/x_cube_size)*(height/y_cube_size)*(math.ceil(max(events[1, :] / t_cube_size))))
events_cube = [[] for _ in range(num)]
究极鬼畜的表示方法，这东西是二维的，第一维表示xyzp，第二维是N个数，也就是4xN结构，第一维尤四个数，第二维有N个数。我的其实也是类似，是我自己没明白
"""
# num = int((width/x_cube_size)*(height/y_cube_size)*(math.ceil(max(events[1, :] / t_cube_size))))
# events_cube = [[] for _ in range(num)]