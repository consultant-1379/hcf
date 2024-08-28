#ifndef __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER__
#define __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER__ object_implementer_handler

/**
 *  @file object_implementer_implementer.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-11-12
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
 *	| 2014-001 | 2014-11-12 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/imm/imm_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER__

TSL_NS_HCF_IMM_BEGIN

// Forward declarations
class object_implementer_handler_impl;

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	TSL_CTOR ();

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual TSL_DTOR ();

	//===========//
	// Functions //
	//===========//
public:
	/**	@brief addObjectImpl method
	 *	addObjectImpl method: Initialize the IMM connection and setting for the object specified by objName attribute
	 *	of obj, an object Implementer.
	 *	This function initializes the object Implementer functions of the IMM service for the invoking process
	 *	and registers the callback function.
	 *
	 *  @param obj A pointer of instance of object_implementer_interface
	 *
	 *	@return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 *
	 */
	IMM_API_ReturnTypes addObjectImpl (object_implementer_interface * obj);

	/**	@brief removeObjectImpl method
	 *	removeObjectImpl method: used to inform IMM that the implementer that was previously registered with
	 *	the method addObjectImpl must no longer be considered as the implementer of the set of objects
	 *	identified by scope attribute and objcetName attribute of obj parameter.
	 *
	 *  @param obj A pointer of instance of object_implementer_interface
	 *
	 *	@return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 *
	 */
	IMM_API_ReturnTypes removeObjectImpl (object_implementer_interface * obj);

	/**	@brief addClassImpl method
	 *	addClassImpl method: Initialize the IMM connection and setting for all the object that are
	 *	instances of the object class whose name is specified by clasName parameter an Object
	 *	implementer whose name is specified by implName attribute of obj .
	 *
	 *  @param obj A pointer of instance of object_implementer_interface
	 *
	 *  @param className Object class name
	 *
	 *	@return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 *
	 */
	IMM_API_ReturnTypes addClassImpl (object_implementer_interface * obj, const char * className);

	/**	@brief addMultipleClassImpl method
	 *	addMultipleClassImpl method: Initialize the IMM connection and setting for all the object that are
	 *	instances of the object classes whose name is specified by p_classNameVec parameter an Object
	 *	implementer whose name is specified by implName attribute of obj .
	 *
	 *  @param p_obj A pointer of instance of object_implementer_interface
	 *
	 *  @param p_classNameVec Vector of string of object class name
	 *
	 *	@return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 *
	 */
	IMM_API_ReturnTypes addMultipleClassImpl (object_implementer_interface * p_obj, std::vector<std::string> p_classNameVec);

	/**	@brief removeClassImpl method
	 *	removeClassImpl method: used to inform IMM that the implementer that was previously registered with
	 *	the method addClassImpl must not be considered anymore as the implementer of the objects that are instances
	 *	of the object class whose name is specified by className attribute
	 *
	 *  @param obj A pointer of instance of object_implementer_interface
	 *
	 *  @param className Object class name
	 *
	 *	@return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 *
	 */
	IMM_API_ReturnTypes removeClassImpl (object_implementer_interface * obj, const char * className);

	/**	@brief removeMultipleClassImpl method
	 *	removeMultipleClassImpl method: used to inform IMM that the implementer that was previously registered with
	 *	the method addMultipleClassImpl must not be considered anymore as the implementer of the objects that are instances
	 *	of the classes whose name is specified by p_classNameVec vector attribute
	 *
	 *  @param p_obj A pointer of instance of object_implementer_interface
	 *
	 *  @param p_classNameVec Object class name
	 *
	 *	@return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 *
	 */
	IMM_API_ReturnTypes removeMultipleClassImpl (object_implementer_interface * p_obj, std::vector<std::string> p_classNameVec);

	/**
	 * getInternalLastError method: this method returns the error code of the last error occurred
	 *
	 * @return int. the error code of the last error occurred
	 */
	int getInternalLastError ();

	/**
	 * getInternalLastErrorText method: this method returns the message error of the last error occurred
	 *
	 * @return const char*. the message error of the last error occurred
	 */
	const char * getInternalLastErrorText ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	object_implementer_handler_impl * _object_implementer_handler_impl;
};

TSL_NS_HCF_IMM_END

#endif	// __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER__
