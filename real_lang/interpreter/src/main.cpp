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

#include <iostream>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include "LinkedList.h"
#include "externs.h"
#include "return.cpp"
#include "variable.cpp"
#include "callstack.cpp"
#include "function.cpp"
#include "operation.cpp"
#include "segment_insight.cpp"
#include "file.cpp"

unsigned short int recursion_level = 0;
float rtv[4] = {0,0,0,0};
bool debug = false;
bool debug_all = false;

/**
 * @brief Get the length of char array
 * 
 * @param char_array which length is to be evaluated
 * @return char array length
 */
unsigned long long int get_char_size(const char *char_array)
{
	unsigned long long int char_array_size = 0;	
	while (char_array[char_array_size] != '\0') char_array_size++;
	return char_array_size;
}

/**
 * @brief Convert hexadecimal to be respresented as an integer
 * 
 * @param hex_string hexadecimal char array to be evaluated as an integer
 * @return integer value
 */
unsigned int hex2int(char* &hex_string)
{
    unsigned int val = 0;
    unsigned int len = get_char_size(hex_string);
    for(unsigned int i=0;i<len;i++)
       if(hex_string[i] <= 57)
        val += (hex_string[i]-48)*(1<<(4*(len-1-i)));
       else
        val += (hex_string[i]-55)*(1<<(4*(len-1-i)));
    return val;
}

/**
 * @brief Validate of 2 char array's values match
 * 
 * @param str1 char array 
 * @param str2 char array
 * @return `true` if they match, and `false` otherwise 
 */
bool strcmp(const char* str1, const char* str2)
{
	unsigned int str_length = get_char_size(str1);
	if (str_length != get_char_size(str2))
		return false;
    for(unsigned int i=0;i<str_length;i++)
		if (str1[i] != str2[i])
			return false;
	return true;
}

/**
 * @brief Runs the parsed package
 * 
 * @param functions that are accessible to the package
 * @param execution_version the version which the package supports
 * @return exit code
 */
unsigned int run(LinkedList<function*> *functions, unsigned char &execution_version)
{
	LinkedList<variable*> *variables = new LinkedList<variable*>();
	(*functions).First()->execute(execution_version,functions,variables);
	variables->Clear();
	if (recursion_level == 5000)
		return 1;
	return 0;
}

/**
 * @brief Parses and executes the packaged program
 * 
 * @param input_file the file path for the input package 
 * @return exit code
 */
