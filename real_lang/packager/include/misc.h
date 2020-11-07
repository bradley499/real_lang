/**
 * @file misc.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#ifndef misc_h
#define misc_h

/**
 * @brief Get the char size object
 * 
 * @param char_array chat array where length is to be calculated
 * @return the length of the char array
 */
unsigned long long int get_char_size(const char *char_array);

/**
 * @brief Creates an unreferenced char array
 * 
 * @param value input char array to be dereferenced
 * @return deferenced char array
 */
const char* dereference(const char* value);

/**
 * @brief Generates a pretty error output message
 * 
 * @param message the text content of the message
 * @param line the line number (in parsed file) where the error occures
 * @param verbose if any prior outputs are verbose
 * @return the value of 1 for standard program failure
 */
unsigned char error(const char* message, long long int line, bool verbose);

/**
 * @brief Generates a hexadecimal delimiter
 * 
 * @param delimiter_numeric the numeric decimal delimiter value
 * @return hexadecimal delimiter value
 */
const char* generate_delimiter(unsigned int delimiter_numeric);

/**
 * @brief colours input text to keep uniform success/failure output grouping
 * 
 * @param text text to be colourized
 * @return colourized text
 */
const char* operation_conclusion(const char* text);

#endif
