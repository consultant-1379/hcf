#include <string.h>
#include <errno.h>

#include "hcf/imm/imm_error_constants.h"

#include "hcf/trace/com_ericsson_common_hcf_imm_omhandler.h"
#include "hcf/trace/tracing_macros.h"

#include "om_handler_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_OM_IMM_HANDLER_IMPL__

TSL_NS_HCF_IMM_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

__TSL_CLASS_NAME__::TSL_CTOR() :
	_immHandle(0), _ccbHandle(0), _version(), _ownerHandle(0), _accessorHandle(0), _errorCode(0),
	_transactionList(), _transactionListOwnerHandle(), _ccbFlagVal(0), _exitCodeStringRetrieved() {
	HCF_CC_TRACE_FUNCTION;
	::memset(_processName, 0, sizeof(_processName));
	::memset(&_objOwnedName, 0, sizeof(_objOwnedName));
	_objOwnedNameList[0] = &_objOwnedName;
	_objOwnedNameList[1] = 0;
}

/**__TSL_CLASS_NAME__::Init(int p_registeredImpl) method used to initialize the handlers needed to interact with
 * IMM. The following handler are initialized:
 * _immHandle
 * _accessorHandle
 * This Init method has one parameter that is used to specify if the Application now registering and initializating on IMM intends to performs operations on Objects only if an Object Implementer is registered for it or regardless of Object Implementer running.
 */
