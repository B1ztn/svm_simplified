/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   svm_solver.cpp
 * Author: b1
 *
 * Created on January 27, 2018, 4:41 PM
 */

#include <cstdlib>
#include "svm_common.h"
#include "svm_solver.h"
#include <vector>
using namespace std;



/*
 * 
 */
//first we need to calibrate the models
SVMSolver::SVMSolver(SVMOption* opt){
    this->_C = opt->_C;
    this->_eps = opt->_eps;
    this->_sig = opt->_sig;
    this->_is_linear_kernel = opt->_is_linear_kernel;
    this->_fname_train = opt->_fname_train;
    this->_fname_valid = opt->_fname_valid;
    this->_fname_model = opt->_fname_model;
    
    _n = 0;
    _n_sv = 0;
    _b = 0.0;
    _delta_b = 0.0;
}

SVMSolver::~SVMSolver(){}
//here we only have two kernel functions one is for linear function another one is rbf.
float SVMSolver::kernel(int i1, int i2){
    float k =dot_product(_x_array[i1],_x_array[i2]);
    if(!_is_linear_kernel){
        //k = exp((-2*x1*x2+x1^2 + x2^2 +x1^2)/(-2*sigma^2)) 
        k *= -2.0;
        k+=_d[i1] + _d[i2];
        k /= -(2.0* _sig * _sig);
        k = exp(k);
    }
    return k;
}

float SVMSolver::learned_func(int k){
    float s = 0.0;
    if(_is_linear_kernel){
        s = dot_product(_w, _x_array[k]);
    }else{
        for(int i=0; i<_n; i++){
            if(_alpha[i] > 0){
                
                s+= _alpha[i] * _y_array[i] * kernel(i,k);
            }
        }
    }
    s-= _b;
    //the linear function: y = wx-b
    
    return s;
}

int SVMSolver::predict(){
    cout<<"now we predict"<<endl;
    _x_array.clear();
    _y_array.clear();
    _w.clear();
    _alpha.clear();
    
    ifstream is_model(_fname_model);
    load_model(is_model);
    //load model and data...
    ifstream is_valid(_fname_valid);
    batch_read_sample(is_valid, _x_array, _y_array, _n);
    
    _n += _n_sv;
    if(!_is_linear_kernel){
        _d.resize(_n);
        for(int i=0; i<_n; i++){
            _d[i] = dot_product(_x_array[i], _x_array[i]);
        }
    }
    
    int n_correct = 0;
    float y_pred = 0.0;
    
    for(int i= _n_sv; i< _n; i++){
        float s = 0.0;
        if(_is_linear_kernel){
            s=dot_product(_w,_x_array[i]);
        }
        else{
            for(int j =0; j< _n_sv;j++){
                // 
                s+=_alpha[j] * _y_array[j]*kernel(i,j);
            }
        }
        s-= _b;
        //predict the y value related to x value 
        y_pred = s>= 0.0? 1.0:-1.0;
        cout<< y_pred<<endl;
        if((y_pred >0 and _y_array[i]>0) || (y_pred <0 and _y_array[i]<0)){
            n_correct++;
        }
    }
    // measure the accuracy of the model
    cerr << setprecision(5)
            << "Accuracy:" << 100.0 * n_correct /(_n - _n_sv)
            << "%(" << n_correct << "/" << (_n - _n_sv)<<")"
            <<endl;
    return 0;
    
}

int SVMSolver :: train(){
    cout<<"yes we train data"<<endl;
    _x_array.clear();
    _y_array.clear();
    _w.clear();
    _d.clear();
    
    ifstream is_train(_fname_train);
    batch_read_sample(is_train, _x_array, _y_array, _n);
    
    _alpha.resize(_n,0.0);
    _b = 0.0;
    
    _error_cache.resize(_n,0.0);
    if(!_is_linear_kernel){
        _d.resize(_n);
        for(int i=0; i<_n; i++){
            _d[i] = dot_product(_x_array[i], _x_array[i]);
        }
    }
    int num_changed = 0;
    int examine_all = 1;
    while(num_changed >0 || examine_all){
        num_changed = 0;
        if(examine_all){
            for(int k = 0; k< _n; k++){
                num_changed += examine_example(k);
            }
        }else{
            for(int k =0; k<_n ; k++){
                if(_alpha[k] != 0 && _alpha[k] != _C){
                    num_changed += examine_example(k);
                }
            }
        }
        if(examine_all ==1){
            examine_all =0;
        }else if(num_changed == 0){
            examine_all = 1;
        }
        float s =0.0;
        float t = 0.0;
        float obj = 0.0;
        for(int i=0; i< _n ; i++){
            s += _alpha[i];
        }
        for(int i=0; i<_n; i++){
            for(int j=0; j<_n; j++){
                t += _alpha[i] * _alpha[j] *_y_array[i] * _y_array[j] * kernel(i,j);
            }
        }
        /*
         * the smo algorithm is that sum(alpha[i]) - sum(0.5* alpha[i]*alpha[j]*y[i]*y[j]*kernel(i,j))
         */
        obj = s-0.5*t;
        cerr << setprecision(5)
                
                << "Objectie func: " << obj <<"\t\t"
                << "Error rate: " << error_rate()
                <<endl;
        for(int i=0; i<_n; i++){
            if(_alpha[i] <1e-6){
                _alpha[i] = 0.0;
            }
            
        }
    }
    
    ofstream os_model(_fname_model);
    dump_model(os_model);
    return 0;
    
}

