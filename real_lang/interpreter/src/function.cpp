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
#include "LinkedList.h"
#include "function.h"
#include "segment_insight.h"
#include "callstack.h"
#include "variable.h"
#include "return.h"
#include "operation.h"

function::function(unsigned int set_id, unsigned int set_total_parameters) : id(set_id), parameters(set_total_parameters)
{
	operations = LinkedList<operation*>();
};
function::~function()
{
	operations.Clear();
}
unsigned int function::get_id()
{
	return id;
}
void function::add_operations(const unsigned int operation_call_id)
{
	operations.Append(new operation(operation_call_id));
}
void function::add_operation_callstack(const bool set_callstack_standard, const float set_callstack_value)
{
	operations.Last()->add_callstack(new callstack(set_callstack_standard,set_callstack_value));
}
void function::execute(unsigned char &execution_version, LinkedList<const float*> &parameter_values, LinkedList<function*> &functions, LinkedList<variable*> &variables) {
	while (parameter_values.getLength() > parameters)
		parameter_values.DeleteLast();
	unsigned char *logic_gate = new unsigned char(0);
	operations.moveToStart();
	unsigned long long int function_read_position = 0;
	LinkedList<unsigned long long int> loop_bases = LinkedList<unsigned long long int>();
	bool end_loop = false;
	do
	{
		LinkedList<callstack*> operation_callstack = operations.getCurrent()->get_callstacks();
		if (execution_version == 0)
		{
			LinkedList<const float*> *argument_parameters = new LinkedList<const float*>();
			if (operations.getCurrent()->get_id() == 15)
				argument_parameters->Append(operation_callstack.At(0)->value);
			else
				for (int i = (operations.getCurrent()->get_id() == 0); i < operation_callstack.getLength(); i++)
					argument_parameters->Append(get_callstack_value(*operation_callstack.At(i),execution_version));
			if (end_loop)
			{
				if(operations.getCurrent()->get_id() != 26)
				{
					function_read_position++;
					continue;
				}
			}
			switch (operations.getCurrent()->get_id())
			{
				case 0:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<"Calling new function"<<std::endl;
						recursion_level++;
						if (recursion_level == 4999)
						{
							recursion_level = 5000;
							return;
						}
						functions.At(*operation_callstack.At(0)->value)->execute(execution_version,*argument_parameters,functions,variables);
						if (recursion_level == 5000)
							return;
						recursion_level--;
						operations.At(function_read_position);
					}
					break;
				}
				case 1:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Adding:"<<*(argument_parameters)->At(0)<<" to "<<*(argument_parameters)->At(1)<<std::endl;
						rt(*(argument_parameters)->At(0) + *(argument_parameters)->At(1));
					}
					break;
				}
				case 2:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Subtracting:"<<*(argument_parameters)->At(1)<<" from "<<*(argument_parameters)->At(0)<<std::endl;
						rt(*(argument_parameters)->At(0) - *(argument_parameters)->At(1));
					}
					break;
				}
				case 3:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Multiplying: "<<*(argument_parameters)->At(0)<<" by "<<*(argument_parameters)->At(1)<<std::endl;
						rt(*(argument_parameters)->At(0) * *(argument_parameters)->At(1));
					}
					break;
				}
				case 4:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Dividing: "<<*(argument_parameters)->At(0)<<" by "<<*(argument_parameters)->At(1)<<std::endl;
						if (*(argument_parameters)->At(0) == 0 or *(argument_parameters)->At(1) == 0)
							rt(0);
						else
							rt(*(argument_parameters)->At(0) / *(argument_parameters)->At(1));
					}
					break;
				}
				case 5:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Incrementing: "<<*(argument_parameters)->At(0)<<" by 1"<<std::endl;
						rt(*(argument_parameters)->At(0) + 1);
					}
					break;
				}
				case 6:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Reducting: "<<*(argument_parameters)->At(0)<<" by 1"<<std::endl;
						rt(*(argument_parameters)->At(0) - 1);
					}
					break;
				}
				case 7:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Square root of: "<<*(argument_parameters)->At(0)<<std::endl;
						rt(sqrt(*(argument_parameters)->At(0)));
					}
					break;
				}
				case 8:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Sine of "<<*(argument_parameters)->At(0)<<std::endl;
						rt(sin(*(argument_parameters)->At(0)));
					}
					break;
				}
				case 9:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Cosine of "<<*(argument_parameters)->At(0)<<std::endl;
						rt(cos(*(argument_parameters)->At(0)));
					}
					break;
				}
				case 10:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Tangent of: "<<*(argument_parameters)->At(0)<<std::endl;
						rt(tan(*(argument_parameters)->At(0)));
					}
					break;
				}
				case 11:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Rounding down: "<<*(argument_parameters)->At(0)<<std::endl;
						rt(floor(*(argument_parameters)->At(0)));
					}
					break;
				}
				case 12:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Rounding up: "<<*(argument_parameters)->At(0)<<std::endl;
						rt(ceil(*(argument_parameters)->At(0)));
					}
					break;
				}
				case 13:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Rounding: "<<*(argument_parameters)->At(0)<<std::endl;
						rt(round(*(argument_parameters)->At(0)));
					}
					break;
				}
				case 14:
				{
					if (*logic_gate != 2)
					{
						if (argument_parameters->getLength() == 1)
						{
							if (debug)
								std::cout<<"Getting variable value..."<<std::endl;
							get_variable_value(variables,*(argument_parameters)->At(0));
						}
						else
						{
							if (debug)
								std::cout<<std::fixed<<"Setting variable value to: "<<*(*argument_parameters).At(1)<<std::endl;
							const unsigned int variable_id = *(argument_parameters)->At(0);
							bool new_variable = true;
							if (variables.getLength() > 0)
							{
								variables.moveToStart();
								do
								{
									if (variables.getCurrent()->id() == variable_id)
									{
										new_variable = false;
										variables.getCurrent()->set(*(*argument_parameters).At(1));
										break;
									}
								} while (variables.next());
							}
							if (new_variable)
							{
								variable *new_variable_value = new variable(variable_id);
								new_variable_value->set(*(*argument_parameters).At(1));
								variables.Append(new_variable_value);
							}
						}
					}
					break;
				}
				case 15:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Getting parameter value..."<<std::endl;
						if (*(argument_parameters)->At(0) >= parameter_values.getLength())
							rt(0);
						else
							rt(*parameter_values.At(*(argument_parameters)->At(0)));
					}
					break;
				}
				case 16:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Returning: "<<*(argument_parameters)->At(0)<<std::endl;
						rt(*(argument_parameters)->At(0));
						delete argument_parameters;
						return;
					}
					break;
				}
				case 17:
				{
					if (*logic_gate != 1)
					{
						delete logic_gate;
						if (*(argument_parameters)->At(0) == *(argument_parameters)->At(1))
							logic_gate = new unsigned char(1);
						else
							logic_gate = new unsigned char(2);
					}
					else
					{
						delete logic_gate;
						logic_gate = new unsigned char(2);
					}
					break;
				}
				case 18:
				{
					if (*logic_gate != 1)
					{
						delete logic_gate;
						if (*(argument_parameters)->At(0) != *(argument_parameters)->At(1))
							logic_gate = new unsigned char(1);
						else
							logic_gate = new unsigned char(2);
					}
					else
					{
						delete logic_gate;
						logic_gate = new unsigned char(2);
					}
					break;
				}
				case 19:
				{
					if (*logic_gate != 1)
					{
						delete logic_gate;
						if (*(argument_parameters)->At(0) < *(argument_parameters)->At(1))
							logic_gate = new unsigned char(1);
						else
							logic_gate = new unsigned char(2);
					}
					else
					{
						delete logic_gate;
						logic_gate = new unsigned char(2);
					}
					break;
				}
				case 20:
				{
					if (*logic_gate != 1)
					{
						delete logic_gate;
						if (*(argument_parameters)->At(0) > *(argument_parameters)->At(1))
							logic_gate = new unsigned char(1);
						else
							logic_gate = new unsigned char(2);
					}
					else
					{
						delete logic_gate;
						logic_gate = new unsigned char(2);
					}
					break;
				}
				case 21:
				{
					if (*logic_gate != 1)
					{
						delete logic_gate;
						if (*(argument_parameters)->At(0) <= *(argument_parameters)->At(1))
							logic_gate = new unsigned char(1);
						else
							logic_gate = new unsigned char(2);
					}
					else
					{
						delete logic_gate;
						logic_gate = new unsigned char(2);
					}
					break;
				}
				case 22:
				{
					if (*logic_gate != 1)
					{
						delete logic_gate;
						if (*(argument_parameters)->At(0) >= *(argument_parameters)->At(1))
							logic_gate = new unsigned char(1);
						else
							logic_gate = new unsigned char(2);
					}
					else
					{
						delete logic_gate;
						logic_gate = new unsigned char(2);
					}
					break;
				}
				case 23:
				{
					if (*logic_gate != 0)
					{
						delete logic_gate;
						logic_gate = new unsigned char(1);
					}
					else
					{
						delete logic_gate;
						logic_gate = new unsigned char(2);
					}
					break;
				}
				case 24:
				{
					delete logic_gate;
					logic_gate = new unsigned char(0);
					break;
				}
				case 25:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<"Starting loop..."<<std::endl;
						loop_bases.Append(function_read_position);
					}
					break;
				}
				case 26:
				{
					if (*logic_gate != 2)
					{
						if (!end_loop)
						{
							if (debug)
								std::cout<<"Restarting loop..."<<std::endl;
							function_read_position = loop_bases.Last();
							operations.At(function_read_position);
						}
						end_loop = false;
						break;
					}
				}
				case 27:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Breaking if: "<<*(argument_parameters)->At(0)<<" is equal to "<<*(argument_parameters)->At(1)<<std::endl;
						if (*(argument_parameters)->At(0) == *(argument_parameters)->At(1))
						{
							if (debug)
								std::cout<<"Exiting loop..."<<std::endl;
							end_loop = true;
							loop_bases.DeleteLast();
						}
					}
					break;
				}
				case 28:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<"Generating random value..."<<std::endl;
						rt(rand());
					}
					break;
				}
				case 29:
				{
					if (*logic_gate != 2)
					{
						if (debug)
							std::cout<<std::fixed<<"Setting: "<<*(argument_parameters)->At(0)<<std::endl;
						rt(*(argument_parameters)->At(0));
					}
					break;
				}
				default:
				{
					break;
				}
			}
			delete argument_parameters;
		}
		function_read_position++;
	} while (operations.next());
	delete logic_gate;

}
void function::execute(unsigned char &execution_version, LinkedList<function*> *functions, LinkedList<variable*> *variables)
{
	LinkedList<const float*> *no_parameters = new LinkedList<const float*>();
	execute(execution_version,*no_parameters,*functions,*variables);
	delete no_parameters;
}
bool function::verify(unsigned int &total_functions, unsigned char &execution_version)
{
	bool logic_gates = false;
	signed long int loop_level = 0;
	operations.moveToStart();
	do {
		LinkedList<callstack*> operation_callstack = operations.getCurrent()->get_callstacks();
		switch (execution_version)
		{
			case 0:
			{
				switch (operations.getCurrent()->get_id())
				{
					case 0:
					{
						if (operation_callstack.getLength() >= 1)
						{
							if (*operation_callstack.At(0)->value >= total_functions)
								return false;
						}
						else
							return false;
						break;
					}
					case 1: case 2: case 3: case 4:
					{
						if (operation_callstack.getLength() != 2)
							return false;
						break;
					}
					case 17: case 18: case 19: case 20: case 21: case 22:
					{
						if (operation_callstack.getLength() != 2)
							return false;
						logic_gates = true;
						break;
					}
					case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 16: case 29:
					{
						if (operation_callstack.getLength() != 1)
							return false;
						break;
					}
					case 14:
					{
						if (operation_callstack.getLength() > 2)
							return false;
						if (operation_callstack.getLength() == 0)
							return false;
						break;
					}
					case 15:
					{
						if (operation_callstack.getLength() != 1)
							return false;
						if (*operation_callstack.At(0)->value >= parameters)
							return false;
						break;
					}
					case 23:
					{
						if (!logic_gates)
							return false;
						if (operation_callstack.getLength() != 0)
							return false;
						break;
					}
					case 24:
					{
						if (!logic_gates)
							return false;
						if (operation_callstack.getLength() != 0)
							return false;
						logic_gates = false;
						break;
					}
					case 25:
					{
						loop_level++;
						break;
					}
					case 26:
					{
						if (loop_level == 0)
							return false;
						loop_level--;
						break;
					}
					case 27:
					{
						if (operation_callstack.getLength() != 2)
							return false;
						break;
					}
					case 28:
					{
						if (operation_callstack.getLength() != 0)
							return false;
						break;
					}
					default:
					{
						return false;
					}
				}
				for (int i = 0; i < operation_callstack.getLength(); ++i)
					if (operation_callstack.At(i)->standard)
						if (*operation_callstack.At(i)->value < 0 && *operation_callstack.At(i)->value >= 10)
							return false;
				break;
			}
			default:
			{
				return false;
			}
		}
	} while (operations.next());
	return (logic_gates == false && loop_level == 0);
}

#endif
