#ifndef __INCLUDE_GUARD_CLASS_CORE_PROCESS_DAEMONIZER__
#define __INCLUDE_GUARD_CLASS_CORE_PROCESS_DAEMONIZER__ process_daemonizer

/*
 *	@file process_daemonizer.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-10
 *
 *	COPYRIGHT Ericsson AB, 2014
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
 *	| 2014-001 | 2014-11-10 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_PROCESS_DAEMONIZER__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
private:
	inline TSL_CTOR () {}

	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	static inline int daemonize () { return daemonize(0, 0, 0, 0, 0, 0); }
	static inline int daemonize (int nochdir, int noclose) { return daemonize(nochdir, noclose, 0, 0, 0, 0); }
	static inline int daemonize (const char * daemon_name, const char * user_name, const char * group_name, const char ** secondary_groups_names) {
		return daemonize(0, 0, daemon_name, user_name, group_name, secondary_groups_names);
	}
	static int daemonize (int nochdir, int noclose, const char * daemon_name, const char * user_name, const char * group_name, const char ** secondary_groups_names);

private:
	static int set_user_id (const char * user_name);

	static int set_group_id (const char * group_name);

	static int set_secondary_groups_ids (const char ** secondary_groups_names);

	static inline int create_pid_file () { return 0; }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_PROCESS_DAEMONIZER__ */