int SVMSolver::dump_model(ofstream& os){
    TString s;
    os << _is_linear_kernel << endl;
    os << _b << endl;
    if(_is_linear_kernel){
        os << _w.size() <<endl;
        for(int i=0; i<_w.size();i++){
            os << _w[i].first <<' '<<_w[i].second << endl;
        }
    }
    else{
        // count the alpha that is greater than 0
        os << _sig << endl;
        _n_sv =0;
        for(int i=0; i< _n; i++){
            if(_alpha[i] >0){
                _n_sv += 1;
            }
        }
        os<<_n_sv <<endl;
        for(int i=0; i<_n; i++){
            if(_alpha[i] > 0){
                os << _alpha[i] <<endl;
            }
        }
        
        for(int i=0; i<_n; i++){
            if(_alpha[i] >0){
                write_sample(s,_x_array[i], _y_array[i]);
                os << s << endl;
            }
        }
    }
}

int SVMSolver:: load_model(ifstream& is){
    int d =0;
    int m =0;
    TString s;
    is >> _is_linear_kernel;
    is >> _b;
    if(_is_linear_kernel){
        is >> d;
        for(int i=0; i<d; i++){
            TVectorDim p;
            is >> p.first >> p.second;
            _w.push_back(p);
        }
        sort(_w.begin(), _w.end(),comp);
    }else{
        is >> _sig;
        is >> _n_sv;
        _alpha.resize(_n_sv,0.0);
        for(int i=0; i<_n_sv;i++){
            is >> _alpha[i];
        }
        getline(is,s,'\n');
        batch_read_sample(is,_x_array, _y_array,m);
    }
    return 0;
}

//we calculate the error rate for the predict value and true value....
float SVMSolver :: error_rate(){
    int n_error = 0;
    for(int i=0; i<_n; i++){
        if((learned_func(i) >= 0 && _y_array[i] < 0) || (learned_func(i)<0 && _y_array[i]>0)){
            n_error++;
        }
    }
    return 1.0*n_error/_n;
}



int SVMSolver::examine_example(int i1){
    float y1 = 0.0;
    float _alpha1 =0.0;
    float e1 =0.0;
    float r1 =0.0;
    
    y1 = _y_array[i1];
    _alpha1 = _alpha[i1];
    if(_alpha1 >0 && _alpha1 < _C){
        e1 = _error_cache[i1];
    }else{
        //we re-calcualte it again for the error!
        e1 = learned_func(i1) - y1;
    }
    
    r1 = y1 * e1;
    if((r1 < -TOLERANCE && _alpha1 < _C) || (r1 > TOLERANCE && _alpha1> 0)){
        int k0 = 0;
        int k = 0;
        int i2 = -1;
        float tmax = 0.0;
        for(i2 = -1, tmax =0, k=0; k<_n; k++){
            if(_alpha[k] >0 && _alpha[k]< _C){
                float e2 = 0.0;
                float temp = 0.0;
                e2 =_error_cache[k];
                temp = fabs(e1 - e2);
                if(temp > tmax){
                    tmax = temp;
                    i2 = k;
                }
            }
            if(i2 >=0 ){
                if(take_step(i1,i2)){
                    return 1;
                }
            }
        }
        for(k0 = (int)(drand48() * _n), k=k0; k<_n+k0; k++){
            i2 = k % _n;
            if(_alpha[i2]>0 && _alpha[i2]< _C){
                if(take_step(i1,i2)){
                    return 1;
                }
            }
        }
        for(k0 = (int)(drand48()*_n),k=k0;k<_n+k0;k++){
            i2 = k % _n;
            if(take_step(i1,i2)){
                return 1;
            }
        }
    }
    return 0;
}

