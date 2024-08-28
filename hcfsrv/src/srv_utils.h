#ifndef __INCLUDE_GUARD_CLASS_SRV_UTILS__
#define __INCLUDE_GUARD_CLASS_SRV_UTILS__ utils

/*
 *	@file utils.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-01-30
 *
 *	COPYRIGHT Ericsson AB, 2015
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+==========+============+==============+=====================================+
 *	| REV      | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+==========+============+==============+=====================================+
 *	| 2015-001 | 2015-01-30 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <dirent.h>
#include <string.h>
#include <string>
#include <set>

#include "srv_macros.h"
#include "app_types.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_UTILS__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
private:
	TSL_CTOR ();
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method calculates the current timestamp according to the following format: "YYYY-MM-DDTHH:MM:SS".
	 *
	 * @param[out] output_buffer - The buffer in which the timestamp will be written.
	 * @param[in]  output_buffer_size - The output_buffer size.
	 * @param[in]  date_separator - The string to be used to separate year, month and day.
	 * @param[in]  time_separator - The string to be used to separate hour, minute and second.
	 */
	static void get_current_timestamp (char * output_buffer, size_t output_buffer_size, const char * date_separator = "-", const char * time_separator = ":");

	/**
	 * This method maps each string representing a category into the respective value of category_t type.
	 *
	 * @param[in]  category_str - The string representing a category.
	 * @return the value representing the given category.
	 */
	static category_t map_category_string_to_category_value (const char * category_str);

	/**
	 * This method maps each value representing a category into the respective string.
	 *
	 * @param[in]  category - The value representing a category.
	 * @return the string corresponding to the given category.
	 */
	static const char * map_category_value_to_category_string (category_t category);

	/**
	 * This method maps each value representing a health status into the respective string.
	 *
	 * @param[in]  status - The value representing a health status.
	 * @return the string corresponding to the status.
	 */
	static const char * map_health_status_to_string (health_status_t status);

	/**
	 * This method checks if the provided character is a special one or not.
	 *
	 * @param[in] c - The character
	 * @return true if the character is a special one, false otherwise.
	 */
	static bool is_special_character (char c);

	/**
	 * This method escapes the special characters present into the input string.
	 * The characters currently escaped are: double quotas (""), dollar sign ($) and backslash (\).
	 *
	 * @param[in/out] string - On input, the string which contains special characters. On output it's modified with escaping.
	 * @param[in]     string_size - The size of the input string.
	 */
	static void escape_special_characters (char * string, size_t string_size);

	/**
	 * This method maps each string representing a category into the respective value of skip_log_t type.
	 *
	 * @param[in]  category_str - The string representing a category.
	 * @return the value representing the given category.
	 */
	static skip_log_t map_skip_log_string_to_skip_log_value (const char * skip);

	/**
	 * This method maps each value representing a skip log into the respective string.
	 *
	 * @param[in]  category - The value representing a category.
	 * @return the string corresponding to the given category.
	 */
	static const char * map_skip_log_value_to_skip_log_string (skip_log_t skip);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_UTILS__ */
