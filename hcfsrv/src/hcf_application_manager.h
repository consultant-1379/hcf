#ifndef __INCLUDE_GUARD_CLASS_SRV_HCF_APPLICATION_MANAGER__
#define __INCLUDE_GUARD_CLASS_SRV_HCF_APPLICATION_MANAGER__ hcf_application_manager

/*
 *	@file hcf_application_manager.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-01-07
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
 *	| 2015-001 | 2015-01-07 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "srv_macros.h"
#include "root_worker_thread.h"
#include "application_manager.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_HCF_APPLICATION_MANAGER__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_INTERNALAPI::application_manager {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * daemon_name, bool ha_mode)
		TSL_CTOR_INIT_LIST(application_manager(daemon_name), _root_worker_thread(0), _ha_mode(ha_mode)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//=====================================//
	// Application Manager class interface //
	//=====================================//
public:
	virtual int perform_state_transition_to_active (TSL_NS_HCF_INTERNALAPI::amf_ha_state_t previous_state);
	virtual int perform_state_transition_to_passive (TSL_NS_HCF_INTERNALAPI::amf_ha_state_t previous_state);
	virtual int perform_state_transition_to_quiesing (TSL_NS_HCF_INTERNALAPI::amf_ha_state_t previous_state);
	virtual int perform_state_transition_to_quiesced (TSL_NS_HCF_INTERNALAPI::amf_ha_state_t previous_state);
	virtual int perform_component_health_check ();
	virtual int perform_component_terminate ();
	virtual int perform_component_remove ();
	virtual int perform_application_shutdown ();

	//===========//
	// Functions //
	//===========//
public:
	inline bool ha_mode () { return _ha_mode; }
	inline int activate () { return (_ha_mode) ? activate_ha_mode() : activate_debug_mode(); }
	void restart_application ();

private:
	int activate_debug_mode ();
	int activate_application ();
	int shutdown_application ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	root_worker_thread * _root_worker_thread;
	bool _ha_mode;
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_HCF_APPLICATION_MANAGER__ */
