/**
 * @file return.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef return_cpp
#define return_cpp
#include <iostream>

void rt(float return_value){
	rtv[3] = rtv[2];
	rtv[2] = rtv[1];
	rtv[1] = rtv[0];
	rtv[0] = return_value;
	if (debug_all)
		std::cout<<std::fixed<<"  rt0: "<<rtv[0]<<std::endl<<"  rt1: "<<rtv[1]<<std::endl<<"  rt2: "<<rtv[2]<<std::endl<<"  rt3: "<<rtv[3]<<std::endl;
}

#endif
