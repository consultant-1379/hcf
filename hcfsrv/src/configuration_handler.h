#ifndef __INCLUDE_GUARD_CLASS_SRV_CONFIGURATION_HANDLER__
#define __INCLUDE_GUARD_CLASS_SRV_CONFIGURATION_HANDLER__ configuration_handler

/**
 *  @file configuration_handler.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-06-22
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
 *	| 2015-001 | 2015-06-22 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/imm/imm_observer.h"

#include "srv_macros.h"
#include "constants.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CONFIGURATION_HANDLER__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_IMM::imm_observer {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const std::string & implementer_name)
		TSL_CTOR_INIT_LIST(imm_observer(std::vector<std::string>(_configuration_class_names,
				(_configuration_class_names + _number_of_configuration_classes)), implementer_name),
				_managed_element_id(), _managed_element_id_old(), _amf_nodes_added_or_removed(false))
		{}

private:
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
	 * This method initializes the object previously created. It must be called only once.
	 *
	 * @return On success, 0 is returned. In the other case a negative value is returned.
	 */
	virtual int initialize ();

private:
	/**
	 * This method is used during initialization phases to load the initial value of the MEID.
	 *
	 * @return 0 on success, a non zero value otherwise.
	 */
	int load_managed_element_id_value ();

	/**
	 * This method handles a creation/modification notification regarding the ManagedElement class.
	 * The main operation performed is the extraction of the MEID for the current node.
	 *
	 * @param[in]  attributes - A vector of pairs representing the new object attributes.
	 *             Each pair has as first item a string indicating the attribute name and second item
	 *             a pointer to the attribute value.
	 */
	void handle_managed_element_class_notification (std::vector<std::pair<std::string, std::vector<void *> > > & attributes);

	/**
	 * This method handles a creation/deletion notification regarding the SaAmfNode class.
	 * This is needed in order to notify the configurator class after a cluster resize operation.
	 */
	inline void handle_amf_node_class_notification () { _amf_nodes_added_or_removed = true; }

	//======================================================//
	// imm_observer class Virtual Functions Implementation //
	//=====================================================//
public:
	/**
	 * This method is used to notify the information about the object creation.
	 * The attributes of the new object instance are also passed to the method.
	 *
	 * @param[in]  class_name - The name of the class which the created object belongs to.
	 * @param[in]  parent_dn - The DN under which the object will be created.
	 * @param[in]  attributes - A vector of pairs representing the new object attributes.
	 *             Each pair has as first item a string indicating the attribute name and second item
	 *             a pointer to the attribute value.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int notify_object_creation (const char * class_name, const char * parent_dn, std::vector<std::pair<std::string, std::vector<void *> > > & attributes);

	/**
	 * This method is used to notify the information about the object modification.
	 * The updated class attributes for the provided object are also passed to the method.
	 *
	 * @param[in]  obj_dn - The DN of the updated object.
	 * @param[in]  attributes - A vector of pairs representing the updated attributes.
	 *             Each pair has as first item a string indicating the attribute name and second item
	 *             a pointer to the attribute value.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int notify_object_modification (const char * obj_dn, std::vector<std::pair<std::string, std::vector<void *> > > & attributes);

	/**
	 * This method is used to notify the information about the object deletion.
	 *
	 * @param[in]  obj_dn - The DN of the deleted object.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int notify_object_deletion (const char * obj_dn);

	/**
	 * This method is used to notify the information about the completion of the ongoing callback.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int notify_object_modification_completion () { return ERR_NO_ERROR; }

	/**
	 * This method is used to notify the information about the failure of the ongoing callback.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int notify_object_modification_abort ();

	/**
	 * This method is used to notify the information about the finalization (with success) of the ongoing callback.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int notify_object_modification_application ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
public:
	static const char * const HCF_CONFIGURATION_HANDLER_IMPLEMENTER_NAME;
	static const char * const HCF_MANAGED_ELEMENT_CLASS_NAME;
	static const char * const HCF_AMF_NODE_CLASS_NAME;
	static const char * const HCF_MANAGED_ELEMENT_DEFAULT_DN;
	static const char * const HCF_NETWORK_MEID_ATTR_NAME;
	static const char * const HCF_MANAGED_ELEMENT_ID_DEFAULT_VALUE;
	static const char * const HCF_AMF_NODE_DN_PATTERN_VALUE;

private:
	static const int HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES;
	static std::string _configuration_class_names[];
	static int _number_of_configuration_classes;

	std::string _managed_element_id;
	std::string _managed_element_id_old;
	bool _amf_nodes_added_or_removed;
};

HCF_NS_HCF_SRV_END

#endif	// __INCLUDE_GUARD_CLASS_SRV_CONFIGURATION_HANDLER__
