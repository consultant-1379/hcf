#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <linux/limits.h>

#include "hcf/trace/com_ericsson_common_hcf_core_processdaemonizer.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/core/process_daemonizer.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_PROCESS_DAEMONIZER__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::daemonize (int nochdir, int noclose, const char * daemon_name, const char * user_name, const char * group_name, const char ** secondary_groups_names) {
	HCF_CC_TRACE_FUNCTION;

	HCF_CC_TRACE(Daemonize, "Received a request to daemonize this process: setting the group ID!" , nochdir, noclose, ((daemon_name) ? daemon_name : ""), user_name, group_name);
	if (set_group_id(group_name)) {	// ERROR: changing process group id
		const int errno_save = errno;
		HCF_CC_TRACE(DaemonizeErr, "Call 'set_group_id' failed.", errno_save);
		return -1;
	}

	HCF_CC_TRACE(Daemonize, "Received a request to daemonize this process: setting the secondary groups IDs!" , nochdir, noclose, ((daemon_name) ? daemon_name : ""), user_name, group_name);
	if (set_secondary_groups_ids(secondary_groups_names)) {	// ERROR: changing process secondary groups IDs
		const int errno_save = errno;
		HCF_CC_TRACE(DaemonizeErr, "Call 'set_secondary_groups_ids' failed.", errno_save);
		return -1;
	}

	HCF_CC_TRACE(Daemonize, "Received a request to daemonize this process: setting the user ID!" , nochdir, noclose, ((daemon_name) ? daemon_name : ""), user_name, group_name);
	if (set_user_id(user_name)) {	// ERROR: changing process user id
		const int errno_save = errno;
		HCF_CC_TRACE(DaemonizeErr, "Call 'set_user_id' failed.", errno_save);
		return -1;
	}

	HCF_CC_TRACE(Daemonize, "Daemonizing the process!" , nochdir, noclose, ((daemon_name) ? daemon_name : ""), user_name, group_name);
	if (::daemon(nochdir, noclose)) {	// Error on daemonizing the running process
		const int errno_save = errno;
		HCF_CC_TRACE(DaemonizeErr, "Call 'daemon' failed.", errno_save);
		return -1;
	}

	create_pid_file();

	return 0;
}

int __TSL_CLASS_NAME__::set_user_id (const char * user_name) {
	HCF_CC_TRACE_FUNCTION;

	// Do nothing if no user and we are not running as root
	if (!user_name || !*user_name || (::getuid() && ::geteuid())){
		HCF_CC_TRACE(SetUserID, "The provided user name is NULL/empty or the process is not running as root: nothing to do!", user_name);
		return 0;
	}

	struct passwd pwd;
	struct passwd * result = 0;
	char buffer [16 * 1024] = {0};

	HCF_CC_TRACE(SetUserID, "Retrieving the information about the provided user name!", user_name);
	int call_result = ::getpwnam_r(user_name, &pwd, buffer, TSL_ARRAY_SIZE(buffer), &result);

	if (result) { // User user_name found. Changing process uid
		HCF_CC_TRACE(SetUserID, "User found: changing the process UID!", user_name);
		call_result = ::setuid(pwd.pw_uid);
	} else { // Check user not found case and set errno accordingly
		HCF_CC_TRACE(SetUserIDErr, "Failed to retrieve info about the user (user not found or "
				"error while trying to retrieve info)!", call_result);

		if (call_result == 0)	// User not found
			call_result = ENOENT;

		errno = call_result;
		call_result = -1;
	}

	return call_result;
}

int __TSL_CLASS_NAME__::set_group_id (const char * group_name) {
	HCF_CC_TRACE_FUNCTION;

	// Do nothing if no user and we are not running as root
	if (!group_name || !*group_name || (::getgid() && ::getegid())) {
		HCF_CC_TRACE(SetGroupID, "The provided group name is NULL/empty or the process is not running as root: nothing to do!", group_name);
		return 0;
	}

	struct group grp;
	struct group * result = 0;
	char buffer [16 * 1024] = {0};

	HCF_CC_TRACE(SetGroupID, "Retrieving the information about the provided group name!", group_name);
	int call_result = ::getgrnam_r(group_name, &grp, buffer, TSL_ARRAY_SIZE(buffer), &result);

	if (result) { // Group group_name found. Changing process gid
		HCF_CC_TRACE(SetGroupID, "Group found: changing the process GID!", group_name);
		call_result = ::setgid(grp.gr_gid);
	} else { // Check user not found case and set errno accordingly
		HCF_CC_TRACE(SetGroupIDErr, "Failed to retrieve info about the group (group not found or "
				"error while trying to retrieve info)!", call_result);

		if (call_result == 0) // User not found
			call_result = ENOENT;

		errno = call_result;
		call_result = -1;
	}

	return call_result;
}

int __TSL_CLASS_NAME__::set_secondary_groups_ids (const char ** secondary_groups_names) {
	HCF_CC_TRACE_FUNCTION;

	// Do nothing if no list of secondary groups is provided
	if (!secondary_groups_names) {
		HCF_CC_TRACE(SetSecondaryGroupsIDs, "The provided list of secondary groups is empty: nothing to do!", "");
		return 0;
	}

	gid_t secondary_groups_gids[NGROUPS_MAX];
	size_t number_of_secondary_groups = 0;

	// For each provided secondary group name, retrieve its GID and add to the list of GIDs
	for (int i = 0; secondary_groups_names[i]; ++i) {
		struct group grp;
		struct group * result = 0;
		char buffer [16 * 1024] = {0};

		HCF_CC_TRACE(SetSecondaryGroupsIDs, "Retrieving the information about the following secondary group:", secondary_groups_names[i]);
		int call_result = ::getgrnam_r(secondary_groups_names[i], &grp, buffer, TSL_ARRAY_SIZE(buffer), &result);

		if (result) {	// Group name found. Adding to the list of the GIDs
			HCF_CC_TRACE(SetSecondaryGroupsIDs, "Group found: adding to the list of secondary GIDs!", secondary_groups_names[i]);
			secondary_groups_gids[number_of_secondary_groups++] = grp.gr_gid;
		} else { // Check group not found case and set errno accordingly
			HCF_CC_TRACE(SetSecondaryGroupsIDsErr, "Failed to retrieve info about the group (group "
					"not found or error while trying to retrieve info)!", call_result);

			if (call_result == 0) // Group not found
				call_result = ENOENT;

			errno = call_result;
			return -1;
		}
	}

	// After retrieving the list of GIDs for the provided groups, set them as process secondary groups
	HCF_CC_TRACE(SetSecondaryGroupsIDs, "The list of groups has been retrieved, setting the process secondary groups!", "");
	if (::setgroups(number_of_secondary_groups, secondary_groups_gids)) {
		const int errno_save = errno;
		HCF_CC_TRACE(SetSecondaryGroupsIDsErr, "Failed to set the process secondary groups!", errno_save);
		errno = errno_save;
		return -1;
	}

	return 0;
}

TSL_NS_HCF_CORE_END
