#ifndef __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER__
#define __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER__ runtime_owner

/**
 *  @file runtime_owner.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
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
 *	| 2014-001 | 2014-11-10 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <string>
#include <vector>

#include "imm_types.h"
#include "imm_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER__

TSL_NS_HCF_IMM_BEGIN

// Forward declarations
class runtime_owner_impl;

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
	/**	@brief init method
	 *	init method: perform initialization of IMM interaction.
	 *	Initialize the IMM connection and registers the invoking process as Object implementer
	 *	having the name which is specified in the implementerName parameter.
	 *
	 *  @param p_implementerName the name of the Object Implementer
	 *
	 *	@return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 *
	 */
	IMM_API_ReturnTypes init (std::string p_implementerName);

	/**
	 * @brief finalize method
	 * finalize method: used to finalize interaction with IMM.
	 * The method clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer.
	 * After calling this method no other operation on IMM and that involves Object implementer could be performed.
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes finalize ();

	/**
	 * @brief createRuntimeObj method
	 * createRuntimeObj method: used to create a runtime object in IMM structure.
	 *
	 * @param p_className      : the Name of the class implemented by the Object.
	 *
	 * @param p_parentName     : the Name of the IMM Object that will be the parent of the creating OBJ
	 *
	 * @param p_attrValuesList : a vector of ACS_CC_ValuesDefinitionType elements containing the Attributes Values
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes createRuntimeObj (const char * p_className, const char * p_parentName, std::vector<IMM_API_ValuesDefinitionTypes> p_attrValuesList);

	/**
	 * @brief deleteRuntimeObj method
	 * deleteRuntimeObj method: used to delete a runtime object in IMM structure.
	 *
	 * @param p_objName the full DN of the runtime object in IMM tree to be deleted
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 *
	 */
	IMM_API_ReturnTypes deleteRuntimeObj (const char * p_objName);

	/**
	 * @brief modifyRuntimeObj method
	 * modifyRuntimeObj method: this method is used to modify the value of a attribute belonging
	 * to a runtime object.
	 *
	 *  @param  p_objName 		:	the full DN of the object containing the value to be retrieved
	 *
	 *  @param  p_attributeValue:	the struct containing the information about the attribute to be modify:
	 *  							attrName the name of the attribute to be changed
	 *  							attrType the type of the attribute to be changed
	 *  							attrValuesNum the number of value of the attribute
	 *                              attrValues the new values for the attributes
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes modifyRuntimeObj (const char * p_objName, IMM_API_ImmParameter * p_attributeValue);

	/**
	 * @brief dispatch method
	 * dispatch method: This method is used to dispatch callbacks on the Object Implementer
	 *
	 * @param  p_flag:			flag that specify the callback execution behavior of the dispatcher.
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes dispatch (IMM_API_DispatchFlags p_flag);

	/**
	 * @brief getSelObj method
	 * getSelObj method: This method return the operating system handle that the invoking process
	 * can use to detect pending callbacks
	 *
	 * @return int. The operating system handle to be used to dispatch pending callbacks
	 */
	int getSelObj () const;

	/**
	 * @brief adminOperationResult method: this method has to be usd by the class expanding the base class to return
	 * the result of administrative operation to IMM.
	 *
	 * @param oiHandle: input param the oi handle
	 *
	 * @param invocation: input param the invocation
	 *
	 * @param result : input param the result to be passed to IMM as administrative operation result.
	 *
	 * @return IMM_API_ReturnTypes . On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes adminOperationResult (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result);

	/**
	 * @brief adminOperationResult method: this method has to be usd by the class expanding the base class to return
	 * the result of administrative operation to IMM.
	 *
	 * @param oiHandle: input param the oi handle
	 *
	 * @param invocation: input param the invocation
	 *
	 * @param result : input param the result to be passed to IMM as administrative operation result.
	 *
	 * @param outParamVector: output parameter vector
	 *
	 * @return IMM_API_ReturnTypes . On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes adminOperationResult (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result, std::vector<IMM_API_AdminOperationParamType> outParamVector);

	/**
	 * getInternalLastError method: this method returns the error code of the last error occurred
	 *
	 * @return int. the error code of the last error occurred
	 */
	int getInternalLastError ();

	/**
	 * getInternalLastErrorText method: this method returns the message error of the last error occurred
	 *
	 * @return char*. the message error of the last error occurred
	 */
	const char * getInternalLastErrorText ();

	//===================//
	// Virtual Functions //
	//===================//
public:
	/**
	 * @brief updateCallback method
	 * updateCallback method: This is a virtual method to be implemented by the Designer when extending
	 * the base class. This method will be called as a callback when an non-cached attribute of a runtime Object
	 * is updated.
	 *
	 * @param  p_objName:	the distinguished name of the object for which the update is requested.
	 *
	 * @param  p_attrNames:	pointer to a null terminate array of attribute name for which values must be updated.
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	virtual IMM_API_ReturnTypes updateCallback (const char * p_objName, const char ** p_attrNames) = 0;

	/**
	 * @brief adminOperationCallback method
	 * adminOperationCallback method: This is a virtual method to be implemented by the Designer when extending
	 * the base class. This method will be called as a callback to manage an administrative operation invoked, on the
	 * implemented object, using the adminOperationInvoke method of acs_apgcc_adminoperation class.
	 *
	 * @param  p_objName:	the distinguished name of the object for which the administrative operation
	 * has to be managed.
	 *
	 * @param  oiHandle : input parameter,ACS_APGCC_OiHandle this value has to be used for returning the callback
	 *  result to IMM.
	 *
	 * @param invocation: input parameter,the invocation id used to match the invocation of the callback with the invocation
	 * of result function
	 *
	 * @param  p_objName: input parameter,the name of the object
	 *
	 * @param  operationId: input parameter, the administrative operation identifier
	 *
	 * @param paramList: a null terminated array of pointers to operation params elements. each element of the list
	 * is a pointer toACS_APGCC_AdminOperationParamType element holding the params provided to the Administretive operation..
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	virtual void adminOperationCallback (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, const char * p_objName, IMM_API_AdminOperationIdType operationId, IMM_API_AdminOperationParamType ** paramList) = 0;

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	runtime_owner_impl * _runtime_owner_impl;
};

TSL_NS_HCF_IMM_END

#endif	// __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER__
