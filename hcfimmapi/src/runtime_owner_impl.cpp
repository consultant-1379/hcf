#include <string.h>

#include "hcf/imm/runtime_owner.h"
#include "hcf/imm/imm_error_constants.h"

#include "hcf/trace/com_ericsson_common_hcf_imm_runtimeowner.h"
#include "hcf/trace/tracing_macros.h"

#include "runtime_owner_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER_IMPL__

TSL_NS_HCF_IMM_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
std::map<SaUint64T, runtime_owner *> __TSL_CLASS_NAME__::_implementer_list;

IMM_API_ReturnTypes __TSL_CLASS_NAME__::init (std::string impName, runtime_owner * p_runtimeOwn) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;

	_runtime_owner = p_runtimeOwn;

	SaVersionT version;
	version.releaseCode  = IMM_LIBRARY_RELEASE_CODE;
	version.majorVersion = IMM_LIBRARY_MAJOR_VERSION;
	version.minorVersion = IMM_LIBRARY_MINOR_VERSION;

	SaImmOiCallbacksT_2 immCallbacks;

	/** Set the SaImmOiCallbacksT_2 structure which contains the callback function
	 * of the process that the IMM Service may invoke**/
	immCallbacks.saImmOiCcbObjectCreateCallback = 0;
	immCallbacks.saImmOiCcbObjectModifyCallback = 0;
	immCallbacks.saImmOiCcbObjectDeleteCallback = 0;
	immCallbacks.saImmOiCcbCompletedCallback = 0;
	immCallbacks.saImmOiCcbApplyCallback = 0;
	immCallbacks.saImmOiCcbAbortCallback = 0;
	immCallbacks.saImmOiRtAttrUpdateCallback = __TSL_CLASS_NAME__::OiRtAttUpdateCallback;
	immCallbacks.saImmOiAdminOperationCallback = __TSL_CLASS_NAME__::adminOperationCallback;

	/**Initialize the Object Implementer functions of the IMM Service for the invoking process
	 * and register the various callback functions **/
	HCF_CC_TRACE(Init, "Calling 'saImmOiInitialize_2' function.", "");
	if ((errorCode = saImmOiInitialize_2(&_oi_handle, &immCallbacks , &version)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(InitErr, "Call 'saImmOiInitialize_2' failed", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	SaImmOiImplementerNameT implementerName = const_cast<char*>(impName.c_str());

	/**Set the implementer name specified in the implementerName for the handle immOihandle
	 * and registers the invoking process as Object implementer having the name which is
	 * specified in the implementerName parameter.  **/
	HCF_CC_TRACE(Init, "Calling 'saImmOiImplementerSet' function with implementer name:", impName.c_str());
	if ((errorCode = saImmOiImplementerSet(_oi_handle, implementerName)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(InitErr, "Call 'saImmOiImplementerSet' failed: finalizing IMM OI handle!", getInternalLastError(), getInternalLastErrorText());

		/**Release immOihandle*/
		if ((errorCode = saImmOiFinalize(_oi_handle)) != SA_AIS_OK) {
			HCF_CC_TRACE(InitErr, "Call 'saImmOiFinalize' failed.", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}
		/**End release immOiHandle*/

		return OPERATION_FAILURE;
	}

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	HCF_CC_TRACE(Init, "Calling 'saImmOiSelectionObjectGet' to get the selectable handle for runtime owner.", impName.c_str());
	if ((errorCode = saImmOiSelectionObjectGet(_oi_handle, &_sel_obj)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(InitErr, "Call 'saImmOiSelectionObjectGet' failed", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/**Add implementer at the list**/
	_implementer_list[_oi_handle] = _runtime_owner;
	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::finalize () {
	HCF_CC_TRACE_FUNCTION;

	/* clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer for the name previously associated
	 *
	 * immOiHandle is the handle witch was obtained by a previous invocation of the
	 * saImmOiInitialize_2 function
	 * */
	HCF_CC_TRACE(Finalize, "Calling 'saImmOiImplementerClear' function.");
	SaAisErrorT errorCode;
	if ((errorCode = saImmOiImplementerClear(_oi_handle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(FinalizeErr, "Call 'saImmOiImplementerClear' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	_implementer_list.erase(_oi_handle);

	/* closes the association represented by the immOiHandle parameter between the
	 * invoking process and the information Model
	 *
	 * immOiHandle is the ahandle witch was obtained by a previous invocation of the
	 * saImmOiInitialize_2 and witch identifies this particular initization of the
	 * IMM service
	 * */
	HCF_CC_TRACE(Finalize, "Calling 'saImmOiFinalize' function.");
	if ((errorCode = saImmOiFinalize(_oi_handle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(FinalizeErr, "Call 'saImmOiFinalize' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::createRuntimeObj (const char * p_className, const char * p_parentName, std::vector<IMM_API_ValuesDefinitionTypes> m_attrValuesList) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaNameT * name = 0;
	std::vector<SaNameT *> SaNameVector;
	std::vector<IMM_API_ValuesDefinitionTypes>::iterator it;
	SaImmClassNameT className = const_cast<char*>(p_className);

	SaNameT parentName = {0, {0}};
	parentName.length = ::strlen(p_parentName);
	::memcpy(parentName.value, p_parentName, parentName.length);

	HCF_CC_TRACE(CreateRuntimeObject, "Received a request to create a runtime object: creating the attributes vector!", p_className, p_parentName);

	/* Create the list of Attribute Values to be provided as input to the saImmOiRtObjectCreate_2 function*/
	/*allocate a  list of m_attrValuesList.size()+1 (SaImmAttrValuesT_2*) */
	SaImmAttrValuesT_2 ** attributesValues = 0;
	attributesValues = new SaImmAttrValuesT_2 * [m_attrValuesList.size() + 1];

	int i = 0;
	int numAttribute = m_attrValuesList.size();
	SaImmAttrValuesT_2 _singleAttributeValues[numAttribute];

	for (it = m_attrValuesList.begin(); it != m_attrValuesList.end(); ++it) {
		/*Set the attribute Name*/
		_singleAttributeValues[i].attrName = (SaImmAttrNameT)((*it).attrName);

		/*Set the attribute Type*/
		_singleAttributeValues[i].attrValueType = (SaImmValueTypeT)((*it).attrType);

		/*Set the Attribute Values Number*/
		unsigned int numElem = (*it).attrValuesNum;

		_singleAttributeValues[i].attrValuesNumber = (SaUint32T)(numElem);

		/*Set attribute values To be done*/

		_singleAttributeValues[i].attrValues = new void * [numElem];

		for (unsigned int j = 0; j < numElem; ++j) {

			switch ((*it).attrType) {
			case ATTR_INT32T:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT32T:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_INT64T:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_TIMET:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_UINT64T:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_DOUBLET:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_FLOATT:
				_singleAttributeValues[i].attrValues[j] = (*it).attrValues[j];
				break;

			case ATTR_STRINGT:
				_singleAttributeValues[i].attrValues[j] = &((*it).attrValues[j]);
				break;

			case ATTR_NAMET: {
				name = new SaNameT;
				unsigned int howlong = ::strlen(reinterpret_cast<char*>((*it).attrValues[j]));

				name->length = howlong;
				::memcpy(name->value, (*it).attrValues[j], name->length);
				_singleAttributeValues[i].attrValues[j] = reinterpret_cast<void *>(name);

				/**Used to deallocate memory for name variables **/
				SaNameVector.push_back(name);
			}
				break;

			default:
				break;

			}/*end switch case on the attribute Type*/

		}/**End for on each multiple value for a single attribute*/
		/*END set Attribute values*/

		/*add the singleAttributeValues to the vector attributesValues*/
		attributesValues[i] = &(_singleAttributeValues[i]);
		i++;

	}/*end for that goes through the input vector and builds the list to be passed to the function*/

	/*Last element in attributes Value list must be NULL*/
	attributesValues[i] = 0;

	HCF_CC_TRACE(CreateRuntimeObject, "Received a request to create a runtime object: calling 'saImmOiRtObjectCreate_2' function.", p_className, p_parentName);
	errorCode = saImmOiRtObjectCreate_2(_oi_handle, className, &parentName, (const SaImmAttrValuesT_2 **)attributesValues);

	/* delete allocated memory*/
	i = 0;
	for (it = m_attrValuesList.begin(); it != m_attrValuesList.end(); ++it) {
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

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(CreateRuntimeObjectErr, "Call 'saImmOiRtObjectCreate_2' failed", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteRuntimeObj (const char * p_objName) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaNameT objToDelete = {0,{0}};

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = ::strlen(p_objName);

	/*Copy in objToDelete.value the m_objectName value*/
	::memcpy(objToDelete.value, p_objName, objToDelete.length);

	/*Call the saImmOiRtObjectDelete */
	HCF_CC_TRACE(DeleteRuntimeObject, "Received a request to delete a runtime object: calling 'saImmOiRtObjectDelete' function!", p_objName);
	if ((errorCode = saImmOiRtObjectDelete(_oi_handle, &objToDelete)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteRuntimeObjectErr, "Call 'saImmOiRtObjectDelete' failed", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyRuntimeObj (const char * p_objName, IMM_API_ImmParameter * p_attributeValue) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaNameT * parSaName = 0;
	std::vector<SaNameT *> parSaNameVector;

	/*objectName. The Name of the object whose attribute has to be modified*/
	SaNameT objectName = {0,{0}};

	/*attrMods. Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modifications to perform*/
	SaImmAttrModificationT_2 * attrMods[2] = {0, 0};
	SaImmAttrModificationT_2 attr;

	/*Prepares the OBJECT NAME to be passed to saImmOiRtObjectUpdate_2()*/
	objectName.length = ::strlen(p_objName);
	::memcpy(objectName.value, p_objName, ::strlen(p_objName));

	HCF_CC_TRACE(ModifyRuntimeObject, "Received a request to modify an attribute value for a runtime object: preparing the attribute structure!", p_objName, p_attributeValue->attrName);

	/*Prepares data of the attribute to be change */
	/*Set the type of modification to apply on the object attribute*/
	attr.modType = SA_IMM_ATTR_VALUES_REPLACE;

	/*Set the attribute name*/
	attr.modAttr.attrName = (*p_attributeValue).attrName;

	/*Set the attribute Type*/
	attr.modAttr.attrValueType = (SaImmValueTypeT)(*p_attributeValue).attrType;

	/*Set the Attribute Values Number*/
	attr.modAttr.attrValuesNumber = (*p_attributeValue).attrValuesNum;

	void * attrValues[(*p_attributeValue).attrValuesNum];
	attr.modAttr.attrValues = attrValues;

	/*Set the new values of the attributes*/
	if (attr.modAttr.attrValueType == SA_IMM_ATTR_SANAMET) {
		for (unsigned int i = 0; i < (*p_attributeValue).attrValuesNum; ++i) {
			parSaName = new SaNameT;
			parSaName->length = ::strlen(reinterpret_cast<char *>((*p_attributeValue).attrValues[i]));
			::memcpy(parSaName->value, ((*p_attributeValue).attrValues[i]), parSaName->length);
			attr.modAttr.attrValues[i] = reinterpret_cast<void *>(parSaName);
			parSaNameVector.push_back(parSaName);
		}
	}
	else if (attr.modAttr.attrValueType == SA_IMM_ATTR_SASTRINGT) {
		for (unsigned int i = 0; i < (*p_attributeValue).attrValuesNum; ++i) {
			attr.modAttr.attrValues[i] = &((*p_attributeValue).attrValues[i]);
		}
	}
	else{
		attr.modAttr.attrValues = (*p_attributeValue).attrValues;
	}

	/*Prepares the LIST OF THE ATTRIBUTES TO BE CHANGE. It must be a NULL terminated array*/
	attrMods[0] = &attr;
	attrMods[1] = 0;

	/* saImmOiRtObjectUpdate_2 update runtime object of a configuration or runtime object.*/
	HCF_CC_TRACE(ModifyRuntimeObject, "Received a request to modify an attribute value for a runtime object: calling 'saImmOiRtObjectUpdate_2' function!", p_objName, p_attributeValue->attrName);
	errorCode = saImmOiRtObjectUpdate_2(_oi_handle, &objectName, (const SaImmAttrModificationT_2 **)attrMods);

	/*to free allocate memory*/
	if (parSaNameVector.size() > 0) {
		std::vector<SaNameT *>::iterator it_2;

		for (it_2 = parSaNameVector.begin(); it_2 != parSaNameVector.end(); ++it_2) {
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyRuntimeObjectErr, "Call 'saImmOiRtObjectUpdate_2' failed", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::dispatch (IMM_API_DispatchFlags p_flag) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaDispatchFlagsT flag = (SaDispatchFlagsT) p_flag;

	/** This function invokes pending callbacks for handle immOiHandle in the way
	 * that is specified by the flag parameter**/
	HCF_CC_TRACE(Dispatch, "Dispatching callback!", p_flag);
	if ((errorCode = saImmOiDispatch(_oi_handle, flag)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DispatchErr, "Call 'saImmOiDispatch' failed.", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::adminOperationResult (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result) {
		HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;

	HCF_CC_TRACE(AdminOperationResult, "Calling 'saImmOiAdminOperationResult' function", oiHandle, invocation, result);
	if ((errorCode = saImmOiAdminOperationResult(oiHandle, invocation, (SaAisErrorT)result)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AdminOperationResultErr, "Call 'saImmOiAdminOperationResult' failed,", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::adminOperationResult (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result, std::vector<IMM_API_AdminOperationParamType> outParamVector) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(AdminOperationResult, "Preparing the output parameters vector!", oiHandle, invocation, result);

	SaAisErrorT errorCode;
	SaImmAdminOperationParamsT_2 ** operationOutParamList = 0;
	int numOfParam = outParamVector.size();
	operationOutParamList = new SaImmAdminOperationParamsT_2 * [numOfParam + 1];
	SaImmAdminOperationParamsT_2 singleParamVector[numOfParam];
	std::vector<IMM_API_AdminOperationParamType>::iterator it;
	int i = 0;

	for (it = outParamVector.begin(); it != outParamVector.end(); ++it) {
		if ((((SaImmValueTypeT)(*it).attrType) == SA_IMM_ATTR_SANAMET) || (((SaImmValueTypeT)(*it).attrType) == SA_IMM_ATTR_SASTRINGT)) {
			singleParamVector[i].paramType = SA_IMM_ATTR_SASTRINGT;
			singleParamVector[i].paramName = reinterpret_cast<SaStringT>(((*it).attrName));
			singleParamVector[i].paramBuffer = &(*it).attrValues;
			operationOutParamList[i] = &(singleParamVector[i]);
			i++;
		}
		else {
			/*Set the param value*/
			singleParamVector[i].paramName = reinterpret_cast<SaStringT>(((*it).attrName));
			singleParamVector[i].paramBuffer = (*it).attrValues;
			singleParamVector[i].paramType = (SaImmValueTypeT)(((*it).attrType));
			operationOutParamList[i] = &(singleParamVector[i]);
			i++;
		}
	}

	/*last element in the parameter list must be 0*/
	operationOutParamList[i] = 0;

	HCF_CC_TRACE(AdminOperationResult, "Calling 'saImmOiAdminOperationResult' function", oiHandle, invocation, result);
	errorCode = saImmOiAdminOperationResult_o2(oiHandle, invocation, (SaAisErrorT)result, (const SaImmAdminOperationParamsT_2 **) operationOutParamList);
	/*free allocated memory*/
	delete[] operationOutParamList;

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AdminOperationResultErr, "Call 'saImmOiAdminOperationResult' failed", getInternalLastError(), getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

const char * __TSL_CLASS_NAME__::getInternalLastErrorText () {
	if ((_errorCode > 0) && (_errorCode < IMM_API_INTERNAL_NUMBER_OF_ERROR_MESSAGES)) {
		return IMM_API_ERROR_MESSAGES[_errorCode];
	}
	return 0;
}

SaAisErrorT __TSL_CLASS_NAME__::OiRtAttUpdateCallback (SaImmOiHandleT immOiHandle, const SaNameT * objectName, const SaImmAttrNameT * attributeNames) {
	HCF_CC_TRACE_FUNCTION;

	/**Find in the map the implementer associated at the immOiHandle**/
	HCF_CC_TRACE(UpdateRuntimeCallback, "Received a request to update some runtime attributes for the object: looking for implementer!", immOiHandle, reinterpret_cast<const char *>(objectName->value));
	std::map<SaUint64T, runtime_owner *>::iterator it;
	if ((it = _implementer_list.find(immOiHandle)) == _implementer_list.end()) {
		HCF_CC_TRACE(UpdateRuntimeCallbackErr, "No implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	runtime_owner * obj;
	if (!(obj = it->second)) {
		HCF_CC_TRACE(UpdateRuntimeCallbackErr, "NULL implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	//Save the name of object
	char * nameObj = new char[(objectName->length) + 1];
	::memset(nameObj, '\0', (objectName->length) +1 );
	::memcpy(nameObj, objectName->value, objectName->length);

	HCF_CC_TRACE(UpdateRuntimeCallback, "Received a request to update some runtime attributes for the object: preparing the attributes vector!", immOiHandle, reinterpret_cast<const char *>(objectName->value));

	//save a local copy of pointer of attribute name
	const SaImmAttrNameT * myPtr = attributeNames;

	//number of attribute to update
	int numAttr = 0;
	while (*attributeNames != 0) {
		numAttr++;
		attributeNames++;
	}

	//Instance of char* vector
	char * nameAttrs[numAttr + 1];
	attributeNames = myPtr;

	int i = 0;
	while (*attributeNames != 0) {
		nameAttrs[i] = *attributeNames;
		i++;
		attributeNames++;
	}
	//close the vector with a null pointer
	nameAttrs[numAttr] = 0;

	//call the updateCallback defined by application
	HCF_CC_TRACE(UpdateRuntimeCallback, "Received a request to update some runtime attributes for the object: calling update callback!", immOiHandle, reinterpret_cast<const char *>(objectName->value));
	IMM_API_ReturnTypes errorCode = obj->updateCallback(nameObj, (const char **)(nameAttrs));
	delete []nameObj;

	if (errorCode != OPERATION_SUCCESS) {
		HCF_CC_TRACE(UpdateRuntimeCallbackErr, "Call 'obj->updateCallback' failed!", reinterpret_cast<const char *>(objectName->value));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	return SA_AIS_OK;
}

void __TSL_CLASS_NAME__::adminOperationCallback (SaImmOiHandleT immOiHandle, SaInvocationT invocation, const SaNameT * objectName, SaImmAdminOperationIdT operationId, const SaImmAdminOperationParamsT_2 ** params) {
	HCF_CC_TRACE_FUNCTION;

	/**Find in the map the implementer associated at the immOiHandle**/
	HCF_CC_TRACE(AdminCallback, "Received a request to execute an administrative operation: looking for implementer!", immOiHandle, invocation, operationId, reinterpret_cast<const char *>(objectName->value));
	std::map<SaUint64T, runtime_owner *>::iterator it;
	if ((it = _implementer_list.find(immOiHandle)) == _implementer_list.end()) {
		HCF_CC_TRACE(AdminCallbackErr, "No implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return;
	}

	runtime_owner * obj;
	if (!(obj = it->second)) {
		HCF_CC_TRACE(AdminCallbackErr, "NULL implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return;
	}

	/*set the object name to be passed*/
	char * nameObj = new char[(objectName->length) + 1];
	::memset( nameObj, '\0', (objectName->length) +1 );
	::memcpy(nameObj, objectName->value, objectName->length);

	HCF_CC_TRACE(AdminCallback, "Received a request to execute an administrative operation: preparing the arguments vector!", immOiHandle, invocation, operationId, reinterpret_cast<const char *>(objectName->value));

	/**Calculate the size of array of attribute descriptor**/
	int dim = 0;
	int i = 0;

	while (params[i]) {
		i++;
		dim++;
	}

	/*Prepares data to be provided to adminOperationCallback function*/
	IMM_API_AdminOperationParamType * myAttributeList[dim + 1];
	IMM_API_AdminOperationParamType oneAttr[dim];
	i = 0;

	while (params[i]) {
		oneAttr[i].attrName = params[i]->paramName;
		oneAttr[i].attrType = (IMM_API_AttrValueTypes)(params[i]->paramType);

		if (params[i]->paramType == SA_IMM_ATTR_SASTRINGT) {
				SaStringT stringVal = *(reinterpret_cast<SaStringT *>(params[i]->paramBuffer));
				oneAttr[i].attrValues = reinterpret_cast<char*>(stringVal);
		}
		else if (params[i]->paramType == SA_IMM_ATTR_SANAMET) {
			SaNameT * localSaName;
			localSaName = reinterpret_cast<SaNameT*>(params[i]->paramBuffer);
			oneAttr[i].attrValues = localSaName->value;
		}
		else {
			oneAttr[i].attrValues = params[i]->paramBuffer;
		}

		myAttributeList[i] = &(oneAttr[i]);
		i++;
	}

	/**the myAttributeList must be a null terminated array**/
	myAttributeList[dim] = 0;

	HCF_CC_TRACE(AdminCallback, "Received a request to execute an administrative operation: calling the admin callback!", immOiHandle, invocation, operationId, reinterpret_cast<const char *>(objectName->value));
	obj->adminOperationCallback(immOiHandle, (IMM_API_InvocationType) invocation, nameObj, (IMM_API_AdminOperationIdType)operationId, myAttributeList);
	delete []nameObj;
}

TSL_NS_HCF_IMM_END
