/**
 * @file file_manager.cpp
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#ifndef file_manager_cpp
#define file_manager_cpp
#include "file_manager.h"
#include "file.cpp"
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <stdio.h>

file_manager::file_manager()
{
}

file_manager::~file_manager()
{
	for (unsigned int i = 0; i < open_files.size(); i++)
		close(i,true);
	open_files.clear();
}

char* file_manager::tmp_path_generator()
{
	const char tmp_skel[] = "/tmp/real-lang_packaging-";
	for (int i = 1; i <= 1024; ++i)
	{
		char *tmp_path_generator = new char[30];
		strcpy(tmp_path_generator,tmp_skel);
		strcat(tmp_path_generator,std::to_string(i).c_str());
		strcat(tmp_path_generator,"\0");
		struct stat buffer;
		if (!(stat (tmp_path_generator, &buffer) == 0))
		{
			return tmp_path_generator;
			break;
		}
	}
	return new char;
}

void file_manager::bof(unsigned int file_ref)
{
	open_files[file_ref]->bof();
}

unsigned int file_manager::open(char* filename, std::ios_base::openmode mode, bool writing, bool auto_delete)
{
	unsigned int pos = 0;
	std::vector<int> directories;
	while (filename[pos++] != '\0')
		if (filename[pos] == '/' || filename[pos] == '\\')
			directories.push_back(pos);
	pos = 0;
	if (directories.size() > 0)
	{
		do
		{
			char directory_path[directories.at(pos)];
			strncpy(directory_path,filename,directories.at(pos));
			directory_path[directories.at(pos)] = '\0';
			struct stat buffer;
			if (stat(directory_path, &buffer) != 0)
				mkdir(directory_path, 0733);
			pos++;
		} while (pos < directories.size());
	}
	open_files.emplace_back(new file(filename,mode,writing));
	if (auto_delete)
		delete_after_use.push_back(open_files.size() - 1);
	return open_files.size() - 1;
}

void file_manager::close(unsigned int file_ref, bool enforce_deletion)
{
	open_files[file_ref]->close();
	if (enforce_deletion)
	{
		for (unsigned int i = 0; i < delete_after_use.size(); i++)
		{
			if (delete_after_use[i] == file_ref)
			{
				struct stat buffer;
				if (stat(open_files[file_ref]->get_file_path(), &buffer) == 0)
				{
					if (remove(open_files[file_ref]->get_file_path()) != 0)
					{
						char error_message[] = "Failed to remove the file \"";
						strcat(error_message,open_files[file_ref]->get_file_path());
						strcat(error_message,"\"\0");
						error(error_message,-1);
					}
				}
				return;
			}
		}
	}
}

void file_manager::clear(unsigned int file_ref)
{
	open_files[file_ref]->clear();
}

char file_manager::get_next_char(unsigned int file_ref)
{
	return open_files[file_ref]->get_char_stream();
}

unsigned int file_manager::get_file_size(unsigned int file_ref)
{
	return open_files[file_ref]->get_file_size();
}

void file_manager::go_to_pos(unsigned int file_ref, long long int read_position)
{
	read_position -= 1;
	if (read_position < 0)
		read_position = 0;
	open_files[file_ref]->go_to_pos(read_position);
}

void file_manager::write(unsigned int file_ref, const char* s)
{
	unsigned int length = 0;
	std::string ss = std::string(s);
	while (length < ss.size())
	{
		char to_write = (char)s[length++];
		if (to_write == '0' || to_write == '1' || to_write == '2' || to_write == '3' || to_write == '4' || to_write == '5' || to_write == '6' || to_write == '7' || to_write == '8' || to_write == '9' || to_write == 'A' || to_write == 'B' || to_write == 'C' || to_write == 'D' || to_write == 'E' || to_write == 'F')
			open_files[file_ref]->write(to_write);
	}
}

#endif
