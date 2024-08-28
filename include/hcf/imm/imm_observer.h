#ifndef __INCLUDE_GUARD_CLASS_IMM_IMM_OBSERVER__
#define __INCLUDE_GUARD_CLASS_IMM_IMM_OBSERVER__ imm_observer

/**
 *  @file imm_observer.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-06-23
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
 *	| 2015-001 | 2015-06-23 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <string>
#include <vector>

#include "hcf/core/event_handler.h"

#include "imm_macros.h"
#include "object_implementer_interface.h"
#include "object_implementer_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_IMM_OBSERVER__

TSL_NS_HCF_IMM_BEGIN

class __TSL_CLASS_NAME__ : public object_implementer_interface, public TSL_NS_HCF_CORE::event_handler {
	//==============//
	// Constructors //
	//==============//
public:
	TSL_CTOR (std::vector<std::string> classes, const std::string & implementer_name);

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () { (_object_initialized) && finalize(); }

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

	/**
	 * This method finalizes the object before destroying it. It must be called only once.
	 *
	 * @return On success, 0 is returned. In the other case a negative value is returned.
	 */
	virtual int finalize ();

	//===================//
	// Virtual Functions //
	//===================//
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
	virtual int notify_object_creation (const char * class_name, const char * parent_dn, std::vector<std::pair<std::string, std::vector<void *> > > & attributes) = 0;

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
	virtual int notify_object_modification (const char * obj_dn, std::vector<std::pair<std::string, std::vector<void *> > > & attributes) = 0;

	/**
	 * This method is used to notify the information about the object deletion.
	 *
	 * @param[in]  obj_dn - The DN of the deleted object.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int notify_object_deletion (const char * obj_dn) = 0;

	/**
	 * This method is used to notify the information about the completion of the ongoing callback.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int notify_object_modification_completion () = 0;

	/**
	 * This method is used to notify the information about the failure of the ongoing callback.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int notify_object_modification_abort () = 0;

	/**
	 * This method is used to notify the information about the finalization (with success) of the ongoing callback.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int notify_object_modification_application () = 0;

	//=====================================================================//
	// object_implementer_interface class Virtual Functions Implementation //
	//=====================================================================//
private:
	/**
	 * This method implements the callback method to be called when a new instance of the current class is created.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes create (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, const char * class_name, const char * parent_dn, IMM_API_AttrValues ** attr);

	/**
	 * This method implements the callback method to be called when an instance of the current class is deleted.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes deleted (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, const char * obj_dn);

	/**
	 * This method implements the callback method to be called when an instance of the current class is modified.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes modify (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, const char * obj_dn, IMM_API_AttrModification ** attrMods);

	/**
	 * This method implements the callback method to be called when the object
	 * creation/modification/deletion has been correctly executed.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes complete (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId);

	/**
	 * This method implements the callback method to be called when the object creation/modification/deletion is failed.
	 * @see object_implementer_interface class for further info.
	 */
	virtual void abort (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId);

	/**
	 * This method implements the callback method to be called when the object
	 * creation/modification/deletion has been correctly executed and the complete method called.
	 * @see object_implementer_interface class for further info.
	 */
	virtual void apply (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId);

	/**
	 * This method implements the callback method to be called when trying to modify
	 * a runtime not-cached attribute inside a configuration class.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes updateRuntime (const char * obj_dn, const char ** attributes);

	/**
	 * This method implements the callback method to be called when
	 * an administrative operation is invoked on an object of the current class.
	 * @see object_implementer_interface class for further info.
	 */
	virtual void adminOperationCallback (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, const char * obj_dn, IMM_API_AdminOperationIdType operationId, IMM_API_AdminOperationParamType ** paramList);

	//======================================================//
	// event_handler class Virtual Functions Implementation //
	//======================================================//
public:
	/**
	 * This method returns the I/O handle associated to the Event Handler object.
	 *
	 * @return the I/O handle associated to the class implementer.
	 */
	inline virtual int get_handle () { return getSelObj(); }

	/**
	 * This method is called when an input operation is signaled on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_input ();

	/**
	 * This method is called when an output operation is signaled on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_output ();

	/**
	 * This method is called when an error occurs on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_exception (bool error_flag, bool hungup_flag, bool read_hungup_flag);

	/**
	 * This method is called before removing the Event Handler from the reactor.
	 *
	 * @return 0 on success, -1 on failure.
	 */
	virtual int handle_close ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
protected:
	bool _object_initialized;

private:
	static const int MAX_NUM_OF_RETRIES = 10;
	static const char * const HCF_SHADOW_IMPLEMENTER_NAME_PREFIX;

	object_implementer_handler _oi_handler;
	std::vector<std::string> _classes;
};

TSL_NS_HCF_IMM_END

#endif	// __INCLUDE_GUARD_CLASS_IMM_IMM_OBSERVER__
