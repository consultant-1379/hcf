#ifndef __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HEALTHCHECKM__
#define __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HEALTHCHECKM__ imm_object_healthcheckm

/**
 *  @file imm_object_healthcheckm.h
 *	@brief
 *	@author xcapdar (Dario Capua)
 *	@date 2014-11-17
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
 *	| 2014-001 | 2014-11-26 | xcapdar      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/imm/imm_object.h"

#include "srv_macros.h"
#include "app_types.h"
#include "constants.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_healthcheckm.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HEALTHCHECKM__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_IMM::imm_object {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * class_name)
		TSL_CTOR_INIT_LIST(imm_object(class_name), _last_executed_job(), _last_status(), _last_update_time(), _max_no_of_report_files(),
				_last_executed_job_old(), _last_status_old(), _last_update_time_old(), _max_no_of_report_files_old()) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//====================================================//
	// imm_object class Virtual Functions Implementation //
	//===================================================//
public:
	/**
	 * This method sets the attributes values for the current instance of the current class.
	 * The value of each class field is set according to the values present in attributes argument.
	 *
	 * @param[in]  attributes - A vector of pairs representing the new object attributes.
	 *             Each pair has as first item a string indicating the attribute name and second item
	 *             a pointer to the attribute value.
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0);

	/**
	 * This method finalizes the current instance of current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual inline int finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0) {
		HCF_CC_TRACE_FUNCTION;
		// Nothing to do for now!
		return ERR_NO_ERROR;
	}

	/**
	 * This method completes the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int complete_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0) {
		HCF_CC_TRACE_FUNCTION;
		return 0;
	}

	/**
	 * This method applies the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int apply_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0) {
		HCF_CC_TRACE_FUNCTION;
		return 0;
	}

	/**
	 * This method aborts the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0);

	/**
	 * This method updates the value of the runtime attribute with the provided name.
	 *
	 * @param[in]  attr_name - The name of the attribute to be updated.
	 * @param[out] param_to_update - A structure containing all the info about the parameter.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int update_runtime_attribute (const char * TSL_UNUSED_PAR(attr_name), TSL_NS_HCF_IMM::IMM_API_ImmParameter & TSL_UNUSED_PAR(param_to_update)) {
		HCF_CC_TRACE_FUNCTION;
		// No runtime attributes for now!
		return ERR_OPERATION_NOT_IMPLEMENTED;
	}

	/**
	 * This method executes the given administrative operation on the current object.
	 *
	 * @param[in]  operation_id - The administrative operation id.
	 * @param[in]  arguments - The administrative operation optional arguments.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int execute_admin_operation (uint64_t TSL_UNUSED_PAR(operation_id), TSL_NS_HCF_IMM::IMM_API_AdminOperationParamType ** TSL_UNUSED_PAR(arguments)) {
		HCF_CC_TRACE_FUNCTION;
		// No administrative operations for now!
		return ERR_OPERATION_NOT_IMPLEMENTED;
	}

	//==================//
	// Fields Modifiers //
	//==================//
public:
	inline std::string get_last_executed_job () const { return _last_executed_job; }
	inline void set_last_executed_job (const std::string & new_last_executed_job) {
		modify_object_attribute(HCF_HEALTHCHECKM_LAST_EXECUTED_JOB_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_NAMET, const_cast<char *>(new_last_executed_job.c_str()));
	}

	inline health_status_t get_last_status () const { return _last_status; }
	inline void set_last_status (health_status_t & new_last_status) {
			modify_object_attribute(HCF_HEALTHCHECKM_STATUS_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_INT32T, &new_last_status);
	}

	inline std::string get_last_update_time () const { return _last_update_time; }
	inline void set_last_update_time (const std::string & new_last_update_time) {
			modify_object_attribute(HCF_HEALTHCHECKM_LAST_UPDATE_TIME_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_STRINGT, const_cast<char *>(new_last_update_time.c_str()));
	}

	inline int get_max_no_of_report_files () const { return _max_no_of_report_files; }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
public:
	static const char * const HCF_HEALTHCHECKM_CLASS_NAME;
	static const char * const HCF_HEALTHCHECKM_IMPLEMENTER_NAME;
	static const char * const HCF_HEALTHCHECKM_HEALTH_CHECK_M_ID_ATTR_NAME;
	static const char * const HCF_HEALTHCHECKM_LAST_EXECUTED_JOB_ATTR_NAME;
	static const char * const HCF_HEALTHCHECKM_STATUS_ATTR_NAME;
	static const char * const HCF_HEALTHCHECKM_LAST_UPDATE_TIME_ATTR_NAME;
	static const char * const HCF_HEALTHCHECKM_MAX_NO_OF_REPORT_FILES_ATTR_NAME;
	static const char * const HCF_HEALTHCHECKM_ID_DEFAULT_VALUE;
	static const int HCF_HEALTHCHECKM_MAX_NO_OF_REPORT_FILES_DEFAULT_VALUE;

private:
	std::string _last_executed_job;
	health_status_t _last_status;
	std::string _last_update_time;
	int _max_no_of_report_files;

	// Class members to be used to implement the rollback mechanism for the abort callback
	std::string _last_executed_job_old;
	health_status_t _last_status_old;
	std::string _last_update_time_old;
	int _max_no_of_report_files_old;
};

HCF_NS_HCF_SRV_END

#endif	// __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HEALTHCHECKM__
