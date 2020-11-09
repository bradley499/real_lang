/**
 * @file function.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef function_h
#define function_h
#include "LinkedList.h"
#include "segment_insight.h"
#include "callstack.h"
#include "variable.h"
#include "return.h"
#include "operation.h"

class function
{
private:
	const unsigned int id;
	const unsigned int parameters;
	LinkedList<operation*> operations;
public:
	/**
	 * @brief Construct a new function object
	 * 
	 * @param set_id the ID of the function
	 * @param set_total_parameters the total amount of parameters that the function will support
	 */
	function(unsigned int set_id, unsigned int set_total_parameters);

	/**
	 * @brief Destroy the function object
	 * 
	 */
	~function();

	/**
	 * @brief Get the ID of the function
	 * 
	 * @return the function ID
	 */
	unsigned int get_id();

	/**
	 * @brief Add a new operation to the function
	 * 
	 * @param operation_call_id the operation type ID
	 */
	void add_operations(const unsigned int operation_call_id);

	/**
	 * @brief add a callstack to the latest operation
	 * 
	 * @param set_callstack_standard if is part of the standard values or not
	 * @param set_callstack_value the value to store
	 */
	void add_operation_callstack(const bool set_callstack_standard, const float set_callstack_value);

	/**
	 * @brief Execute the function
	 * 
	 * @param execution_version the version which the package supports
	 * @param parameter_values the parameter values feed to the function
	 * @param functions the list of all other functions
	 * @param variables the list of all variables
	 */
	void execute(unsigned char &execution_version, LinkedList<const float*> &parameter_values, LinkedList<function*> &functions, LinkedList<variable*> &variables);

	/**
	 * @brief Execute teh function (without any parameters)
	 * 
	 * @param execution_version the version which the package supports
	 * @param functions the list of all other functions
	 * @param variables the list of all variables
	 */
	void execute(unsigned char &execution_version, LinkedList<function*> *functions, LinkedList<variable*> *variables);

	/**
	 * @brief Verify if the function is valid
	 * 
	 * @param total_functions the total amount of functions
	 * @param execution_version the version which the package supports
	 * @return if the function is valid `true` is returned, otherwise `false` 
	 */
	bool verify(unsigned int &total_functions, unsigned char &execution_version);
};

#endif
