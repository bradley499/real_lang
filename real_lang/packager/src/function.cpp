/**
 * @file function.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef function_cpp
#define function_cpp
#include "function.h"

function::function(const char* set_name, unsigned long long line) : function_name(set_name){
	line_ref.push_back(line);
};
void function::add_parameter(const char* parameter_name)
{
	params.push_back(dereference(parameter_name));
}
void function::new_operation()
{
	update_line_ref = true;
	if (operations.size() == 0)
		operations.push_back(std::vector<const char*>());
	else if (operations.at(operations.size() - 1).size() != 0)
		operations.push_back(std::vector<const char*>());
}
void function::add_to_operation(const char* operation_segment, unsigned long long line){
	if (update_line_ref) {
		line_ref.push_back(line);
		update_line_ref = false;
	}
	operations.at(operations.size() - 1).push_back(dereference(operation_segment));
}
const char* function::name()
{
	return function_name;
}
std::vector<const char*> function::list_operations()
{
	if (operations.size() == 0 || operations.size() == (list_operations_iter + 1))
	{
		list_operations_iter = 0;
		return std::vector<const char*>();
	}
	else
		return operations.at(list_operations_iter++);
}
void function::reset_operations_iter()
{
	list_operations_iter = 0;
}
const char* function::list_params()
{
	if (params.size() == 0 || params.size() == list_params_iter)
	{
		list_params_iter = 0;
		return nullptr;
	}
	else
		return dereference(params.at(list_params_iter++));
}
void function::reset_params_iter()
{
	list_params_iter = 0;
}
unsigned long long int function::get_line_ref()
{
	if (list_operations_iter == 0)
		return 0;
	return line_ref.at((list_operations_iter - 1));
}
bool function::validate_parameter(const char* parameter_name)
{
	for (int i = 0; i < params.size(); ++i)
		if (strcmp(parameter_name,params[i]) == 0)
			return false;
	return true;
}

#endif
