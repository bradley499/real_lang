/**
 * @file segment_insight.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#pragma once
#ifndef segment_insight_h
#define segment_insight_h

class segment_insight
{
private:
	const unsigned long long int read_position;
	const unsigned short int segment_type;
public:
	/**
	 * @brief Construct a new segment insight object
	 * 
	 * @param set_read_position the position of the packaged file where the segment is at
	 * @param set_segment_type the type of segment from the package file
	 */
	segment_insight(const unsigned long long int set_read_position, const unsigned short int set_segment_type);

	/**
	 * @brief Get the start position of the segment
	 * 
	 * @return segment start position
	 */
	unsigned long long int get_start_position();
	
	/**
	 * @brief Get the type of the segment
	 * 
	 * @return the numeric value of the segment type
	 */
	unsigned short int get_type();
};

#endif
