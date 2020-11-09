/**
 * @file callstack.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef callstack_cpp
#define callstack_cpp
#include "callstack.h"

callstack::callstack(bool set_standard, float set_value) : standard(set_standard), value(new float(set_value))
{};

callstack::~callstack()
{
	delete value;
}

float* get_callstack_value(callstack &callstack_value, unsigned char &execution_version)
{
	if (callstack_value.standard)
	{
		if (execution_version == 0)
		{
			switch ((int)*callstack_value.value)
			{
				case 0: case 1:
				{
					return &rtv[0];
				}
				case 2:
				{
					return &rtv[1];
				}
				case 3:
				{
					return &rtv[2];
				}
				case 4:
				{
					return &rtv[3];
				}
				case 5:
				{
					return new float(3.1415927410);
				}
				case 6:
				{
					return new float(57.295779513);
				}
				case 7:
				{
					return new float(1.0);
				}
				case 8: case 9: default:
				{
					return new float(0.0);
				}
			}
		}
		else
			return 0;
	}
	else
		return (float*) callstack_value.value;
};

#endif
