/**
 * @file file.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef file_cpp
#define file_cpp
#include "file.h"

file::file(const char* filename)
{
	in_file = std::ifstream{filename,std::fstream::in};
}

file::~file()
{
	close();
}

void file::close()
{
	in_file.close();
}

char file::get_next_char()
{
	in_file.seekg(in_file.tellg());
	current_read_character = in_file.get();
	if (in_file.tellg() == -1)
		return '\0';
	return current_read_character;
}

char file::get_current_char()
{
	if ((int)current_read_character == -1)
		return '\0';
	return current_read_character;
}

void file::go_to_pos(unsigned int position)
{
	position++;
	for (short i = 0; i < 2; i++)
	{
		in_file.clear();
		in_file.seekg(position-1+i,in_file.beg);
		if (i == 0)
			get_next_char();
	}
}

#endif