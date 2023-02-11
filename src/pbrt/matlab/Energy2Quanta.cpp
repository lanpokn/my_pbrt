#include"Energy2Quanta.h"
#include"vcConstants.h"
//完成的不全！
Photons Energy2Quanta(Wave wavelength,Energy energy){

        //这些matlab独有操作暂时不管，强制变成列向量之类的，但我向量都是自己强行设计的
    //     if size(wavelength,2) ~= 1 && size(wavelength,1) ~= 1
    //         error('Energy2Quanta:  Wavelength must be a vector');
    // else    wavelength = wavelength(:);      % Force to column
    // end

    //     % Fundamental constants.  
    // h = vcConstants('h');		% Planck's constant [J sec]
    // c = vcConstants('c');		% speed of light [m/sec]
    double h = vcConstants("h");
    double c = vcConstants("c");
    // if ndims(energy) == 3
    //     [n,m,w] = size(energy);
    //     if w ~= length(wavelength)
    //         error('Energy2Quanta:  energy must have third dimension length equal to numWave');
    //     end
    //     energy = reshape(energy,n*m,w)';
        //.*是对应位置乘法，本质上是所有不同的第三维度都对应一个波长即可
    //     photons = (energy/(h*c)) .* (1e-9 * wavelength(:,ones(1,n*m))); %单个光子能量e=h*c/lamda
    //     photons = reshape(photons',n,m,w);
    // else
    //     [n,m] = size(energy);
    //     if n ~= length(wavelength)
    //         errordlg('Energy2Quanta:  energy must have row length equal to numWave');
    //     end
    //     photons = (energy/(h*c)) .* (1e-9 * wavelength(:,ones(1,m)));
    // end

    //它这里操作非常奇怪，energy还能存成二维？
    //TOCHECK 下边的else是否需要做,建议matlab跑一次，看看到底调用了哪些内容！
    int n = energy.size();
    int m = energy.at(0).size();
    int w = energy.at(0).at(0).size();
    Photons photons = energy;//先初始化一下，方便一会用at进行操作
    for(int i =0;i<n;i++){
        for(int j = 0;j<m;j++){
            for(int k = 0;k<w;k++){
                //这里似乎没有那么简单，是一部分不变，另一部分被转化
                photons.at(n).at(m).at(w) = (energy.at(n).at(m).at(w))/(h*c)*(1e-9*wavelength.wave.at(w));
        }
    }
    return photons;
}

    // wavelength(:,ones(1,n*m))
    // bitoai 的解释
    // The above code is creating a matrix with n*m columns of the same wavelength values.
    //  The first row of the matrix will contain the wavelength values 
    //  and the remaining rows will be filled with the same values, repeated n*m times.