/***
 * @brief
 * @author Johann Fotsing
 * @date 2024-09-11
 * @file pert.h
 */

#include <iostream>
#include <pert.h>
#include <fstream>
#include <sstream>
#include <streambuf>

using namespace pert;

template class network<int, int>;
using Network = network<int, int>;
//template bool operator<(const network<int, int>::activity&, const network<int, int>::activity&);


// Test functions
int test_basic(const Network&);
int test_from_dummy();
int test_from_txt(const char*);
int test_interactive(const char*);