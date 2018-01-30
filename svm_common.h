/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   svm_common.h
 * Author: b1
 *
 * Created on January 26, 2018, 9:59 PM
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
#include <string.h>
#include <iomanip>



#ifndef SVM_COMMON_H
#define SVM_COMMON_H
#define MAX_PATH_LENGTH 500
using namespace std;
typedef string TString;
typedef vector<string> TStringArray;
typedef pair<int, float> TVectorDim;
typedef vector<TVectorDim> TVector;
typedef vector<TVector> TVectorArray;
typedef vector<float> TFloatArray;
int split(const TString& s, char c, TStringArray& v);
//like the cmp 
bool comp(const TVectorDim& p1, const TVectorDim& p2);
int read_sample(TString& s, TVector& x, float& y);
int write_sample(TString& s, TVector& x, float& y);
//ifstream read the files
int batch_read_sample(ifstream& is, TVectorArray& x_array, TFloatArray& y_array, int& n);
//ofstream
int batch_write_sample(ofstream& os, TVectorArray& x_array, TFloatArray& y_array, int& n);
void print_vector(const TVector& v);
float dot_product(const TVector& v1, const TVector& v2);
TVector operator*(const TVector& v, float f);
TVector operator*(float f, const TVector& v);
TVector operator+(const TVector& v1, const TVector& v2);

#endif /* SVM_COMMON_H */

