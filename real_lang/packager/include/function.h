/**
 * @file function.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#ifndef function_h
#define function_h

class function
{
private:
	const char* function_name;
	std::vector<const char*> params;
	std::vector<std::vector<const char*>> operations;
	std::vector<unsigned long long int> line_ref;
	bool update_line_ref = false;
	unsigned int list_params_iter = 0;
	unsigned int list_operations_iter = 0;
public:
	/**
	 * @brief Construct a new function object
	 * 
	 * @param set_name the name of the function
	 * @param line the line number of the function being parsed
	 */
	function(const char* set_name, unsigned long long line);

	/**
	 * @brief Adds a paramter to be provided with the function
	 * 
	 * @param parameter_name the name of the parameter for the function
	 */
	void add_parameter(const char* parameter_name);

	/**
	 * @brief Moves onto/creates a new operation stack for the function
	 * 
	 */
	void new_operation();

	/**
	 * @brief Adds new operation segment to the current operation
	 * 
	 * @param operation_segment the current operation to be added 
	 * @param line the line number of the operation segment being parsed
	 */
	void add_to_operation(const char* operation_segment, unsigned long long line);

	/**
	 * @brief Returns the name of the function
	 * 
	 * @return function name
	 */
	const char* name();

	/**
	 * @brief Returns the vector of operations
	 * 
	 * @return operation vector
	 */
	std::vector<const char*> list_operations();

	/**
	 * @brief Resets operation iteration count to 0
	 * 
	 */
	void reset_operations_iter();

	/**
	 * @brief Returns the current parameter
	 * 
	 * @return current parameter
	 */
	const char* list_params();

	/**
	 * @brief Resets parameter iteration count to 0
	 * 
	 */
	void reset_params_iter();

	/**
	 * @brief Returns the current line reference of the function
	 * 
	 * @return function line reference
	 */
	unsigned long long int get_line_ref();

	/**
	 * @brief Checks if the parameter requested if already defined
	 * 
	 * @param parameter_name value to check
	 * @return true if `parameter_name` does not exists
	 */
	bool validate_parameter(const char* parameter_name);
};

#endif
