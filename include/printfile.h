//
// Created by 张奥林 on 2023/8/25.
//
#pragma once
#ifndef DSEA2_PRINTFILE_H
#define DSEA2_PRINTFILE_H
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include "parser.h"
#define ERROR 0
#define OK 1
using namespace std;
typedef int status;
status PrintFile(FILE* fp);
status PrintFile(ifstream & fp);
#endif //DSEA2_PRINTFILE_H
