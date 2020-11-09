/**
 * @file operation.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#ifndef operation_cpp
#define operation_cpp
#include "LinkedList.h"
#include "operation.h"
#include "callstack.h"
#include "function.h"

operation::operation(const unsigned int set_id) : id(set_id)
{
	callstacks = new LinkedList<callstack*>();
};
operation::~operation()
{
	callstacks->Clear();
	delete[] callstacks;
}
void operation::add_callstack(callstack* new_callstack)
{
	callstacks->Append(new_callstack);
};
unsigned int operation::get_id()
{
	return id;
};
LinkedList<callstack*>& operation::get_callstacks()
{
	return *callstacks;
}

#endif