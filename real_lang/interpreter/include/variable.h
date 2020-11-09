/**
 * @file variable.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef variable_h
#define variable_h
#include "LinkedList.h"
#include "return.h"

class variable
{
private:
	const unsigned int variable_id;
	float value;
public:
	/**
	 * @brief Construct a new variable object
	 * 
	 * @param set_id the ID of the variable
	 */
	variable(const unsigned int set_id);

	/**
	 * @brief get the ID of the variable
	 * 
	 * @return variable ID 
	 */
	unsigned int id();

	/**
	 * @brief Set the value of the variable
	 * 
	 * @param set_value the value to be set
	 */
	void set(const float &set_value);

	/**
	 * @brief Get the value of the variable
	 * 
	 * @return the value of the variable
	 */
	float get();
};

/**
 * @brief Adds the value of the variable to the return registers
 * 
 * @param variables the list of all variables
 * @param id the ID of the variable to be returned
 */
void get_variable_value(LinkedList<variable*> &variables, unsigned int id);

#endif
