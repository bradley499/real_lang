/**
 * @file main.cpp
 * @author Bradley Marshall (bradley499) 
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#define VERSION_NUM "0.01"
#define RELEASE_NUM "0"
#define colorize_output false

#include <fstream>
#include <ctype.h>
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include "externs.h"
#include "misc.cpp"
#include "file_manager.cpp"
#include "build_structure.cpp"
#include "packaged_function.cpp"
#include "function.cpp"

bool is_verbose = false;

/**
 * @brief Generates the packaged real file
 * 
 * @param verbose if the output should be verbose
 * @param input_file_path the input file path
 * @param output_file_path the output file path
 * @param raw if a raw output is required
 * @return success of packaging
 */
unsigned char generate(bool verbose, char * input_file_path, char * output_file_path, bool raw)
{
	// Verifying the validity of the provided parameters, and existence of files/directories
	if (get_char_size(input_file_path) == 0)
		return error("A source file needs to be provided!",-1,false);
	if (get_char_size(output_file_path) == 0 && !raw)
		return error("An output location needs to be defined!",-1,false);
	struct stat buffer;
	if (stat (input_file_path, &buffer) != 0)
		return error("The defined input file does not exists!",-1,false);
	else
		if(!(buffer.st_mode & S_IFREG))
			return error("The defined input destination does not resolve to a file!",-1,false);
	if (stat (output_file_path, &buffer) == 0)
		if(buffer.st_mode & S_IFDIR)
			return error("The defined output destination already resolves to a directory!",-1,false);
	
	file_manager *file_operations = new file_manager();
	unsigned int input_file = file_operations->open(input_file_path,std::ofstream::out,false,false);
	if (verbose)
		std::cout<<"\u001b[1m\u001b[4mPARSING:\u001b[0m";
	if (verbose)
		std::cout<<std::endl<<"Generating program structure...";

	std::vector<function*> function_operations;

	unsigned long int position = 0;
	unsigned short int function_position = 0;
	unsigned long int program_length = file_operations->get_file_size(input_file)+1;
	long int line_length = 0;
	long int line_init[2] = {0,0};
	unsigned long long int line_iteration = 0;
	bool is_function[2] = {false,false};
	bool is_end = false;
	// Reading of the provided file
	while (position <= program_length)
	{
		char character;
		if (line_init[0] != 0)
		{
			character = file_operations->get_next_char(input_file);
			position++;
		}
		char character_buffer[(line_length + 1)];
		const unsigned long int line_length_const = line_length + 1;
		switch (line_init[0])
		{
			case 0:
				line_init[0] = 1;
				line_init[1] = position - 1;
				if (is_end)
				{
					line_init[1]++;
					line_length++;
				}
				if (line_init[1] < 0)
				{
					line_init[1] = 0;
				}
				line_length = 0;
				break;
			case 1:
			{
				if (character != '\n' && position < program_length)
					line_length++;
				else if (!(character == '\n' && position == program_length))
				{
					line_length++;
					line_iteration++;
					line_init[0] = 2;
				}
				else
					line_init[0] = 2;
				is_end = position == program_length;
				break;
			}
			case 2:
			{
				line_init[0] = 0;
				if (line_length > 0)
				{
					long long int position_insert_relative = 0;
					file_operations->go_to_pos(input_file,line_init[1]);
					while (line_length >= 0)
					{
						char character_relative = file_operations->get_next_char(input_file);
						if (character_relative != '\n')
							character_buffer[position_insert_relative++] = character_relative;
						line_length--;
					}
					character_buffer[position_insert_relative] = '\0';
					char character_buffer_primer[position_insert_relative];
					int character_buffer_primer_position = 0;
					std::vector<char*> function_call_stack;
					int line_length_verification = 0;
					for (int i = 0; i <= line_length_const; ++i)
					{
						line_length_verification++;
						if (character_buffer[i] == ':')
						{
							if (is_function[1])
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								return error("Unexpected \":\" on line ",line_iteration,verbose);
							}
							else
							{
								is_function[0] = true;
								is_function[1] = true;
								bool function_valid = true;
								character_buffer_primer[character_buffer_primer_position] = '\0';
								for (int ii = 0; ii < syntax_total; ++ii)
								{
									if (strcmp(syntax[ii],character_buffer_primer) == 0)
									{
										char error_message[] = "Cannot use \"";
										strcat(error_message,syntax[ii]);
										strcat(error_message,"\" as a function as it is part of the standard syntax. Change function name on line \0");
										if (verbose)
											std::cout<<operation_conclusion(" failed");
										return error(error_message,line_iteration,verbose);
									}
								}
								for (int ii = 0; ii < function_operations.size(); ++ii)
								{
									if (strcmp(function_operations.at(ii)->name(),character_buffer_primer) == 0)
									{
										function_valid = false;
										break;
									}
								}
								if (function_valid)
								{
									function_position++;
									char *character_buffer_full = new char[i+1];
									strcpy(character_buffer_full,character_buffer_primer);
									character_buffer_full[i] = '\0';
									function_operations.push_back(new function(character_buffer_full, line_iteration));
								}
								else
								{
									char error_message[] = "Redeclaration of the function \"";
									strcat(error_message,character_buffer_primer);
									strcat(error_message,"\" on line \0");
									if (verbose)
										std::cout<<operation_conclusion(" failed");
									return error(error_message,line_iteration,verbose);
								}
							}
							memset(character_buffer_primer, 0, sizeof(character_buffer_primer));
							character_buffer_primer_position = 0;
						}
						else if (character_buffer[i] == ' ' or character_buffer[i] == '\t' or character_buffer[i] == ';' or i == line_length_const)
						{
							if (line_length_verification == 1)
								line_length_verification = 0;
							else if (strlen(character_buffer_primer) != 0)
							{
								character_buffer_primer[character_buffer_primer_position] = '\0';
								if (is_function[1])
								{
									if (function_operations.at((function_position - 1))->validate_parameter(character_buffer_primer))
										function_operations.at((function_position - 1))->add_parameter(dereference(character_buffer_primer));
									else
									{
										char error_message[] = "The parameter \"";
										strcat(error_message,character_buffer_primer);
										strcat(error_message,"\" was already defined for function \"");
										strcat(error_message,function_operations.at((function_position - 1))->name());
										strcat(error_message,"\" on line ");
										if (verbose)
											std::cout<<operation_conclusion(" failed");
										return error(error_message,line_iteration,verbose);
									}
								}
								else
								{
									if (is_function[0])
										function_operations.at((function_position - 1))->add_to_operation(character_buffer_primer,line_iteration);
									else
										return error("Invalid code definition outside of function",-1,verbose);
								}
							}
							if (character_buffer[i] == ';')
								break;
							memset(character_buffer_primer, 0, sizeof(character_buffer_primer));
							character_buffer_primer_position = 0;
						}
						else
						{
							character_buffer_primer[character_buffer_primer_position++] = character_buffer[i];
						}
					}
					if (is_function[0])
					{
						function_operations.at((function_position - 1))->new_operation();
						if (is_function[1])
							is_function[1] = false;
					}
					if (line_length_verification == 0)
						is_function[0] = false;
				}
				else
					line_iteration++;
				line_length = 0;
				break;
			}
		};
	}
	if (function_operations.size() == 0)
	{
		// An invalid input file provided
		if (verbose)
			std::cout<<operation_conclusion(" failed")<<std::endl;
		delete file_operations;
		return error("No functional constructs could be generated from the provided input file",-1,verbose);
	}
	if (verbose)
	{
		std::cout<<operation_conclusion(" done");
		std::cout<<std::endl<<"Verifying program syntax...";
	}
	bool entry_point = false;
	std::vector<packaged_function*> function_operations_converted;
	std::vector<const char*> real_values = {"0"};
	std::vector<const char*> glbl_variables;
	std::vector<const char*> function_names = {"__start"};
	// Reading the sytax of the provided file
	for (unsigned long long int i = 0; i < function_operations.size(); ++i)
	{
		const char* function_name = function_operations.at(i)->name();
		unsigned long long int line_ref = function_operations.at(i)->get_line_ref();
		function_operations.at(i)->reset_operations_iter();
		if (strcmp(function_name,"__start") == 0)
		{
			if (!entry_point)
				entry_point = true;
			else
			{
				if (verbose)
					std::cout<<operation_conclusion(" failed")<<std::endl;
				delete function_name;
				return error("An entry point (__start) has already been defined. Packaging halted on line \0",line_ref,verbose);
			}
		}
		else
			function_names.push_back(function_name);
		std::vector<const char*> parameters;
		function_operations.at(i)->reset_params_iter();
		while (true)
		{
			const char* parameter = function_operations.at(i)->list_params();
			if (parameter == nullptr)
				break;
			parameters.push_back(dereference(parameter));
			delete parameter;
		}
		int if_statement_logic = 0;
		int loop_level = 0;
		while (true)
		{
			std::vector<const char*> operations = function_operations.at(i)->list_operations();
			unsigned long long int line_ref = function_operations.at(i)->get_line_ref();
			if (operations.size() != 0)
			{
				if (strcmp(operations.at(0),"call") == 0)
				{
					if (operations.size() == 1)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("The \"call\" command requires a function to be called on line \0",line_ref,verbose);
					}
					else
					{
						unsigned long long int total_parameters = 0;
						bool match = false;
						for (int ii = 0; ii < function_operations.size(); ++ii)
						{
							if (strcmp(function_operations.at(ii)->name(),operations.at(1)) == 0)
							{
								match = true;
								function_operations.at(ii)->reset_params_iter();
								while (true)
								{
									if (function_operations.at(ii)->list_params() == nullptr)
										break;
									total_parameters++;
								}
								break;
							}	
						}
						if ((operations.size() - 2) > total_parameters)
						{
							char parameter_count[2];
							char parameter_requested_count[2];
							char error_message[] = "A function was called with \"";
							sprintf(parameter_requested_count,"%ld", (operations.size() - 2));
							strcat(error_message,parameter_requested_count);
							strcat(error_message,"\" parameter");
							if ((operations.size() - 2) >= 2)
								strcat(error_message,"s");
							if (!match)
								strcat(error_message,", but was not defined");
							else
							{
								strcat(error_message,", but was defined with \"");
								sprintf(parameter_count,"%lld", total_parameters);
								strcat(error_message,parameter_count);
								strcat(error_message,"\" parameter");
								if (total_parameters != 1)
									strcat(error_message,"s");
							}
							strcat(error_message,". Packaging halted at line \0");
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error(error_message,line_ref,verbose);
						}
						for (int ii = 2; ii < operations.size(); ++ii)
						{
							bool valid = false;
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;
								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								char error_message[] = "A \"call\" function had an invalid request parameter \"";
								strcat(error_message,operations.at(ii));
								strcat(error_message,"\" on line ");
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error(error_message,line_ref,verbose);
							}
						}
					}
				}
				else if (strcmp(operations.at(0),"add") == 0)
				{
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a sum at a time. Packaging halted on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a sum. Packaging halted on line \0",line_ref,verbose);
					}
					else
					{
						for (int ii = 1; ii <= 2; ii++)
						{
							bool valid = false;
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
											zero_value_check = 0;
									}
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;
								}
								valid = ((zero_value_check - 1) >= 0);
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error("Invalid value type was provided as a parameter for \"add\" command on lin,verbosee \0",line_ref);
							}
						}
					}
				}
				else if (strcmp(operations.at(0),"sub") == 0)
				{
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a sum at a time. Packaging halted on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a sum. Packaging halted on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int ii = 1; ii <= 2; ii++)
						{
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;

								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error("Invalid value type was provided as a parameter for \"sub\" command on lin,verbosee \0",line_ref);
							}
						}
					}
				}
				else if (strcmp(operations.at(0),"mul") == 0)
				{
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a sum at a time. Packaging halted on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a sum. Packaging halted on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int ii = 1; ii <= 2; ii++)
						{
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;

								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error("Invalid value type was provided as a parameter for \"mul\" command on lin,verbosee \0",line_ref);
							}
						}
					}
				}
				else if (strcmp(operations.at(0),"div") == 0)
				{
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a sum at a time. Packaging halted on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a sum. Packaging halted on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int ii = 1; ii <= 2; ii++)
						{
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;

								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error("Invalid value type was provided as a parameter for \"div\" command on lin,verbosee \0",line_ref);
							}
						}
					}
				}
				else if (strcmp(operations.at(0),"sqrt") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 variable can be used to calculate the square root on line \0",line_ref,verbose);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A variable needs to be used to calculate the square root on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a parameter on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"inc") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 variable can be used to be incremented on line \0",line_ref,verbose);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A variable needs to be used to be incremented on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a parameter on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"dec") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 variable can be used to be decremented on line \0",line_ref,verbose);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A variable needs to be used to be decremented on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a parameter on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"sin") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 variable can be used to calculate the sine on line \0",line_ref,verbose);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A variable needs to be used to calculate the sine on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a parameter on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"cos") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 variable can be used to calculate the cosine on line \0",line_ref,verbose);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A variable needs to be used to calculate the cosine on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a parameter on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"tan") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 variable can be used to calculate the tangent on line \0",line_ref,verbose);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A variable needs to be used to calculate the tangent on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a parameter on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"ceil") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 variable can be used on a ceil operation on line \0",line_ref,verbose);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A variable needs to be used to perform ceil operation on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a parameter on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"floor") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 variable can be used on a floor operation on line \0",line_ref,verbose);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A variable needs to be used to perform floor operation on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a parameter on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"rnd") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 variable can be used on a rnd operation on line \0",line_ref,verbose);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A variable needs to be used to perform rnd operation on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a parameter on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"glbl") == 0)
				{
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("The \"glbl\" command can only set one value to one variable at a time. Packaging halted o,verbosen line \0",line_ref);
					}
					else if (operations.size() == 1)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("The \"glbl\" command requires at least a variable reference to be defined to return a value,verbose. Packaging halted on line \0",line_ref);
					}
					else if (operations.size() == 3)
					{
						if (!isalpha(operations.at(1)[0]))
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("The \"glbl\" command requires the first character of a variable name to be alphabetical,verbose. Packaging halted on line \0",line_ref);
						}
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(2),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(2)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(2)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(2)[iii]))
									zero_value_check += ((int)operations.at(2)[iii] - 48);
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(2)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(2));
								}
							}
						}
						if (valid)
							glbl_variables.push_back(operations.at(1));
					}
					else
					{
						if (!isalpha(operations.at(1)[0]))
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("The \"glbl\" command requires the first character of a variable name to be alphabetical,verbose. Packaging halted on line \0",line_ref);
						} else {
							bool match = false;
							for (int iii = 0; iii < glbl_variables.size(); ++iii)
							{
								if (strcmp(glbl_variables.at(iii),operations.at(1)) == 0)
								{
									match = true;
									break;
								}
							}
							if (!match)
								glbl_variables.push_back(operations.at(1));
						}
						
					}
				}
				else if (strcmp(operations.at(0),"parm") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("The \"parm\" command can only return one parameter reference. Packaging halted on lin,verbosee \0",line_ref);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("The \"parm\" command requires a parameter reference in order to return value. Packaging halte,verbosed on line \0",line_ref);
					}
					else
					{
						bool valid_parameter = false;
						for (int iiii = 0; iiii < parameters.size(); ++iiii)
						{
							if (strcmp(parameters.at(iiii), operations.at(1)) == 0)
							{
								valid_parameter = true;
								break;
							}
						}
						if (!valid_parameter)
						{
							char error_message[] = "The parameter \"";
							strcat(error_message,operations.at(1));
							strcat(error_message,"\" is not defined within the context of the function \"");
							strcat(error_message,function_name);
							strcat(error_message,"\". Packaging halted on line \0");
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error(error_message,line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"clr") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("The \"clr\" command can only remove one variable at a time. Packaging halted on lin,verbosee \0",line_ref);
					}
					else if (operations.size() < 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("The \"clr\" command requires a variable to be defined in order to remove it. Packaging halte,verbosed on line \0",line_ref);
					}
				}
				else if (strcmp(operations.at(0),"rt") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 value can be returned on line \0",line_ref,verbose);
					}
					else if (operations.size() == 2)
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a return value on line \0",line_ref,verbose);
						}
					}
				}
				else if (strcmp(operations.at(0),"ifeq") == 0 || strcmp(operations.at(0),"break") == 0)
				{
					if (loop_level != 0 && strcmp(operations.at(0),"break") != 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Contemporary logic cannot be performed within a looped context; consider using \"break\" fo,verboser boolean logic to exit loop on line \0",line_ref);
					}
					else if (loop_level == 0 && strcmp(operations.at(0),"break") == 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A \"break\" statement was defined outside the reach of an existing loop on line \0",line_ref,verbose);
					}
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a logical comparison at a time on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a logical comparison on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int ii = 1; ii <= 2; ii++)
						{
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;

								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								if (strcmp(operations.at(0),"break") == 0)
									return error("Invalid value type was provided as a parameter for \"break\" command on lin,verbosee \0",line_ref);
								else
									return error("Invalid value type was provided as a parameter for \"ifeq\" command on lin,verbosee \0",line_ref);
							}
						}
						if (if_statement_logic == 0 && strcmp(operations.at(0),"break") != 0)
							if_statement_logic++;
					}
				}
				else if (strcmp(operations.at(0),"ifneq") == 0)
				{
					if (loop_level != 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Contemporary logic cannot be performed within a looped context; consider using \"break\" fo,verboser boolean logic to exit loop on line \0",line_ref);
					}
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a logical comparison at a time on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a logical comparison on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int ii = 1; ii <= 2; ii++)
						{
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;

								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error("Invalid value type was provided as a parameter for \"ifneq\" command on lin,verbosee \0",line_ref);
							}
						}
						if (if_statement_logic == 0)
							if_statement_logic++;
					}
				}
				else if (strcmp(operations.at(0),"ifls") == 0)
				{
					if (loop_level != 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Contemporary logic cannot be performed within a looped context; consider using \"break\" fo,verboser boolean logic to exit loop on line \0",line_ref);
					}
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a logical comparison at a time on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a logical comparison on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int ii = 1; ii <= 2; ii++)
						{
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;

								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error("Invalid value type was provided as a parameter for \"ifls\" command on lin,verbosee \0",line_ref);
							}
						}
						if (if_statement_logic == 0)
							if_statement_logic++;
					}
				}
				else if (strcmp(operations.at(0),"ifmr") == 0)
				{
					if (loop_level != 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Contemporary logic cannot be performed within a looped context; consider using \"break\" fo,verboser boolean logic to exit loop on line \0",line_ref);
					}
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a logical comparison at a time on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a logical comparison on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int ii = 1; ii <= 2; ii++)
						{
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;

								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error("Invalid value type was provided as a parameter for \"ifmr\" command on lin,verbosee \0",line_ref);
							}
						}
						if (if_statement_logic == 0)
							if_statement_logic++;
					}
				}
				else if (strcmp(operations.at(0),"iflsq") == 0)
				{
					if (loop_level != 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Contemporary logic cannot be performed within a looped context; consider using \"break\" fo,verboser boolean logic to exit loop on line \0",line_ref);
					}
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a logical comparison at a time on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a logical comparison on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int ii = 1; ii <= 2; ii++)
						{
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;
								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error("Invalid value type was provided as a parameter for \"iflsq\" command on lin,verbosee \0",line_ref);
							}
						}
						if (if_statement_logic == 0)
							if_statement_logic++;
					}
				}
				else if (strcmp(operations.at(0),"ifmrq") == 0)
				{
					if (loop_level != 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Contemporary logic cannot be performed within a looped context; consider using \"break\" fo,verboser boolean logic to exit loop on line \0",line_ref);
					}
					if (operations.size() > 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 2 values can be used in a logical comparison at a time on line \0",line_ref,verbose);
					}
					else if (operations.size() < 3)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Expected 2 values to be used in a logical comparison on line \0",line_ref,verbose);
					}
					else
					{
						bool valid = false;
						for (int ii = 1; ii <= 2; ii++)
						{
							for (int iii = 0; iii < default_values_length; iii++)
							{
								if (strcmp(operations.at(ii),default_values[iii]) == 0)
								{
									valid = true;
									break;
								}
							}
							if (!valid)
							{
								int zero_value_check = 1;
								unsigned char is_zero = 0;
								bool decimal_check = false;
								int iii = 1;
								while (operations.at(ii)[iii] != '\0') iii++;
								while (iii > (int)(operations.at(ii)[0] == '-'))
								{
									iii--;
									if (operations.at(ii)[iii] == '.')
									{
										if (!decimal_check)
											decimal_check = true;
										else
										{
											zero_value_check = 0;
											break;
										}
									}
									else if (operations.at(ii)[iii] == '-')
										zero_value_check = 0;
									else if (isdigit(operations.at(ii)[iii]))
									{
										if (((int)operations.at(ii)[iii] - 48) == 0 && is_zero != 2)
											is_zero = 1;
										else
										{
											is_zero = 2;
											zero_value_check += ((int)operations.at(ii)[iii] - 48);
										}
									}
									else
										zero_value_check = 0;
									if (zero_value_check == 0)
										break;
								}
								valid = ((zero_value_check - 1) != 0);
								if (!valid && is_zero == 1)
									valid = true;
								if (valid)
								{
									bool match = false;
									for (int iii = 0; iii < real_values.size(); ++iii)
									{
										if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
										{
											match = true;
											break;
										}
									}
									if (!match)
									{
										real_values.push_back(operations.at(ii));
									}
								}
							}
							if (!valid)
							{
								if (verbose)
									std::cout<<operation_conclusion(" failed");
								delete function_name;
								return error("Invalid value type was provided as a parameter for \"ifmrq\" command on lin,verbosee \0",line_ref);
							}
						}
						if (if_statement_logic == 0)
							if_statement_logic++;
					}
				}
				else if (strcmp(operations.at(0),"ifelse") == 0)
				{
					if (loop_level != 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Contemporary logic cannot be performed within a looped context; consider using \"break\" fo,verboser boolean logic to exit loop on line \0",line_ref);
					}
					if (operations.size() > 1)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("An \"ifelse\" statement does not accept any parameter. Packaging halted on line \0",line_ref,verbose);
					}
					if (if_statement_logic <= 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("An \"ifelse\" statement was defined before any prior logical statement on line \0",line_ref,verbose);
					}
				}
				else if (strcmp(operations.at(0),"ifend") == 0)
				{
					if (loop_level != 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Contemporary logic cannot be performed within a looped context; consider using \"break\" fo,verboser boolean logic to exit loop on line \0",line_ref);
					}
					if (operations.size() > 1)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("An \"ifend\" statement does not accept any parameter. Packaging halted on line \0",line_ref,verbose);
					}
					if (if_statement_logic <= 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("An \"ifend\" statement was defined before an valid logical statement on line \0",line_ref,verbose);
					}
					if_statement_logic--;
				}
				else if (strcmp(operations.at(0),"loop") == 0)
				{
					if (operations.size() > 1)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A \"loop\" statement does not accept any parameter. Packaging halted on line \0",line_ref,verbose);
					}
					loop_level++;
				}
				else if (strcmp(operations.at(0),"loopend") == 0)
				{
					if (operations.size() > 1)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A \"loopend\" statement does not accept any parameter. Packaging halted on line \0",line_ref,verbose);
					}
					if (loop_level <= 0)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A \"loopend\" statement was defined outside the reach of an existing loop on lin,verbosee \0",line_ref);
					}
					loop_level--;
				}
				else if (strcmp(operations.at(0),"rand") == 0)
				{
					if (operations.size() > 1)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("A \"rand\" statement does not accept any parameter. Packaging halted on line \0",line_ref,verbose);
					}
				}
				else if (strcmp(operations.at(0),"set") == 0)
				{
					if (operations.size() > 2)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed");
						delete function_name;
						return error("Only 1 value can be used as a returning set on line \0",line_ref,verbose);
					}
					else if (operations.size() == 2)
					{
						bool valid = false;
						for (int iii = 0; iii < default_values_length; iii++)
						{
							if (strcmp(operations.at(1),default_values[iii]) == 0)
							{
								valid = true;
								break;
							}
						}
						if (!valid)
						{
							int zero_value_check = 1;
							unsigned char is_zero = 0;
							bool decimal_check = false;
							int iii = 0;
							while (operations.at(1)[iii] != '\0') iii++;
							while (iii > (int)(operations.at(1)[0] == '-'))
							{
								iii--;
								if (operations.at(1)[iii] == '.')
								{
									if (!decimal_check)
										decimal_check = true;
									else
									{
										zero_value_check = 0;
										break;
									}
								}
								else if (isdigit(operations.at(1)[iii]))
								{
									if (((int)operations.at(1)[iii] - 48) == 0 && is_zero != 2)
										is_zero = 1;
									else
									{
										is_zero = 2;
										zero_value_check += ((int)operations.at(1)[iii] - 48);
									}
								}
								else
									zero_value_check = 0;
								if (zero_value_check == 0)
									break;

							}
							valid = ((zero_value_check - 1) != 0);
							if (!valid && is_zero == 1)
								valid = true;
							if (valid)
							{
								bool match = false;
								for (int iii = 0; iii < real_values.size(); ++iii)
								{
									if (strcmp(real_values.at(iii),operations.at(1)) == 0)
									{
										match = true;
										break;
									}
								}
								if (!match)
								{
									real_values.push_back(operations.at(1));
								}
							}
						}
						if (!valid)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							delete function_name;
							return error("Invalid value type was provided as a returning set value on line \0",line_ref,verbose);
						}
					}
				}
				else
				{
					char error_message[] = "The function \"";
					strcat(error_message,operations.at(0));
					strcat(error_message,"\" does not exist in the standard syntax on line \0");
					if (verbose)
						std::cout<<operation_conclusion(" failed");
					delete function_name;
					return error(error_message,line_ref,verbose);
				}
				for (int ii = 1; ii < operations.size(); ++ii)
				{
					bool match = false;
					for (int iii = 0; iii < real_values.size(); ++iii)
					{
						if (strcmp(real_values.at(iii),operations.at(ii)) == 0)
						{
							match = true;
							break;
						}
					}
					if (!match)
					{
						real_values.push_back(operations.at(ii));
					}
				}
			}
			else
			{
				break;
			}
		}
		if (if_statement_logic > 0)
		{
			if (verbose)
				std::cout<<operation_conclusion(" failed")<<std::endl;
			delete file_operations;
			return error("A logical comparison was not properly defined, an \"ifend\" statemtent is required\0",-1,verbose);
		}
		if (loop_level > 0)
		{
			if (verbose)
				std::cout<<operation_conclusion(" failed")<<std::endl;
			delete file_operations;
			return error("A loop was not properly defined, an \"loopend\" statemtent is required\0",-1,verbose);
		}
	}
	if (!entry_point)
	{
		delete file_operations;
		if (verbose)
			std::cout<<operation_conclusion(" failed")<<std::endl;
		return error("No entry point (__start) was defined!\0",-1,verbose);
	}
	if (verbose)
	{
		std::cout<<operation_conclusion(" done");
		std::cout<<std::endl<<std::endl<<"\u001b[1m\u001b[4mPACKAGING:\u001b[0m";
		std::cout<<std::endl<<"Generating operation linkings...";
	}
	for (unsigned long long int i = 0; i < function_operations.size(); ++i)
	{
		std::vector<const char*> parameters;
		function_operations.at(i)->reset_params_iter();
		while (true)
		{
			const char* parameter = function_operations.at(i)->list_params();
			if (parameter == nullptr)
				break;
			parameters.push_back(dereference(parameter));
		}
		function_operations.at(i)->reset_operations_iter();
		function_operations_converted.push_back(new packaged_function(i,parameters));
		parameters.clear();
		while (true)
		{
			std::vector<const char*> operations = function_operations.at(i)->list_operations();
			unsigned long long int line_ref = function_operations.at(i)->get_line_ref();
			if (operations.size() != 0)
			{
				unsigned long int numeric_approval = function_operations_converted.at(i)->new_operation(operations,glbl_variables,function_names,line_ref);
				if (numeric_approval > 0)
				{
					function_operations.clear();
					function_operations_converted.clear();
					operations.clear();
					if (verbose)
						std::cout<<operation_conclusion(" failed");
					return error("A numeric value exceeded the bounds of: -9999999, and 9999999; on line \0",numeric_approval,verbose);
				}
				operations.clear();
			}
			else
			{
				operations.clear();
				break;
			}
		}
	}
	if (verbose)
		std::cout<<operation_conclusion(" done");
	bool valid = false;
	
	if (verbose)
		std::cout<<std::endl<<"Creating file...";
	char *tmp_path = file_operations->tmp_path_generator();
	if (tmp_path == "")
	{
		if (verbose)
			std::cout<<operation_conclusion(" failed - too busy");
		return error("Service is very busy at the moment... Please try again later...",-1,verbose);
	}
	unsigned int tmp_file = file_operations->open(tmp_path, std::ofstream::out | std::ofstream::app | std::ofstream::binary, true, true); // Create temporary file
	if (verbose)
	{
		std::cout<<operation_conclusion(" done");
		std::cout<<std::endl<<"Calculating segmentors...";
	}

	int delimiter_numerics[7] = {7,6,5,4,3,2,1};
	int checks = 0;
	while (!valid)
	{
		checks++;
		const char* segment_function = generate_delimiter(delimiter_numerics[0]);
		delimiter_numerics[0]++;
		const char *segment_parameter = generate_delimiter(delimiter_numerics[1]);
		delimiter_numerics[1]++;
		const char *segment_operation = generate_delimiter(delimiter_numerics[2]);
		delimiter_numerics[2]++;
		const char *segment_operation_sub = generate_delimiter(delimiter_numerics[3]);
		delimiter_numerics[3]++;
		const char *segment_real_value = generate_delimiter(delimiter_numerics[4]);
		delimiter_numerics[4]++;
		const char *segment_decimal = generate_delimiter(delimiter_numerics[5]);
		delimiter_numerics[5]++;
		const char *segment_negative = generate_delimiter(delimiter_numerics[6]);
		delimiter_numerics[6]++;
		
		bool invalid = false;

		if (strcmp(segment_function,segment_parameter) == 0)
			invalid = true;
		else if (strcmp(segment_function,segment_operation) == 0)
			invalid = true;
		else if (strcmp(segment_function,segment_operation_sub) == 0)
			invalid = true;
		else if (strcmp(segment_function,segment_real_value) == 0)
			invalid = true;
		else if (strcmp(segment_function,segment_decimal) == 0)
			invalid = true;
		else if (strcmp(segment_function,segment_negative) == 0)
			invalid = true;
		else if (strcmp(segment_parameter,segment_operation) == 0)
			invalid = true;
		else if (strcmp(segment_parameter,segment_operation_sub) == 0)
			invalid = true;
		else if (strcmp(segment_parameter,segment_real_value) == 0)
			invalid = true;
		else if (strcmp(segment_parameter,segment_decimal) == 0)
			invalid = true;
		else if (strcmp(segment_parameter,segment_negative) == 0)
			invalid = true;
		else if (strcmp(segment_operation,segment_operation_sub) == 0)
			invalid = true;
		else if (strcmp(segment_operation,segment_real_value) == 0)
			invalid = true;
		else if (strcmp(segment_operation,segment_decimal) == 0)
			invalid = true;
		else if (strcmp(segment_operation,segment_negative) == 0)
			invalid = true;
		else if (strcmp(segment_operation_sub,segment_real_value) == 0)
			invalid = true;
		else if (strcmp(segment_operation_sub,segment_decimal) == 0)
			invalid = true;
		else if (strcmp(segment_operation_sub,segment_negative) == 0)
			invalid = true;
		else if (strcmp(segment_real_value,segment_decimal) == 0)
			invalid = true;
		else if (strcmp(segment_real_value,segment_negative) == 0)
			invalid = true;
		else if (strcmp(segment_decimal,segment_negative) == 0)
			invalid = true;
		else
		{
			invalid = true;
			if (get_char_size(segment_function) == get_char_size(segment_parameter))
				if (get_char_size(segment_parameter) == get_char_size(segment_operation))
					if (get_char_size(segment_operation) == get_char_size(segment_operation_sub))
						if (get_char_size(segment_operation_sub) == get_char_size(segment_real_value))
							if (get_char_size(segment_real_value) == get_char_size(segment_decimal))
								if (get_char_size(segment_decimal) == get_char_size(segment_negative))
									invalid = false;
		}
		signed long long int total_segment_function_count = 0;
		signed long long int total_segment_parameter_count = 0;
		signed long long int total_segment_operation_count = 0;
		signed long long int total_segment_operation_sub_count = 0;
		signed long long int total_segment_real_value_count = 0;
		signed long long int total_segment_decimal_count = 0;
		signed long long int total_segment_negative_count = 0;
		signed long long int total_packaging_size = 0;
		if (!invalid)
		{
			const unsigned long long int segmentation_size = (get_char_size(segment_function) * 2);
			const unsigned long long int default_segmentation_size = get_char_size(segment_function);
			int parse_offest = 0;
			while (parse_offest < default_segmentation_size)
			{
				for (int i = parse_offest; i < function_operations.size(); ++i)
				{
					build_structure *function_build_data = function_operations_converted.at(i)->build(segment_function,segment_parameter,segment_operation,segment_operation_sub,segment_real_value,segment_decimal,segment_negative);
					signed long long int segment_function_count = function_build_data->get_functions();
					signed long long int segment_parameter_count = function_build_data->get_parameters();
					signed long long int segment_operation_count = function_build_data->get_operations();
					signed long long int segment_operation_sub_count = function_build_data->get_operation_subs();
					signed long long int segment_real_values = function_build_data->get_real_values();
					signed long long int segment_decimal_count = function_build_data->get_decimals();
					signed long long int segment_negative_count = function_build_data->get_negatives();
					total_segment_function_count += segment_function_count;
					total_segment_parameter_count += segment_parameter_count;
					total_segment_operation_count += segment_operation_count;
					total_segment_operation_sub_count += segment_operation_sub_count;
					total_segment_real_value_count += segment_real_values;
					total_segment_decimal_count += segment_decimal_count;
					total_segment_negative_count += segment_negative_count;
					const char *function_build_string = function_build_data->get_build_string();
					total_packaging_size += get_char_size(function_build_string);
					char hex_verify[3];
					bool length_verify = true;
					for (int ii = 0; ii < get_char_size(function_build_string); ++ii)
					{
						length_verify = !length_verify;
						hex_verify[length_verify] = function_build_string[ii];
						if (length_verify)
						{
							hex_verify[2] = '\0';
							if (strcmp(hex_verify,segment_function) == 0)
								segment_function_count--;
							if (strcmp(hex_verify,segment_parameter) == 0)
								segment_parameter_count--;
							if (strcmp(hex_verify,segment_operation) == 0)
								segment_operation_count--;
							if (strcmp(hex_verify,segment_operation_sub) == 0)
								segment_operation_sub_count--;
							if (strcmp(hex_verify,segment_real_value) == 0)
								segment_real_values--;
							if (strcmp(hex_verify,segment_decimal) == 0)
								segment_decimal_count--;
							if (strcmp(hex_verify,segment_negative) == 0)
								segment_decimal_count--;
						}
						if (length_verify)
							memset(hex_verify, 0, sizeof(hex_verify));
					}
					// Checks for for the delimiters do not have any duplicates in the output
					if (segment_function_count != 0 || segment_parameter_count != 0 || segment_operation_count != 0 || segment_operation_sub_count != 0 || segment_real_values != 0 || segment_decimal_count != 0 || segment_negative_count != 0)
					{
						total_segment_function_count = 0;
						total_segment_parameter_count = 0;
						total_segment_operation_count = 0;
						total_segment_operation_sub_count = 0;
						total_segment_real_value_count = 0;
						total_segment_decimal_count = 0;
						total_segment_negative_count = 0;
						total_packaging_size = 0;
						delete function_build_string;
						valid = false;
						file_operations->clear(tmp_file);
						break;
					}
					else
					{
						file_operations->write(tmp_file,function_build_string);
						delete function_build_string;
						valid = true;
					}
				}
				if (valid)
				{
					try
					{
						file_operations->close(tmp_file, false);
						char tmp_file_parse[segmentation_size];
						unsigned int tmp_file_check = file_operations->open(tmp_path, std::fstream::in, false, true);
						int ii = 0;
						bool read_state = false;
						for (int i = parse_offest; i < total_packaging_size; ++i)
						{
							read_state = true;
							char character = file_operations->get_next_char(tmp_file_check);
							tmp_file_parse[ii++] = character;
							if (ii == segmentation_size)
							{
								read_state = false;
								char tmp_file_parse_partition[(default_segmentation_size + 1)];
								for (int iii = 0; iii < default_segmentation_size; ++iii)
								{
									tmp_file_parse_partition[iii] = tmp_file_parse[iii];
									tmp_file_parse[iii] = tmp_file_parse[segmentation_size + 1];
								}
								ii = segmentation_size - 1;
								tmp_file_parse_partition[default_segmentation_size] = '\0';
								if (strcmp(tmp_file_parse_partition,segment_function) == 0)
									total_segment_function_count--;
								if (strcmp(tmp_file_parse_partition,segment_parameter) == 0)
									total_segment_parameter_count--;
								if (strcmp(tmp_file_parse_partition,segment_operation) == 0)
									total_segment_operation_count--;
								if (strcmp(tmp_file_parse_partition,segment_operation_sub) == 0)
									total_segment_operation_sub_count--;
								if (strcmp(tmp_file_parse_partition,segment_real_value) == 0)
									total_segment_real_value_count--;
								if (strcmp(tmp_file_parse_partition,segment_decimal) == 0)
									total_segment_decimal_count--;
								if (strcmp(tmp_file_parse_partition,segment_negative) == 0)
									total_segment_negative_count--;
								if (total_segment_function_count < 0 || total_segment_parameter_count < 0 || total_segment_operation_count < 0 || total_segment_operation_sub_count < 0 || total_segment_real_value_count < 0 || total_segment_negative_count < 0)
								{
									valid = false;
									break;
								}
							}
						}
						if (read_state || (total_segment_function_count + total_segment_parameter_count + total_segment_operation_count + total_segment_operation_sub_count + total_segment_real_value_count + total_segment_decimal_count + total_segment_negative_count) == 0)
						{
							if (verbose)
								std::cout<<operation_conclusion(" failed - unable to reverify packaging.");
							delete segment_function;
							delete segment_parameter;
							delete segment_operation;
							delete segment_operation_sub;
							delete segment_real_value;
							delete segment_decimal;
							delete segment_negative;
							delete file_operations;
							return error("Package verfication error occured",-1,verbose);
						}
						if (valid && parse_offest == (default_segmentation_size - 2))
						{
							// Valid build string with delimiters
							if (verbose)
								std::cout<<operation_conclusion(" done");
							char *delimiter_size = new char[8];
							sprintf(delimiter_size,"%01llX",get_char_size(segment_function));
							unsigned int output_file;
							char *tmp_output_path = new char;
							output_file = file_operations->open(output_file_path, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary,true,false);
							if (raw)
							{
								if (verbose)
									std::cout<<std::endl<<std::endl<<"\u001b[1m\u001b[4mPackaged program output:\u001b[0m"<<std::endl;
								std::cout<<build_version_id;
								std::cout<<delimiter_size;
							}
							else
							{
								if (verbose)
									std::cout<<std::endl<<"Writing output to file...";
								file_operations->write(output_file,build_version_id);
								file_operations->write(output_file,delimiter_size);
							}
							
							delete[] delimiter_size;
							if (raw)
							{
								std::cout<<segment_function;
								std::cout<<segment_parameter;
								std::cout<<segment_operation;
								std::cout<<segment_operation_sub;
								std::cout<<segment_real_value;
								std::cout<<segment_decimal;
								std::cout<<segment_negative;
							}
							else
							{
								file_operations->write(output_file,segment_function);
								file_operations->write(output_file,segment_parameter);
								file_operations->write(output_file,segment_operation);
								file_operations->write(output_file,segment_operation_sub);
								file_operations->write(output_file,segment_real_value);
								file_operations->write(output_file,segment_decimal);
								file_operations->write(output_file,segment_negative);
							}
							delete segment_function;
							delete segment_parameter;
							delete segment_operation;
							delete segment_operation_sub;
							delete segment_real_value;
							delete segment_decimal;
							delete segment_negative;
							file_operations->bof(tmp_file_check);
							if (raw)
							{
								for (int i = 0; i < (total_packaging_size); ++i)
									std::cout<<file_operations->get_next_char(tmp_file_check);
								std::cout<<std::endl;
							}
							else
							{
								for (int i = 0; i < total_packaging_size; ++i)
									file_operations->write(output_file,new char(file_operations->get_next_char(tmp_file_check)));
								if (verbose)
									std::cout<<operation_conclusion(" done");
							}
							delete file_operations;
							if (verbose)
								std::cout<<std::endl;
							return 0;
						};
					}
					catch (...)
					{
						if (verbose)
							std::cout<<operation_conclusion(" failed - unable to reverify packaging.");
						delete file_operations;
						return 1;
					}
					if (!valid)
					{
						total_segment_function_count = 0;
						total_segment_parameter_count = 0;
						total_segment_operation_count = 0;
						total_segment_operation_sub_count = 0;
						total_segment_decimal_count = 0;
						total_segment_negative_count = 0;
						total_packaging_size = 0;
					}
				}
				else
					break;
			}
		}
		else if (strcmp(segment_function,"00") == 0)
			if (strcmp(segment_parameter,"00") == 0)
				if (strcmp(segment_operation,"00") == 0)
					if (strcmp(segment_operation_sub,"00") == 0)
						if (strcmp(segment_real_value,"00") == 0)
						{
							delete segment_function;
							delete segment_parameter;
							delete segment_operation;
							delete segment_operation_sub;
							delete segment_real_value;
							delete segment_decimal;
							delete segment_negative;
							delete file_operations;
							if (verbose)
								std::cout<<operation_conclusion(" failed");
							return error("Program exceeds the packaging target size",-1,verbose);
						}
		delete segment_function;
		delete segment_parameter;
		delete segment_operation;
		delete segment_operation_sub;
		delete segment_real_value;
		delete segment_decimal;
		delete segment_negative;
	}
	delete file_operations;
	if (verbose)
		std::cout<<" unknown";
	return 1;
}

