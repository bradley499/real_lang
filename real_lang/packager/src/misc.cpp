/**
 * @file misc.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#ifndef misc_cpp
#define misc_cpp
#include "misc.h"

bool color = true;

unsigned long long int get_char_size(const char *char_array)
{
	unsigned long long int char_array_size = 0;	
	while (char_array[char_array_size] != '\0') char_array_size++;
	return char_array_size;
}

const char* dereference(const char* value)
{
	long long int char_array_length = get_char_size(value);
	char* dereference_char_array = new char[char_array_length];
	dereference_char_array[char_array_length] = '\0';
	while (char_array_length >= 0) dereference_char_array[char_array_length--] = value[char_array_length];
	return dereference_char_array;
}

unsigned char error(const char* message, long long int line, bool verbose = true)
{
	unsigned short line_display = 10;
	if (line > 9999999999)
		line_display = 33;
	else if (line < 0)
		line_display = 0;
	else
	{
		unsigned long long int line_reference = line;
		line_display = 0;
		do {
			++line_display; 
			line_reference /= 10;
		} while (line_reference);
	}
	char line_iteration[line_display];
	if (line_display > 0)
	{
		if (line > 9999999999)
			strcpy(line_iteration,"UNKNOWN (too many lines to count)");
		else
			sprintf(line_iteration,"%lld", line);
	}
	line_iteration[(sizeof(line_iteration))] = '\0';
	int message_size = 0;
	while (message[message_size] != '\0') message_size++;
	const unsigned long int response_length = message_size + line_display;
	char response[(response_length + 1)];
	int ii = 0;
	for (int i = 0; i < response_length; ++i)
	{
		if ((i + line_display) >= response_length)
		{
			if (line_iteration[ii] == '\0')
			{
				break;
			}
			response[i] = line_iteration[ii++];
		}
		else
		{
			response[i] = message[i];
		}
	}
	response[response_length] = '\0';
	if (line != -1 || verbose)
		std::cout<<std::endl<<std::endl;
	std::cout<<operation_conclusion("ERROR: ")<<response<<std::endl;
	return 1;
}

const char* generate_delimiter(unsigned int delimiter_numeric){
	if (delimiter_numeric >= 4294967294){
		return "00";
	}
	char *hex_delimiter = new char[66];
	sprintf(hex_delimiter,"%02X",delimiter_numeric);
	return hex_delimiter;
}

const char* operation_conclusion(const char* text)
{
	#ifdef colorize_output
	if (!color)
		return text;
	unsigned int text_length = get_char_size(text);
	if (strcmp(text," done") == 0)
		return " \033[0;32mdone\033[0m";
	if (text_length >= 7)
	{
		if (text[0] == ' ' && text[1] == 'f' && text[2] == 'a' && text[3] == 'i' && text[4] == 'l' && text[5] == 'e' && text[6] == 'd')
		{
			char *text_colourised = new char;
			strcat(text_colourised," \033[0;31mfailed\033[0m");
			for (unsigned int i = 7; i < text_length; i++)
				text_colourised += text[i];
			text_colourised += '\0';
			return text_colourised;
		}
		else if (text[0] == 'E' && text[1] == 'R' && text[2] == 'R' && text[3] == 'O' && text[4] == 'R' && text[5] == ':' && text[6] == ' ')
		{
			return "\033[0;31m\u001b[1m\u001b[4mERROR\033[0m\u001b[0m: ";
		}
	}
	#endif
	return text;
}

#endif
