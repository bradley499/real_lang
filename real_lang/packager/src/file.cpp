/**
 * @file file.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#ifndef file_cpp
#define file_cpp
#include "file.h"
#include <sys/stat.h>

file::file(char* filename, std::ios_base::openmode file_mode, bool writing) : file_path(filename), mode(file_mode)
{
	read_type = !writing + 1;
	if (writing)
		out_stream = std::ofstream{filename, mode};
	else
		in_stream = std::ifstream{filename, mode};
};

file::~file()
{
	close();
	delete file_path;
}

void file::bof()
{
	if (read_type != 2)
		return;
	in_stream.clear();
	in_stream.seekg(in_stream.beg);
}

void file::close()
{
	if (read_type == 1)
	{
		if (out_stream.is_open())
		{
			out_stream.flush();
			out_stream.close();
		}
	}
	else if (read_type == 2)
	{
		if (in_stream.is_open())
		{
			bof();
			in_stream.close();
		}
	}
}

void file::clear()
{
	close();
	out_stream.open(file_path, std::ofstream::out | std::ofstream::trunc);
	close();
	out_stream.open(file_path,mode);
}

char file::get_char_stream()
{
	// returns the next character in the file input stream
	in_stream.seekg(in_stream.tellg());
	char character = in_stream.get();
	return character;
}

char* file::get_file_path()
{
	return file_path;
}

unsigned int file::get_file_size()
{
	if (in_stream.is_open())
	{
		struct stat buffer;
		if (stat (get_file_path(), &buffer) != 0)
			return 0;
	}				
	in_stream.seekg(0,out_stream.end);
	unsigned int length = in_stream.tellg();
	bof();
	return length;
}

void file::go_to_pos(long long int read_position)
{
	in_stream.clear();
	in_stream.seekg(read_position);
}

void file::write(char s)
{
	char *to_write = new char[2]{s,'\0'};
	out_stream << to_write;
	out_stream.flush();
	delete to_write;
}

#endif