IMM_API_ReturnTypes __TSL_CLASS_NAME__::Init(int p_registeredImpl) {
	HCF_CC_TRACE_FUNCTION;

	_ccbFlagVal = 0;

	_version.releaseCode  = IMM_LIBRARY_RELEASE_CODE;
	_version.majorVersion = IMM_LIBRARY_MAJOR_VERSION;
	_version.minorVersion = IMM_LIBRARY_MINOR_VERSION;

	/** Set the value for the ccbFlag according to the provided input parameter p_registeredImpl
	* if p_registeredImpl is set to NO_REGISTERED_OI then ccbFlag=0 so any change is performed regardless OI running
	* if p_registeredImpl is set to REGISTERED_OI then ccbFlag=SA_IMM_CCB_REGISTERED_OI so any change is performed only if an OI is registered for the Object to be modified
	*/
	if (p_registeredImpl == REGISTERED_OI)
		_ccbFlagVal = SA_IMM_CCB_REGISTERED_OI;

	/**Initialize the IMM connection calling the saImmOmInitialize().
	 *
	 * _immHandle is an output parameter where the function will return the handle
	 * to be used for further access to imm.
	 *
	 * the second parameter contains the callbacks
	 *
	 *  _version is an input parameter containing the info about which imm version
	 *  the code is compliant to.
	 *
	 */
	HCF_CC_TRACE(Init, "Calling 'saImmOmInitialize' function.", p_registeredImpl);
	SaAisErrorT errorCode;
	if ((errorCode = saImmOmInitialize(&_immHandle, 0, &_version)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(InitErr, "Call 'saImmOmInitialize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/**Obtain _accessorHandle */
	HCF_CC_TRACE(Init, "Calling 'saImmOmAccessorInitialize' function.", p_registeredImpl);
	if ((errorCode = saImmOmAccessorInitialize(_immHandle, &_accessorHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(InitErr, "Call 'saImmOmAccessorInitialize' failed: finalizing the IMM handle.", errorCode, getInternalLastErrorText());

		/*release _immHandle*/
		if ((errorCode = saImmOmFinalize(_immHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(InitErr, "Call 'saImmOmFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		return OPERATION_FAILURE;
	}

#ifdef _GNU_SOURCE
	::strncpy(_processName, program_invocation_short_name, sizeof(_processName));
#else
	::strncpy(_processName, "unknownProcess", sizeof(_processName))
#endif

	return OPERATION_SUCCESS;
}

/**__TSL_CLASS_NAME__::Finalize() method used to release all handler retrieved in Init()
 * method. the following handler have to be released:
 *
 * _ccbHandle
 * _ownerHandle
 * _immHandle
 *
 */
IMM_API_ReturnTypes __TSL_CLASS_NAME__::Finalize() {
	HCF_CC_TRACE_FUNCTION;

	HCF_CC_TRACE(Finalize, "Calling 'saImmOmAccessorFinalize' function.");
	SaAisErrorT errorCode;
	if ((errorCode = saImmOmAccessorFinalize(_accessorHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(FinalizeErr, "Call 'saImmOmAccessorFinalize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/* release innHandler*/
	HCF_CC_TRACE(Finalize, "Calling 'saImmOmFinalize' function.");
	if ((errorCode = saImmOmFinalize(_immHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(FinalizeErr, "Call 'saImmOmFinalize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

/** __TSL_CLASS_NAME__::defineClass method. This method define a new class in IMM
 * according to parameter provided as input. Input parameter are:
 *
 * char* className : the name of the class to be created
 *
 * ACS_CC_ClassCategoryType classCategory: the class category. It may asuume the following values
 * 					CONFIGURATION if the class to be defined is related to IMM Configuration Objects
 * 					RUNTIME       if the class to be defined is related to IMM Runtime Object
 *vector<IMM_API_AttrDefinitionTypes> attrList: a vector containing the definition of all attributes
 *					that are present in the class
 *
 */
IMM_API_ReturnTypes __TSL_CLASS_NAME__::defineClass (char * m_className, IMM_API_CategoryTypes m_classCategory, std::vector<IMM_API_AttrDefinitionTypes> m_attrList) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaImmClassCategoryT classCategory;
	SaNameT * StringDefVal = 0;
	std::vector<SaNameT *> SaNameVector;
	std::vector<IMM_API_AttrDefinitionTypes>::iterator it;
	SaImmAttrDefinitionT_2 ** attributesDefs = 0;

	/*Provided input parameter are here mapped on the  SAF specification
	 *here m_classCategory is mapped on the related SAF enum value*/
	switch (m_classCategory) {
		case CONFIGURATION:
			classCategory = SA_IMM_CLASS_CONFIG;
			break;

		case RUNTIME:
			classCategory = SA_IMM_CLASS_RUNTIME;
			break;

		default:
			classCategory = SA_IMM_CLASS_CONFIG;
			break;
	}
	HCF_CC_TRACE(DefineClass, "Received a request to define a class: preparing the attributes vector.", m_className, m_classCategory, m_attrList.size());

	/*
	 * here the list of attributes is built. All attributes provided in
	 * m_attrList are copied in a structure  SaImmAttrDefinitionT_2 and
	 * the structure is  added to a list that will be passed as  input
	 * parameter to the saImmOmClassCreate_2 function
	 *
	 */
	attributesDefs = new SaImmAttrDefinitionT_2 * [m_attrList.size() + 1];
	SaImmAttrDefinitionT_2 oneMoreAttribute[m_attrList.size()];

	int i = 0;
	for (it = m_attrList.begin(); it != m_attrList.end(); ++it) {

		/*For each element in the vector a new attribute definitions provided*/
		oneMoreAttribute[i].attrName = (*it).attrName;
		oneMoreAttribute[i].attrValueType=(SaImmValueTypeT)(*it).attrType;
		oneMoreAttribute[i].attrFlags =(SaImmAttrFlagsT)((*it).attrFlag);

		/*modify*/
		if ((oneMoreAttribute[i].attrValueType == SA_IMM_ATTR_SANAMET) &&
				!(oneMoreAttribute[i].attrFlags & SA_IMM_ATTR_RDN) && (*it).attrDefaultVal != 0) {

			StringDefVal = new (std::nothrow) SaNameT;
			if (!StringDefVal)
				return OPERATION_FAILURE;

			StringDefVal->length = ::strlen((const char *)((*it).attrDefaultVal));
			::memcpy(StringDefVal->value, (*it).attrDefaultVal, StringDefVal->length);

			oneMoreAttribute[i].attrDefaultValue = (void *)StringDefVal;

			/**Used to deallocate memory for name variables **/
			SaNameVector.push_back(StringDefVal);
		}
		else {
			oneMoreAttribute[i].attrDefaultValue = (void*)((*it).attrDefaultVal);
		}

		attributesDefs[i] = &(oneMoreAttribute[i]);
		++i;
	}

	/* Last element in the list of Attributes definition is mandatory NULL */
	attributesDefs[i] = 0;

	/* call the function saImmOmClassCreate_2 to create the class*/
	HCF_CC_TRACE(DefineClass, "Calling 'saImmOmClassCreate_2' function.", m_className, m_classCategory, m_attrList.size());
	errorCode = saImmOmClassCreate_2(_immHandle, m_className, classCategory, (const SaImmAttrDefinitionT_2 **)attributesDefs);

	//to free memory
	if (SaNameVector.size() > 0) {
		for (std::vector<SaNameT *>::iterator it_2 = SaNameVector.begin(); it_2 != SaNameVector.end(); ++it_2) {
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}

	delete [] attributesDefs;

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DefineClassErr, "Call 'saImmOmClassCreate_2' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// REMOVE CLASS METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::removeClass(char * m_className) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(RemoveClass, "Received a request to delete a class.", m_className);

	SaAisErrorT errorCode;
	if ((errorCode = saImmOmClassDelete(_immHandle, (SaImmClassNameT) m_className)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(RemoveClassErr, "Call 'saImmOmClassDelete' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// CREATE OBJECT METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::createObject (const char * m_className, const char * m_parentName, std::vector<IMM_API_ValuesDefinitionTypes> m_attrValuesList) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	IMM_API_ReturnTypes retVal = OPERATION_SUCCESS;
	SaNameT * name = 0;
	std::vector<SaNameT *> SaNameVector;
	SaNameT parentName = {0, {0}};
	std::vector<IMM_API_ValuesDefinitionTypes>::iterator it;
	SaImmAttrValuesT_2 ** attributesValues = 0;
	SaImmCcbHandleT ccbHandle;
	SaImmAdminOwnerHandleT ownerHandle;

	/**Initialize the Object Administration ownership to perform operations on IMM
	 *
	 *_immHandle is the handle retrieved by the previous call second parameter is the Administrative Owner name
	 *third parameter specifies whether to release the Management Ownership when finalizing the ownerHandle
	 *ownerHandle is the output parameter where the Management Ownership handler is stored
	 */
	HCF_CC_TRACE(CreateObject, "Received a request to create an object, setting the admin owner!", m_className, m_parentName, "");
	if ((errorCode = saImmOmAdminOwnerInitialize(_immHandle, _processName, SA_TRUE, &ownerHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmAdminOwnerInitialize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	const SaNameT * objOwnedNameList[] = {&parentName, 0};

	/*Build the SaNameT variable to be passed to the  saImmOmCcbObjectCreate_2 function*/
	HCF_CC_TRACE(CreateObject, "Received a request to create an object, setting the parent admin owner (if needed)!", m_className, m_parentName, "");
	if (m_parentName) {
		parentName.length = ::strlen(m_parentName);
		::memcpy(parentName.value, m_parentName, parentName.length);

		if ((errorCode = saImmOmAdminOwnerSet(ownerHandle, objOwnedNameList, SA_IMM_ONE)) != SA_AIS_OK)	{
			setInternalError(errorCode);
			HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmAdminOwnerSet' failed: finalizing admin owner.", errorCode, getInternalLastErrorText());

			/*release ownerHandle*/
			if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
				HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
			}

			return OPERATION_FAILURE;
		}
	}

	/*Retrieve the CCB handler to perform operation IMM*/
	HCF_CC_TRACE(CreateObject, "Calling 'saImmOmCcbInitialize' function", m_className, m_parentName, "");
	if ((errorCode = saImmOmCcbInitialize(ownerHandle, _ccbFlagVal, &ccbHandle)) != SA_AIS_OK)	{
			setInternalError(errorCode);
			HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmCcbInitialize' failed: finalizing admin owner.", errorCode, getInternalLastErrorText());

			/*release ownerHandle*/
			if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
				HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
			}

			return OPERATION_FAILURE;
	}
	HCF_CC_TRACE(CreateObject, "Received a request to create an object, preparing the attributes vector.", m_className, m_parentName, "");

	/* Create the list of Attribute Values to be provided as input to the saImmOmCcbObjectCreate_2 function*/
	/* allocate a  list of m_attrValuesList.size()+1 (SaImmAttrValuesT_2*) */
	attributesValues = new SaImmAttrValuesT_2*[m_attrValuesList.size()+1];

	int numAttribute = m_attrValuesList.size();

	SaImmAttrValuesT_2 singleAttributeValues[numAttribute];

	int i = 0;

	for (it = m_attrValuesList.begin(); it != m_attrValuesList.end(); ++it) {

		/*Set the attribute Name*/
		singleAttributeValues[i].attrName = (SaImmAttrNameT)((*it).attrName);

		/*Set the attribute Type*/
		singleAttributeValues[i].attrValueType = (SaImmValueTypeT)((*it).attrType);

		/*Set the Attribute Values Number*/
		unsigned int numElem = (*it).attrValuesNum;
		singleAttributeValues[i].attrValuesNumber = (SaUint32T)(numElem);

		/*Set attribute values To be done*/
		singleAttributeValues[i].attrValues = new void*[numElem];

		for (unsigned int j=0; j<numElem; j++) {

			switch ((*it).attrType){
			case ATTR_INT32T:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT32T:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_INT64T:
			case ATTR_TIMET:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT64T:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_DOUBLET:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_FLOATT:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_STRINGT:
				singleAttributeValues[i].attrValues[j] = &((*it).attrValues[j]);
				break;

			case ATTR_NAMET:
				name = new SaNameT;
				unsigned int howlong ;

				howlong = strlen ( reinterpret_cast<char*>((*it).attrValues[j]));
				name->length =  howlong ;

				memcpy(name->value, (*it).attrValues[j], name->length);
				singleAttributeValues[i].attrValues[j] = reinterpret_cast<void*>(name);

				/**Used to deallocate memory for name variables **/
				SaNameVector.push_back(name);
				break;

			default:
				break;

			}/*end switch case on the attribute Type*/

		}/**End for on each multiple value for a single attribute*/
		/*END set Atribute values*/

		/*add the singleAttributeValues to the vector attributesValues*/
		attributesValues[i] = &(singleAttributeValues[i]);

		i++;
	}/*end for that goes through the input vector and builds the list to be passed to the function*/

	/*LAst element in attributes Value list must be NULL*/
	attributesValues[i] = 0;

	HCF_CC_TRACE(CreateObject, "Calling 'saImmOmCcbObjectCreate_2' function.", m_className, m_parentName, "");
	errorCode = saImmOmCcbObjectCreate_2(ccbHandle, (SaImmClassNameT)m_className, &parentName, (const SaImmAttrValuesT_2**)(attributesValues));

	/* delete allocated memory*/
	i = 0;
	for (it = m_attrValuesList.begin(); it != m_attrValuesList.end(); ++it) {
		delete[] attributesValues[i]->attrValues;
		++i;
	}

	if (SaNameVector.size() > 0) {
		std::vector<SaNameT *>::iterator it_2;
		for (it_2 = SaNameVector.begin(); it_2 != SaNameVector.end(); ++it_2) {
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}
	delete[] attributesValues;

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmCcbObjectCreate_2' failed: finalizing CCB and admin owner.", errorCode, getInternalLastErrorText());

		callSaImmOmCcbGetErrStr(ccbHandle);
		/*ticket 1904: end*/

		/*Added for CCB corruption management*/

		/** release ccbHandler**/
		if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		/* release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		return OPERATION_FAILURE;
	}

	HCF_CC_TRACE(CreateObject, "Calling 'saImmOmCcbApply' function.", m_className, m_parentName, "");
	if ((errorCode = saImmOmCcbApply(ccbHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmCcbApply' failed: finalizing CCB and admin owner.", errorCode, getInternalLastErrorText());

		callSaImmOmCcbGetErrStr(ccbHandle);

		/** release ccbHandler**/
		if ((errorCode = saImmOmCcbFinalize(ccbHandle))!= SA_AIS_OK) {
			HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		/* release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		return OPERATION_FAILURE;
	}

	/** release ccbHandler**/
	HCF_CC_TRACE(CreateObject, "Calling 'saImmOmCcbFinalize' function.", m_className, m_parentName, "");
	if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, getInternalLastErrorText());
		retVal = OPERATION_FAILURE;
	}

	/* release ownerHandle*/
	HCF_CC_TRACE(CreateObject, "Calling 'saImmOmAdminOwnerFinalize' function.", m_className, m_parentName, "");
	if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return retVal;
}

//---------------------------------------------------------------------------------------------------------------------------
// CREATE OBJECT METHOD TRANSCATIONAL
//Added For Object creation without apply the request to CCB
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::createObject (const char * p_className, const char * p_parentName, std::vector<IMM_API_ValuesDefinitionTypes> p_attrValuesList, std::string p_transactionName) {
	HCF_CC_TRACE_FUNCTION;

	/**Check the name transaction**/
	if (p_transactionName.length() == 0) {
		setInternalError(7);	// INVALID_PARAM
		HCF_CC_TRACE(CreateObjectErr, "Empty transaction name provided!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newownerHandle;

	/**Get the CCB handle of transaction**/
	HCF_CC_TRACE(CreateObject, "Received a request to create an object, looking for the transaction!", p_className, p_parentName, p_transactionName.c_str());
	int returnCode = getCcbHandle(p_transactionName, &newCcbHandle, &newownerHandle);
	if (returnCode != 0) {
		setInternalError(returnCode);
		HCF_CC_TRACE(CreateObjectErr, "Failed to get handles for the provided transaction.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaNameT * name = 0;
	std::vector<SaNameT *> SaNameVector;
	SaNameT parentName = {0, {0}};
	std::vector<IMM_API_ValuesDefinitionTypes>::iterator it;
	SaImmAttrValuesT_2 ** attributesValues = 0;

	/*Set the administrative Owner on the parent of object to create*/
	HCF_CC_TRACE(CreateObject, "Received a request to create an object, setting the parent admin owner (if needed)!", p_className, p_parentName, p_transactionName.c_str());
	if (p_parentName != 0) {
		parentName.length = strlen(p_parentName);
		memcpy(parentName.value, p_parentName, parentName.length);
		_objOwnedName.length = sprintf((char*)_objOwnedName.value, "%s", p_parentName);

		/*Set the administrative owner on the parent of object*/
		if ((_errorCode = saImmOmAdminOwnerSet (newownerHandle, _objOwnedNameList, SA_IMM_ONE)) != SA_AIS_OK)	{
			setInternalError(_errorCode);
			HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmAdminOwnerSet' failed.", getInternalLastError(), getInternalLastErrorText());
			return OPERATION_FAILURE;
		}
	}
	HCF_CC_TRACE(CreateObject, "Received a request to create an object, preparing the attributes vector.", p_className, p_parentName, p_transactionName.c_str());

	/* Create the list of Attribute Values to be provided as input to the saImmOmCcbObjectCreate_2 function*/
	/*allocate a  list of m_attrValuesList.size()+1 (SaImmAttrValuesT_2*) */
	attributesValues = new SaImmAttrValuesT_2 * [p_attrValuesList.size() +1];
	int numAttribute = p_attrValuesList.size();
	SaImmAttrValuesT_2 singleAttributeValues[numAttribute];

	int i = 0;
	for (it = p_attrValuesList.begin(); it != p_attrValuesList.end(); ++it) {
		singleAttributeValues[i].attrName = (SaImmAttrNameT)((*it).attrName);
		singleAttributeValues[i].attrValueType = (SaImmValueTypeT)((*it).attrType);
		unsigned int numElem = (*it).attrValuesNum;
		singleAttributeValues[i].attrValuesNumber = (SaUint32T)(numElem);
		singleAttributeValues[i].attrValues = new void*[numElem];

		for (unsigned int j=0; j<numElem; j++) {

			switch ((*it).attrType){
			case ATTR_INT32T:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT32T:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_INT64T:
			case ATTR_TIMET:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT64T:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_DOUBLET:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_FLOATT:
				singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_STRINGT:
				singleAttributeValues[i].attrValues[j] = &((*it).attrValues[j]);
				break;

			case ATTR_NAMET:
				name = new SaNameT;
				unsigned int howlong ;

				howlong = strlen ( reinterpret_cast<char*>((*it).attrValues[j]));
				name->length =  howlong ;

				memcpy(name->value, (*it).attrValues[j], name->length);
				singleAttributeValues[i].attrValues[j] = reinterpret_cast<void*>(name);

				/**Used to deallocate memory for name variables **/
				SaNameVector.push_back(name);
				break;

			default:
				break;

			}/*end switch case on the attribute Type*/

		}/**End for on each multiple value for a single attribute*/
		/*END set Atribute values*/

		/*add the singleAttributeValues to the vector attributesValues*/
		attributesValues[i] = &(singleAttributeValues[i]);

		i++;
	}/*end for that goes through the input vector and builds the list to be passed to the function*/

	/*LAst element in attributes Value list must be NULL*/
	attributesValues[i] = 0;

	HCF_CC_TRACE(CreateObject, "Calling 'saImmOmCcbObjectCreate_2' function.", p_className, p_parentName, p_transactionName.c_str());
	SaAisErrorT error = saImmOmCcbObjectCreate_2 (newCcbHandle, (SaImmClassNameT)p_className, &parentName, (const SaImmAttrValuesT_2**)(attributesValues));

	/* delete allocated memory*/
	i = 0;
	for (it = p_attrValuesList.begin(); it != p_attrValuesList.end(); ++it) {
		delete[] attributesValues[i]->attrValues;
		i++;
	}

	if (SaNameVector.size() > 0) {
		std::vector<SaNameT *>::iterator it_2;
		for (it_2 = SaNameVector.begin(); it_2 != SaNameVector.end(); ++it_2) {
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}
	delete[] attributesValues;

	if (error != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(CreateObjectErr, "Call 'saImmOmCcbObjectCreate_2' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObject (const char * m_objectName) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	IMM_API_ReturnTypes retVal = OPERATION_SUCCESS;

	SaNameT objToDelete;

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(m_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	memcpy(objToDelete.value, m_objectName, objToDelete.length);

	SaImmCcbHandleT ccbHandle;
	SaImmAdminOwnerHandleT ownerHandle;

	/**Initialize the Object Administration ownership to perform operations on IMM
	 *
	 *_immHandle is the handle retrieved by the previous call second parameter is the Administrative Owner name
	 *third parameter specifies whether to release the Management Ownership when finalizing the ownerHandle
	 *ownerHandle is the output parameter where the Management Ownership handler is stored
	 */
	HCF_CC_TRACE(DeleteObject, "Received a request to delete an object: initializing admin owner!", m_objectName, -1, "");
	if ((errorCode = saImmOmAdminOwnerInitialize(_immHandle, _processName, SA_TRUE, &ownerHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerInitialize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	const SaNameT * objOwnedNameList[] = {&objToDelete, 0};

	HCF_CC_TRACE(DeleteObject, "Received a request to delete an object: setting admin owner!", m_objectName, -1, "");
	if ((errorCode = saImmOmAdminOwnerSet (ownerHandle,objOwnedNameList, SA_IMM_ONE)) != SA_AIS_OK)	{
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerSet' failed: finalizing admin owner!", errorCode, getInternalLastErrorText());

		/*release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}
		/*end release ownerHandle*/

		return OPERATION_FAILURE;
	}

	/*Retrieve the CCB handler to perform operation IMM*/
	HCF_CC_TRACE(DeleteObject, "Calling 'saImmOmCcbInitialize' function.", m_objectName, -1, "");
	if ((errorCode = saImmOmCcbInitialize(ownerHandle, _ccbFlagVal ,&ccbHandle)) != SA_AIS_OK)	{
			setInternalError(errorCode);
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbInitialize' failed: finalizing admin owner!", errorCode, getInternalLastErrorText());

			/*release ownerHandle*/
			if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
				HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
			}
			/*end release ownerHandle*/

			return OPERATION_FAILURE;
		}

	/*Call the saImmOmCcbObjectDelete */
	HCF_CC_TRACE(DeleteObject, "Deleting the object!", m_objectName, -1, "");
	if ((errorCode = saImmOmCcbObjectDelete(ccbHandle, &objToDelete)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbObjectDelete' failed: finalizing CCB and admin owner!", errorCode, getInternalLastErrorText());
		callSaImmOmCcbGetErrStr(ccbHandle);

		/** release ccbHandler**/
		if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		/* release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		return OPERATION_FAILURE;
	}

	HCF_CC_TRACE(DeleteObject, "Calling 'saImmOmCcbApply' function.", m_objectName, -1, "");
	if ((errorCode = saImmOmCcbApply(ccbHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbApply' failed: finalizing CCB and admin owner!", errorCode, getInternalLastErrorText());
		callSaImmOmCcbGetErrStr(ccbHandle);

		/** release ccbHandler**/
		if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		/* release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		return OPERATION_FAILURE;
	}

	/** release ccbHandler**/
	HCF_CC_TRACE(DeleteObject, "Calling 'saImmOmCcbFinalize' function.", m_objectName, -1, "");
	if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, getInternalLastErrorText());
		retVal = OPERATION_FAILURE;
	}

	/* release ownerHandle*/
	HCF_CC_TRACE(DeleteObject, "Calling 'saImmOmAdminOwnerFinalize' function.", m_objectName, -1, "");
	if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return retVal;
}

//---------------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT METHOD WITH SCOPE PARAMETER
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObject (const char * m_objectName, IMM_API_ScopeTypes p_scope) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	IMM_API_ReturnTypes retVal = OPERATION_SUCCESS;
	SaNameT objToDelete;

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(m_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	memcpy(objToDelete.value, m_objectName, objToDelete.length);

	/*Initialization Section Start*/

	SaImmCcbHandleT ccbHandle;
	SaImmAdminOwnerHandleT ownerHandle;

	/*added for scope management*/
	SaImmScopeT scopeVal = SA_IMM_ONE;

	if (p_scope == SCOPE_ONE)
		scopeVal = SA_IMM_ONE;
	else if (p_scope == SCOPE_SUBLEVEL)
		scopeVal = SA_IMM_SUBLEVEL;
	else if (p_scope == SCOPE_SUBTREE)
		scopeVal = SA_IMM_SUBTREE;
	/*end added for scope management*/

	/**Initialize the Object Administration ownership to perform operations on IMM
	 *
	 *_immHandle is the handle retrieved by the previous call second parameter is the Administrative Owner name
	 *third parameter specifies whether to release the Management Ownership when finalizing the ownerHandle
	 *ownerHandle is the output parameter where the Management Ownership handler is stored
	 */
	HCF_CC_TRACE(DeleteObject, "Received a request to delete an object: initializing admin owner!", m_objectName, p_scope, "");
	if ((errorCode = saImmOmAdminOwnerInitialize(_immHandle, _processName, SA_TRUE, &ownerHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerInitialize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	const SaNameT *objOwnedNameList[] = {&objToDelete, 0};

	HCF_CC_TRACE(DeleteObject, "Received a request to delete an object: setting admin owner!", m_objectName, p_scope, "");
	if ((errorCode = saImmOmAdminOwnerSet (ownerHandle, objOwnedNameList, scopeVal)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerSet' failed: finalizing admin owner!", errorCode, getInternalLastErrorText());

		/*release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}
		/*end release ownerHandle*/

		return OPERATION_FAILURE;
	}

	/*Retrieve the CCB handler to perform operation IMM*/
	HCF_CC_TRACE(DeleteObject, "Calling 'saImmOmCcbInitialize' function.", m_objectName, p_scope, "");
	if ((errorCode = saImmOmCcbInitialize(ownerHandle, _ccbFlagVal, &ccbHandle)) != SA_AIS_OK) {
			setInternalError(errorCode);
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbInitialize' failed: finalizing admin owner!", errorCode, getInternalLastErrorText());

			/*release ownerHandle*/
			if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
				HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
			}
			/*end release ownerHandle*/

			return OPERATION_FAILURE;
		}

	/*Initialization Section End*/

	/*Call the saImmOmCcbObjectDelete */
	HCF_CC_TRACE(DeleteObject, "Deleting the object!", m_objectName, p_scope, "");
	if ((errorCode = saImmOmCcbObjectDelete(ccbHandle, &objToDelete)) !=  SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbObjectDelete' failed: finalizing CCB and admin owner!", errorCode, getInternalLastErrorText());
		callSaImmOmCcbGetErrStr(ccbHandle);

		/** release ccbHandler**/
		if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		/* release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		return OPERATION_FAILURE;
	}

	HCF_CC_TRACE(DeleteObject, "Calling 'saImmOmCcbApply' function.", m_objectName, p_scope, "");
	if ((errorCode = saImmOmCcbApply(ccbHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbApply' failed: finalizing CCB and admin owner!", errorCode, getInternalLastErrorText());
		callSaImmOmCcbGetErrStr(ccbHandle);

		/** release ccbHandler**/
		if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		/* release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}
		/*End Added for CCB corruption management*/

		return OPERATION_FAILURE;
	}

	/** release ccbHandler**/
	HCF_CC_TRACE(DeleteObject, "Calling 'saImmOmCcbFinalize' function.", m_objectName, p_scope, "");
	if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, getInternalLastErrorText());
		retVal = OPERATION_FAILURE;
	}

	/* release ownerHandle*/
	HCF_CC_TRACE(DeleteObject, "Calling 'saImmOmAdminOwnerFinalize' function.", m_objectName, p_scope, "");
	if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return retVal;
}

//---------------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT METHOD TRANSACTIONAL
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObject (const char * p_objectName, std::string p_transactionName) {
	HCF_CC_TRACE_FUNCTION;

	if (p_transactionName.length() == 0) {
		setInternalError(7);	// INVALID_PARAM
		HCF_CC_TRACE(DeleteObjectErr, "Empty transaction name provided!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newownerHandle;

	HCF_CC_TRACE(DeleteObject, "Received a request to delete an object, looking for the transaction!", p_objectName, -1, p_transactionName.c_str());
	int returnCode = getCcbHandle(p_transactionName, &newCcbHandle, &newownerHandle);
	if (returnCode != 0) {
		setInternalError(returnCode);
		HCF_CC_TRACE(DeleteObjectErr, "Failed to get handles for the provided transaction.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaNameT objToDelete;

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(p_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	memcpy(objToDelete.value, p_objectName, objToDelete.length);

	_objOwnedName.length = sprintf((char*)_objOwnedName.value, "%s", p_objectName);

	/*Set the administrative owner on the object to delete*/
	HCF_CC_TRACE(DeleteObject, "Received a request to delete an object: setting admin owner!", p_objectName, -1, p_transactionName.c_str());
	_errorCode = saImmOmAdminOwnerSet (newownerHandle, _objOwnedNameList, SA_IMM_ONE);
	if (_errorCode != SA_AIS_OK)	{
		setInternalError(_errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerSet' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/*Call the saImmOmCcbObjectDelete */
	HCF_CC_TRACE(DeleteObject, "Deleting the object!", p_objectName, -1, p_transactionName.c_str());
	SaAisErrorT error;
	if ((error = saImmOmCcbObjectDelete(newCcbHandle, &objToDelete)) != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbObjectDelete' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT METHOD TRANSACTIONAL WITH SCOPE MANAGEMENT
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObject (const char * p_objectName, std::string p_transactionName, IMM_API_ScopeTypes p_scope) {
	HCF_CC_TRACE_FUNCTION;

	if (p_transactionName.length() == 0) {
		setInternalError(7);	// INVALID_PARAM
		HCF_CC_TRACE(DeleteObjectErr, "Empty transaction name provided!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newownerHandle;

	HCF_CC_TRACE(DeleteObject, "Received a request to delete an object, looking for the transaction!", p_objectName, p_scope, p_transactionName.c_str());
	int returnCode = getCcbHandle(p_transactionName, &newCcbHandle, &newownerHandle);
	if (returnCode != 0) {
		setInternalError(returnCode);
		HCF_CC_TRACE(DeleteObjectErr, "Failed to get handles for the provided transaction.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/*added for scope management*/
	SaImmScopeT scopeVal = SA_IMM_ONE;

	if (p_scope == SCOPE_ONE)
		scopeVal = SA_IMM_ONE;
	else if (p_scope == SCOPE_SUBLEVEL)
		scopeVal = SA_IMM_SUBLEVEL;
	else if (p_scope == SCOPE_SUBTREE)
		scopeVal = SA_IMM_SUBTREE;
	/*end added for scope management*/

	SaNameT objToDelete;

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = strlen(p_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	memcpy(objToDelete.value, p_objectName, objToDelete.length);

	_objOwnedName.length = sprintf((char*)_objOwnedName.value, "%s", p_objectName);

	/*Set the administrative owner on the object to delete*/
	HCF_CC_TRACE(DeleteObject, "Received a request to delete an object: setting admin owner!", p_objectName, p_scope, p_transactionName.c_str());
	_errorCode = saImmOmAdminOwnerSet (newownerHandle, _objOwnedNameList, scopeVal);
	if (_errorCode != SA_AIS_OK)	{
		setInternalError(_errorCode);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmAdminOwnerSet' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/*Call the saImmOmCcbObjectDelete */
	SaAisErrorT error;
	if ((error = saImmOmCcbObjectDelete(newCcbHandle, &objToDelete)) != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(DeleteObjectErr, "Call 'saImmOmCcbObjectDelete' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// GET ATTRIBUTE METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::getAttribute (const char * m_objectName, IMM_API_ImmParameter * m_attributeValue) {
	HCF_CC_TRACE_FUNCTION;

	/*1 objectName. The Name of the object whose attribute has to be retrieved*/
	SaNameT objectName;

	/*2 attToBeRetrievedList. The attribute to be retrieved List must be a null terminated array of attribute names, according
	 * to saImmOmAccessorGet_2 function */
	SaImmAttrNameT attToBeRetrievedList[2];
	/*attToBeRetrieved is the data where is set attribute name for which values must be returned*/

	/*3 valuesRetrieved. The list of output values for saImmOmAccessorGet_2 */
	SaImmAttrValuesT_2** valuesRetrieved;

	/*Prepares the OBJECT NAME to be passed to saImmOmAccessorGet_2()*/
	objectName.length = strlen(m_objectName);
	memcpy(objectName.value, m_objectName, objectName.length);

	/*Prepares the ATTRIBUTES TO BE RETRIEVED LIST it must be a NULL terminated array*/
	attToBeRetrievedList[0] = m_attributeValue->attrName;
	attToBeRetrievedList[1] = 0;

	/*Prepares to Call the saImmOmAccessorGet  to perform get on the m_objectName for the attribute whose name
	 * is set in m_attributeValue->attrName field*/
	HCF_CC_TRACE(GetAttribute, "Received a request to retrieve an attribute value!", m_objectName, m_attributeValue->attrName);
	SaAisErrorT errorCode;
	if ((errorCode = saImmOmAccessorGet_2(_accessorHandle, &objectName, attToBeRetrievedList, &valuesRetrieved)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(GetAttributeErr, "Call 'saImmOmAccessorGet_2' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;

	}/*end if on saImmOmAccessorGet_2*/
	HCF_CC_TRACE(GetAttribute, "Attribute value correctly retrieved, preparing the attribute structure to be returned!", m_objectName, m_attributeValue->attrName);

	/*Prepares data to be returned to calling*/
	/*Set the attribute matching the condition found Type*/
	int i = 0;
	m_attributeValue->attrType = (IMM_API_AttrValueTypes)valuesRetrieved[0]->attrValueType;
	m_attributeValue->attrValuesNum = valuesRetrieved[i]->attrValuesNumber;

	// TR HO83231
	if ( m_attributeValue->flag )
		delete [] m_attributeValue->attrValues;

	m_attributeValue->attrValues = new void*[valuesRetrieved[i]->attrValuesNumber];
	m_attributeValue->IMM_API_IMMFreeMemory(1);

	/*only one attribute is returned according to the search criteria otherwise(more att searched) a while
	 * is needed
	 * while (valuesRetrieved[i]){*/
	for (unsigned int j=0; j<valuesRetrieved[i]->attrValuesNumber; j++){

		switch (valuesRetrieved[i]->attrValueType) {

			case SA_IMM_ATTR_SANAMET:{
				SaNameT* localSaName;
				localSaName = reinterpret_cast<SaNameT*>(valuesRetrieved[i]->attrValues[j]);
				m_attributeValue->attrValues[j]=localSaName->value;
				break;
			}/*end case SA_IMM_ATTR_SANAMET*/

			case SA_IMM_ATTR_SASTRINGT:{
				SaStringT stringVal = *(reinterpret_cast<SaStringT*>(valuesRetrieved[i]->attrValues[j]));
				m_attributeValue->attrValues[j] = reinterpret_cast<char*>(stringVal);
				break;
			}

			default:
				m_attributeValue->attrValues[j]=valuesRetrieved[i]->attrValues[j];
			break;
		}/*end switch */

	}
	i++;

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// GET MULTI - ATTRIBUTE METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::getAttribute (const char * p_objectName, std::vector<IMM_API_ImmAttribute *> p_attributeList) {
	HCF_CC_TRACE_FUNCTION;

	int numAttrToRetrive = p_attributeList.size();
	if (numAttrToRetrive == 0) {
		setInternalError(7);	// INVALID_PARAM
		HCF_CC_TRACE(GetAttributeErr, "Empty attributes list provided!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/*2 attToBeRetrievedList. The attribute to be retrieved List must be a null terminated array of attribute names, according
	 * to saImmOmAccessorGet_2 function */
	SaImmAttrNameT attToBeRetrievedList[ (numAttrToRetrive + 1) ];
	/*attToBeRetrieved is the data where is set attribute name for which values must be returned*/

	/*3 valuesRetrieved. The list of output values for saImmOmAccessorGet_2 */
	SaImmAttrValuesT_2** valuesRetrieved;

	/*The Name of the object whose attribute has to be retrieved*/
	SaNameT objectName;

	objectName.length = strlen(p_objectName);
	memcpy(objectName.value, p_objectName, objectName.length);

	/*Prepares the ATTRIBUTES TO BE RETRIEVED LIST it must be a NULL terminated array*/
	for (int i = 0; i < numAttrToRetrive; i++){
		attToBeRetrievedList[i] = const_cast<char*>(p_attributeList[i]->attrName.c_str());
	}

	attToBeRetrievedList[numAttrToRetrive] = 0;

	/*Prepares to Call the saImmOmAccessorGet  to perform get on the m_objectName for the attribute whose name
	 * is set in m_attributeValue->attrName field*/
	HCF_CC_TRACE(GetAttribute, "Received a request to retrieve some attributes values!", p_objectName, "MULTIPLE ATTRIBUTES");
	SaAisErrorT errorCode;
	if ((errorCode = saImmOmAccessorGet_2(_accessorHandle, &objectName, attToBeRetrievedList, &valuesRetrieved)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(GetAttributeErr, "Call 'saImmOmAccessorGet_2' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;

	}/*end if on saImmOmAccessorGet_2*/
	HCF_CC_TRACE(GetAttribute, "Attribute value correctly retrieved, preparing the attribute structure to be returned!", p_objectName, "MULTIPLE ATTRIBUTES");

	/*Prepares data to be returned to calling*/
	/*Set the attribute matching the condition found Type*/
	int i = 0;
	while( *valuesRetrieved != 0 ){

		for(unsigned k=0; k<p_attributeList.size(); k++ ){
			if( strcmp(p_attributeList[k]->attrName.c_str(), (*valuesRetrieved)->attrName) == 0){
				p_attributeList[k]->attrType = (IMM_API_AttrValueTypes)(*valuesRetrieved)->attrValueType;
				p_attributeList[k]->attrValuesNum = (*valuesRetrieved)->attrValuesNumber;

				for (unsigned int j = 0; j < (*valuesRetrieved)->attrValuesNumber; j++){
					switch ((*valuesRetrieved)->attrValueType) {

						case SA_IMM_ATTR_SANAMET:{
							SaNameT* localSaName;
							localSaName = reinterpret_cast<SaNameT*>((*valuesRetrieved)->attrValues[j]);

							StringType *val = new (std::nothrow) StringType;
							if ( val != 0 ){
								val->value = reinterpret_cast<char*>(localSaName->value);
								p_attributeList[k]->attrValues[j] = const_cast<char *>((val->value).c_str());

								p_attributeList[k]->pointers.push_back(val);
							}else {
								return OPERATION_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SASTRINGT:{
							SaStringT stringVal = *(reinterpret_cast<SaStringT*>((*valuesRetrieved)->attrValues[j]));

							StringType *val = new (std::nothrow) StringType;
							if ( val != 0 ) {
								if (stringVal){
									val->value = stringVal;
								}else {
									setInternalError(7);	// INVALID_PARAM
									HCF_CC_TRACE(GetAttributeErr, "Empty attribute retrieved!", getInternalLastError(), getInternalLastErrorText());
									return OPERATION_FAILURE;
								}

								p_attributeList[k]->attrValues[j] = const_cast<char *>((val->value).c_str());
								p_attributeList[k]->pointers.push_back(val);
							}else {
								return OPERATION_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SAUINT32T: {
							UnsIntType *val = new (std::nothrow) UnsIntType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<unsigned int*>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {
								return OPERATION_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SAINT32T: {
							IntType *val = new (std::nothrow) IntType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<int*>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {
								return OPERATION_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SAUINT64T: {
							UnsLongLongIntType *val = new (std::nothrow) UnsLongLongIntType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<unsigned long long int*>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {
								return OPERATION_FAILURE;
							}

							break;
						}

						case SA_IMM_ATTR_SAINT64T: {
							LongLongIntType *val = new (std::nothrow) LongLongIntType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<long long int*>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {
								return OPERATION_FAILURE;
							}
							break;
						}

						case SA_IMM_ATTR_SADOUBLET: {
							DoubleType *val = new (std::nothrow) DoubleType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<double*>((*valuesRetrieved)->attrValues[j]));
								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {
								return OPERATION_FAILURE;
							}
							break;
						}

						case SA_IMM_ATTR_SAFLOATT: {
							FloatType *val = new (std::nothrow) FloatType;
							if ( val != 0 ){
								val->value = *(reinterpret_cast<float *>((*valuesRetrieved)->attrValues[j]));

								p_attributeList[k]->attrValues[j] = &(val->value);
								p_attributeList[k]->pointers.push_back(val);
							}else {
								return OPERATION_FAILURE;
							}
							break;
						}

						default:
							p_attributeList[k]->attrValues[j] = (*valuesRetrieved)->attrValues[j];
						break;
					}/*end switch */

				}/*end for attrValuesNumber */

			}/*end if strcmp(...)*/

		}/*end for p_attributeList.size()*/

		i++;
		valuesRetrieved++;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// GET OBJECT METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::getObject(IMM_API_ImmObject * object) {
	HCF_CC_TRACE_FUNCTION;

	//get the DN of object to retrieve
	SaNameT objectName = {0,{0}};
	std::string temp_objName = object->objName;
	objectName.length = temp_objName.size();

	::memcpy(objectName.value, const_cast<char*>((temp_objName).c_str()), objectName.length);

	SaImmAttrValuesT_2** valuesRetrieved;

	/* call the saImmOmAccessorGet to perform get all attribute of object */
	HCF_CC_TRACE(GetObject, "Received a request to retrieve an entire object!", object->objName.c_str());
	SaAisErrorT errorCode;
	if ((errorCode = saImmOmAccessorGet_2(_accessorHandle, &objectName, 0, &valuesRetrieved)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(GetObjectErr, "Call 'saImmOmAccessorGet_2' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}
	HCF_CC_TRACE(GetObject, "The object was correctly retrieved, preparing the attribute structure to be returned!", object->objName.c_str());

	/* Prepares data to be returned to calling*/
	IMM_API_ImmAttribute attributeList;

	/* Use to save temporally the attribute name */
	std::string tmp = "";

	while ( *valuesRetrieved != 0 ){
		tmp = reinterpret_cast<char *>((*valuesRetrieved)->attrName);
		attributeList.attrName = tmp;
		attributeList.attrType = (int)(*valuesRetrieved)->attrValueType;
		attributeList.attrValuesNum = (*valuesRetrieved)->attrValuesNumber;

		for (unsigned int j = 0; j < (*valuesRetrieved)->attrValuesNumber; j++){

			/*Set the attribute matching the condition found Type*/
			switch ((*valuesRetrieved)->attrValueType) {

				case SA_IMM_ATTR_SANAMET:{
					SaNameT* localSaName;
					localSaName = reinterpret_cast<SaNameT*>((*valuesRetrieved)->attrValues[j]);

					StringType *val = new (std::nothrow) StringType;
					if ( val != 0 ){
						val->value = reinterpret_cast<char*>(localSaName->value);
						attributeList.attrValues[j] = const_cast<char *>((val->value).c_str());

						object->pointers.push_back(val);
					}else {
						return OPERATION_FAILURE;
					}

					break;
				}

				case SA_IMM_ATTR_SASTRINGT:{
					SaStringT stringVal = *(reinterpret_cast<SaStringT*>((*valuesRetrieved)->attrValues[j]));

					StringType *val = new (std::nothrow) StringType;
					if ( val != 0 ){
						if (stringVal){
							val->value = stringVal;
						}else {
							setInternalError(7);	// INVALID_PARAM
							HCF_CC_TRACE(GetAttributeErr, "Call 'saImmOmAccessorGet_2' failed.", errorCode, getInternalLastErrorText());
							return OPERATION_FAILURE;
						}
						attributeList.attrValues[j] = const_cast<char *>((val->value).c_str());
						object->pointers.push_back(val);

					}else {
						return OPERATION_FAILURE;
					}

					break;
				}

				case SA_IMM_ATTR_SAUINT32T: {
					UnsIntType *val = new (std::nothrow) UnsIntType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<unsigned int*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {
						return OPERATION_FAILURE;
					}
					break;
				}

				case SA_IMM_ATTR_SAINT32T: {
					IntType *val = new (std::nothrow) IntType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<int*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {
						return OPERATION_FAILURE;
					}

					break;
				}

				case SA_IMM_ATTR_SAUINT64T: {
					UnsLongLongIntType *val = new (std::nothrow) UnsLongLongIntType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<unsigned long long int*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {
						return OPERATION_FAILURE;
					}
					break;
				}

				case SA_IMM_ATTR_SAINT64T: {
					LongLongIntType *val = new (std::nothrow) LongLongIntType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<long long int*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {
						return OPERATION_FAILURE;
					}
					break;
				}

				case SA_IMM_ATTR_SADOUBLET: {
					DoubleType *val = new (std::nothrow) DoubleType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<double*>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {
						return OPERATION_FAILURE;
					}
					break;
				}

				case SA_IMM_ATTR_SAFLOATT: {
					FloatType *val = new (std::nothrow) FloatType;
					if ( val != 0 ){
						val->value = *(reinterpret_cast<float *>((*valuesRetrieved)->attrValues[j]));
						attributeList.attrValues[j] = &(val->value);
						object->pointers.push_back(val);
					}else {
						return OPERATION_FAILURE;
					}
					break;
				}

				default:
					attributeList.attrValues[j] = (*valuesRetrieved)->attrValues[j];
				break;
			}/*end switch */

		}

		object->attributes.push_back(attributeList);
		valuesRetrieved++;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// GET CHILDREN METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::getChildren (const char * p_rootName, IMM_API_ScopeTypes p_scope, std::vector<std::string> * p_rdnList) {
	HCF_CC_TRACE_FUNCTION;

	if (p_rootName == 0) {
		setInternalError(7);	// INVALID_PARAM
		HCF_CC_TRACE(GetChildrenErr, "NULL root DN provided: not possible to search children under it!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaNameT rootName;
	rootName.length = strlen(p_rootName);
	::memcpy(rootName.value, p_rootName, rootName.length);

	SaImmScopeT scope;

	if(p_scope == SCOPE_ONE){
		scope = SA_IMM_ONE;
	}else if( p_scope == SCOPE_SUBLEVEL){
		scope = SA_IMM_SUBLEVEL;
	}else if (p_scope == SCOPE_SUBTREE ){
		scope = SA_IMM_SUBTREE;
	}else {
		scope = SA_IMM_SUBLEVEL;
	}

	SaImmSearchOptionsT searchOption = SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR;
	SaImmSearchParametersT_2 searchParam;
	searchParam.searchOneAttr.attrName = 0;
	searchParam.searchOneAttr.attrValue = 0;

	SaImmSearchHandleT searchHandle;

	HCF_CC_TRACE(GetChildren, "Received a request to retrieve all the children of a given node: initializing search!", p_rootName, p_scope);
	SaAisErrorT errorCode;
	if ((errorCode = saImmOmSearchInitialize_2(_immHandle, &rootName, scope, searchOption, &searchParam, 0, &searchHandle)) != SA_AIS_OK ){
		setInternalError(errorCode);
		HCF_CC_TRACE(GetChildrenErr, "Call 'saImmOmSearchInitialize_2' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaNameT objectName;
	SaImmAttrValuesT_2 **attr;
	std::string rdn = "";

	HCF_CC_TRACE(GetChildren, "Search correctly initialized: looping in order to find all the children!", p_rootName, p_scope);
	while ((errorCode = saImmOmSearchNext_2(searchHandle, &objectName, &attr)) == SA_AIS_OK) {
		rdn = reinterpret_cast<char *>(objectName.value);

		if ((::strcmp(p_rootName, rdn.c_str()) != 0))	{
			p_rdnList->push_back(rdn);
		}
	}

	HCF_CC_TRACE(GetChildren, "Search completed: finalizing search!", p_rootName, p_scope);
	if ((errorCode = saImmOmSearchFinalize(searchHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(GetChildrenErr, "Call 'saImmOmSearchFinalize' failed.", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// GET CLASS INSTANCE METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::getClassInstances (const char * p_className, std::vector<std::string> & p_dnList) {
	HCF_CC_TRACE_FUNCTION;

	if (p_className == 0) {
		setInternalError(7);	// INVALID_PARAM
		HCF_CC_TRACE(GetClassInstancesErr, "NULL class name provided: not possible to search its instances!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaAisErrorT error;
	SaNameT rootName = { 0, "" };
	SaNameT objectName;
	SaImmAttrValuesT_2 ** attributes;
	SaImmSearchHandleT searchHandle;

	SaImmSearchParametersT_2 searchParam;
	searchParam.searchOneAttr.attrName = const_cast<char *>("SaImmAttrClassName");
	searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SASTRINGT;
	searchParam.searchOneAttr.attrValue = &p_className;

	HCF_CC_TRACE(GetClassInstances, "Received a request to retrieve all the instances of a given class: initializing search!", p_className);
	if ((error = saImmOmSearchInitialize_2(_immHandle, &rootName, SA_IMM_SUBTREE, SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, 0, &searchHandle)) != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(GetClassInstancesErr, "Call 'saImmOmSearchInitialize_2' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	std::string rdnValue = "";

	HCF_CC_TRACE(GetClassInstances, "Search correctly initialized: looping in order to find all the instances!", p_className);
	while ((error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes)) == SA_AIS_OK) {
		if ((error != SA_AIS_OK) && (error != SA_AIS_ERR_NOT_EXIST)) {
			setInternalError(error);
			HCF_CC_TRACE(GetClassInstancesErr, "Call 'saImmOmSearchNext_2' failed: finalizing search!", getInternalLastError(), getInternalLastErrorText());

			if ((error = saImmOmSearchFinalize(searchHandle)) != SA_AIS_OK) {
				HCF_CC_TRACE(GetClassInstancesErr, "Call 'saImmOmSearchFinalize' failed.", error, IMM_API_ERROR_MESSAGES[error]);
			}

			return OPERATION_FAILURE;
		}

		if (error != SA_AIS_ERR_NOT_EXIST) {
			rdnValue = reinterpret_cast<char*>(objectName.value);
			p_dnList.push_back(rdnValue);
		}
	}

	if(!p_dnList.size())
		HCF_CC_TRACE(GetClassInstancesErr, "WARNING: Search didn't find any instance!", 0, IMM_API_ERROR_MESSAGES[0]);

	HCF_CC_TRACE(GetClassInstances, "Search completed: finalizing search!", p_className);
	if ((error = saImmOmSearchFinalize(searchHandle)) != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(GetClassInstancesErr, "Call 'saImmOmSearchFinalize' failed: finalizing search!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// MODIFY ATTRIBUTE METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyAttribute (const char * m_objectName, IMM_API_ImmParameter * m_attributeValue) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	IMM_API_ReturnTypes retVal = OPERATION_SUCCESS;
	SaNameT *parSaName = 0;
	std::vector<SaNameT*> parSaNameVector;

	/*objectName. The Name of the object whose attribute has to be modified*/
	SaNameT objectName;

	/*attrMods. Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modifications to perform*/
	SaImmAttrModificationT_2* attrMods[2] = {0, 0};

	SaImmAttrModificationT_2 attr;

	/*Prepares the OBJECT NAME to be passed to saImmOmCcbObjectModify_2()*/
	objectName.length = strlen(m_objectName);
	memcpy(objectName.value, m_objectName, strlen(m_objectName));

	/*Initialization Section Start*/
	SaImmCcbHandleT ccbHandle;
	SaImmAdminOwnerHandleT ownerHandle;

	/**Initialize the Object Administration ownership to perform operations on IMM
	 *
	 *_immHandle is the handle retrieved by the previous call
	 *second parameter is the Administrative Owner name
	 *third parameter specifies whether to release the Management Ownership
	 *				  when finalizing the ownerHandle
	 *ownerHandle is the output parameter where the Management Ownership handler
	 *			  is stored
	 */
	HCF_CC_TRACE(ModifyAttribute, "Received a request to modify the value of an object attribute: initializing admin owner!", m_objectName, m_attributeValue->attrName, "");
	if ((errorCode = saImmOmAdminOwnerInitialize(_immHandle, _processName, SA_TRUE, &ownerHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmAdminOwnerInitialize' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	const SaNameT *objOwnedNameList[] = {&objectName, 0};

	HCF_CC_TRACE(ModifyAttribute, "Received a request to modify the value of an object attribute: setting admin owner!", m_objectName, m_attributeValue->attrName, "");
	if ((errorCode = saImmOmAdminOwnerSet(ownerHandle, objOwnedNameList, SA_IMM_ONE)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmAdminOwnerSet' failed: finalizing admin owner!", getInternalLastError(), getInternalLastErrorText());

		/*release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}
		/*end release ownerHandle*/

		return OPERATION_FAILURE;
	}

	/*Retrieve the CCB handler to perform operation IMM*/
	HCF_CC_TRACE(ModifyAttribute, "Received a request to modify the value of an object attribute: initializing the CCB!", m_objectName, m_attributeValue->attrName, "");
	if ((errorCode = saImmOmCcbInitialize(ownerHandle, _ccbFlagVal, &ccbHandle)) != SA_AIS_OK) {
			setInternalError(errorCode);
			HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmCcbInitialize' failed: finalizing admin owner!", getInternalLastError(), getInternalLastErrorText());

			/*release ownerHandle*/
			if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
				HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
			}
			/*end release ownerHandle*/
			return OPERATION_FAILURE;
	}
	/*Initialization Section End*/
	HCF_CC_TRACE(ModifyAttribute, "Received a request to modify the value of an object attribute: preparing the attributes vector!", m_objectName, m_attributeValue->attrName, "");

	/*Prepares data of the attribute to be change */
	/*Set the type of modification to apply on the object attribute*/
	attr.modType=SA_IMM_ATTR_VALUES_REPLACE;

	/*Set the attribute name*/
	attr.modAttr.attrName = (*m_attributeValue).attrName;

	/*Set the attribute Type*/
	attr.modAttr.attrValueType = (SaImmValueTypeT)(*m_attributeValue).attrType;

	/*Set the Attribute Values Number*/
	attr.modAttr.attrValuesNumber = (*m_attributeValue).attrValuesNum;

	/*Set the array where is memorize the new value of attribute*/
	void* attrValues[(*m_attributeValue).attrValuesNum];
	attr.modAttr.attrValues = attrValues;

	/*Set the new values of the attributes*/
	if(attr.modAttr.attrValueType == SA_IMM_ATTR_SANAMET){

		for(unsigned int i=0; i<(*m_attributeValue).attrValuesNum; i++){
			parSaName = new SaNameT;
			parSaName->length = strlen(reinterpret_cast<char*>((*m_attributeValue).attrValues[i]));
			memcpy(parSaName->value, ((*m_attributeValue).attrValues[i]), parSaName->length);
			attr.modAttr.attrValues[i] = reinterpret_cast<void *>(parSaName);
			parSaNameVector.push_back(parSaName);
		}

	}else if(attr.modAttr.attrValueType==SA_IMM_ATTR_SASTRINGT){

		for(unsigned int i=0; i<(*m_attributeValue).attrValuesNum; i++){
			attr.modAttr.attrValues[i] = &((*m_attributeValue).attrValues[i]);
		}

	}else{
		attr.modAttr.attrValues = (*m_attributeValue).attrValues;
	}

	/*Prepares the LIST OF THE ATTRIBUTES TO BE CHANGE. It must be a NULL terminated array*/
	attrMods[0] = &attr;
	attrMods[1] = 0;

	/* saImmOmCcbObjectModify_2 adds to the CCB identified by its handle ccbHandle a request to modify
	 * configuration attributes of an IMM Service object.*/
	HCF_CC_TRACE(ModifyAttribute, "Calling 'saImmOmCcbObjectModify_2' function.", m_objectName, m_attributeValue->attrName, "");
	errorCode = saImmOmCcbObjectModify_2(ccbHandle, &objectName, (const SaImmAttrModificationT_2**)attrMods);

	/*to free allocate memory*/
	if(parSaNameVector.size() > 0){
		std::vector<SaNameT *>::iterator it_2;
		for(it_2 = parSaNameVector.begin(); it_2 != parSaNameVector.end(); it_2++){
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmCcbObjectModify_2' failed: finalizing CCB and admin owner!", getInternalLastError(), getInternalLastErrorText());

		callSaImmOmCcbGetErrStr(ccbHandle);

		/** release ccbHandler**/
		if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		/* release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}
		/*End Added for CCB corruption management*/

		return OPERATION_FAILURE;
	}

	/*saImmOmCcbApply applies all requests included in the configuration change bundle identified
	 * by its handle ccbHandle*/
	HCF_CC_TRACE(ModifyAttribute, "Calling 'saImmOmCcbApply' function.", m_objectName, m_attributeValue->attrName, "");
	if ((errorCode = saImmOmCcbApply(ccbHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmCcbApply' failed: finalizing CCB and admin owner!", getInternalLastError(), getInternalLastErrorText());

		callSaImmOmCcbGetErrStr(ccbHandle);

		/** release ccbHandler**/
		if ((errorCode = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmCcbFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		/* release ownerHandle*/
		if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmAdminOwnerFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}
		/*End Added for CCB corruption management*/

		return OPERATION_FAILURE;
	}

	/*Finalize handle Section Start*/

	/** release ccbHandler**/
	HCF_CC_TRACE(ModifyAttribute, "Calling 'saImmOmCcbFinalize' function.", m_objectName, m_attributeValue->attrName, "");
	if ((errorCode = saImmOmCcbFinalize(ccbHandle)) !=  SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmCcbFinalize' failed.", getInternalLastError(), getInternalLastErrorText());
		retVal = OPERATION_FAILURE;
	}

	/* release ownerHandle*/
	HCF_CC_TRACE(ModifyAttribute, "Calling 'saImmOmAdminOwnerFinalize' function.", m_objectName, m_attributeValue->attrName, "");
	if ((errorCode = saImmOmAdminOwnerFinalize(ownerHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmAdminOwnerFinalize' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}
	/*Finalize handle Section End*/

	return retVal;
}

//---------------------------------------------------------------------------------------------------------------------------
// MODIFY ATTRIBUTE METHOD TRANSACTIONAL
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyAttribute (const char * p_objectName, IMM_API_ImmParameter * p_attributeValue, std::string p_transactionName) {
	HCF_CC_TRACE_FUNCTION;

	if (p_transactionName.length() == 0) {
		setInternalError(7);	// INVALID_PARAM
		HCF_CC_TRACE(ModifyAttributeErr, "Empty transaction name provided!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/*Get the CCB handle of transaction*/
	SaImmCcbHandleT newCcbHandle;
	SaImmAdminOwnerHandleT newownerHandle;

	HCF_CC_TRACE(ModifyAttribute, "Received a request to modify the value of an object attribute: looking for the transaction!", p_objectName, p_attributeValue->attrName, p_transactionName.c_str());
	int returnCode = getCcbHandle(p_transactionName, &newCcbHandle, &newownerHandle);
	if (returnCode != 0) {
		setInternalError(returnCode);
		HCF_CC_TRACE(ModifyAttributeErr, "Failed to get handles for the provided transaction.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaNameT *parSaName = 0;
	std::vector<SaNameT*> parSaNameVector;
	SaNameT objectName;

	/*attrMods. Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modifications to perform*/
	SaImmAttrModificationT_2* attrMods[2] = {0, 0};
	SaImmAttrModificationT_2 attr;

	/*Prepares the OBJECT NAME to be passed to saImmOmCcbObjectModify_2()*/
	objectName.length = strlen(p_objectName);
	memcpy(objectName.value, p_objectName, strlen(p_objectName));

	_objOwnedName.length = sprintf((char*)_objOwnedName.value, "%s", p_objectName);

	/*Set Administrative Owner on the object to modify */
	HCF_CC_TRACE(ModifyAttribute, "Received a request to modify the value of an object attribute: setting admin owner!", p_objectName, p_attributeValue->attrName, p_transactionName.c_str());
	if ((_errorCode = saImmOmAdminOwnerSet (newownerHandle, _objOwnedNameList, SA_IMM_ONE)) != SA_AIS_OK)	{
		setInternalError(_errorCode);
		HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmAdminOwnerSet' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}
	HCF_CC_TRACE(ModifyAttribute, "Received a request to modify the value of an object attribute: preparing the attributes vector!", p_objectName, p_attributeValue->attrName, p_transactionName.c_str());

	/*Prepares data of the attribute to be change */
	/*Set the type of modification to apply on the object attribute*/
	attr.modType=SA_IMM_ATTR_VALUES_REPLACE;

	/*Set the attribute name*/
	attr.modAttr.attrName = (*p_attributeValue).attrName;

	/*Set the attribute Type*/
	attr.modAttr.attrValueType = (SaImmValueTypeT)(*p_attributeValue).attrType;

	/*Set the Attribute Values Number*/
	attr.modAttr.attrValuesNumber = (*p_attributeValue).attrValuesNum;

	/*Set the array where is memorize the new value of attribute*/
	void* attrValues[(*p_attributeValue).attrValuesNum];
	attr.modAttr.attrValues = attrValues;

	/*Set the new values of the attributes*/
	if(attr.modAttr.attrValueType == SA_IMM_ATTR_SANAMET){

		for(unsigned int i=0; i<(*p_attributeValue).attrValuesNum; i++){
			parSaName = new SaNameT;
			parSaName->length = strlen(reinterpret_cast<char*>((*p_attributeValue).attrValues[i]));
			memcpy(parSaName->value, ((*p_attributeValue).attrValues[i]), parSaName->length);
			attr.modAttr.attrValues[i] = reinterpret_cast<void *>(parSaName);
			parSaNameVector.push_back(parSaName);
		}

	}else if(attr.modAttr.attrValueType==SA_IMM_ATTR_SASTRINGT){

		for(unsigned int i=0; i<(*p_attributeValue).attrValuesNum; i++){
			attr.modAttr.attrValues[i] = &((*p_attributeValue).attrValues[i]);
		}

	}else{
		attr.modAttr.attrValues = (*p_attributeValue).attrValues;
	}

	/*Prepares the LIST OF THE ATTRIBUTES TO BE CHANGE. It must be a NULL terminated array*/
	attrMods[0] = &attr;
	attrMods[1] = 0;

	/* saImmOmCcbObjectModify_2 adds to the CCB identified by its handle ccbHandle a request to modify
	 * configuration attributes of an IMM Service object.*/
	HCF_CC_TRACE(ModifyAttribute, "Calling 'saImmOmCcbObjectModify_2' function.", p_objectName, p_attributeValue->attrName, p_transactionName.c_str());
	SaAisErrorT error = saImmOmCcbObjectModify_2(newCcbHandle, &objectName, (const SaImmAttrModificationT_2**)attrMods);

	/*to free allocate memory*/
	if(parSaNameVector.size() > 0){
		std::vector<SaNameT *>::iterator it_2;
		for(it_2 = parSaNameVector.begin(); it_2 != parSaNameVector.end(); it_2++){
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}

	if (error != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(ModifyAttributeErr, "Call 'saImmOmCcbObjectModify_2' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// APPLY REQUEST METHOD
// applyRequest applies all requests included in the configuration change bundle identified
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::applyRequest (std::string p_transactionName) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT error;
	std::map<std::string, SaImmCcbHandleT>::iterator it;
	if ((it = _transactionList.find(p_transactionName)) == _transactionList.end()){
		setInternalError(42);	// TRANSACTION NOT FOUND
		HCF_CC_TRACE(ApplyRequestErr, "The provided transaction does not exist!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaImmCcbHandleT newCcbHandle = it->second;
	SaImmAdminOwnerHandleT newOwnerHandle = _transactionListOwnerHandle.find(p_transactionName)->second;

	HCF_CC_TRACE(ApplyRequest, "Received a request to apply the transaction: calling 'saImmOmCcbApply' function.", p_transactionName.c_str());
	if ((error = saImmOmCcbApply(newCcbHandle)) != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(ApplyRequestErr, "Call 'saImmOmCcbApply' failed.", getInternalLastError(), getInternalLastErrorText());

		callSaImmOmCcbGetErrStr(newCcbHandle);
		return OPERATION_FAILURE;
	}

	HCF_CC_TRACE(ApplyRequest, "Transaction applied: finalizing CCB.", p_transactionName.c_str());
	if ((error = saImmOmCcbFinalize(newCcbHandle)) != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(ApplyRequestErr, "Call 'saImmOmCcbFinalize' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}
	_transactionList.erase(p_transactionName);

	HCF_CC_TRACE(ApplyRequest, "Transaction applied: finalizing admin owner.", p_transactionName.c_str());
	if ((_errorCode = saImmOmAdminOwnerFinalize(newOwnerHandle)) != SA_AIS_OK) {
		setInternalError(_errorCode);
		HCF_CC_TRACE(ApplyRequestErr, "Call 'saImmOmAdminOwnerFinalize' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}
	_transactionListOwnerHandle.erase(p_transactionName);

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// RESET REQUEST METHOD
// applyRequest applies all requests included in the configuration change bundle identified
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::resetRequest (std::string p_transactionName) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT error;
	std::map<std::string, SaImmCcbHandleT>::iterator it;
	if ((it = _transactionList.find(p_transactionName)) == _transactionList.end()){
		setInternalError(42);	// TRANSACTION NOT FOUND
		HCF_CC_TRACE(ResetRequestErr, "The provided transaction does not exist!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaImmCcbHandleT newCcbHandle = it->second;
	SaImmAdminOwnerHandleT newOwnerHandle = _transactionListOwnerHandle.find(p_transactionName)->second;

	HCF_CC_TRACE(ResetRequest, "Received a request to reset the transaction: finalizing the CCB.", p_transactionName.c_str());
	if ((error = saImmOmCcbFinalize(newCcbHandle)) != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(ResetRequestErr, "Call 'saImmOmCcbFinalize' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}
	_transactionList.erase(p_transactionName);

	HCF_CC_TRACE(ResetRequest, "Received a request to reset the transaction: finalizing the admin owner.", p_transactionName.c_str());
	if ((_errorCode = saImmOmAdminOwnerFinalize(newOwnerHandle)) != SA_AIS_OK) {
		setInternalError(_errorCode);
		HCF_CC_TRACE(ResetRequestErr, "Call 'saImmOmAdminOwnerFinalize' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}
	_transactionListOwnerHandle.erase(p_transactionName);

	return OPERATION_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------------
// GET INTERNAL LAST ERROR TEXT METHOD
//---------------------------------------------------------------------------------------------------------------------------
const char * __TSL_CLASS_NAME__::getInternalLastErrorText() {
	if ((_errorCode > 0) && (_errorCode < IMM_API_INTERNAL_NUMBER_OF_ERROR_MESSAGES)) {
		return IMM_API_ERROR_MESSAGES[_errorCode];
	}
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------
// GET EXIT CODE TEXT METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::getExitCode (int & p_errorId, std::string & p_errorText) {
	HCF_CC_TRACE_FUNCTION;

	IMM_API_ReturnTypes rVal = OPERATION_SUCCESS;

	/*added for temporary version : COMSA Rule 101
	 * currently the lib returns just the errorString passed by the OI implementer and 1 as default value*/
	std::string tag = "@ComNbi@";
	size_t lenMessageString = _exitCodeStringRetrieved.size() - tag.size();

	if ((_exitCodeStringRetrieved.find(tag) != std::string::npos) && (lenMessageString > 0)) {
		p_errorText = _exitCodeStringRetrieved.substr(tag.size(), lenMessageString);
		p_errorId = 1;
	}
	else {	/*Not Found */
		setInternalError(44);//set generic error
		rVal = OPERATION_FAILURE;/*--not found*/
	}

	HCF_CC_TRACE(ExitCode, "Retrieved values for exit code.", p_errorId, p_errorText.c_str(), "");
	_exitCodeStringRetrieved = "";
	return rVal;
}

//---------------------------------------------------------------------------------------------------------------------------
// GET EXIT CODE METHOD
//---------------------------------------------------------------------------------------------------------------------------
IMM_API_ReturnTypes __TSL_CLASS_NAME__::getExitCode (std::string p_transactionName, int & p_errorId, std::string & p_errorText) {
	HCF_CC_TRACE_FUNCTION;

	IMM_API_ReturnTypes rVal = OPERATION_SUCCESS;

	std::map<std::string, SaImmCcbHandleT>::iterator it;
	if ((it = _transactionList.find(p_transactionName)) == _transactionList.end()) {
		setInternalError(42);	// TRANSACTION NOT FOUND
		HCF_CC_TRACE(ExitCodeErr, "The provided transaction does not exist!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaImmCcbHandleT newCcbHandle;
	newCcbHandle = _transactionList.find(p_transactionName)->second;

	//search Error string if present
	const SaStringT * errorStringFromOi = 0;
	SaAisErrorT error;

	HCF_CC_TRACE(ExitCode, "Retrieving the string from the OI:", 0, p_errorText.c_str(), "");
	if ((error = saImmOmCcbGetErrorStrings(newCcbHandle, &errorStringFromOi)) != SA_AIS_OK) {
		setInternalError(error);
		HCF_CC_TRACE(ExitCodeErr, "Call 'saImmOmCcbGetErrorStrings' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	if (!errorStringFromOi) {
		setInternalError(44);//set generic error
		HCF_CC_TRACE(ExitCodeErr, "Call 'saImmOmCcbGetErrorStrings' retrieved NULL string!", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	std::string errorFound(*errorStringFromOi);

	/*added for temporary version : COMSA Rule 101
	 * currently the lib returns just the errorString passed by the OI implementer and 1 as default value*/
	std::string tag = "@ComNbi@";
	size_t lenMessageString = errorFound.size() - tag.size();

	if ((errorFound.find(tag) != std::string::npos) && (lenMessageString > 0)) {
		p_errorText = errorFound.substr(tag.size(), lenMessageString );
		p_errorId = 1;
	}
	else { /*Not Found */
		setInternalError(44);//set generic error
		rVal = OPERATION_FAILURE;
	}
	/*end added for COMSA*/

	HCF_CC_TRACE(ExitCode, "Retrieved values for exit code.", p_errorId, p_errorText.c_str(), p_transactionName.c_str());
	return rVal;
}

//---------------------------------------------------------------------------------------------------------------------------
// GET Ccb HANDLE METHOD
//---------------------------------------------------------------------------------------------------------------------------
int __TSL_CLASS_NAME__::getCcbHandle (std::string p_transactionName, SaImmCcbHandleT * p_newCcbHandle, SaImmAdminOwnerHandleT * p_ownerHandle) {
	HCF_CC_TRACE_FUNCTION;

	std::map<std::string, SaImmCcbHandleT>::iterator it = _transactionList.find(p_transactionName);
	if(it == _transactionList.end()) {

		/*Initialize new ownerHandle*/
		HCF_CC_TRACE(GetCcbHandle, "Calling 'saImmOmAdminOwnerInitialize' function.", p_transactionName.c_str());
		if ((_errorCode = saImmOmAdminOwnerInitialize(_immHandle, (char *) "CBA_TRANSACTION", SA_TRUE, p_ownerHandle)) != SA_AIS_OK) {
			setInternalError(_errorCode);
			HCF_CC_TRACE(GetCcbHandleErr, "Call 'saImmOmAdminOwnerInitialize' failed.", getInternalLastError(), getInternalLastErrorText());
			return OPERATION_FAILURE;
		}

		/*Initialize new ccbHandle*/
		HCF_CC_TRACE(GetCcbHandle, "Calling 'saImmOmCcbInitialize' function.", p_transactionName.c_str());
		SaAisErrorT error;
		if ((error = saImmOmCcbInitialize((*p_ownerHandle), _ccbFlagVal , p_newCcbHandle)) != SA_AIS_OK) {
			setInternalError(error);
			HCF_CC_TRACE(GetCcbHandleErr, "Call 'saImmOmCcbInitialize' failed.", getInternalLastError(), getInternalLastErrorText());
			return OPERATION_FAILURE;
		}

		/*Add new ccb at the map*/
		_transactionList[p_transactionName] = *p_newCcbHandle;
		/*Add new ownerHandle at the map*/
		_transactionListOwnerHandle[p_transactionName] = *p_ownerHandle;
	}
	else {
		*p_newCcbHandle = it->second;
		*p_ownerHandle = _transactionListOwnerHandle.find(p_transactionName)->second;
	}

	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------
// Method to get the error string from OI and to set the retrieved string in exitCodeStringRetrieved variable Ticket 1904
//---------------------------------------------------------------------------------------------------------------------------
int __TSL_CLASS_NAME__::callSaImmOmCcbGetErrStr(SaImmCcbHandleT p_ccbVal){
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCodeForGetErr;
	const SaStringT * errorStringFromOi = 0;

	/*clear the previous value for exitCodeStringRetrieved*/
	_exitCodeStringRetrieved.clear();

	/*get the error*/
	if ((errorCodeForGetErr = saImmOmCcbGetErrorStrings(p_ccbVal, &errorStringFromOi)) != SA_AIS_OK) {
		setInternalError(errorCodeForGetErr);
		return -1; /*on failure*/
	}

	if (errorStringFromOi != 0) {
		_exitCodeStringRetrieved = (*errorStringFromOi);
		errorStringFromOi++;
	}

	return 0;
}

TSL_NS_HCF_IMM_END