unsigned int parse(const char* input_file)
{
	// Verifying the validity of the provided parameters, and existence of files/directories
	struct stat buffer;
	if (stat (input_file, &buffer) != 0)
	{
		std::cout<<"The defined input package file does not exists!"<<std::endl;
		return 1;
	}
	else
	{
		if(!(buffer.st_mode & S_IFREG))
		{
			std::cout<<"The defined input destination does not resolve to a package file!"<<std::endl;
			return 1;
		}
	}
	file *program = new file(input_file);
	unsigned long long int program_read = 0;
	unsigned char execution_version = 0;
	char *build_version = new char[4];
	char *build_version_check = new char[4];
	unsigned short delimiter_length;
	bool valid = false;
	if (debug)
		std::cout<<"Parsing package:"<<std::endl<<"Detecting build version...";
	while (program_read < 5)
	{
		if (program->get_next_char() == '\0')
			break;
		if (program_read < 4)
		{
			build_version[program_read] = program->get_current_char();
			if (program_read == 3)
			{
				// Validates the build version to the supported ones
				build_version_check[3] = build_version_check[2] = build_version_check[1] = build_version_check[0] = '0';
				if (strcmp(build_version,build_version_check))
				{
					if (debug)
						std::cout<<" done (version: "<<build_version_check<<")"<<std::endl;
					execution_version = 0;
					delete[] build_version;
				}
				else
				{
					if (debug)
						std::cout<<" failed - unsupported build version ("<<build_version<<")"<<std::endl;
					delete[] build_version;
					delete[] build_version_check;
					return 1; // invalid build format
				}
				delete[] build_version_check;
			}
		}
		else
		{
			if (debug)
				std::cout<<"Detecting delimiter length...";
			if (!(program->get_current_char() == '0' || program->get_current_char() == '1' || program->get_current_char() == '2' || program->get_current_char() == '3' || program->get_current_char() == '4' || program->get_current_char() == '5' || program->get_current_char() == '6' || program->get_current_char() == '7' || program->get_current_char() == '8' || program->get_current_char() == '9'))
			{
				if (debug)
					std::cout<<" failed"<<std::endl;
				delete[] build_version;
				return 2;
			}
			delimiter_length = ((int)program->get_current_char() - '0'); // Extracts the length of the delimiters to use
			if (!(delimiter_length >= 2 && delimiter_length <= 8 && delimiter_length % 2 == 0))
			{
				if (debug)
					std::cout<<" failed"<<std::endl;
				delete[] build_version;
				return 3;
			}
			if (debug)
				std::cout<<" done"<<std::endl;
		}
		program_read++;
		valid = (program_read == 5);
	}
	if (!valid)
	{
		if (debug)
			std::cout<<std::endl<<"Package does not meet the sizing requirements to be run."<<std::endl;
		delete[] build_version_check;
		return 4;
	}
	if (debug)
		std::cout<<"Deciphering segment delimiters...";
	valid = false;
	char *segment_function = new char[delimiter_length+1];
	segment_function[delimiter_length] = '\0';
	char *segment_parameter = new char[delimiter_length+1];
	segment_parameter[delimiter_length] = '\0';
	char *segment_operation = new char[delimiter_length+1];
	segment_operation[delimiter_length] = '\0';
	char *segment_operation_sub = new char[delimiter_length+1];
	segment_operation_sub[delimiter_length] = '\0';
	char *segment_real_value = new char[delimiter_length+1];
	segment_real_value[delimiter_length] = '\0';
	char *segment_decimal_value = new char[delimiter_length+1];
	segment_decimal_value[delimiter_length] = '\0';
	char *segment_negative_value = new char[delimiter_length+1];
	segment_negative_value[delimiter_length] = '\0';
	while (program_read < (5 + (delimiter_length * 7)))
	{
		if (program->get_next_char() == '\0')
			break;
		// Extracts the segment delimiters from the package
		if (program_read < (5 + delimiter_length))
			segment_function[(program_read - 5)] = program->get_current_char();
		else if (program_read < (5 + delimiter_length * 2))
			segment_parameter[(program_read - 5 - delimiter_length)] = program->get_current_char();
		else if (program_read < (5 + delimiter_length * 3))
			segment_operation[(program_read - 5 - (delimiter_length * 2))] = program->get_current_char();
		else if (program_read < (5 + delimiter_length * 4))
			segment_operation_sub[(program_read - 5 - (delimiter_length * 3))] = program->get_current_char();
		else if (program_read < (5 + delimiter_length * 5))
			segment_real_value[(program_read - 5 - (delimiter_length * 4))] = program->get_current_char();
		else if (program_read < (5 + delimiter_length * 6))
			segment_decimal_value[(program_read - 5 - (delimiter_length * 5))] = program->get_current_char();
		else
			segment_negative_value[(program_read - 5 - (delimiter_length * 6))] = program->get_current_char();
		program_read++;
		valid = (program_read == (5 + (delimiter_length * 7)));
	}
	if (!valid)
	{
		if (debug)
			std::cout<<" failed"<<std::endl;
		delete[] segment_function;
		delete[] segment_parameter;
		delete[] segment_operation;
		delete[] segment_operation_sub;
		delete[] segment_real_value;
		delete[] segment_decimal_value;
		delete[] segment_negative_value;
		return 5;
	}
	if (strcmp(segment_function,segment_parameter))
		valid = false;
	else if (strcmp(segment_function,segment_operation))
		valid = false;
	else if (strcmp(segment_function,segment_operation_sub))
		valid = false;
	else if (strcmp(segment_function,segment_real_value))
		valid = false;
	else if (strcmp(segment_function,segment_decimal_value))
		valid = false;
	else if (strcmp(segment_function,segment_negative_value))
		valid = false;
	else if (strcmp(segment_parameter,segment_operation))
		valid = false;
	else if (strcmp(segment_parameter,segment_operation_sub))
		valid = false;
	else if (strcmp(segment_parameter,segment_real_value))
		valid = false;
	else if (strcmp(segment_parameter,segment_decimal_value))
		valid = false;
	else if (strcmp(segment_parameter,segment_negative_value))
		valid = false;
	else if (strcmp(segment_operation,segment_operation_sub))
		valid = false;
	else if (strcmp(segment_operation,segment_real_value))
		valid = false;
	else if (strcmp(segment_operation,segment_decimal_value))
		valid = false;
	else if (strcmp(segment_operation,segment_negative_value))
		valid = false;
	else if (strcmp(segment_operation_sub,segment_real_value))
		valid = false;
	else if (strcmp(segment_operation_sub,segment_decimal_value))
		valid = false;
	else if (strcmp(segment_operation_sub,segment_negative_value))
		valid = false;
	else if (strcmp(segment_real_value,segment_decimal_value))
		valid = false;
	else if (strcmp(segment_real_value,segment_negative_value))
		valid = false;
	else if (strcmp(segment_decimal_value,segment_negative_value))
		valid = false;
	if (!valid)
	{
		// There is an invalid overlap of matching delimiters
		if (debug)
			std::cout<<" failed - segmentation mismatch occurred"<<std::endl;
		delete[] segment_function;
		delete[] segment_parameter;
		delete[] segment_operation;
		delete[] segment_operation_sub;
		delete[] segment_real_value;
		delete[] segment_decimal_value;
		delete[] segment_negative_value;
		return 6;
	}
	if (debug)
		std::cout<<" done"<<std::endl<<"Calculating segmentational occurences...";
	LinkedList<function*> *functions = new LinkedList<function*>();
	{
		bool hex_read_segmentor_position = false;
		unsigned char segmentation_max_insert_positions = (delimiter_length / 2);
		int segmentation_max_insert_relative = segmentation_max_insert_positions - 1;
		if (segmentation_max_insert_relative == 0)
			segmentation_max_insert_relative += 1;
		unsigned int segmentation_positions[6][segmentation_max_insert_relative];
		char *hex_read_segmentor = new char[2];
		LinkedList<segment_insight> segmentations;
		unsigned long long int program_read_start = program_read;
		program->get_next_char();
		while (program->get_current_char() != '\0')
		{
			// Generates structure of segments from parsed package
			hex_read_segmentor[hex_read_segmentor_position] = program->get_current_char();
			hex_read_segmentor_position = !hex_read_segmentor_position;
			if (!hex_read_segmentor_position)
			{
				unsigned char segment_offset = 0;
				for (int i = 0; i < segmentation_max_insert_positions; ++i)
				{
					char *segment_function_segment = new char[3];
					segment_function_segment[0] = segment_function[segment_offset];
					segment_function_segment[1] = segment_function[(segment_offset + 1)];
					segment_function_segment[2] = '\0';
					char *segment_parameter_segment = new char[3];
					segment_parameter_segment[0] = segment_parameter[segment_offset];
					segment_parameter_segment[1] = segment_parameter[(segment_offset + 1)];
					segment_parameter_segment[2] = '\0';
					char *segment_operation_segment = new char[3];
					segment_operation_segment[0] = segment_operation[segment_offset];
					segment_operation_segment[1] = segment_operation[(segment_offset + 1)];
					segment_operation_segment[2] = '\0';
					char *segment_operation_sub_segment = new char[3];
					segment_operation_sub_segment[0] = segment_operation_sub[segment_offset];
					segment_operation_sub_segment[1] = segment_operation_sub[(segment_offset + 1)];
					segment_operation_sub_segment[2] = '\0';
					char *segment_real_value_segment = new char[3];
					segment_real_value_segment[0] = segment_real_value[segment_offset];
					segment_real_value_segment[1] = segment_real_value[(segment_offset + 1)];
					segment_real_value_segment[2] = '\0';
					char *segment_decimal_value_segment = new char[3];
					segment_decimal_value_segment[0] = segment_decimal_value[segment_offset];
					segment_decimal_value_segment[1] = segment_decimal_value[(segment_offset + 1)];
					segment_decimal_value_segment[2] = '\0';
					char *segment_negative_value_segment = new char[3];
					segment_negative_value_segment[0] = segment_negative_value[segment_offset];
					segment_negative_value_segment[1] = segment_negative_value[(segment_offset + 1)];
					segment_negative_value_segment[2] = '\0';
					segment_offset += 2;
					if (segment_offset == delimiter_length)
						segment_offset = 0;
					if (strcmp(hex_read_segmentor,segment_function_segment))
					{
						bool match = false;
						if (segmentation_max_insert_positions == 1 or i == 0)
						{
							segmentation_positions[0][0] = 1;
							match = true;
						}
						else
						{
							segmentation_positions[0][i - 1] += 1;
							if (segmentation_positions[0][i - 1] == segmentation_max_insert_positions)
								match = true;
						}
						if (match)
							segmentations.Append(segment_insight((unsigned long long int)(program_read - ((i+1) * 2) + 1),0));
					}
					else
						for(int ii = 0; ii < segmentation_max_insert_positions; ii++)
							segmentation_positions[0][i] = 0;
					if (strcmp(hex_read_segmentor,segment_parameter_segment))
					{
						bool match = false;
						if (segmentation_max_insert_positions == 1 or i == 0)
						{
							segmentation_positions[1][0] = 1;
							match = true;
						}
						else
						{
							segmentation_positions[1][i - 1] += 1;
							if (segmentation_positions[1][i - 1] == segmentation_max_insert_positions)
								match = true;
						}
						if (match)
							segmentations.Append(segment_insight((unsigned long long int)(program_read - ((i+1) * 2) + 1),1));
					}
					else
						for(int ii = 0; ii < segmentation_max_insert_positions; ii++)
							segmentation_positions[1][i] = 0;
					if (strcmp(hex_read_segmentor,segment_operation_segment))
					{
						bool match = false;
						if (segmentation_max_insert_positions == 1 or i == 0)
						{
							segmentation_positions[2][0] = 1;
							match = true;
						}
						else
						{
							segmentation_positions[2][i - 1] += 1;
							if (segmentation_positions[2][i - 1] == segmentation_max_insert_positions)
								match = true;
						}
						if (match)
							segmentations.Append(segment_insight((unsigned long long int)(program_read - ((i+1) * 2) + 1),2));
					}
					else
						for(int ii = 0; ii < segmentation_max_insert_positions; ii++)
							segmentation_positions[2][i] = 0;
					if (strcmp(hex_read_segmentor,segment_operation_sub_segment))
					{
						bool match = false;
						if (segmentation_max_insert_positions == 1 or i == 0)
						{
							segmentation_positions[3][0] = 1;
							match = true;
						}
						else
						{
							segmentation_positions[3][i - 1] += 1;
							if (segmentation_positions[3][i - 1] == segmentation_max_insert_positions)
								match = true;
						}
						if (match)
							segmentations.Append(segment_insight((unsigned long long int)(program_read - ((i+1) * 2) + 1),3));
					}
					else
						for(int ii = 0; ii < segmentation_max_insert_positions; ii++)
							segmentation_positions[3][i] = 0;
					if (strcmp(hex_read_segmentor,segment_real_value_segment))
					{
						bool match = false;
						if (segmentation_max_insert_positions == 1 or i == 0)
						{
							segmentation_positions[4][0] = 1;
							match = true;
						}
						else
						{
							segmentation_positions[4][i - 1] += 1;
							if (segmentation_positions[4][i - 1] == segmentation_max_insert_positions)
								match = true;
						}
						if (match)
							segmentations.Append(segment_insight((unsigned long long int)(program_read - ((i+1) * 2) + 1),4));
					}
					else
						for(int ii = 0; ii < segmentation_max_insert_positions; ii++)
							segmentation_positions[4][i] = 0;
					if (strcmp(hex_read_segmentor,segment_decimal_value_segment))
					{
						bool match = false;
						if (segmentation_max_insert_positions == 1 or i == 0)
						{
							segmentation_positions[5][0] = 1;
							match = true;
						}
						else
						{
							segmentation_positions[5][i - 1] += 1;
							if (segmentation_positions[5][i - 1] == segmentation_max_insert_positions)
								match = true;
						}
						if (match)
							segmentations.Append(segment_insight((unsigned long long int)(program_read - ((i+1) * 2) + 1),5));
					}
					else
						for(int ii = 0; ii < segmentation_max_insert_positions; ii++)
							segmentation_positions[5][i] = 0;
					if (strcmp(hex_read_segmentor,segment_negative_value_segment))
					{
						bool match = false;
						if (segmentation_max_insert_positions == 1 or i == 0)
						{
							segmentation_positions[6][0] = 1;
							match = true;
						}
						else
						{
							segmentation_positions[6][i - 1] += 1;
							if (segmentation_positions[6][i - 1] == segmentation_max_insert_positions)
								match = true;
						}
						if (match)
							segmentations.Append(segment_insight((unsigned long long int)(program_read - ((i+1) * 2) + 1),6));
					}
					else
					{
						for(int ii = 0; ii < segmentation_max_insert_positions; ii++)
							segmentation_positions[6][i] = 0;
					}
					hex_read_segmentor[0] = '\0';
					delete[] segment_function_segment;
					delete[] segment_parameter_segment;
					delete[] segment_operation_segment;
					delete[] segment_operation_sub_segment;
					delete[] segment_real_value_segment;
					delete[] segment_decimal_value_segment;
					delete[] segment_negative_value_segment;
				}
			}
			program_read++;
			program->get_next_char();
		}
		delete[] segment_function;
		delete[] segment_parameter;
		delete[] segment_operation;
		delete[] segment_operation_sub;
		delete[] segment_real_value;
		delete[] segment_decimal_value;
		delete[] segment_negative_value;
		if (segmentations.getLength() == 0)
		{
			// No segments were generated
			if (debug)
				std::cout<<" failed"<<std::endl;
			return 7;
		}
		else if (debug)
			std::cout<<" done"<<std::endl<<"Generating operation structure...";
		delete[] hex_read_segmentor;
		unsigned long long program_end_position = program_read;
		program_read = program_read_start;
		{
			unsigned int function_counter = 0;
			unsigned char set_type = 0;
			float numeric = 0;
			char *hex_string = new char[9];
			hex_string[8] = '\0';
			unsigned char hex_string_position = 0;
			unsigned char hex_current_operation = 0;
			bool function_id_stated = false;
			program->go_to_pos(program_read);
			while (program_read < program_end_position)
			{
				if (segmentations.getLength() > 0)
				{
					if (segmentations.First().get_start_position() == program_read)
					{
						program_read += delimiter_length;
						program->go_to_pos(program_read);
						hex_string[hex_string_position] = '\0';
						switch (set_type)
						{
							case 1:
							{
								if ((signed int)hex2int(hex_string) != (signed int)(function_counter - 1))
								{
									functions->Clear();
									segmentations.Clear();
									delete[] hex_string;
									return 8;
								}
								if (segmentations.getLength() > 0)
								{
									if (segmentations.First().get_type() != 1)
									{
										functions->Clear();
										segmentations.Clear();
										delete[] hex_string;
										return 9;
									}
								}
								else
								{
									functions->Clear();
									segmentations.Clear();
									delete[] hex_string;
									return 9;
								}
								numeric = 0;
								break;
							}
							case 2:
							{
								functions->Append(new function((function_counter - 1), (int)hex2int(hex_string)));
								numeric = 0;
								break;
							}
							case 3:
							{
								if (functions->getLength() == 0)
								{
									functions->Clear();
									segmentations.Clear();
									delete[] hex_string;
									return 10;
								}
								hex_current_operation = (int)hex2int(hex_string);
								(*functions).Last()->add_operations(hex_current_operation);
								numeric = 0;
								break;
							}
							case 4:
							{
								if (segmentations.getCurrent().get_type() != 6)
								{
									if (get_char_size(hex_string) != 0)
									{
										if (functions->getLength() > 0)
										{
											(*functions).Last()->add_operation_callstack((hex_current_operation != 14 || function_id_stated),(int)hex2int(hex_string));
											if (hex_current_operation == 14)
												function_id_stated = true;
										}
										else
										{
											functions->Clear();
											segmentations.Clear();
											delete[] hex_string;
											return 9;
										}
									}
								}
								break;
							}
							case 5:
							{
								if (segmentations.getCurrent().get_type() != 5 && segmentations.getCurrent().get_type() != 6)
								{
									if (get_char_size(hex_string) != 0)
									{
										(*functions).Last()->add_operation_callstack(false,(int)hex2int(hex_string));
										break;
									}
								}
								else if (get_char_size(hex_string) != 0)
								{
									numeric = (int)hex2int(hex_string);
									break;
								}
								else if (segmentations.getCurrent().get_type() == 5 or segmentations.getCurrent().get_type() == 6)
								{
									numeric = 0;
									break;
								}
								functions->Clear();
								segmentations.Clear();
								delete[] hex_string;
								return 9;
							}
							case 6:
							{
								float numeric_decimal = (float)hex2int(hex_string);
								while (numeric_decimal > 1)
									numeric_decimal = numeric_decimal / 10;
								if (numeric < 0)
									numeric_decimal = -numeric_decimal;
								numeric += numeric_decimal;
								if (functions->getLength() > 0)
									(*functions).Last()->add_operation_callstack(false,numeric);
								else
									return 9;
								break;
							}
							case 7:
							{
								if (functions->getLength() > 0)
								{
									numeric = -(int)hex2int(hex_string);
									if (segmentations.getCurrent().get_type() != 5)
									{
										(*functions).Last()->add_operation_callstack(false,numeric);
										numeric = 0;
									}
								}
								else
								{
									functions->Clear();
									segmentations.Clear();
									delete[] hex_string;
									return 9;
								}
								break;
							}
						}
						hex_string_position = 0;
						switch (segmentations.First().get_type())
						{
							case 0: //segment_function_segment
							{
								function_counter++;
								set_type = 1;
								break;
							}
							case 1: //segment_parameter_segment
							{
								set_type = 2;
								break;
							}
							case 2: //segment_operation_segment
							{
								function_id_stated = false;
								set_type = 3;
								break;
							}
							case 3: //segment_operation_sub_segment
							{
								set_type = 4;
								break;
							}
							case 4: //segment_real_value_segment
							{
								set_type = 5;
								break;
							}
							case 5: //segment_decimal_value_segment
							{
								set_type = 6;
								break;
							}
							case 6: //segment_negative_value_segment
							{
								set_type = 7;
								break;
							}
							default:
							{
								set_type = 0;
							}
						}
						hex_string[0] = '\0';
						segmentations.DeleteFirst();
						continue;
					}
				}
				hex_string[hex_string_position++] = program->get_current_char();
				hex_string[hex_string_position] = '\0';
				program_read++;
				program->get_next_char();
				if (hex_string_position > 8)
				{
					functions->Clear();
					segmentations.Clear();
					delete[] hex_string;
					return 11;
				}
			}
			// Overflow case check if `set_type` still in operation
			switch (set_type)
			{
				case 1:
				{
					if ((int)hex2int(hex_string) != (function_counter - 1))
					{
						functions->Clear();
						segmentations.Clear();
						delete[] hex_string;
						return 8;
					}
					else if (functions->getLength() == 0)
					{
						functions->Clear();
						segmentations.Clear();
						delete[] hex_string;
						return 9;
					}
					break;
				}
				case 2:
				{
					functions->Append(new function((function_counter - 1), (int)hex2int(hex_string)));
					break;
				}
				case 3:
				{
					if (functions->getLength() == 0)
					{
						functions->Clear();
						segmentations.Clear();
						delete[] hex_string;
						return 10;
					}
					(*functions).Last()->add_operations((int)hex2int(hex_string));
					break;
				}
				case 4:
				{
					if (get_char_size(hex_string) != 0)
					{
						if (functions->getLength() > 0)
							(*functions).Last()->add_operation_callstack((hex_current_operation != 14 || function_id_stated),(int)hex2int(hex_string));
						else
						{
							functions->Clear();
							segmentations.Clear();
							delete[] hex_string;
							return 9;
						}
					}
					else
						(*functions).Last()->add_operation_callstack(true,(int)hex2int(hex_string));
					break;
				}
				case 5:
				{
					if (get_char_size(hex_string) != 0)
					{
						(*functions).Last()->add_operation_callstack(false,(int)hex2int(hex_string));
						break;
					}
					functions->Clear();
					segmentations.Clear();
					delete[] hex_string;
					return 9;
				}
				case 6:
				{
					float numeric_decimal = (float)hex2int(hex_string);
					while (numeric_decimal > 1)
						numeric_decimal = numeric_decimal / 10;
					numeric += numeric_decimal;
					if (functions->getLength() > 0)
						(*functions).Last()->add_operation_callstack(false,numeric);
					else
						return 9;
					break;
				}
				case 7:
				{
					if (functions->getLength() > 0)
					{
						numeric = -(int)hex2int(hex_string);
						(*functions).Last()->add_operation_callstack(false,numeric);
						numeric = 0;
					}
					else
					{
						functions->Clear();
						segmentations.Clear();
						delete[] hex_string;
						return 9;
					}
					break;
				}
				default:
				{
					return 9;
				}
				segmentations.Clear();
			}
			delete[] hex_string;
		}
	}
	if (debug)
		std::cout<<" done"<<std::endl;
	delete program;
	if(functions->moveToStart())
	{
		if (debug)
			std::cout<<"Validating operation structure...";
		unsigned int total_functions = functions->getLength();
		do {
			// Validate each function
			if (!(*functions).getCurrent()->verify(total_functions,execution_version))
			{
				if (debug)
					std::cout<<" failed"<<std::endl;
				return 9;
			}
		} while (functions->next());
		if (debug)
			std::cout<<" done"<<std::endl;
	}
	if (debug)
		std::cout<<"Generating random seed...";
	srand(time(0));
	if (debug)
		std::cout<<" done"<<std::endl<<std::endl<<"Executing package:";
	unsigned int exit_status = run(&*functions,execution_version);
	if (exit_status > 0)
		exit_status = 12 + exit_status - 1;
	functions->Clear();
	return exit_status;
}

