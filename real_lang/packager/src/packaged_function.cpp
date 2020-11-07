/**
 * @file packaged_function.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#ifndef packaged_function_cpp
#define packaged_function_cpp
#include "packaged_function.h"

const char* packaged_function::int_to_char (float integer_in, const char* decimal_delimiter, const char* negative_delimiter)
{
	unsigned int delimiter_length;
	bool is_negative = false;
	if (decimal_delimiter == nullptr)
		delimiter_length = 8;
	else
		delimiter_length = 16 + get_char_size(decimal_delimiter);
	if (integer_in < 0)
	{
		is_negative = true;
		integer_in = fabs(integer_in);
	}
	char *hex_delimiter = new char[delimiter_length];
	if (ceilf(integer_in) == integer_in)
	{
		delimiter_length = 8;
		sprintf(hex_delimiter,"%08X",(int)integer_in);
	}
	else
	{
		if (decimal_delimiter == nullptr)
			return ".";
		sprintf(hex_delimiter,"%08X",(int)floorf(integer_in));
		strcat(hex_delimiter,decimal_delimiter);
		char *hex_delimiter_decimal = new char[8];
		sprintf(hex_delimiter_decimal,"%08X",(int)abs((integer_in - floorf(integer_in)) * 1000000));
		strcat(hex_delimiter,hex_delimiter_decimal);
		delete[] hex_delimiter_decimal;
	}
	bool delimiter_hex_position = false;
	unsigned int delimiter_hex_padding_start = 0;
	char *delimiter_hex_padding_verification = new char[3];
	delimiter_hex_padding_verification[2] = '\0';
	for (int i = 0; i < (8 + delimiter_length); i++)
	{
		delimiter_hex_padding_verification[delimiter_hex_position] = hex_delimiter[i];
		if (delimiter_hex_position)
		{
			if (delimiter_hex_padding_verification[0] == '0' && delimiter_hex_padding_verification[1] == '0')
				delimiter_hex_padding_start += 2;
			else
				break;
		}
		delimiter_hex_position = !delimiter_hex_position;
	}
	delete[] delimiter_hex_padding_verification;
	if (delimiter_hex_padding_start == delimiter_length)
	{
		delete[] hex_delimiter;
		return "00";
	}
	else if (delimiter_hex_padding_start > 0)
	{
		if (is_negative)
		{
			delimiter_length += get_char_size(negative_delimiter);
			//delimiter_hex_padding_start += get_char_size(negative_delimiter);
		}
		char *hex_delimiter_truncated = new char[(delimiter_length - delimiter_hex_padding_start + 1)];
		if (is_negative)
			strcpy(hex_delimiter_truncated,negative_delimiter);
		for (int i = delimiter_hex_padding_start; i < delimiter_length; ++i)
		{
			if (is_negative)
				hex_delimiter_truncated[(i - delimiter_hex_padding_start + get_char_size(negative_delimiter))] = hex_delimiter[i];
			else
				hex_delimiter_truncated[(i - delimiter_hex_padding_start)] = hex_delimiter[i];
		}
		hex_delimiter_truncated[(delimiter_length - delimiter_hex_padding_start)] = '\0';
		delete[] hex_delimiter;
		return hex_delimiter_truncated;
	}
	return hex_delimiter;
}
packaged_function::packaged_function(unsigned long long int set_function_id, std::vector<const char*> &set_default_parameters) : function_id(set_function_id){
	default_parameters = set_default_parameters;
};
unsigned long int packaged_function::new_operation(std::vector<const char*> &set_operation, std::vector<const char*> &glbl_variables, std::vector<const char*> &function_names, unsigned long long line_ref)
{
	std::vector<const char*> packaged_operations;
	for (int i = 0; i < syntax_total; ++i)
	{
		if (strcmp(syntax[i],set_operation.at(0)) == 0)
		{
			packaged_operations.push_back(int_to_char((float)i,nullptr,nullptr));
			if (strcmp(syntax[i],"parm") == 0)
			{
				for (int ii = 0; ii < default_parameters.size(); ++ii)
				{
					if (strcmp(set_operation.at(1),default_parameters.at(ii)) == 0)
					{
						packaged_operations.push_back(int_to_char((float)ii,nullptr,nullptr));
						break;
					}
				}
			}
			else
			{
				unsigned int parameter_position = 1;
				if (strcmp(syntax[i],"glbl") == 0)
				{
					for (int ii = 0; ii < glbl_variables.size(); ++ii)
					{
						if (strcmp(set_operation.at(1),glbl_variables.at(ii)) == 0)
						{
							packaged_operations.push_back(int_to_char((float)ii,nullptr,nullptr));
							parameter_position = 2;
							break;
						}
					}
				}
				else if (strcmp(syntax[i],"call") == 0)
				{
					for (int ii = 0; ii < function_names.size(); ++ii)
					{
						if (strcmp(set_operation.at(1),function_names.at(ii)) == 0)
						{
							packaged_operations.push_back(int_to_char((float)ii,nullptr,nullptr));
							parameter_position = 2;
							break;
						}
					}
				}
				for (int ii = parameter_position; ii < set_operation.size(); ++ii)
				{
					bool set = false;
					for (int iii = 0; iii < default_values_length; ++iii)
					{
						if (strcmp(default_values[iii],set_operation.at(ii)) == 0)
						{
							packaged_operations.push_back(int_to_char((float)iii,nullptr,nullptr));
							set = true;
							break;
						}
					}
					if (set)
						continue;
					else
					{
						char *packaged_numeric = new char[get_char_size(set_operation.at(ii)) + 1];
						for (int iii = 0; iii < get_char_size(set_operation.at(ii)); ++iii)
							packaged_numeric[iii] = set_operation.at(ii)[(iii)];
						packaged_numeric[get_char_size(set_operation.at(ii)) + 1] = '\0';
						if (atof(packaged_numeric) < -9999999 || atof(packaged_numeric) > 9999999)
							return line_ref + 1;
						char *packaged_numeric_real = new char[get_char_size(set_operation.at(ii)) + 2];
						packaged_numeric_real[0] = 'n';
						strcat(packaged_numeric_real,packaged_numeric);
						packaged_operations.push_back(packaged_numeric_real);
						delete packaged_numeric;
					}
				}
			}
			break;
		}
	}
	operations.push_back(packaged_operations);
	packaged_operations.clear();
	return 0;
}
build_structure* packaged_function::build(const char *segment_function, const char* segment_parameter, const char* segment_operation, const char* segment_operation_sub, const char* segment_real_value, const char* segment_decimal, const char* segment_negative)
{
	unsigned long long int segment_function_size = get_char_size(segment_function);
	unsigned long long int segment_parameter_size = get_char_size(segment_parameter);
	unsigned long long int segment_operation_size = get_char_size(segment_operation);
	unsigned long long int segment_operation_sub_size = get_char_size(segment_operation_sub);
	
	unsigned long long int build_string_size = 0;
	
	unsigned long long int total_segments = 0;
	unsigned long long int total_segment_subs = 0;
	unsigned long long int total_real_values = 0;
	unsigned long long int total_decimal_values = 0;
	unsigned long long int total_negative_values = 0;
	

	build_string_size += segment_function_size;
	build_string_size += get_char_size(int_to_char(function_id,nullptr,nullptr));
	build_string_size += segment_parameter_size;
	build_string_size += get_char_size(int_to_char(default_parameters.size(),nullptr,nullptr));
	for (int i = 0; i < operations.size(); ++i)
	{
		build_string_size += segment_operation_size;
		total_segments++;
		build_string_size += get_char_size(operations.at(i).at(0));
		if (operations.at(i).size() > 1)
		{
			for (int ii = 1; ii < operations.at(i).size(); ++ii)
			{
				build_string_size += segment_operation_sub_size;
				total_segment_subs++;
				if (get_char_size(operations.at(i).at(ii)) > 1)
				{
					if (operations.at(i).at(ii)[0] == 'n')
					{
						build_string_size += total_real_values;
						total_real_values++;
						char* substr = new char[(get_char_size(operations.at(i).at(ii)) + 1)];
						for (int iii = 1; iii < get_char_size(operations.at(i).at(ii)); ++iii)
							substr[(iii - 1)] = operations.at(i).at(ii)[iii];
						substr[get_char_size(operations.at(i).at(ii))] = '\0';
						if ((float)atof(substr) != (int)atof(substr))
							total_decimal_values++;
						float float_value = (float)atof(substr);
						if (float_value < 0)
							total_negative_values++;
						build_string_size += (get_char_size(int_to_char(float_value,segment_decimal,segment_negative)));
						delete[] substr;
						continue;
					}
					else
						build_string_size += get_char_size(operations.at(i).at(ii));
				}
				build_string_size += get_char_size(operations.at(i).at(ii));
			}
		}
	}
	build_string_size++;
	char output[build_string_size];
	strcpy(output,segment_function);
	strcat(output,int_to_char(function_id,nullptr,nullptr));
	strcat(output,segment_parameter);
	strcat(output,int_to_char(default_parameters.size(),nullptr,nullptr));
	for (int i = 0; i < operations.size(); ++i)
	{
		strcat(output,segment_operation);
		strcat(output,operations.at(i).at(0));
		if (operations.at(i).size() > 1)
		{
			for (int ii = 1; ii < operations.at(i).size(); ++ii)
			{
				strcat(output,segment_operation_sub);
				if (get_char_size(operations.at(i).at(ii)) > 1)
				{
					if (operations.at(i).at(ii)[0] == 'n')
					{
						strcat(output,segment_real_value);
						char* substr = new char[get_char_size(operations.at(i).at(ii) + 1)];
						for (int iii = 1; iii <= get_char_size(operations.at(i).at(ii)); ++iii)
							substr[(iii - 1)] = operations.at(i).at(ii)[iii];
						substr[get_char_size(operations.at(i).at(ii))] = '\0';
						strcat(output,int_to_char((float)atof(substr),segment_decimal,segment_negative));
						delete[] substr;
						continue;
					}
				}
				strcat(output,operations.at(i).at(ii));
			}
		}
	}
	output[build_string_size] = '\0';
	return new build_structure(total_segments,total_segment_subs,total_real_values,total_decimal_values,total_negative_values,output);
}

#endif
