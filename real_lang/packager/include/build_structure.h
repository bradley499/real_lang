/**
 * @file build_structure.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef build_structure_h
#define build_structure_h
#include <iostream>

class build_structure
{
private:
	const unsigned long long int functions = 1;
	const unsigned long long int parameters = 1;
	const unsigned long long int operations = 0;
	const unsigned long long int operation_subs = 0;
	const unsigned long long int real_values = 0;
	const unsigned long long int decimals = 0;
	const unsigned long long int negatives = 0;
	std::string build_string;
public:
	/**
	 * @brief Construct a new build structure object
	 * 
	 * @param set_operations the total amount of operations for the build string
	 * @param set_operation_subs the total amount of operation subs for the build string
	 * @param set_real_values the total amount of real values for the build string
	 * @param set_decimals the total amount of decimals for the build string
	 * @param set_negatives the total amount of negatives for the build string
	 * @param set_build_string the build string
	 */
	build_structure(unsigned long long int set_operations, unsigned long long int set_operation_subs, unsigned long long int set_real_values, unsigned long long int set_decimals, unsigned long long int set_negatives, const char* set_build_string);
	
	/**
	 * @brief Gets the amount of functions
	 * 
	 * @return total amount of functions
	 */
	unsigned long long int get_functions();
	
	/**
	 * @brief Gets the amount of parameters
	 * 
	 * @return total amount of parameters
	 */
	unsigned long long int get_parameters();
	
	/**
	 * @brief Gets the amount of operations
	 * 
	 * @return total amount of operations
	 */
	unsigned long long int get_operations();
	
	/**
	 * @brief Gets the amount of operation subs
	 * 
	 * @return total operation amount of subs
	 */
	unsigned long long int get_operation_subs();
	
	/**
	 * @brief Gets the amount of real values
	 * 
	 * @return total real amount of values
	 */
	unsigned long long int get_real_values();
	
	/**
	 * @brief Gets the amount of decimals
	 * 
	 * @return total amount of decimals
	 */
	unsigned long long int get_decimals();
	
	/**
	 * @brief Gets the amount of negatives
	 * 
	 * @return total amount of negatives
	 */
	unsigned long long int get_negatives();
	
	/**
	 * @brief Gets the build string
	 * 
	 * @return the build string
	 */
	const char* get_build_string();
};

#endif
