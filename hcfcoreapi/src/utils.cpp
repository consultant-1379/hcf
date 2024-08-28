#include <sys/sendfile.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>

#include "hcf/core/utils.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_UTILS__

TSL_NS_HCF_CORE_BEGIN

int __TSL_CLASS_NAME__::create_dir_recursive (std::string path, mode_t mode, std::string parent) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CreateDir, "Received a request to create a directory!", path.c_str(), parent.c_str(), mode);

	size_t slash_pos = path.find_first_of('/', 1);
	std::string dir_to_create = parent + ((slash_pos == std::string::npos) ? path : path.substr(0, slash_pos)) + "/";

	struct stat st;
	errno = 0;
	const int call_result = ::stat(dir_to_create.c_str(), &st);
	int errno_save = errno;

	/*
	 * The check on the folder brings to three possible results:
	 * 		1. An error was occurred => return to the caller
	 * 		2. The provided folder does not exist => create it!
	 * 		3. The provided folder already exists => nothing to do, go ahead with the remaining path!
	 */
	if ((call_result) && (errno_save != ENOENT)) {	// CASE 1
		HCF_CC_TRACE(CreateDirErr, "Call 'stat' failed.", errno_save);
		errno = errno_save;
		return -1;
	}
	else if (errno_save == ENOENT) {	// CASE 2
		HCF_CC_TRACE(CreateDir, "The first directory in the provided path does not exits, creating it!", path.c_str(), parent.c_str(), mode);
		if (::mkdir(dir_to_create.c_str(), mode) < 0) {
			// The call to mkdir is considered failed only if the errno is different from EEXIST
			errno_save = errno;
			if (errno_save != EEXIST) {
				HCF_CC_TRACE(CreateDirErr, "Call 'mkdir' failed.", errno_save);
				errno = errno_save;
				return -1;
			}
		}
	}

	return (slash_pos == std::string::npos) ? 0 : create_dir_recursive(path.substr(slash_pos+1), mode, dir_to_create);
}

int __TSL_CLASS_NAME__::copy (const char * from_file, const char * to_file) {
	HCF_CC_TRACE_FUNCTION;

	// First, open the source file in read mode
	HCF_CC_TRACE(Copy, "Received a request to copy a file content: opening input file!", from_file, to_file);
	int from_fd = -1;
	if ((from_fd = ::open(from_file, O_RDONLY)) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(CopyErr, "Call 'open' failed.", errno_save);
		return -1;
	}

	// Second, get the informations about the opened file
	HCF_CC_TRACE(Copy, "Received a request to copy a file content: retrieving information about the input file!", from_file, to_file);
	struct stat st;
	if (::fstat(from_fd, &st) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(CopyErr, "Call 'fstat' failed.", errno_save);
		::close(from_fd);
		return -1;
	}

	/*
	 * Then, open the destination file in write mode (creating it
	 * if not existing), with the same permission of the source file.
	 */
	HCF_CC_TRACE(Copy, "Received a request to copy a file content: opening output file!", from_file, to_file);
	int to_fd = -1;
	if ((to_fd = ::open(to_file, O_WRONLY | O_CREAT, st.st_mode)) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(CopyErr, "Call 'open' failed.", errno_save);
		::close(from_fd);
		return -1;
	}

	// Copy the source file content with sendfile system call
	HCF_CC_TRACE(Copy, "Received a request to copy a file content: copying file content!", from_file, to_file);
	bool failed = false;
	off_t offset = 0;
	if (::sendfile(to_fd, from_fd, &offset, st.st_size) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(CopyErr, "Call 'sendfile' failed.", errno_save);
		failed = true;
	}

	::close(from_fd);
	::close(to_fd);
	return (failed) ? -1 : 0;
}

int __TSL_CLASS_NAME__::move (const char * from, const char * to, bool change_user_and_group) {
	HCF_CC_TRACE_FUNCTION;

	// If the user/group information must be kept, return immediately, calling the rename system call
	if (!change_user_and_group)	{
		HCF_CC_TRACE(Move, "Received a request to move a file without changing the user/group settings: using 'rename' system call!", from, to);
		return ::rename(from, to);
	}

	/*
	 * Otherwise, the move operation is performed with two steps:
	 * 		1. The new file is created, copying the old content.
	 * 		2. The old file is removed.
	 */
	HCF_CC_TRACE(Move, "Received a request to move a file, also changing the user/group settings to the process "
			"current settings: performing the operation as a copy + remove operation!", from, to);
	if (copy(from, to) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(MoveErr, "Call 'copy' failed.", errno_save);
		return -1;
	}

	return remove(from);
}

int __TSL_CLASS_NAME__::remove_folder_recursive (const char * path) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(RemoveFolder, "Receive a request to remove the following folder and its content:", path);

	DIR * dir;
	if (!(dir = ::opendir(path))) {
		const int errno_save = errno;
		HCF_CC_TRACE(RemoveFolderErr, "Call 'opendir' failed.", errno_save);
		return -1;
	}

	// Then, scan the entire directory to find all the files
	HCF_CC_TRACE(RemoveFolder, "Looping over the folder content in order to recursively remove its content:", path);
	struct dirent * file;
	while ((file = ::readdir(dir))) {
		// Ignore the special files '.' and '..'
		if (!(::strcmp(file->d_name, ".")) || !(::strcmp(file->d_name, "..")))	continue;

		/*
		 * For each not-special entry, check if it's a file or not.
		 * In the first case, the file can be deleted. In case of a
		 * directory a recursive call must be performed.
		 */
		char abs_path[PATH_MAX + 1];
		::snprintf(abs_path, PATH_MAX, "%s/%s", path, file->d_name);

		struct stat st;
		HCF_CC_TRACE(RemoveFolder, "Retrieving information about the following file system item:", abs_path);
		if (::stat(abs_path, &st) < 0) {
			const int errno_save = errno;
			HCF_CC_TRACE(RemoveFolderErr, "Call 'stat' failed.", errno_save);
			::closedir(dir);
			return -1;
		}
		HCF_CC_TRACE(RemoveFolder, "Calling the right removing procedure according to the file "
				"system item type!", ((st.st_mode & S_IFDIR) ? "DIRECTORY" : "FILE"));

		if (st.st_mode & S_IFDIR)	remove_folder_recursive(abs_path);
		else if (remove(abs_path) < 0) {
			const int errno_save = errno;
			HCF_CC_TRACE(RemoveFolderErr, "Call 'remove' failed.", errno_save);
			::closedir(dir);
			return -1;
		}
	}

	// After scanning the directory, close the opened handle and remove the directory
	::closedir(dir);

	HCF_CC_TRACE(RemoveFolder, "The directory is now empty: removing it!", path);
	if (remove(path) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(RemoveFolderErr, "Call 'remove' failed.", errno_save);
		return -1;
	}

	return 0;
}

const std::string __TSL_CLASS_NAME__::calculate_time_string (const ::tm * tm_ptr) {
	HCF_CC_TRACE_FUNCTION;
	std::string time_string;

	if (tm_ptr) {
		char buffer[64] = {0};
		::snprintf(buffer, TSL_ARRAY_SIZE(buffer), "%04d-%02d-%02d %02d-%02d-%02d", (tm_ptr->tm_year + 1900),
				(tm_ptr->tm_mon + 1), tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
		time_string.assign(buffer);
	}

	return time_string;
}

TSL_NS_HCF_CORE_END
