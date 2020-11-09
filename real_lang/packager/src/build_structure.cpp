/**
 * @file build_structure.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef build_structure_cpp
#define build_structure_cpp 
#include "build_structure.h"

build_structure::build_structure(unsigned long long int set_operations, unsigned long long int set_operation_subs, unsigned long long int set_real_values, unsigned long long int set_decimals, unsigned long long int set_negatives, const char* set_build_string) : operations(set_operations), operation_subs(set_operation_subs), real_values(set_real_values), decimals(set_decimals), negatives(set_negatives), build_string(set_build_string)
{};
unsigned long long int build_structure::get_functions()
{
	return functions;
}
unsigned long long int build_structure::get_parameters()
{
	return parameters;
}
unsigned long long int build_structure::get_operations()
{
	return operations;
}
unsigned long long int build_structure::get_operation_subs()
{
	return operation_subs;
}
unsigned long long int build_structure::get_real_values()
{
	return real_values;
}
unsigned long long int build_structure::get_decimals()
{
	return decimals;
}
unsigned long long int build_structure::get_negatives()
{
	return negatives;
}
const char* build_structure::get_build_string()
{
	return build_string.c_str();
}

#endif