//we check the pair and pair x_array and we then take the next step...
int SVMSolver::take_step(int i1, int i2){
    int y1 = 0;
    int y2 = 0;
    int s =0;
    float _alpha1 = 0.0;
    float _alpha2 = 0.0;
    float a1 = 0.0;
    float a2 =0.0;
    float e1 =0.0;
    float e2 = 0.0;
    float low = 0.0;
    float high = 0.0;
    float k11 =0.0;
    float k22 = 0.0;
    float k12 = 0.0;
    float low_obj = 0.0;
    float high_obj = 0.0;
    if(i1==i2){
        return 0;
    }
    _alpha1 = _alpha[i1];
    _alpha2 = _alpha[i2];
    y1 = _y_array[i1];
    y2 = _y_array[i2];
    if(_alpha1 >0 && _alpha1 < _C){
        e1 = _error_cache[i1];
    }else {
        e1 = learned_func(i1) - y1;
    }
    
    
    if(_alpha2>0 && _alpha2 <_C){
        e2 = _error_cache[i2];
    }else{
        e2 = learned_func(i2) - y2;
    }
    
    s = y1*y2;
    /*
     * s = y1*y2, s==1 or s == -1
     * alpha1 + alpha2 = gamma (constant)
     * so we can have alpha2 range
     * if s==1 then alpha1 + alpha2 = gamma
     *      if gamma >_C then max alpha2 = _C and min alpha2 = gamma -_C
     *      if gamma < _C then min alpha2 = 0 and max alpha2 = gamma 
     * if s == -1 then alpha1 - alpha2 = gamma 
     *      if gamma >0 then min alpha2 =0 and max alpha2 = _C - gamma
     *      if gamma <0 then min alpha2 = -gamma and max alpha2 = _C
     * we can optimize the alpha1 and alpha2 then we gradually decrease the Error!!!!!
     */
    if(y1 == y2){
        float gamma = _alpha1 + _alpha2;
        if( gamma > _C){
            low = gamma - _C;
            high = _C;
        }else{
            low = 0;
            high = gamma;
        }
    }else{
        float gamma = _alpha1 - _alpha2;
        if(gamma > 0){
            low = 0;
            high = _C - gamma;
        }else {
            low = -gamma;
            high = _C;
        }
    }
    //THIS IS THE CONDITION THAT WE STOP !!!!!!!!
    // the reason is that and those two conerge ....
    if(fabs(low - high )<1e-6){
        return 0;
    }
    /*
     * s = y1*y2
     * delta alpha2 = y2*(E2 - E1)/eta
     * delta alpha1 = -s*delta alpha2
     */
    float eta = 0.0;
    k11 = kernel(i1,i1);
    k12 = kernel(i1,i2);
    k22 = kernel(i2,i2);
    eta = 2*k12 - k11 - k22;
    //update the new a2 but we need to adjust the [low, high] 
    if(eta < 0){
        a2 = _alpha2 + y2 *(e2 -e1 )/eta;
        if(a2 < low ){
            a2 = low;
        }else if(a2 > high){
            a2 = high;
        }
    }else{
        float c1 = eta/2.0;
        float c2 = y2 * (e1 -e2)-eta * _alpha2;
        low_obj = c1 * low * low +c2 *low;
        high_obj = c1 * high *high +c2 *high;
        if(low_obj > high_obj + _eps){
            a2 = low;
        }else if(low_obj < high_obj- _eps){
            a2 = high;
        }else{
            a2 = _alpha2;
        }
    }
    
    if(fabs(a2 - _alpha2)< _eps*(a2 + _alpha2 + _eps)){
        return 0;
    }
    // we update the new a1 
    a1 = _alpha1 - s*(a2 - _alpha2);
    if(a1 < 0){
        a2 += s*a1;
        a1 = 0;
    }else if(a1 > _C){
        float t = a1 - _C;
        a2 += s*t;
        a1 = _C;
    }
    
    float b1 = 0.0;
    float b2 = 0.0;
    float bnew = 0.0;
    if(a1 > 0 && a1 < _C){
        bnew = _b +e1 + y1*(a1 -_alpha1) * k11 + y2*(a2 -_alpha2)*k12;
    }else if(a2 > 0 && a2 < _C){
        bnew = _b +e2 + y1* (a1 - _alpha1)* k12 + y2 * (a2 -_alpha2)*k12;
    }else{
        b1 =_b + e1 +y1 * (a1 - _alpha1) * k11 + y2 * (a2 - _alpha2)*k12;
        b2 = _b + e2 +y1 * (a1 - _alpha1) * k12 + y2 * (a2 -_alpha2)*k22;
        bnew = (b1 + b2) /2.0;
    }
    // update the _b......
    _delta_b = bnew - _b;
    _b = bnew;
    
    float t1 = y1*(a1 - _alpha1);
    float t2 = y2 * (a2 - _alpha2);
    if(_is_linear_kernel){
        _w = _w + t1 *  _x_array[i1] +t2 * _x_array[i2];
    }
    for(int i=0; i<_n; i++){
        if(_alpha[i] >0 && _alpha[i] < _C){
            _error_cache[i] += t1*kernel(i1,i) + t2 *kernel(i2,i) - _delta_b;
        }
    }
    _error_cache[i1] = 0.0;
    _error_cache[i2] = 0.0;
    _alpha[i1] = a1;
    _alpha[i2] = a2;
    return 1;
    
}
