#ifndef __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE_PARAMETER__
#define __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE_PARAMETER__ imm_object_hcrule_parameter

/**
 *  @file imm_object_hcrule_parameter.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-01-26
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
 *	| 2015-001 | 2015-01-26 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/imm/imm_object.h"

#include "constants.h"
#include "srv_macros.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_hcruleparameter.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE_PARAMETER__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_IMM::imm_object {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * class_name)
		TSL_CTOR_INIT_LIST(imm_object(class_name), _hcrule_id(), _name(),
				_value(), _job(), _hcrule_id_old(), _name_old(), _value_old()) {}

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
	 * This method sets the attributes values for the current instance of the current class.
	 * The value of each class field is set according to the values present in attributes argument.
	 *
	 * @param[in]  attributes - A vector of pairs representing the new object attributes.
	 *             Each pair has as first item a string indicating the attribute name and second item
	 *             is a vector of values.
	 * @param[in]  parent_dn - The dn of the parent object of the current one.
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int set_object_attributes_and_dn (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, const char * parent_dn, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info);

	/**
	 * This method finalizes the current instance of current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0);

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

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method extracts the ID for the current object type from the provided DN.
	 *
	 * @param[in]  obj_dn - The DN from which the ID must be extracted.
	 * @return a string object representing the extracted ID.
	 */
	static std::string get_obj_id_from_dn (const char * obj_dn);

	//==================//
	// Fields Modifiers //
	//==================//
public:
	inline std::string get_hcrule_id () const { return _hcrule_id; }

	inline std::string get_name () const { return _name; }

	inline std::string get_value () const { return _value; }

	inline std::string get_job () const { return _job; }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
public:
	static const char * const HCF_HCRULEPARAMETER_CLASS_NAME;
	static const char * const HCF_HCRULEPARAMETER_IMPLEMENTER_NAME;
	static const char * const HCF_HCRULEPARAMETER_ID_ATTR_NAME;
	static const char * const HCF_HCRULEPARAMETER_HCRULE_ATTR_NAME;
	static const char * const HCF_HCRULEPARAMETER_NAME_ATTR_NAME;
	static const char * const HCF_HCRULEPARAMETER_VALUE_ATTR_NAME;
	static const char * const HCF_HCRULEPARAMETER_HCRULE_RDN_PREFIX;
	static const char * const HCF_HCRULEPARAMETER_ID_SEPARATOR_VALUE;

private:
	std::string _hcrule_id;
	std::string _name;
	std::string _value;
	std::string _job;

	// Class members to be used to implement the rollback mechanism for the abort callback
	std::string _hcrule_id_old;
	std::string _name_old;
	std::string _value_old;
};

HCF_NS_HCF_SRV_END

#endif	// __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE_PARAMETER__
