#ifndef __INCLUDE_GUARD_CLASS_CORE_UTILS__
#define __INCLUDE_GUARD_CLASS_CORE_UTILS__ utils

/*
 *	@file utils.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-02-06
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
 *	| 2015-001 | 2015-02-06 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <sys/stat.h>
#include <sys/time.h>

#include <dirent.h>
#include <string.h>
#include <errno.h>

#include <string>
#include <set>

#include "macros.h"

#include "hcf/trace/com_ericsson_common_hcf_core_utils.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_UTILS__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

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
	 * This method creates recursively the folder having the provided path.
	 * It's equivalent to "mkdir -p" bash command.
	 *
	 * @param[in]  path - The path to be created.
	 * @param[in]  mode - The permissions to be used to create the directory.
	 * @param[in]  parent - The parent of the path to be created.
	 * @return 0 on success, -1 on failure.
	 */
	static int create_dir_recursive (std::string path, mode_t mode = 0755, std::string parent = "");

	/**
	 * This method copies the content of the first file into a newly created file.
	 *
	 * @param[in]  from_file - The path of the input file.
	 * @param[in]  to_file - The path of the output file to be created.
	 * @return 0 on success, -1 on failure.
	 */
	static int copy (const char * from_file, const char * to_file);

	/**
	 * This method renames a file, moving it between directories if required.
	 * It is also possible to change the file user/group to the calling process ones.
	 *
	 * @param[in]  from_file - The path of the input file.
	 * @param[in]  to_file - The path of the output file. If already exists it is replaced.
	 * @return 0 on success, -1 on failure.
	 */
	static int move (const char * from, const char * to, bool change_user_and_group = false);

	/**
	 * This method deletes the file having the provided path.
	 *
	 * @param[in]  file_path - The path of the file to be removed.
	 * @return 0 on success, -1 on failure.
	 */
	static inline int remove (const char * file_path) { return ::remove(file_path); }

	/**
	 * This method deletes recursively the folder having the provided path and its content.
	 *
	 * @param[in]  path - The path of the folder to be removed.
	 * @return 0 on success, -1 on failure.
	 */
	static int remove_folder_recursive (const char * path);

	/**
	 * This method lists all the files under the specified directory.
	 * It's also possible to provide an implementation of a string comparator to be used to sort the input set.
	 *
	 * @param[in]  directory - The path of the directory under which the files must be listed.
	 * @param[out] files - The (sorted) set containing the retrieved files.
	 * @param[in]  include_entry - An optional pointer to a callback function providing a
	 * 									condition to the inclusion of the entry in the output set.
	 * @return 0 on success, -1 on failure.
	 */
	template <typename lesser_t>
	static int list (const char * directory, std::set<std::string, lesser_t> & files, bool (*include_entry)(struct dirent *) = 0) {
		HCF_CC_TRACE_FUNCTION;

		// First, open the directory having the provided path
		HCF_CC_TRACE(List, "Received a request to retrieve all the files in the following directory: opening it!", directory);
		DIR * dir;
		if (!(dir = ::opendir(directory))) {
			const int errno_save = errno;
			HCF_CC_TRACE(ListErr, "Call 'opendir' failed.", errno_save);
			return -1;
		}

		// Then, scan the entire directory to find all the files
		HCF_CC_TRACE(List, "Received a request to retrieve all the files in the following directory: reading the directory content!", directory);
		struct dirent * file;
		while ((file = ::readdir(dir))) {
			// Ignore the special files '.' and '..'
			if (!(::strcmp(file->d_name, ".")) || !(::strcmp(file->d_name, "..")))	continue;

			// If provided, invoke the inclusion check callback function
			if ((include_entry) && (!include_entry(file))) {
				// Callback function not provided or false value returned: skipping file!
				HCF_CC_TRACE(List, "The following file will be skipped:", file->d_name);
			}
			else {
				// All the other files must be added to the provided set
				HCF_CC_TRACE(List, "Found the following file:", file->d_name);
				files.insert(file->d_name);
			}
		}

		// After scanning the directory, close the opened handle
		::closedir(dir);
		return 0;
	}

	/**
	 * This method creates a string object starting from the tm struct provided.
	 *
	 * @param[in]  tm_ptr - The pointer to the structure containing the date info.
	 * @return the string object with the time information
	 */
	static const std::string calculate_time_string (const ::tm * tm_ptr);

	/**
	 * This method creates a string object starting from the timespec struct provided.
	 *
	 * @param[in]  ts - The pointer to the structure containing the time info.
	 * @return the string object with the time information
	 */
	static inline const std::string calculate_time_string (const ::timespec * ts) {
		::timeval tv;
		TIMESPEC_TO_TIMEVAL(&tv, ts);
		return calculate_time_string(&tv);
	}

	/**
	 * This method creates a string object starting from the timeval struct provided.
	 *
	 * @param[in]  tv - The pointer to the structure containing the time info.
	 * @return the string object with the time information
	 */
	static inline const std::string calculate_time_string (const ::timeval * tv) {
		::tm tm;
		::localtime_r(&(tv->tv_sec), &tm);
		return calculate_time_string(&tm);
	}

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

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_UTILS__ */
