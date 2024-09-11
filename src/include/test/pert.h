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
#include "../depends/rapidxml-1.13/rapidxml.hpp"

using namespace pert;
using namespace rapidxml;

template class activity_t<int>;
template class network_t<int, int>;

using network = network_t<int, int>;

// Test functions
int test_basic(const network&);
int test_from_dummy();
int test_from_txt(const char*);
int test_interactive(const char*);