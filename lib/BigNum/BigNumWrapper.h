#pragma once

#include <iostream>

class bnw_int_t {

};

class bnw_uint_t {

};

class bnw_float_t {

};

std::ostream& operator<<(std::ostream& os, const big_int_t& n);
std::ostream& operator<<(std::ostream& os, const big_float_t& n);