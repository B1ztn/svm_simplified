/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   svm_option..h
 * Author: b1
 *
 * Created on January 27, 2018, 2:07 PM
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
//actually why those three packages????
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "svm_common.h"


#ifndef SVM_OPTION__H
#define SVM_OPTION__H

#define FLAG_TRAIN  (1<<0)
#define FLAG_MODEL  (1<<1)
#define FLAG_VALID  (1<<2)
#define FLAG_LINEAR_KERNEL (1<<3)
#define FLAG_C             (1<<4)
#define FLAG_EPSILON       (1<<5)
#define FLAG_SIGMA         (1<<6)
#define FLAG_HELP         (1<<7)

#define SVM_OPTION_C        1.0
// here is the self set section , we can adjust it by matching our projects...
#define SVM_OPTION_EPSILON  0.001
#define SVM_OPTION_SIGMA    4.0

class SVMOption{
public:
    SVMOption();
    ~SVMOption();
    int parse_command_line(int argc, char *argv[]);
    void print();
public:
    float _C;
    float _eps;
    float _sig;
    bool _is_linear_kernel;
    char _fname_train[MAX_PATH_LENGTH];
    char _fname_valid[MAX_PATH_LENGTH];
    char _fname_model[MAX_PATH_LENGTH];
    
protected:
    int _flags;
};

#endif /* SVM_OPTION__H */

