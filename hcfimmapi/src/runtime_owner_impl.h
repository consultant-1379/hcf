#ifndef __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER_IMPL__
#define __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER_IMPL__ runtime_owner_impl

/**
 *  @file runtime_owner_impl.h
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
#include <map>

#include "hcf/imm/imm_macros.h"
#include "hcf/imm/imm_types.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER_IMPL__

TSL_NS_HCF_IMM_BEGIN

// Forward declarations
class runtime_owner;

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(_runtime_owner(0), _oi_handle(0), _sel_obj(0), _errorCode(0)) {}

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
	IMM_API_ReturnTypes init (std::string p_implementerName, runtime_owner * runtimeOwn);

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
	 * @param p_attrValuesList : a vector of IMM_API_ValuesDefinitionTypes elements containing the Attributes Values
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
	 * @return int. The operating system handle to be used to dispatch pending callbacks return (int)selObj
	 */
	inline int getSelObj () const { return (int)_sel_obj; }

	/**
	 * @brief adminOperationResult method: this method has to be used by the class expanding the base class to return
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
	IMM_API_ReturnTypes  adminOperationResult (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result);

	/**
	 * @brief adminOperationResult method: this method has to be used by the class expanding the base class to return
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
	IMM_API_ReturnTypes  adminOperationResult (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result, std::vector<IMM_API_AdminOperationParamType> outParamVector);

	/**
	 * getInternalLastError method: this method returns the error code of the last error occurred
	 *
	 * @return int. the error code of the last error occurred
	 */
	inline int getInternalLastError () { 	return ((-1)* _errorCode); }

	/**
	 * getInternalLastErrorText method: this method returns the message error of the last error occurred
	 *
	 * @return const char*. the message error of the last error occurred
	 */
	const char * getInternalLastErrorText ();

private:
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
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	static SaAisErrorT OiRtAttUpdateCallback (SaImmOiHandleT immOiHandle, const SaNameT * objectName, const SaImmAttrNameT * attributeNames);

	/**
	 * @brief adminOperationCallback method
	 * adminOperationCallback method:
	 */
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
	static std::map<SaUint64T, runtime_owner *> _implementer_list;

	runtime_owner * _runtime_owner;
	IMM_API_OiHandle _oi_handle;
	SaSelectionObjectT _sel_obj;
	int _errorCode;
};

TSL_NS_HCF_IMM_END

#endif	// __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER_IMPL__
