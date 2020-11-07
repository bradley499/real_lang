/**
 * @file file.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#ifndef file_h
#define file_h
#include <fstream>

class file
{
private:
	std::ofstream out_stream;
	std::ifstream in_stream;
	unsigned char read_type = 0;
	char *file_path;
	const std::ios_base::openmode mode;
public:
	/**
	 * @brief Construct a new file object
	 * 
	 * @param filename the file to open
	 * @param file_mode the modes of operation on the file
	 * @param writing if the file is to be writen to
	 */

	file(char* filename, std::ios_base::openmode file_mode, bool writing);
	
	/**
	 * @brief Destroy the file object
	 * 
	 */
	~file();
	
	/**
	 * @brief Moves position of input stream to beginning
	 * 
	 */
	void bof();

	/**
	 * @brief Closes the file
	 * 
	 */
	void close();

	/**
	 * @brief Closes and reopens the file
	 * 
	 */
	void clear();

	/**
	 * @brief Get the next char from stream
	 * 
	 * @return current character in output stream
	 */
	char get_char_stream();

	/**
	 * @brief Get the file path
	 * 
	 * @return char array of file path
	 */
	char* get_file_path();

	/**
	 * @brief Moves the position of input stream to `read_position`
	 * 
	 * @param read_position the position to be set
	 */
	void go_to_pos(long long int read_position);

	/**
	 * @brief Gets the size of the stored file
	 * 
	 * @return size of file
	 */
	unsigned int get_file_size();

	/**
	 * @brief Writes the input character to file
	 * 
	 * @param s 
	 */
	void write(char s);
};

#endif
