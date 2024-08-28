#include <string.h>
#include <sstream>
#include <signal.h>

#include <saImmOm.h>
#include <saImmOi.h>

#include "hcf/imm/imm_error_constants.h"

#include "hcf/trace/com_ericsson_common_hcf_imm_oiinterface.h"
#include "hcf/trace/tracing_macros.h"

#include "object_implementer_interface_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_INTERFACE_IMPL__

TSL_NS_HCF_IMM_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

IMM_API_ReturnTypes __TSL_CLASS_NAME__::dispatch(IMM_API_DispatchFlags p_flag) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaDispatchFlagsT flag = (SaDispatchFlagsT) p_flag;

	/** This function invokes pending callbacks for handle immOiHandle in the way
	 * that is specified by the flag parameter**/
	HCF_CC_TRACE(Dispatch, "Dispatching callback!", p_flag);
	if ((errorCode = saImmOiDispatch(_oi_handle, flag)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DispatchErr, "Call 'saImmOiDispatch' failed. (errorCode = %d)", errorCode, getInternalLastErrorText());
		if (errorCode == SA_AIS_ERR_BAD_HANDLE)
			raise(SIGTERM);
		return OPERATION_FAILURE;
	}

	HCF_CC_TRACE(Dispatch, "Callback correctly dispatched!", p_flag);
	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyRuntimeObj(const char * p_objName, IMM_API_ImmParameter * p_attributeValue) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ModifyRuntimeObj, "Received a request to modify a runtime object!", p_objName, p_attributeValue->attrName);

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

	HCF_CC_TRACE(ModifyRuntimeObj, "Preparing the list of the attributes to be updated!", p_objName, p_attributeValue->attrName);

	/*Set the new values of the attributes*/
	if (attr.modAttr.attrValueType == SA_IMM_ATTR_SANAMET) {
		for (unsigned int i = 0; i < (*p_attributeValue).attrValuesNum; ++i) {
			parSaName = new SaNameT;
			parSaName->length = ::strlen(reinterpret_cast<char*>((*p_attributeValue).attrValues[i]));
			::memcpy(parSaName->value, ((*p_attributeValue).attrValues[i]), parSaName->length);
			attr.modAttr.attrValues[i] = reinterpret_cast<void *>(parSaName);
			parSaNameVector.push_back(parSaName);
		}
	}
	else if (attr.modAttr.attrValueType == SA_IMM_ATTR_SASTRINGT) {
		for(unsigned int i = 0; i < (*p_attributeValue).attrValuesNum; ++i) {
			attr.modAttr.attrValues[i] = &((*p_attributeValue).attrValues[i]);
		}
	}
	else {
		attr.modAttr.attrValues = (*p_attributeValue).attrValues;
	}

	/*Prepares the LIST OF THE ATTRIBUTES TO BE CHANGE. It must be a NULL terminated array*/
	attrMods[0] = &attr;
	attrMods[1] = 0;

	HCF_CC_TRACE(ModifyRuntimeObj, "Calling 'saImmOiRtObjectUpdate_2' function!", p_objName, p_attributeValue->attrName);
	/* saImmOiRtObjectUpdate_2 update runtime object of a configuration or runtime object.*/
	errorCode = saImmOiRtObjectUpdate_2(_oi_handle, &objectName, (const SaImmAttrModificationT_2 **)attrMods);

	/*to free allocate memory*/
	if (parSaNameVector.size() > 0) {
		for (std::vector<SaNameT *>::iterator it_2 = parSaNameVector.begin(); it_2 != parSaNameVector.end(); ++it_2) {
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyRuntimeObjErr, "Call 'saImmOiRtObjectUpdate_2' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::adminOperationResult(IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(AdminOperationalResult, "Calling 'saImmOiAdminOperationResult' function", oiHandle, invocation, result);

	SaAisErrorT errorCode;
	if ((errorCode = saImmOiAdminOperationResult(oiHandle, invocation, (SaAisErrorT)result)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AdminOperationalResultErr, "Call 'saImmOiAdminOperationResult' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes  __TSL_CLASS_NAME__::adminOperationResult(IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result, std::vector<IMM_API_AdminOperationParamType> outParamVector) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(AdminOperationalResult, "Preparing the output parameters vector!", oiHandle, invocation, result);

	SaAisErrorT errorCode;
	SaImmAdminOperationParamsT_2 ** operationOutParamList = 0;
	int numOfParam = outParamVector.size();
	operationOutParamList = new SaImmAdminOperationParamsT_2 * [numOfParam + 1];
	SaImmAdminOperationParamsT_2 singleParamVector[numOfParam];
	int i = 0;

	for (std::vector<IMM_API_AdminOperationParamType>::iterator it = outParamVector.begin(); it != outParamVector.end(); ++it) {
		if ((((IMM_API_AttrValueTypes)(*it).attrType) == ATTR_NAMET) || (((IMM_API_AttrValueTypes)(*it).attrType) == ATTR_STRINGT)) {
			singleParamVector[i].paramType = SA_IMM_ATTR_SASTRINGT;
			singleParamVector[i].paramName = reinterpret_cast<SaStringT>(((*it).attrName));
			singleParamVector[i].paramBuffer = &(*it).attrValues;
			operationOutParamList[i] = &(singleParamVector[i]);
			++i;
		}
		else {
			singleParamVector[i].paramName = reinterpret_cast<SaStringT>(((*it).attrName));
			singleParamVector[i].paramBuffer = (*it).attrValues;
			singleParamVector[i].paramType = (SaImmValueTypeT)(((*it).attrType));
			operationOutParamList[i] = &(singleParamVector[i]);
			++i;
		}
	}

	/*last element in the parameter list must be 0*/
	operationOutParamList[i] = 0;

	HCF_CC_TRACE(AdminOperationalResult, "Calling 'saImmOiAdminOperationResult' function", oiHandle, invocation, result);
	errorCode = saImmOiAdminOperationResult_o2(oiHandle, invocation, (SaAisErrorT)result, (const SaImmAdminOperationParamsT_2 **) operationOutParamList);
	delete[] operationOutParamList;

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AdminOperationalResultErr, "Call 'saImmOiAdminOperationResult' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

void __TSL_CLASS_NAME__::setExitCode(int p_exitCode, std::string p_exitCodeMessage) {
	HCF_CC_TRACE_FUNCTION;

	/*create the string to be passed to CMW API*/
	std::stringstream concatenateString;
	/*added for COMSA Rule 101 */
	concatenateString << "@ComNbi@";
	/*end added for rule 101*/
	concatenateString << p_exitCodeMessage;
	/* the following two lines commented since COMSA Rule 101 must be applied and COM has no way to split
	 * string and int: temporary solution
	concatenateString << "__";
	concatenateString << p_exitCode;
	 */
	_exitCodeString = concatenateString.str();
	HCF_CC_TRACE(ExitCode, "Values used as exit codes:", p_exitCode, _exitCodeString.c_str());
}

const char * __TSL_CLASS_NAME__::getInternalLastErrorText() {
	if ((_errorCode > 0) && (_errorCode < IMM_API_INTERNAL_NUMBER_OF_ERROR_MESSAGES)) {
		return IMM_API_ERROR_MESSAGES[_errorCode];
	}
	return 0;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::getCcbAugmentationInitialize(IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, IMM_API_CcbHandle * ccbHandle, IMM_API_AdminOwnerHandle * adminOwnerHandler) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CcbAugmentationInitialize, "Calling 'saImmOiAugmentCcbInitialize' function.", oiHandle, ccbId, *ccbHandle, *adminOwnerHandler);

	SaAisErrorT errorCode;
	if ((errorCode = saImmOiAugmentCcbInitialize(oiHandle, ccbId, ccbHandle, adminOwnerHandler)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(CcbAugmentationInitializeErr, "Call 'saImmOiAugmentCcbInitialize' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::createObjectAugmentCcb(IMM_API_CcbHandle ccbHandle, IMM_API_AdminOwnerHandle adminOwnerHandler, const char * p_className, const char * p_parentName, std::vector<IMM_API_ValuesDefinitionTypes> p_attrValuesList) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaNameT * name = 0;
	std::vector<SaNameT *> SaNameVector;

	/* The parent Name of the creating OBJECT*/
	SaNameT parentName = {0,{0}};

	/*An iterator to access the attributes list*/
	std::vector<IMM_API_ValuesDefinitionTypes>::iterator it;

	/*The attribute list to be passed to the function saImmOmCcbObjectCreate_2*/
	SaImmAttrValuesT_2 ** attributesValues = 0;

	/*to set the adminowner create the owned obj list*/
	const SaNameT * objOwnedNameList[] = {&parentName, 0};

	/*Build the SaNameT variable to be passed to the  saImmOmCcbObjectCreate_2 function*/
	HCF_CC_TRACE(CreateObjectAugmentCcb, "Received a request to create an object (into an augmented CCB), setting parent admin owner (if needed)!", ccbHandle, adminOwnerHandler, p_className, p_parentName);
	if (p_parentName != 0) {
		/*Set in parentName.length the m_parentName string length*/
		parentName.length = ::strlen(p_parentName);

		/*Copy in parentName.value the m_parentName value*/
		::memcpy(parentName.value, p_parentName, parentName.length);

		/*set the admin ownwer to the obj to be created*/
		if ((errorCode = saImmOmAdminOwnerSet(adminOwnerHandler, objOwnedNameList, SA_IMM_ONE)) != SA_AIS_OK)	{
			setInternalError(errorCode);
			HCF_CC_TRACE(CreateObjectAugmentCcbErr, "Call 'saImmOmAdminOwnerSet' failed", errorCode, getInternalLastErrorText());
			return OPERATION_FAILURE;
		}
	}
	/*no initialization requested for ccb and adminowner since a previous ccb is being augmented*/
	HCF_CC_TRACE(CreateObjectAugmentCcb, "Received a request to create an object (into an augmented CCB), preparing the attributes vector.", ccbHandle, adminOwnerHandler, p_className, p_parentName);

	/* Create the list of Attribute Values to be provided as input to the saImmOmCcbObjectCreate_2 function*/
	/* allocate a  list of m_attrValuesList.size()+1 (SaImmAttrValuesT_2*) */
	attributesValues = new SaImmAttrValuesT_2 * [p_attrValuesList.size()+1];
	int numAttribute = p_attrValuesList.size();
	SaImmAttrValuesT_2 singleAttributeValues[numAttribute];
	int i = 0;

	for (it = p_attrValuesList.begin(); it != p_attrValuesList.end(); ++it) {
		/*Set the attribute Name*/
		singleAttributeValues[i].attrName = (SaImmAttrNameT)((*it).attrName);

		/*Set the attribute Type*/
		singleAttributeValues[i].attrValueType = (SaImmValueTypeT)((*it).attrType);

		/*Set the Attribute Values Number*/
		unsigned int numElem = (*it).attrValuesNum;
		singleAttributeValues[i].attrValuesNumber = (SaUint32T)(numElem);

		/*Set attribute values To be done*/
		singleAttributeValues[i].attrValues = new void*[numElem];

		for (unsigned int j = 0; j < numElem; ++j) {
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

			case ATTR_NAMET: {
				name = new SaNameT;
				unsigned int howlong = ::strlen(reinterpret_cast<char*>((*it).attrValues[j]));
				name->length = howlong;

				::memcpy(name->value, (*it).attrValues[j], name->length);
				singleAttributeValues[i].attrValues[j] = reinterpret_cast<void*>(name);

				/**Used to deallocate memory for name variables **/
				SaNameVector.push_back(name);
			}
				break;

			default:
				break;

			}/*end switch case on the attribute Type*/

		}/**End for on each multiple value for a single attribute*/
		/*END set Atribute values*/

		/*add the singleAttributeValues to the vector attributesValues*/
		attributesValues[i] = &(singleAttributeValues[i]);
		++i;

	}/*end for that goes through the input vector and builds the list to be passed to the function*/

	/*LAst element in attributes Value list must be NULL*/
	attributesValues[i] = 0;

	HCF_CC_TRACE(CreateObjectAugmentCcb, "Calling 'saImmOmCcbObjectCreate_2' function.", ccbHandle, adminOwnerHandler, p_className, p_parentName);
	errorCode = saImmOmCcbObjectCreate_2(ccbHandle, (SaImmClassNameT)p_className, &parentName, (const SaImmAttrValuesT_2 **)(attributesValues));

	/* delete allocated memory*/
	i = 0;
	for (it = p_attrValuesList.begin(); it != p_attrValuesList.end(); ++it) {
		delete[] attributesValues[i]->attrValues;
		++i;
	}

	if (SaNameVector.size() > 0) {
		for (std::vector<SaNameT *>::iterator it_2 = SaNameVector.begin(); it_2 != SaNameVector.end(); ++it_2) {
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}
	delete[] attributesValues;

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(CreateObjectAugmentCcbErr, "Call 'saImmOmCcbObjectCreate_2' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyObjectAugmentCcb(IMM_API_CcbHandle ccbHandle, IMM_API_AdminOwnerHandle adminOwnerHandler, const char * p_objectName, IMM_API_ImmParameter * p_attributeValue) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaNameT * parSaName = 0;
	std::vector<SaNameT *> parSaNameVector;

	/*objectName. The Name of the object whose attribute has to be modified*/
	SaNameT objectName;

	/*attrMods. Pointer to a NULL-terminated array of pointers to descriptors of the
	 * modifications to perform*/
	SaImmAttrModificationT_2 * attrMods[2] = {0, 0};
	SaImmAttrModificationT_2 attr;

	/*Prepares the OBJECT NAME to be passed to saImmOmCcbObjectModify_2()*/
	objectName.length = ::strlen(p_objectName);
	::memcpy(objectName.value, p_objectName, ::strlen(p_objectName));

	/*for set the Admin Owner prepare the list of obj*/
	const SaNameT * objOwnedNameList[] = {&objectName, 0};

	/*set the admin owner*/
	HCF_CC_TRACE(ModifyObjectAugmentCcb, "Received a request to modify an object (into an augmented CCB), setting the object admin owner!", ccbHandle, adminOwnerHandler, p_objectName, p_attributeValue->attrName);
	if ((errorCode = saImmOmAdminOwnerSet (adminOwnerHandler,objOwnedNameList, SA_IMM_ONE)) != SA_AIS_OK)	{
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyObjectAugmentCcbErr, "Call 'saImmOmAdminOwnerSet' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}
	HCF_CC_TRACE(ModifyObjectAugmentCcb, "Received a request to modify an object (into an augmented CCB), preparing the attributes vector.", ccbHandle, adminOwnerHandler, p_objectName, p_attributeValue->attrName);

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
	void * attrValues[(*p_attributeValue).attrValuesNum];
	attr.modAttr.attrValues = attrValues;

	/*Set the new values of the attributes*/
	if (attr.modAttr.attrValueType == SA_IMM_ATTR_SANAMET) {
		for (unsigned int i = 0; i < (*p_attributeValue).attrValuesNum; ++i) {
			parSaName = new SaNameT;
			parSaName->length = ::strlen(reinterpret_cast<char*>((*p_attributeValue).attrValues[i]));
			::memcpy(parSaName->value, ((*p_attributeValue).attrValues[i]), parSaName->length);
			attr.modAttr.attrValues[i] = reinterpret_cast<void *>(parSaName);
			parSaNameVector.push_back(parSaName);
		}
	} else if (attr.modAttr.attrValueType == SA_IMM_ATTR_SASTRINGT) {
		for (unsigned int i = 0; i < (*p_attributeValue).attrValuesNum; ++i) {
			attr.modAttr.attrValues[i] = &((*p_attributeValue).attrValues[i]);
		}
	}
	else {
		attr.modAttr.attrValues = (*p_attributeValue).attrValues;
	}

	/*Prepares the LIST OF THE ATTRIBUTES TO BE CHANGE. It must be a NULL terminated array*/
	attrMods[0] = &attr;
	attrMods[1] = 0;

	/* saImmOmCcbObjectModify_2 adds to the CCB identified by its handle ccbHandle a request to modify
	 * configuration attributes of an IMM Service object.*/
	HCF_CC_TRACE(ModifyObjectAugmentCcb, "Calling 'saImmOmCcbObjectModify_2' function.", ccbHandle, adminOwnerHandler, p_objectName, p_attributeValue->attrName);
	errorCode = saImmOmCcbObjectModify_2(ccbHandle, &objectName, (const SaImmAttrModificationT_2 **)attrMods);

	/*to free allocate memory*/
	if (parSaNameVector.size() > 0) {
		for(std::vector<SaNameT *>::iterator it_2 = parSaNameVector.begin(); it_2 != parSaNameVector.end(); ++it_2) {
			delete reinterpret_cast<SaNameT *>(*it_2);
		}
	}

	if (errorCode != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ModifyObjectAugmentCcbErr, "Call 'saImmOmCcbObjectModify_2' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObjectAugmentCcb(IMM_API_CcbHandle ccbHandle, IMM_API_AdminOwnerHandle adminOwnerHandler, const char * m_objectName) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaNameT objToDelete;

	/*Create the OBJ Name to be passed to the function*/
	objToDelete.length = ::strlen(m_objectName);

	/*Copy in objToDelete.value the m_objectName value*/
	::memcpy(objToDelete.value, m_objectName, objToDelete.length);

	/*prepare to set the admin owner create the list of obj*/
	const SaNameT * objOwnedNameList[] = {&objToDelete, 0};

	/*set the adminowner*/
	HCF_CC_TRACE(DeleteObjectAugmentCcb, "Received a request to delete an object (into an augmented CCB), setting the object admin owner!", ccbHandle, adminOwnerHandler, m_objectName);
	if ((errorCode = saImmOmAdminOwnerSet(adminOwnerHandler, objOwnedNameList, SA_IMM_ONE)) != SA_AIS_OK) {
		setInternalError(errorCode);
		/*release ownerHandler is not required it is performed when releasing the main handle*/
		HCF_CC_TRACE(DeleteObjectAugmentCcbErr, "Call 'saImmOmAdminOwnerSet' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/*Call the saImmOmCcbObjectDelete */
	HCF_CC_TRACE(DeleteObjectAugmentCcb, "Calling 'saImmOmCcbObjectDelete' function.", ccbHandle, adminOwnerHandler, m_objectName);
	if ((errorCode = saImmOmCcbObjectDelete(ccbHandle, &objToDelete)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(DeleteObjectAugmentCcbErr, "Call 'saImmOmCcbObjectDelete' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::applyAugmentCcb(IMM_API_CcbHandle ccbHandle) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ApplyAugmentCcb, "Calling 'saImmOmCcbApply' function.", ccbHandle);

	SaAisErrorT errorCode;
	if ((errorCode = saImmOmCcbApply(ccbHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(ApplyAugmentCcbErr, "Call 'saImmOmCcbApply' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}
	return OPERATION_SUCCESS;
}

TSL_NS_HCF_IMM_END
