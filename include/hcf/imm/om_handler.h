#ifndef __INCLUDE_GUARD_CLASS_IMM_OM_HANDLER__
#define __INCLUDE_GUARD_CLASS_IMM_OM_HANDLER__ om_handler

/**
 *  @file om_handler.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-11-07
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
 *	| 2014-001 | 2014-11-07 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <string>
#include <vector>

#include "imm_types.h"
#include "imm_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_OM_HANDLER__

TSL_NS_HCF_IMM_BEGIN

// Forward declarations
class om_handler_impl;

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
	/**
	 * Init method: used to Initialize Interaction between the calling process and IMM.
	 * Each application that intends to use OmHandler class must call this method to perform any action and
	 * call anyone of the other methods. The p_registeredImpl parameter is used to specify if all further modifies performed by the initializing Application have to be performed on IMM Objects only when an IMM object Implementer is active or not. At init time the appication has to choose if it intends to perform actions being sure that an Object Implementer is running for the rrelated Object or not.
	 * @param p_registeredImpl: a flag indicating if all further modify performed by the application
	 * have to be performed only if an Object Implementer is running for the modified object or not.
	 * Possible values are NO_REGISTERED_OI or REGISTERED_OI.
	 * Calling the Init method with NO_REGISTERED_OI argument the application registers to IMM in such a way that all modify on objects are performed regardless of whether an Object Implementer is registered on the related object or not.
	 * Calling the Init method with REGISTERED_OI argument, the application registers to IMM in such a way that all modify on objects are performed only if an Object Implementer is running for it otherwise the modify Fails.
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes Init (int p_registeredImpl = NO_REGISTERED_OI);

	/**
	 * Finalize method: used to finalize interaction with IMM and release all handler to it.
	 * After calling this method no other operation on IMM could be performed.
	 * No parameter required
	 */
	IMM_API_ReturnTypes Finalize ();

	/**
	 *
	 * defineClass method: used to define a new class in IMM. Provided input parameter are
	 * m_className the name of the class that has to be created;
	 * m_classCategory the category of the class it can be CONFIGURATION or RUNTIME;
	 * m_attrList a vector of IMM_API_AttrDefinitionTypes elements containing the definition of each attribute
	 * in the class that is beeing defined
	 * @param m_className the Name of the class to be defined.
	 * @param m_classCategory the class category. Admitted Values are CONFIGURATION or RUNTIME
	 * @param m_attrList a vector of IMM_API_AttrDefinitionTypes elements containing the definition of Attributes
	 * belonging to the class.
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes defineClass (char * m_className, IMM_API_CategoryTypes m_classCategory, std::vector<IMM_API_AttrDefinitionTypes> m_attrList);

	/**
	 * createObject method: used to create an object in IMM structure. Provided input parameter are
	 * m_className the name of the class that the creating Object will implement; m_parentName the
	 * name of the Object, already existing in IMM tree, that will be the parent of the now creating
	 * Object; m_attrValuesList a vector of IMM_API_ValuesDefinitionTypes elements containing the value
	 * of each attributwe of the class.
	 * @param m_className      : the Name of the class implemented by the Object.
	 * @param m_parentName     : the Name of the IMM Object that will be the parent of the creating OBJ
	 * @param m_attrValuesList : a vector of IMM_API_ValuesDefinitionTypes elements containing the Attributes Values
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes createObject (const char * m_className, const char * m_parentName, std::vector<IMM_API_ValuesDefinitionTypes> m_attrValuesList);

	/**
	 * createObject method: used to create an object in IMM structure. Provided input parameter are
	 * m_className the name of the class that the creating Object will implement; m_parentName the
	 * name of the Object, already existing in IMM tree, that will be the parent of the now creating
	 * Object; m_attrValuesList a vector of IMM_API_ValuesDefinitionTypes elements containing the value
	 * of each attributwe of the class.
	 * @param p_className      : the Name of the class implemented by the Object.
	 * @param p_parentName     : the Name of the IMM Object that will be the parent of the creating OBJ
	 * @param p_attrValuesList : a vector of IMM_API_ValuesDefinitionTypes elements containing the Attributes Values
	 * @param p_transactionName: the string that specify the name of transaction to which the creation request belongs
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes createObject (const char * p_className, const char * p_parentName, std::vector<IMM_API_ValuesDefinitionTypes> p_attrValuesList, std::string p_transactionName);

	/**
	 * deleteObject method: used to delete an object in IMM structure. Provided input parameter is the
	 * DN of the object to be deleted.
	 * @param m_objectName the full DN of the object in IMM tree to be deleted
	 */
	IMM_API_ReturnTypes deleteObject (const char * m_objectName);

	/**
	 * deleteObject method: used to delete an object in IMM structure. Provided input parameter is the
	 * DN of the object to be deleted  and the scope. According to the value of
	 * p_scope parameter, this method can be used to delete: only an object having no child, the object
	 * and related direct child (having no child),the object and all the related subtree.  .
	 * @param m_objectName the full DN of the object in IMM tree to be deleted
	 * @param p_scope			: the scope of delete operation.
	 *  If SCOPE_ONE is provided the operation requests to delete only the specified object
	 *  If SCOPE_SUBLEVEL is provided the operation requests to delete the specified object and direct child
	 *  If SCOPE_SUBTREE is provided the operation requests to delete the specified object and all the subtree
	 */
	IMM_API_ReturnTypes deleteObject (const char * m_objectName, IMM_API_ScopeTypes p_scope);

	/**
	 * deleteObject method: used to delete an object in IMM structure. Provided input parameter is the
	 * DN of the object to be deleted.
	 * @param p_objectName 	   : the full DN of the object in IMM tree to be deleted
	 * @param p_transactionName: the string that specify the name of transaction to which the deletion request belongs
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes deleteObject (const char * p_objectName, std::string p_transactionName);

	/**
	 * deleteObject method: used to delete an object in IMM structure. Provided input parameter is the
	 * DN of the object to be deleted, the transaction name and the scope. According to the value of
	 * p_scope parameter, this method can be used to delete: only an object having no child, the object
	 *  and related direct child (having no child),the object and all the related subtree.
	 *
	 * @param p_objectName 	   : the full DN of the object in IMM tree to be deleted
	 * @param p_transactionName: the string that specify the name of transaction to which the deletion request belongs
	 * @param p_scope			: the scope of delete operation.
	 *  If SCOPE_ONE is provided the operation requests to delete only the specified object
	 *  If SCOPE_SUBLEVEL is provided the operation requests to delete the specified object and direct child
	 *  If SCOPE_SUBTREE is provided the operation requests to delete the specified object and all the subtree
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes deleteObject (const char * p_objectName, std::string p_transactionName, IMM_API_ScopeTypes p_scope);

	/**
	 * getAttribute method: this method is used to retrieve the value of an attribute belonging to an IMM object.
	 * Required parameter are: m_objectName the DN name of the object containing the value that has to be retrieved; and
	 *  m_attribute: a pointer to the struct containing the name of the attribute to be retrieved
	 *  @param  m_objectName      :	the full DN of the object containing the value to be retrieved
	 *  @param  m_attributeValue :	the struct where the calling application has to set:
	 *  							attrName the name of the attribute to be retrieved
	 *  							attrType the type of the attribute to be retrieved
	 *                              and where the method returns the value for the attribute in the field:
	 *                              attrValues
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes getAttribute (const char * m_objectName, IMM_API_ImmParameter * m_attributeValue);

	/**
	 * getAttribute method: this method is used to retrieve the value of a list of attributes belonging to an IMM object.
	 * Required parameter are: p_objectName the DN name of the object containing the value that has to be retrieved; and
	 *  p_attributeList: a vector of pointer to the struct containing the name of the attribute to be retrieved
	 *  @param  p_objectName     :	the full DN of the object containing the value to be retrieved
	 *  @param  p_attributeList  :	the vector of pointer at the structs IMM_API_ImmParameter where the calling application has to set:
	 *  							attrName the name of the attribute to be retrieved
	 *                              and where the method returns the value for the attribute in the field:
	 *                              attrValues
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes getAttribute (const char * p_objectName, std::vector<IMM_API_ImmAttribute *> p_attributeList);

	/**
	 * getObject method: this method is used to retrieve the value of all attribute belonging to an IMM object.
	 * Required parameter is: p_object a pointer to an object  IMM_API_ImmObject
	 *  @param  p_object         :	the struct where the calling application has to set:
	 *  							objName the dn of the object whose attributes should be retrieved
	 *                              and where the method returns the value for the attributes in the vector:
	 *                              attributes of type IMM_API_ImmAttribute
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes getObject(IMM_API_ImmObject * p_object);

	/**
	 * getChildren method: this method is used to retrieve the rdn of all object that are children of the object having the name
	 * to which p_rootName point.
	 * Required parameter is:
	 *  @param  p_rootName :		pointer to the name of the root object for the search
	 *  @param 	p_scope	:			scope of the search:
	 *  							SCOPE_SUBLEVEL	:	indicates that the scope of the operation is targeted to one
	 *														object and its direct children.
	 *  							SCOPE_SUBTREE 	:	indicates that the scope of the operation is targeted to one
	 *														object and the entire subtree rooted at that object.
	 *	@param p_rdnList :			a vector of std::string elements containing the rdn of the object that maching the search criteria
	 *
	 *	@return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes getChildren(const char * p_rootName, IMM_API_ScopeTypes p_scope, std::vector<std::string> * p_rdnList);

	/**
	 * getClassInstances method: this method is used to retrieve the dn of all object that are instances of the class having the name
	 * to which p_className point.
	 * Required parameter is:
	 *  @param  p_className :		pointer to the name of the class to which retrieve the instances
	 *	@param p_dnList :			a vector of std::string elements containing the dn of the object that maching the search criteria
	 *
	 *	@return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes getClassInstances(const char * p_className, std::vector<std::string> & p_dnList);

	/**
	 * modifyAttribute method: this method is used to modify the value of an attribute belonging to an IMM object.
	 * Required parameter are: m_objectName the DN name of the object containing the value that has to be changed; and
	 *  m_attributeValue: a pointer to the struct containing the name of the attribute to be changed
	 *  @param  m_objectName      :	the full DN of the object containing the value to be retrieved
	 *  @param  m_attributeValue :	the struct containing the information about the attribute to be modify:
	 *  							attrName the name of the attribute to be changed
	 *  							attrType the type of the attribute to be changed
	 *  							attrValuesNum the number of value of the attribute
	 *                              attrValues the new values for the attributes
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes modifyAttribute(const char * m_objectName, IMM_API_ImmParameter * m_attributeValue);

	/**
	 * modifyAttribute method: this method is used to modify the value of an attribute belonging to an IMM object.
	 * Required parameter are: m_objectName the DN name of the object containing the value that has to be changed; and
	 *  m_attributeValue: a pointer to the struct containing the name of the attribute to be changed
	 *  @param  p_objectName      :	the full DN of the object containing the value to be retrieved
	 *  @param  p_attributeValue :	the struct containing the information about the attribute to be modify:
	 *  							attrName the name of the attribute to be changed
	 *  							attrType the type of the attribute to be changed
	 *  							attrValuesNum the number of value of the attribute
	 *                              attrValues the new values for the attributes
	 *  @param p_transactionName: a string that specify the name of transaction to which the change request belongs
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes modifyAttribute(const char * p_objectName, IMM_API_ImmParameter * p_attributeValue, std::string p_transactionName);

	/**
	 * removeClass method: used to remove the definition of a class in IMM. Provided input parameter is the
	 * Name of the Class to be deleted.
	 * @param m_className the Name of the class to be deleted.
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes removeClass (char * m_className);

	/**
	 * applyRequest method: this method applies all request associated at the transaction identified by its name in the
	 * configuration change bundle (ccb) associated and finalize the ccb handle.
	 * @param p_transactionName the Name of the transaction.
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes applyRequest (std::string p_transactionName);

	/**
	 * resetRequest method: this method remove all request associated at the transaction identified by its name in the
	 * configuration change bundle (ccb) associated and finalize the ccb handle.
	 * @param p_transactionName the Name of the transaction.
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes resetRequest (std::string p_transactionName);

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

	/**
	 * geExitCode method: retrieve by immsv the error code and error message when an OM operation
	 * performed by an AP command fails.
	 *
	 * @param p_errorID :	 (Output) apg error code to be returned by the command
	 * @param p_errorText :	 (Output) apg problem text to be printed by the command
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes getExitCode (int & p_errorId, std::string & p_errorText);

	/**
	 * geExitCode method: retrieve by immsv the error code and error message when an OM operation
	 * performed by an AP command fails in case of transaction actions.
	 *
	 * @param p_transactionName :(Input) an std::string holding the name of the transaction
	 * @param p_errorID         :(Output) an int.apg error code to be returned by the command
	 * @param p_errorText       :(Output) an std::string. apg problem text to be printed by the command
	 *
	 * @return IMM_API_ReturnTypes. On success OPERATION_SUCCESS on Failure OPERATION_FAILURE
	 */
	IMM_API_ReturnTypes getExitCode (std::string p_transactionName, int & p_errorId, std::string & p_errorText);

	/**
	 * getCcbHandleValue method: returns the ccbHandle value
	 *
	 * @return unsigned long long int. the ccbHandle value
	 */
	unsigned long long int getCcbHandleValue ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	om_handler_impl * _om_handler_implementation;
};

TSL_NS_HCF_IMM_END

#endif // __INCLUDE_GUARD_CLASS_IMM_OM_HANDLER__
