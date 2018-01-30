/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   svm_solver.h
 * Author: b1
 *
 * Created on January 27, 2018, 4:31 PM
 */

#include <iostream>
#include <fstream>
#include <strstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <iomanip>
#include "svm_common.h"
#include "svm_option.h"
#include  <string.h>
#include <fstream>
#define TOLERANCE 1e-6
using namespace std;


#ifndef SVM_SOLVER_H
#define SVM_SOLVER_H

class SVMSolver{
public:
    SVMSolver(SVMOption *opt);
    ~SVMSolver();
    int train();
    int predict();
protected:
    float error_rate();
    
    int load_model(ifstream& is);
    int dump_model(ofstream& os);
    float kernel(int i1, int i2);//define the kernel function
    
    float learned_func(int k); 
    int examine_example(int i1); // tried to find the optimal one 
    int take_step(int i1, int i2); // after finding the optimal one, we need to move forward
protected:
    //input:
    float _C;
    float _eps;
    float _sig;
    bool _is_linear_kernel;
    const char *_fname_train;
    const char *_fname_valid;
    const char *_fname_model;
    
    //internal options
    int _n;
    int _n_sv;
    TVectorArray _x_array;
    TFloatArray _y_array;
    TVector _w;
    TFloatArray _alpha;
    TFloatArray _d;
    TFloatArray _error_cache;
    float _b;
    float _delta_b;
    
};


#endif /* SVM_SOLVER_H */