int main(int argc, char *argv[])
{
	char lan_intro[] = "The real language package interpreter";
	char* input_file_path = new char;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i],"-h") || strcmp(argv[i],"--help"))
		{
			std::cout<<lan_intro<<std::endl<<"Usage: [options] {-i input_path} ..."<<std::endl<<"Interprets a Real portable package"<<std::endl<<std::endl<<"List of options:"<<std::endl<<"  -i, --input <file>       Expects the Real portable package path to be"<<std::endl<<"                           subsequently appended after this argument"<<std::endl<<"  --debug                  Outputs what is happening to the console, use twice"<<std::endl<<"                           to view all debug information or alternatively use"<<std::endl<<"                           the argument `--debug-all`"<<std::endl<<"  --debug-all              Outputs all debug information to the console"<<std::endl<<"  --version                Prints out the version number, release number, and"<<std::endl<<"                           compilation time"<<std::endl;
			return 0;
		}
		else if (strcmp(argv[i],"-i") || strcmp(argv[i],"--input"))
		{
			if ((i + 1) < argc)
			{
				if (get_char_size(input_file_path) != 0)
				{
					std::cout<<"Another input file has already been defined!"<<std::endl;
					return 1;
				}
				if (argv[(i + 1)][0] == '-')
				{
					std::cout<<"A filepath is expected after an `"<<argv[i]<<"` argument!"<<std::endl;
					return 1;
				}
				input_file_path = argv[++i];
			}
		}
		else if (strcmp(argv[i],"--debug") || strcmp(argv[i],"--debug-all"))
		{
			if (debug || strcmp(argv[i],"--debug-all"))
				debug_all = true;
			debug = true;
		}
		else if (strcmp(argv[i],"--version"))
		{
			std::cout<<lan_intro<<std::endl<<"  Version: "<<VERSION_NUM<<std::endl<<"  Release: "<<RELEASE_NUM<<std::endl<<" Compiled: "<<__DATE__<<", "<<__TIME__<<std::endl<<std::endl<<"Written by Bradley Marshall"<<std::endl;
			return !(i == argc - 1);
		}
	}
	if (get_char_size(input_file_path) == 0)
	{
		std::cout<<"An input package has to be defined!"<<std::endl;
		return 1;
	}
	unsigned char boot_response = parse(input_file_path);
	if (debug)
	{
		if (boot_response > 1)
			std::cout<<std::endl;
		switch (boot_response)
		{
			case 0:
			{
				return 0;
			}
			case 1: // Invalid build format
			{
				return 1;
			}
			case 2:
			{
				std::cout<<"Invalid segmentor format";
				break;
			}
			case 3:
			{
				std::cout<<"Invalid segmentor value";
				break;
			}
			case 4:
			{
				std::cout<<"Invalid delimiter segmentor";
				break;
			}
			case 5:
			{
				std::cout<<"Invalid delimiter segmentation partions";
				break;
			}
			case 6:
			{
				std::cout<<"Duplicate delimiter segmentation partions";
				break;
			}
			case 7:
			{
				std::cout<<"No defined segments of any kind";
				break;
			}
			case 8:
			{
				std::cout<<std::endl<<"Function hierarchy failure";
				break;
			}
			case 9:
			{
				std::cout<<std::endl<<"Invalid function structure formation";
				break;
			}
			case 10:
			{
				std::cout<<std::endl<<"Function operation was out of scope";
				break;
			}
			case 11:
			{
				std::cout<<std::endl<<"Value overflowed from buffer";
				break;
			}
			case 12:
			{
				std::cout<<"Maximum recursion level reached";
				break;
			}
			default:
			{
				std::cout<<"Unknown error";
			}
		}
		std::cout<<std::endl;
	}
	return boot_response;
}