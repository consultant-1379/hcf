#ifndef __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_INTERFACE_IMPL__
#define __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_INTERFACE_IMPL__ object_implementer_interface_impl

/**
 *  @file object_implementer_interface_impl.h
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

#include "hcf/imm/imm_macros.h"
#include "hcf/imm/imm_types.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_INTERFACE_IMPL__

TSL_NS_HCF_IMM_BEGIN

class __TSL_CLASS_NAME__ {
	//=========//
	// Friends //
	//=========//
	friend class object_implementer_handler_impl;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(_obj_name(), _impl_name(), _scope(), _oi_handle(0), _sel_obj(),
										 _exitCode(0), _exitCodeString(), _errorCode(0)) {}

	inline TSL_CTOR (std::string p_impName)
	TSL_CTOR_INIT_LIST(_obj_name(), _impl_name(p_impName), _scope(), _oi_handle(0), _sel_obj(),
										 _exitCode(0), _exitCodeString(), _errorCode(0)) {}

	inline TSL_CTOR (std::string p_objName, std::string p_impName, IMM_API_ScopeTypes p_scope)
	TSL_CTOR_INIT_LIST(_obj_name(p_objName), _impl_name(p_impName), _scope(p_scope),
										 _oi_handle(0), _sel_obj(), _exitCode(0), _exitCodeString(), _errorCode(0)) {}

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
	/**	@brief getObjName method
	 *	getObjName method: return the object name for which has been defined the Object Implementer .
	 *
	 *  No parameter required
	 *
	 *	@return std::string. the object name
	 *
	 */
	inline std::string getObjName () { return _obj_name; }

	/**	@brief setObjName method
	 *	setObjName method: set the object name for which are defining the Object Implementer .
	 *
	 *  @param p_objName Name of the Object
	 *
	 */
	inline void setObjName (std::string p_objName) { _obj_name = p_objName; }

	/**	@brief getImpName method
	 *	getImpName method: return the implementer name.
	 *
	 *  No parameter required
	 *
	 *	@return std::string. the implementer name.
	 *
	 */
	inline std::string getImpName () { return _impl_name; }

	/**	@brief setImpName method
	 *	setImpName method: set the name of implementer.
	 *
	 *  @param p_impName Name of the Implementer
	 *
	 */
	inline void setImpName (std::string p_impName) { _impl_name = p_impName; }

	/**	@brief getScope method
	 *	getScope method: return the scope of the operation for which the Object Implementer is defined.
	 *
	 *  No parameter required
	 *
	 *	@return IMM_API_ScopeTypes scope of operation.
	 *
	 */
	inline IMM_API_ScopeTypes getScope () { return _scope; }

	/**	@brief setScope method
	 *	setScope method: set the scope of the operation for which the Object Implementer is defined.
	 *
	 *  @param p_scope scope of the operation
	 *
	 */
	inline void setScope (IMM_API_ScopeTypes p_scope) {_scope = p_scope; }

	/**	@brief getSelObj method
	 *	getSelObj method: return the operating system handle that the invoking process can use to detect pending
	 *	callback .
	 *
	 *  No parameter required
	 *
	 *	@return int the operating system handle.
	 *
	 */
	inline int getSelObj () const { return (int) _sel_obj; }

	/**	@brief dispatch method
	 *	dispatch method: return the operating system handle that the invoking process can use to detect pending
	 *	callback .
	 *
	 *  @param p_flag: flags that specify the callback execution behavior of the dispatcher.
	 *  The value of this parameter can be:
	 *  	DISPATCH_ONE
	 *		DISPATCH_ALL
	 *		DISPATCH_BLOCKING
	 *
	 *	@return IMM_API_ReturnTypes On success OPERATION_SUCCESS on Failure OPERATION_FAILURE.
	 *
	 */
	IMM_API_ReturnTypes dispatch (IMM_API_DispatchFlags p_flag);

	/**
	 * @brief modifyRuntimeObj method
	 * modifyRuntimeObj method: this method is used to modify the value of a runtime attribute belonging
	 * to a configuration object.
	 *
	 *  @param  p_objName: the full DN of the object containing the value to be retrieved
	 *
	 *  @param  p_attributeValue: the struct containing the information about the attribute to be modify:
	 *  							attrName the name of the attribute to be changed
	 *  							attrType the type of the attribute to be changed
	 *  							attrValuesNum the number of value of the attribute
	 *                              attrValues the new values for the attributes
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes modifyRuntimeObj (const char * p_objName, IMM_API_ImmParameter * p_attributeValue);

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
	IMM_API_ReturnTypes adminOperationResult (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result);

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
	IMM_API_ReturnTypes adminOperationResult (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result, std::vector<IMM_API_AdminOperationParamType> outParamVector);

	/**	@brief setImmOiHandle method
	 *	setImmOiHandle method: set the immOiHandle with the value obtained by a IMM service.
	 *	This value is used also to identify a particular object Implementer in the static map that contain the
	 *	list of all object implementer
	 *
	 *  @param p_immOiHandle handle that identifies a particular initialization of the IMM Service
	 *
	 */
	inline void setImmOiHandle (SaUint64T p_immOiHandle) { _oi_handle = p_immOiHandle; }

	/**	@brief setSelObj method
	 *	setSelObj method: set the operating system handle that the invoking process can use to detect pending
	 *	callback .
	 *
	 *  @param p_selObj the operating system handle that the invoking process can use to detect pending
	 *	callback .
	 *
	 */
	void setSelObj (SaSelectionObjectT p_selObj) { _sel_obj = p_selObj; }


	inline SaUint64T getImmOiHandle () { return _oi_handle; }

	/**	@brief setExitCode method
	 *
	 *  setExitCode method: method to be used to set exitCode for Commands acting as Object implementer.
	 *  This method has to be called by commands acting as OI before returning a call-back because of error
	 *  providing as input parameter the p_exitCode and the p_exitCodeMessage that are respectively the error code
	 *  and the related error message to be reported to OM using the CMW functions.
	 *
	 *  @param p_exitCode        : the exit code of the command
	 *
	 *  @param p_exitCodeMessage : the Error Message string of the command
	 *
	 *  @return void.
	 */
	void setExitCode (int p_exitCode, std::string p_exitCodeMessage);

	/**	@brief getExitCodeString method
	 * getExitCode method: method to be used to get exitCode String, in case of error, for Commands acting
	 * as Object implementer.
	 * the exit code is stored in exitCodeString class variable to be available for object_implementer_handler
	 * class to be passed to CMW API.
	 */
	inline std::string getExitCodeString () { return _exitCodeString; }

	/**	@brief setExitCode method
	 *	setExitCode method: method to be used to set exitCode for Commands acting as Object implementer.
	 *	setExitCode the exit code is stored in exitCode class variable to be available for object_implementer_handler
	 *	setExitCode class to be returned after callback execution.
	 *
	 *  @param p_exitCode the exit code of the command
	 *
	 */
	inline void setExitCode (int p_exitCode) { _exitCode = p_exitCode; }

	/**	@brief getExitCode method
	 *	getExitCode method: method to be used to get exitCode for Commands acting as Object implementer.
	 *	getExitCode the exit code is stored in exitCode class variable to be available for object_implementer_handler
	 *	getExitCode class to be returned after callback execution.
	 *
	 */
	SaAisErrorT getExitCode () { return SA_AIS_OK; }

	/**	@brief getCcbAugmentation method for Ticket 1963
	 * This method get the ccbHandle to be augmented by the calling oi with other operations.
	 *  @param oiHandle        : INPUT   the oiHandle provided by the callback
	 *
	 *  @param ccbId           : INPUT   the ccbId provided by the callback
	 *
	 *  @param ccbHandle       : OUTPUT  a pointer to IMM_API_CcbHandle variable. once returned on success it contains the
	 *  pointer to the ccbHandle value to be used for augmentation. This ccbHandle to be augmented is valid only durig the callback upcall.
	 *
	 *  @param IMM_API_AdminOwnerHandle       : OUTPUT  a pointer to adminOwnerHandle. once returned on success it contains the
	 *  pointer to the adminOwnerHandle. This adminOwnerHandle is valid only during the callback upcall.
	 *
	 *  @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes getCcbAugmentationInitialize (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, IMM_API_CcbHandle * ccbHandle, IMM_API_AdminOwnerHandle * adminOwnwrHandler);

	/**	@brief createObjectAugmentCcb method for Ticket 1963
	 * This method augment the ccbHandle retrieved by calling the getCcbAugmentationInitialize() method creating a new Object .
	 *
	 *  @param ccbHandle       					: INPUT  the  IMM_API_CcbHandle
	 *
	 *  @param IMM_API_AdminOwnerHandle       : INPUT  the adminOwnerHandle.
	 *
	 *  @param p_className       				: INPUT  the  name of the class for which a new instance is to be created
	 *
	 *  @param p_parentName       				: INPUT  the Name of the parent object for the object to be created
	 *
	 *  @param p_attrValuesList					: INPUT the vector of IMM_API_ValuesDefinitionTypes elements each one containing one
	 *  attribute of the new object to be created.
	 *
	 *  @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes createObjectAugmentCcb (IMM_API_CcbHandle ccbHandle, IMM_API_AdminOwnerHandle adminOwnerHandler, const char * p_className, const char * p_parentName, std::vector<IMM_API_ValuesDefinitionTypes> p_attrValuesList);

	/**	@brief modifyObjectAugmentCcb  method for Ticket 1963.
	 * This method augment the ccbHandle retrieved by calling the getCcbAugmentationInitialize() method modifying an Object .
	 *
	 *  @param ccbHandle       					: INPUT  the  IMM_API_CcbHandle
	 *
	 *  @param IMM_API_AdminOwnerHandle       : INPUT  the adminOwnerHandle.
	 *
	 *  @param p_objectName       				: INPUT  the  DN of the object to be modified
	 *
	 *  @param p_parentName       				: INPUT  the Name of the parent object for the object to be created
	 *
	 *  @param p_attributeValue					: INPUT the IMM_API_ImmParameter holding the proposed modify
	 *
	 *  @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes modifyObjectAugmentCcb (IMM_API_CcbHandle ccbHandle, IMM_API_AdminOwnerHandle adminOwnerHandler, const char * p_objectName, IMM_API_ImmParameter * p_attributeValue);

	/**	@brief modifyObjectAugmentCcb  method for Ticket 1963.
	 * This method augment the ccbHandle retrieved by calling the getCcbAugmentationInitialize() method deleting an Object .
	 *
	 *  @param ccbHandle       					: INPUT  the  IMM_API_CcbHandle
	 *
	 *  @param IMM_API_AdminOwnerHandle       : INPUT  the adminOwnerHandle.
	 *
	 *  @param m_objectName       				: INPUT  the  DN of the object to be deleted
	 *
	 *  @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes deleteObjectAugmentCcb (IMM_API_CcbHandle ccbHandle, IMM_API_AdminOwnerHandle adminOwnerHandler, const char * m_objectName);

	/**	@brief applyAugmentCcb  method for Ticket 1963.
	 * This method applies the ccbHandle retrieved by calling the getCcbAugmentationInitialize() It must be called by the OI augmenting the CCB before returning.
	 * If applyAugmentCcb fails the entire CCB originated by the OM (not only the augmented ccb) fails.
	 *
	 *  @param ccbHandle       					: INPUT  the  IMM_API_CcbHandle
	 *
	 *  @param IMM_API_AdminOwnerHandle       : INPUT  the adminOwnerHandle.
	 *
	 *  @param m_objectName       				: INPUT  the  DN of the object to be deleted
	 *
	 *  @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes applyAugmentCcb (IMM_API_CcbHandle ccbHandle);

	/**
	 * getInternalLastError method: this method returns the error code of the last error occurred
	 *
	 * @return int. the error code of the last error occurred
	 */
	inline int getInternalLastError () { return ((-1) * _errorCode); }

	/**
	 * getInternalLastErrorText method: this method returns the message error of the last error occurred
	 *
	 * @return char*. the message error of the last error occurred
	 */
	const char * getInternalLastErrorText ();

private:
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
	std::string _obj_name;
	std::string _impl_name;
	IMM_API_ScopeTypes _scope;
	SaUint64T _oi_handle;
	SaSelectionObjectT _sel_obj;
	int _exitCode;
	std::string _exitCodeString;
	int _errorCode;
};

TSL_NS_HCF_IMM_END

#endif	// __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_INTERFACE_IMPL__
