/**
 * @file callstack.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef callstack_h
#define callstack_h

struct callstack
{
	const bool standard = false;
	const float *value = new float;
	/**
	 * @brief Construct a new callstack object
	 * 
	 * @param set_standard if is part of the standard values or not
	 * @param set_value the value to store
	 */
	callstack(bool set_standard, float set_value);

	/**
	 * @brief Destroy the callstack object
	 * 
	 */
	~callstack();
};

/**
 * @brief Get the value of the callstack object
 * 
 * @param callstack_value the callstack to get the value of
 * @param execution_version the version of the current package's execution
 * @return the value of the execution 
 */
float* get_callstack_value(callstack &callstack_value, unsigned char &execution_version);

#endif
