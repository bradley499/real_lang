/**
 * @file file.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef file_h
#define file_h
#include <fstream>

class file
{
private:
	char current_read_character;
	std::ifstream in_file;
public:
	/**
	 * @brief Construct a new file object
	 * 
	 * @param filename the file path to open
	 */
	file(const char* filename);

	/**
	 * @brief Destroy the file object
	 * 
	 */
	~file();

	/**
	 * @brief Close the file
	 * 
	 */
	void close();

	/**
	 * @brief Get the next character of the file
	 * 
	 * @return the next character
	 */
	char get_next_char();

	/**
	 * @brief Get the current character of the file
	 * 
	 * @return the current character
	 */
	char get_current_char();

	/**
	 * @brief Set the position of the file's input sequence
	 * 
	 * @param position the numerical position to read from
	 */
	void go_to_pos(unsigned int position);
};

#endif