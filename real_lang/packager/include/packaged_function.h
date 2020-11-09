/**
 * @file packaged_function.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef packaged_function_h
#define packaged_function_h
#include "packaged_function.h"

class packaged_function
{
private:
	unsigned long long int function_id;
	std::vector<const char*> default_parameters;
	std::vector<std::vector<const char*>> operations;
	std::vector<unsigned long long int> line_ref;
	bool update_line_ref = false;
	unsigned int list_params_iter = 0;
	unsigned int list_operations_iter = 0;

	/**
	 * @brief Converts an integer to a char array
	 * 
	 * @param integer_in the integer to be converted 
	 * @param decimal_delimiter char array value of decimal delimiter
	 * @param negative_delimiter char array value of negative delimiter
	 * @return the char array of the converted integer
	 */
	const char* int_to_char (float integer_in, const char* decimal_delimiter, const char* negative_delimiter);
public:
	/**
	 * @brief Construct a new packaged function object
	 * 
	 * @param set_function_id the generated ID of the function
	 * @param set_default_parameters a vector of default parameters for the function
	 */
	packaged_function(unsigned long long int set_function_id, std::vector<const char*> &set_default_parameters);

	/**
	 * @brief Adds a new operation to the function
	 * 
	 * @param set_operation a list of operations for per line
	 * @param glbl_variables a list of all variables defined in the input
	 * @param function_names a list of afunctions defined in the input
	 * @param line_ref the current line of which is being parsed
	 * @return the line of error (0 if no errors)
	 */
	unsigned long int new_operation(std::vector<const char*> &set_operation, std::vector<const char*> &glbl_variables, std::vector<const char*> &function_names, unsigned long long line_ref);

	/**
	 * @brief generates the string of the current function as a packaged output from the delimiters provided
	 * 
	 * @param segment_function function delimiter
	 * @param segment_parameter parameter delimiter
	 * @param segment_operation operation delimiter
	 * @param segment_operation_sub operation sub delimiter
	 * @param segment_real_value real value delimiter
	 * @param segment_decimal decimal delimiter
	 * @param segment_negative negative delimiter
	 * @return the packaged build structure
	 */
	build_structure* build(const char *segment_function, const char* segment_parameter, const char* segment_operation, const char* segment_operation_sub, const char* segment_real_value, const char* segment_decimal, const char* segment_negative);
};

#endif