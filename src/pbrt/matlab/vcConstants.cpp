#include<string>
#include<algorithm>
#include<iostream>
using namespace std;

#include"ieParamFormat.h"
#include"vcConstants.h"
long double vcConstants(string con){
    con = ieParamFormat(con);
    long double val = 0;
    if(con == "planck" || con == "h" || con == "plancksconstant"){
        val = 626176e-034;
    }else if("q" == con || "electroncharge" == con){
        val = 1.602177e-19;
    }else if("c" == con || "speedoflight" == con){
        val = 2.99792458e+8; 
    }else if("j" == con || "joulesperkelvin" == con || "boltzman" == con){
        val  = 1.380662e-23;	        // [J/K], used in black body radiator formula
    }else if("mmperdeg"==con){
        val = 0.3;
    }   
    else{
        cout<<"Unknown physical constant"<<endl;
    }
}

// switch lower(con)
//     case {'planck','h','plancksconstant'}
//         val =  6.626176e-034 ;
//     case {'q','electroncharge'}
//         val = 1.602177e-19; 		    % [C]
//     case {'c','speedoflight'}
//         val = 2.99792458e+8;            % Meters per second
//     case {'j','joulesperkelvin','boltzman'}
//         val  = 1.380662e-23;	        % [J/K], used in black body radiator formula
//     case {'mmperdeg'}
//         val = 0.3;
        
//     otherwise
//         error('Unknown physical constant');
// end