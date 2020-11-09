/**
 * @file segment_insight.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef segment_insight_cpp
#define segment_insight_cpp
#include "segment_insight.h"

segment_insight::segment_insight(const unsigned long long int set_read_position, const unsigned short int set_segment_type) : read_position(set_read_position), segment_type(set_segment_type)
{
}
unsigned long long int segment_insight::get_start_position()
{
	return read_position;
}
unsigned short int segment_insight::get_type()
{
	return segment_type;
}

#endif
