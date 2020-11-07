/**
 * @file file_manager.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#ifndef file_manager_h
#define file_manager_h
#include <fstream>
#include "file.h"

class file_manager
{
private:
	std::vector<file*> open_files;
	std::vector<unsigned int> delete_after_use;
public:
	/**
	 * @brief Construct a new file manager object
	 * 
	 */
	file_manager();

	/**
	 * @brief Destroy the file manager object
	 * 
	 */
	~file_manager();

	/**
	 * @brief Generates a temporary file path
	 * 
	 * @return temporary file path
	 */
	char* tmp_path_generator();

	/**
	 * @brief Moves position of input stream to beginning
	 * 
	 * @param file_ref the file id to operate on
	 */
	void bof(unsigned int file_ref);

	/**
	 * @brief Closes the file
	 * 
	 * @param file_ref the file id to operate on
	 * @param enforce_deletion if file was created to be deleted
	 */
	void close(unsigned int file_ref, bool enforce_deletion = true);

	/**
	 * @brief Closes and reopens the file
	 * 
	 * @param file_ref the file id to operate on
	 */
	void clear(unsigned int file_ref);

	/**
	 * @brief Get the next char from stream
	 * 
	 * @param file_ref the file id to operate on
	 * @return current character in output stream 
	 */
	char get_next_char(unsigned int file_ref);

	/**
	 * @brief Gets the size of the stored file
	 * 
	 * @param file_ref the file id to operate on
	 * @return size of file 
	 */
	unsigned int get_file_size(unsigned int file_ref);

	/**
	 * @brief Moves the position of input stream to `read_position`
	 * 
	 * @param file_ref the file id to operate on
	 * @param read_position the position to be set
	 */
	void go_to_pos(unsigned int file_ref, long long int read_position);

	/**
	 * @brief Creates a new input/output stream to/from a file
	 * 
	 * @param filename the fielpath of the file to operate on
	 * @param mode the modes of operation on the file
	 * @param writing if the file is to be writen to
	 * @param auto_delete if the file shall be deleted when done
	 * @return the file id
	 */
	unsigned int open(char* filename, std::ios_base::openmode mode, bool writing, bool auto_delete);

	/**
	 * @brief Writes a char array to file
	 * 
	 * @param file_ref the file id to operate on
	 * @param s the char array to be written
	 */
	void write(unsigned int file_ref, const char * s);
};

#endif
