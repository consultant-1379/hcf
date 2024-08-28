#ifndef __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER_IMPL__
#define __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER_IMPL__ object_implementer_handler_impl

/**
 *  @file object_implementer_implementer_impl.h
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

#include <string>
#include <vector>
#include <map>

#include "hcf/imm/imm_macros.h"
#include "hcf/imm/imm_types.h"
#include "hcf/imm/object_implementer_interface.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER_IMPL__

TSL_NS_HCF_IMM_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () : _errorCode(0) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

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
	 *  @param obj A pointer of instance of ACS_APGCC_ObjectImplementerInterface
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
	inline int getInternalLastError () { return (-1) * (_errorCode); }

	/**
	 * getInternalLastErrorText method: this method returns the message error of the last error occurred
	 *
	 * @return const char*. the message error of the last error occurred
	 */
	const char * getInternalLastErrorText ();

private:
	/**
	 * @brief Finalize method
	 * Finalize method: used to finalize interaction with IMM.
	 * The method clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer.
	 * After calling this method no other operation on IMM and that involves Object implementer could be performed.
	 *
	 * @param handle handle that identifies a particular initialization of the IMM Service
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes Finalize (SaUint64T handle);

	/**
	 * @brief CreateCallback method
	 * CreateCallback method: this method is used by the IMM Service to enable an Object Implementer
	 * to validate and register a change request being added to a CCB identified by ccbId.
	 * The change request is a creation request for a configuration object of a class that is
	 * implemented by the process implementing the callback
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 * @param className Object class name.
	 *
	 * @param parentName Pointer to the name of the parent of the new object
	 *
	 * @param attr Pointer to a NULL-terminated array of pointers to attribute descriptor.
	 *
	 * @return SaAisErrorT. On success SA_AIS_OK on Failure SA_AIS_ERR_BAD_OPERATION
	 */
	static SaAisErrorT CreateCallback (SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId,	const SaImmClassNameT className, const SaNameT * parentName, const SaImmAttrValuesT_2 ** attr);

	/**
	 * @brief DeleteCallback method
	 * DeleteCallback method: this method is used by the IMM Service to enable an Object Implementer
	 * to validate and memorize a deletion request being added to a CCB identified by ccbId.
	 * The deletion request is a request to delete object that are implemented by the process that
	 * provided the callback function
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 * @param objectName Pointer to the object name.
	 *
	 * @return SaAisErrorT. On success SA_AIS_OK on Failure SA_AIS_ERR_BAD_OPERATION
	 */
	static SaAisErrorT DeleteCallback (SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId, const SaNameT * objectName);

	/**
	 * @brief ModifyCallback method
	 * ModifyCallback method: this method is used by the IMM Service to enable an Object Implementer
	 * to validate and memorize a change request being added to a CCB identified by ccbId.
	 * The change request is a request to modify configuration attributes of a configuration object
	 * implemented by the process implementing the callback
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 * @param objectName Pointer to the object name.
	 *
	 * @param attrMods Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modification to perform
	 *
	 * @return SaAisErrorT. On success SA_AIS_OK on Failure SA_AIS_ERR_BAD_OPERATION
	 */
	static SaAisErrorT ModifyCallback (SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId, const SaNameT * objectName, const SaImmAttrModificationT_2 ** attrMods);

	/**
	 * @brief CompletedCallback method
	 * CompletedCallback method: this method is used by the IMM Service to inform an Object Implementer
	 * that the CCB identified by ccbId is now complete. The implementer process must check that
	 * the sequence of change requests contained in the CCB is valid and that non error will be generated
	 * when these changes are applied
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 * @return SaAisErrorT. On success SA_AIS_OK on Failure SA_AIS_ERR_BAD_OPERATION
	 */
	static SaAisErrorT CompletedCallback (SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId);

	/**
	 * @brief AbortCallback method
	 * AbortCallback method: this method is used by the IMM Service to inform an Object Implementer
	 * that the CCB identified by ccbId is aborted, so the object implementer can remove all change
	 * request memorized for this CCB
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 */
	static void AbortCallback (SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId);

	/**
	 * @brief ApplyCallback method
	 * ApplyCallback method: this method is used by the IMM Service to inform an Object Implementer
	 * that the CCB identified by ccbId has been applied by IMM Service
	 *
	 * @param immOiHandle The handle that identifies a particular initialization of IMM Service.
	 *
	 * @param ccbId CCB identifier.
	 *
	 */
	static void ApplyCallback (SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId);

	/**
	 * @brief OiRtAttUpdateCallback method
	 * OiRtAttUpdateCallback method: The IMM service invokes this callback fuction to request at the
	 * Object Implementer to update the values of some non-cached attribute of a runtime object.
	 *
	 * @param  immOiHandle:	the object implementer handle. This is an Input Parameter provided by IMMSV Application
	 *
	 * @param  objectName:	the distinguished name of the object for which the update is requested.
	 *
	 * @param  attributeNames:	pointer to a NULL-terminated array of pointers of attribute name for which values must be updated.
	 *
	 * @return SaAisErrorT. On success SA_AIS_OK on Failure SA_AIS_ERR_BAD_OPERATION
	 *
	 */
	static SaAisErrorT OiRtAttUpdateCallback (SaImmOiHandleT immOiHandle, const SaNameT * objectName,	const SaImmAttrNameT * attributeNames);

	static void adminOperationCallback (SaImmOiHandleT immOiHandle, SaInvocationT invocation, const SaNameT * objectName, SaImmAdminOperationIdT operationId, const SaImmAdminOperationParamsT_2 ** params);

	/**
	 * setImmError method: this method is used to set the error returned by ImmService
	 *
	 * @param p_errorCode :	integer that represents the error
	 */
	inline void setInternalError (int p_errorCode) { _errorCode = p_errorCode; }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	static std::map<SaUint64T, object_implementer_interface *> _implementer_list;

	int _errorCode;
};

TSL_NS_HCF_IMM_END

#endif	// __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER_IMPL__
