/**
 * @file operation.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef operation_h
#define operation_h
#include "LinkedList.h"
#include "callstack.h"
#include "function.h"

class operation
{
private:
	const unsigned int id;
	LinkedList<callstack*> *callstacks;
public:
	/**
	 * @brief Construct a new operation object
	 * 
	 * @param set_id the ID of the operation
	 */
	operation(const unsigned int set_id);

	/**
	 * @brief Destroy the operation object
	 * 
	 */
	~operation();

	/**
	 * @brief Add a callstack to the operation
	 * 
	 * @param new_callstack callstack to be appended
	 */
	void add_callstack(callstack* new_callstack);

	/**
	 * @brief Get the ID of the callstack
	 * 
	 * @return the ID of the callstack
	 */
	unsigned int get_id();

	/**
	 * @brief Get the callstacks object
	 * 
	 * @return the list of the callstacks 
	 */
	LinkedList<callstack*>& get_callstacks();
};

#endif