int main(int argc, char *argv[])
{
	bool verbose = false;
	bool raw = false;
	char* input_file_path = new char;
	char* output_file_path = new char;
	char lan_intro[] = "The real language packager";
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0)
		{
			std::cout<<lan_intro<<std::endl<<"Usage: [options] {-i input_path} ... {-o output_path}"<<std::endl<<"Packages the Real language into a portable interpretive redistributable package"<<std::endl<<std::endl<<"List of options:"<<std::endl<<"  -i, --input <file>       Expects the Real language input file path to be"<<std::endl<<"                           subsequently appended after this argument"<<std::endl<<"  --no-color, --no-colour  Removes all colour from any outputs"<<std::endl<<"  -o, --output <file>      Expects the file path of the package output file"<<std::endl<<"                           to be subsequently appended after this statment"<<std::endl<<"  --raw                    Outputs the raw packaged program to the console"<<std::endl<<"  -v, --verbose            Gives a verbose output of what is currently"<<std::endl<<"                           occuring"<<std::endl<<"  --version                Prints out the version number, release number, and"<<std::endl<<"                           compilation time"<<std::endl;
			return 0;
		}
		else if (strcmp(argv[i],"-i") == 0 || strcmp(argv[i],"--input") == 0)
		{
			if ((i + 1) < argc)
			{
				if (get_char_size(input_file_path) != 0)
					return error("Another input file has already been defined!",-1,false);
				if (argv[(i + 1)][0] == '-')
				{
					char error_message[55] = "A filepath is expected after an `";
					strcat(error_message,argv[i]);
					strcat(error_message,"` argument!\0");
					return error(error_message,-1,false);
				}
				input_file_path = argv[++i];
			}
		}
		else if (strcmp(argv[i],"--no-color") == 0 || strcmp(argv[i],"--no-colour") == 0)
			color = false;
		else if (strcmp(argv[i],"-o") == 0 || strcmp(argv[i],"--ouput") == 0)
		{
			if ((i + 1) <= argc)
			{
				i++;
				output_file_path = argv[i];
			}
		}
		else if (strcmp(argv[i],"--raw") == 0)
			raw = true;
		else if (strcmp(argv[i],"--version") == 0)
		{
			std::cout<<lan_intro<<std::endl<<"  Version: "<<VERSION_NUM<<std::endl<<"  Release: "<<RELEASE_NUM<<std::endl<<" Compiled: "<<__DATE__<<", "<<__TIME__<<std::endl<<std::endl<<"Written by Bradley Marshall"<<std::endl;
			return !(i == argc - 1);
		}
		else if (strcmp(argv[i],"-v") == 0 || strcmp(argv[i],"--verbose") == 0)
			verbose = true;
		else if (strstr(argv[i],"="))
		{
			char *argument_splitters = strtok(argv[i],"=");
			while (argument_splitters)
			{
				argv[argc++] = argument_splitters;
				argument_splitters = strtok(NULL,"=");
			}
		}
	}
	if (raw && get_char_size(output_file_path) > 0)
		return error("A raw output cannot be used in unison with a file output.",-1,false);
	return generate(verbose,input_file_path,output_file_path,raw);
}