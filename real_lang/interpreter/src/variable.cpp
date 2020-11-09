/**
 * @file variable.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef variable_cpp
#define variable_cpp
#include "variable.h"
#include "return.cpp"

variable::variable(const unsigned int set_id) : variable_id(set_id)
{};
unsigned int variable::id()
{
	return variable_id;
}
void variable::set(const float &set_value)
{
	value = set_value;
};
float variable::get()
{
	return value;
};

void get_variable_value(LinkedList<variable*> &variables, unsigned int id)
{
	for (int i = 0; i < variables.getLength(); ++i)
		if (variables.At(id)->id() == id)
			return rt(variables.At(id)->get());
	rt(0);
};

#endif
