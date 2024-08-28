#ifndef __INCLUDE_GUARD_CLASS_INTERNALAPI_APPLICATION_MANAGER__
#define __INCLUDE_GUARD_CLASS_INTERNALAPI_APPLICATION_MANAGER__ application_manager

/*
 *	@file application_manager.h
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
#include <linux/limits.h>
#include <string.h>

#include "saAmf.h"

#include "internalapi_macros.h"
#include "internalapi_types.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_INTERNALAPI_APPLICATION_MANAGER__

TSL_NS_HCF_INTERNALAPI_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * daemon_name)
		TSL_CTOR_INIT_LIST(_component_name(), _amf_handle(0), _selectable_obj(0),
				_ha_state(AMF_HA_UNDEFINED), _health_check_key(0), _termination_event_received(false))
		{ ::strncpy(_daemon_name, daemon_name, TSL_ARRAY_SIZE(_daemon_name)); }

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	inline int get_ha_state () const { return _ha_state; }
	inline int get_selectable_obj () const { return _selectable_obj; }
	int activate_ha_mode ();
	int component_report_error (amf_recommended_recovery_t recovery_action);
	int component_clear_error ();

	virtual int perform_state_transition_to_active (amf_ha_state_t previous_state) = 0;
	virtual int perform_state_transition_to_passive (amf_ha_state_t previous_state) = 0;
	virtual int perform_state_transition_to_quiesing (amf_ha_state_t previous_state) = 0;
	virtual int perform_state_transition_to_quiesced (amf_ha_state_t previous_state) = 0;
	virtual int perform_component_health_check () = 0;
	virtual int perform_component_terminate () = 0;
	virtual int perform_component_remove () = 0;
	virtual int perform_application_shutdown () = 0;

private:
	int initialize ();
	int finalize (bool termination_event_received);
	int dispatch (amf_dispatch_flags_t dispatch_flags);
	static void application_shutdown_handler (int signum);
	static void coremw_csi_set_callback (SaInvocationT invocation, const SaNameT * comp_name, SaAmfHAStateT ha_state, SaAmfCSIDescriptorT csi_descr);
	static void coremw_csi_remove_callback(SaInvocationT invocation, const SaNameT * comp_name, const SaNameT * csi_name, SaAmfCSIFlagsT flags);
	static void coremw_csi_terminate_callback(SaInvocationT invocation, const SaNameT * comp_name);
	static void coremw_healthcheck_callback(SaInvocationT invocation, const SaNameT * comp_name, SaAmfHealthcheckKeyT * health_check_key);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	static int _shutdown_pipe_fd[2];
	static __TSL_CLASS_NAME__ * _application_manager;

	char _daemon_name[PATH_MAX + 1];
	SaNameT _component_name;
	SaAmfHandleT _amf_handle;
	selectable_obj_t _selectable_obj;
	amf_ha_state_t _ha_state;
	amf_health_check_key_t * _health_check_key;
	bool _termination_event_received;
};

TSL_NS_HCF_INTERNALAPI_END

#endif /* __INCLUDE_GUARD_CLASS_INTERNALAPI_APPLICATION_MANAGER__ */
