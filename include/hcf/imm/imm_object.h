#ifndef __INCLUDE_GUARD_CLASS_IMM_IMM_OBJECT__
#define __INCLUDE_GUARD_CLASS_IMM_IMM_OBJECT__ imm_object

/**
 *  @file imm_object.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-11-20
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
 *	| 2014-001 | 2014-11-20 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <string>
#include <vector>

#include "hcf/core/thread_mutex.h"
#include "hcf/imm/object_implementer_interface.h"

#include "imm_types.h"
#include "om_handler.h"
#include "imm_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_IMM_OBJECT__

TSL_NS_HCF_IMM_BEGIN

class __TSL_CLASS_NAME__ {
	//==========//
	// Typedefs //
	//==========//
protected:
	typedef TSL_NS_HCF_CORE::thread_mutex_recursive sync_t;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * class_name)
		TSL_CTOR_INIT_LIST(_class_name(class_name), _obj_dn(), _obj_rdn(), _obj_id(),
				_om_handler(), _om_handler_initialized(false), _om_handler_mutex()) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () { finalize_om_handler(); }

	//============//
	// Functions //
	//===========//
public:
	/**
	 * This method returns the name of the IMM class of the current object.
	 *
	 * @return a string representing the IMM class name.
	 */
	inline std::string get_class_name () { return _class_name; }

	/**
	 * This method returns the rdn of the current object.
	 *
	 * @return a string representing the object rdn.
	 */
	inline std::string get_rdn () { return _obj_rdn; }

	/**
	 * This method returns the complete dn of the current object.
	 *
	 * @return a string representing the object dn.
	 */
	inline std::string get_dn () { return _obj_dn; }

	/**
	 * This method returns an ID for the current object.
	 *
	 * @return a string representing the object id.
	 */
	inline std::string get_id () { return _obj_id; }

	/**
	 * This method returns an error message string indicating why an operation on the object failed.
	 *
	 * @return the error message string.
	 */
	inline std::string get_error_message () const { return _error_message; }

protected:
	/**
	 * This method sets the error message indicating the reason for which the operation failed.
	 *
	 * @param[in]  error_message - The error message string.
	 */
	inline void set_error_message (const std::string & error_message) { _error_message = error_message; }

	//===================//
	// Virtual Functions //
	//===================//
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
	virtual int set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0) = 0;

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
	 * This method finalizes the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0) = 0;

	/**
	 * This method completes the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int complete_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0) = 0;

	/**
	 * This method applies the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int apply_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0) = 0;

	/**
	 * This method aborts the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0) = 0;

	/**
	 * This method updates the value of the runtime attribute with the provided name.
	 *
	 * @param[in]  attr_name - The name of the attribute to be updated.
	 * @param[out] param_to_update - A structure containing all the info about the parameter.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int update_runtime_attribute (const char * attr_name, IMM_API_ImmParameter & param_to_update) = 0;

	/**
	 * This method executes the given administrative operation on the current object.
	 *
	 * @param[in]  operation_id - The administrative operation id.
	 * @param[in]  arguments - The administrative operation optional arguments.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int execute_admin_operation (uint64_t operation_id, IMM_API_AdminOperationParamType ** arguments) = 0;

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

	/**
	 * This method deletes the current IMM object instance.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int delete_object ();

protected:
	/**
	 * This method creates a new instance of the current class.
	 *
	 * @param[in]  class_name - The class name of the object to be created.
	 * @param[in]  parent_name - The dn of the parent object.
	 * @param[in]  attr_values - A vector of IMM_API_ValuesDefinitionTypes items representing the object attributes.
	 * @return 0 is returned on success, -1 on failure and 1 if the object already exists.
	 */
	static int create_object (const char * class_name, const char * parent_name, std::vector<IMM_API_ValuesDefinitionTypes> & attr_values);

	/**
	 * This method changes one of the attribute of the current object.
	 *
	 * @param[in]  attribute_name - The name of the attribute which value must be changes.
	 * @param[in]  attribute_type - The data type of the attribute to be changed.
	 * @param[in]  attribute_value - The new value of the attribute to be changed.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int modify_object_attribute (const char * attribute_name, IMM_API_AttrValueTypes attribute_type, void * attribute_value);

	/**
	 * This method changes one of the attribute of the current object.
	 *
	 * @param[in]  attribute_name - The name of the attribute which value must be changes.
	 * @param[in]  attribute_type - The data type of the attribute to be changed.
	 * @param[in]  num_of_attr - The number of values to be modified.
	 * @param[in]  attribute_values - The new values of the attribute to be changed.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int modify_object_attribute (const char * attribute_name, IMM_API_AttrValueTypes attribute_type, int num_of_attr, void ** attribute_values);

	/**
	 * This method initializes the om_handler object if not done.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int initialize_om_handler ();

private:
	/**
	 * This method finalizes the om_handler object if not done.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int finalize_om_handler ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
protected:
	static const int HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES = 20;

	std::string _class_name;
	std::string _obj_dn;
	std::string _obj_rdn;
	std::string _obj_id;
	om_handler _om_handler;
	bool _om_handler_initialized;
	TSL_NS_HCF_CORE::thread_mutex_recursive _om_handler_mutex;

private:
	std::string _error_message;
};

TSL_NS_HCF_IMM_END

#endif	// __INCLUDE_GUARD_CLASS_IMM_IMM_OBJECT__
