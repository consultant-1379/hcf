#include <string.h>

#include "hcf/imm/imm_error_constants.h"
#include "hcf/imm/object_implementer_interface.h"

#include "hcf/trace/com_ericsson_common_hcf_imm_oihandler.h"
#include "hcf/trace/tracing_macros.h"

#include "object_implementer_handler_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER_IMPL__

TSL_NS_HCF_IMM_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
std::map<SaUint64T, object_implementer_interface *> __TSL_CLASS_NAME__::_implementer_list;

IMM_API_ReturnTypes __TSL_CLASS_NAME__::addObjectImpl(object_implementer_interface * obj) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaVersionT version;
	version.releaseCode  = IMM_LIBRARY_RELEASE_CODE;
	version.majorVersion = IMM_LIBRARY_MAJOR_VERSION;
	version.minorVersion = IMM_LIBRARY_MINOR_VERSION;

	SaImmOiCallbacksT_2 immCallbacks;

	/** Set the SaImmOiCallbacksT_2 structure which contains the callback function
	 * of the process that the IMM Service may invoke**/
	immCallbacks.saImmOiCcbObjectCreateCallback = __TSL_CLASS_NAME__::CreateCallback;
	immCallbacks.saImmOiCcbObjectModifyCallback = __TSL_CLASS_NAME__::ModifyCallback;
	immCallbacks.saImmOiCcbObjectDeleteCallback = __TSL_CLASS_NAME__::DeleteCallback;
	immCallbacks.saImmOiCcbCompletedCallback = __TSL_CLASS_NAME__::CompletedCallback;
	immCallbacks.saImmOiCcbApplyCallback = __TSL_CLASS_NAME__::ApplyCallback;
	immCallbacks.saImmOiCcbAbortCallback = __TSL_CLASS_NAME__::AbortCallback;
	immCallbacks.saImmOiRtAttrUpdateCallback = __TSL_CLASS_NAME__::OiRtAttUpdateCallback;
	immCallbacks.saImmOiAdminOperationCallback = __TSL_CLASS_NAME__::adminOperationCallback;

	SaImmOiHandleT immOiHandle;

	/**Initialize the Object Implementer functions of the IMM Service for the invoking process
	 * and register the various callback functions **/
	HCF_CC_TRACE(AddObjectImpl, "Calling 'saImmOiInitialize_2' function.", "");
	if ((errorCode = saImmOiInitialize_2(&immOiHandle, &immCallbacks , &version)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddObjectImplErr, "Call 'saImmOiInitialize_2' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/**Set the immOiHandle returned by the IMM Service in the instance of
	 * object_implementer_interface passed as parameter**/
	SaImmOiImplementerNameT implementerName;
	std::string impName = obj->getImpName();
	implementerName = const_cast<char *>(impName.c_str());

	/**Set the implementer name specified in the implementerName for the handle immOihandle
	 * and registers the invoking process as Object  implementer having the name which is
	 * specified in the implementerName parameter.  **/
	HCF_CC_TRACE(AddObjectImpl, "Calling 'saImmOiImplementerSet' function with implementer name:", impName.c_str());
	if ((errorCode = saImmOiImplementerSet(immOiHandle, implementerName)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddObjectImplErr, "Call 'saImmOiImplementerSet' failed: finalizing IMM OI handle", errorCode, getInternalLastErrorText());

		// Release OI handle
		if ((errorCode = saImmOiFinalize(immOiHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(AddObjectImplErr, "Call 'saImmOiFinalize' failed", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}
		return OPERATION_FAILURE;
	}

	SaNameT objName;
	objName.length = obj->getObjName().length();
	objName.value[obj->getObjName().copy(reinterpret_cast<char *>(objName.value), SA_MAX_NAME_LENGTH)] = '\0';
	SaImmScopeT scope = (SaImmScopeT) obj->getScope();

	/** this function informs the IMM Service that the object identified by the scope
	 * and obname parameters are implemented by the Object implementer whose name has
	 * been associated with the handle immOihandle**/
	HCF_CC_TRACE(AddObjectImpl, "Calling 'saImmOiObjectImplementerSet' function for the object having name:", obj->getObjName().c_str());
	if ((errorCode = saImmOiObjectImplementerSet(immOiHandle, &objName, scope)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddObjectImplErr, "Call 'saImmOiObjectImplementerSet' failed: finalizing IMM OI handle!", errorCode, getInternalLastErrorText());
		Finalize(immOiHandle);
		return OPERATION_FAILURE;
	}

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	HCF_CC_TRACE(AddObjectImpl, "Calling 'saImmOiSelectionObjectGet' to get the selectable handle for the object:", obj->getObjName().c_str());
	SaSelectionObjectT selObj;
	if ((errorCode = saImmOiSelectionObjectGet(immOiHandle, &selObj)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddObjectImplErr, "Call 'saImmOiSelectionObjectGet' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	obj->setImmOiHandle(immOiHandle);
	obj->setSelObj(selObj);
	_implementer_list[obj->getImmOiHandle()] = obj;
	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::removeObjectImpl(object_implementer_interface * obj) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;

	SaNameT objName;
	objName.length = obj->getObjName().length();
	objName.value[obj->getObjName().copy(reinterpret_cast<char *>(objName.value), SA_MAX_NAME_LENGTH)] = '\0';

	SaUint64T handle=obj->getImmOiHandle();
	SaImmScopeT scope = (SaImmScopeT) obj->getScope();

	/** This function informs the IMM Service that the implementer whose name is
	 * associated  with the IMM handle must no longer be considered as the
	 * implementer of the set of object identified by scope and the name to whith
	 * obname ponts  **/
	HCF_CC_TRACE(RemoveObjectImpl, "Calling 'saImmOiObjectImplementerRelease' to release the implementer for the object:", obj->getObjName().c_str());
	if ((errorCode = saImmOiObjectImplementerRelease(handle, &objName, scope)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(RemoveObjectImplErr, "Call 'saImmOiObjectImplementerRelease' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/** Remove the obj element from the map**/
	_implementer_list.erase(obj->getImmOiHandle());

	/** this method release all the handle retrieved during the initialization  **/
	HCF_CC_TRACE(RemoveObjectImpl, "Finalizing the IMM OI Handle for the object:", obj->getObjName().c_str());
	Finalize(obj->getImmOiHandle());
	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::addClassImpl(object_implementer_interface * obj, const char * className) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;

	SaVersionT version;
	version.releaseCode  = IMM_LIBRARY_RELEASE_CODE;
	version.majorVersion = IMM_LIBRARY_MAJOR_VERSION;
	version.minorVersion = IMM_LIBRARY_MINOR_VERSION;

	/** Set the SaImmOiCallbacksT_2 structure which contains the callback function
		 * of the process that the IMM Service may invoke**/
	SaImmOiCallbacksT_2 immCallbacks;
	immCallbacks.saImmOiCcbObjectCreateCallback = __TSL_CLASS_NAME__::CreateCallback;
	immCallbacks.saImmOiCcbObjectModifyCallback = __TSL_CLASS_NAME__::ModifyCallback;
	immCallbacks.saImmOiCcbObjectDeleteCallback = __TSL_CLASS_NAME__::DeleteCallback;
	immCallbacks.saImmOiCcbCompletedCallback = __TSL_CLASS_NAME__::CompletedCallback;
	immCallbacks.saImmOiCcbApplyCallback = __TSL_CLASS_NAME__::ApplyCallback;
	immCallbacks.saImmOiCcbAbortCallback = __TSL_CLASS_NAME__::AbortCallback;
	immCallbacks.saImmOiRtAttrUpdateCallback = __TSL_CLASS_NAME__::OiRtAttUpdateCallback;
	immCallbacks.saImmOiAdminOperationCallback = __TSL_CLASS_NAME__::adminOperationCallback;

	/**Initialize the Object Implementer functions of the IMM Service for the invoking process
	 * and register the various callback functions **/
	HCF_CC_TRACE(AddClassImpl, "Calling 'saImmOiInitialize_2' function for the class:", className);
	SaImmOiHandleT immOiHandle;
	if ((errorCode = saImmOiInitialize_2(&immOiHandle, &immCallbacks, &version)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiInitialize_2' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/**Set the implementer name specified in the implementerName for the handle immOihandle
	 * and registers the invoking process as Object  implementer having the name which is
	 * specified in the implementerName parameter.  **/
	SaImmOiImplementerNameT implementerName;
	std::string impName = obj->getImpName();
	implementerName = const_cast<char *>(impName.c_str());

	HCF_CC_TRACE(AddClassImpl, "Calling 'saImmOiImplementerSet' function with implementer name:", impName.c_str());
	if ((errorCode = saImmOiImplementerSet(immOiHandle, implementerName)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiImplementerSet' failed: finalizing IMM OI handle", errorCode, getInternalLastErrorText());

		// Release OI handle
		if ((errorCode = saImmOiFinalize(immOiHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiFinalize' failed", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}
		return OPERATION_FAILURE;
	}

	/**This function informs the IMM serivice that all the objects that are istances of the
	 * object class whose name is specified by className parameter are implemented by
	 * Object Implmeneter whose name has associated with the handle immOiHandle**/
	HCF_CC_TRACE(AddClassImpl, "Calling 'saImmOiClassImplementerSet' function for the object having name:", className);
	if ((errorCode = saImmOiClassImplementerSet(immOiHandle, const_cast<char *>(className))) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiClassImplementerSet' failed: finalizing IMM OI handle!", errorCode, getInternalLastErrorText());
		Finalize(immOiHandle);
		return OPERATION_FAILURE;
	}

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	SaSelectionObjectT selObj;
	HCF_CC_TRACE(AddClassImpl, "Calling 'saImmOiSelectionObjectGet' to get the selectable handle for the class:", className);
	if ((errorCode = saImmOiSelectionObjectGet(immOiHandle, &selObj)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiSelectionObjectGet' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	obj->setImmOiHandle(immOiHandle);
	obj->setSelObj(selObj);
	_implementer_list[obj->getImmOiHandle()] = obj;
	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::addMultipleClassImpl(object_implementer_interface * p_obj, std::vector<std::string> p_classNameVec) {
	HCF_CC_TRACE_FUNCTION;
	if ((!p_obj) || (!p_classNameVec.size())) {
		HCF_CC_TRACE(AddClassImplErr, "NULL object_implementer_interface pointer or empty classes vector received: vector size is:", p_classNameVec.size(), "");
		return OPERATION_FAILURE;
	}

	SaAisErrorT errorCode;

	SaVersionT version;
	version.releaseCode  = IMM_LIBRARY_RELEASE_CODE;
	version.majorVersion = IMM_LIBRARY_MAJOR_VERSION;
	version.minorVersion = IMM_LIBRARY_MINOR_VERSION;

	SaImmOiCallbacksT_2 immCallbacks;
	immCallbacks.saImmOiCcbObjectCreateCallback = __TSL_CLASS_NAME__::CreateCallback;
	immCallbacks.saImmOiCcbObjectModifyCallback = __TSL_CLASS_NAME__::ModifyCallback;
	immCallbacks.saImmOiCcbObjectDeleteCallback = __TSL_CLASS_NAME__::DeleteCallback;
	immCallbacks.saImmOiCcbCompletedCallback = __TSL_CLASS_NAME__::CompletedCallback;
	immCallbacks.saImmOiCcbApplyCallback = __TSL_CLASS_NAME__::ApplyCallback;
	immCallbacks.saImmOiCcbAbortCallback = __TSL_CLASS_NAME__::AbortCallback;
	immCallbacks.saImmOiRtAttrUpdateCallback = __TSL_CLASS_NAME__::OiRtAttUpdateCallback;
	immCallbacks.saImmOiAdminOperationCallback = __TSL_CLASS_NAME__::adminOperationCallback;

	/**Initialize the Object Implementer functions of the IMM Service for the invoking process
	 * and register the various callback functions **/
	SaImmOiHandleT immOiHandle;
	HCF_CC_TRACE(AddClassImpl, "Calling 'saImmOiInitialize_2' function.", "Multiple Class Implementers");
	if ((errorCode = saImmOiInitialize_2(&immOiHandle, &immCallbacks, &version)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiInitialize_2' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/**Set the implementer name specified in the implementerName for the handle immOihandle
	 * and registers the invoking process as Object  implementer having the name which is
	 * specified in the implementerName parameter.  **/
	SaImmOiImplementerNameT implementerName;
	std::string impName = p_obj->getImpName();
	implementerName = const_cast<char *>(impName.c_str());

	HCF_CC_TRACE(AddClassImpl, "Calling 'saImmOiImplementerSet' function with implementer name:", impName.c_str());
	if ((errorCode = saImmOiImplementerSet(immOiHandle, implementerName)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiImplementerSet' failed: finalizing IMM OI handle", errorCode, getInternalLastErrorText());

		// Release OI handle
		if ((errorCode = saImmOiFinalize(immOiHandle)) != SA_AIS_OK) {
			HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiFinalize' failed", errorCode, IMM_API_ERROR_MESSAGES[errorCode]);
		}

		return OPERATION_FAILURE;
	}

	for (size_t i = 0; i < p_classNameVec.size(); ++i) {
		/**This function informs the IMM serivice that all the objects that are istances of the
		 * object class whose name is specified by className parameter are implemented by
		 * Object Implmeneter whose name has associated with the handle immOiHandle**/
		HCF_CC_TRACE(AddClassImpl, "Calling 'saImmOiClassImplementerSet' function for the object having name:", p_classNameVec[i].c_str());
		if ((errorCode = saImmOiClassImplementerSet(immOiHandle, const_cast<char *>(p_classNameVec[i].c_str()))) != SA_AIS_OK){
			setInternalError(errorCode);
			HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiClassImplementerSet' failed: releasing the previously "
					"set class implementers and finalizing IMM OI handle!", errorCode, getInternalLastErrorText());

			for (size_t j = 0; j < i; ++j) {
				saImmOiClassImplementerRelease(immOiHandle, const_cast<char *>(p_classNameVec[j].c_str()));
			}

			Finalize(immOiHandle);
			return OPERATION_FAILURE;
		}
	}

	/** this function returns the operating system handle associated with the handle
	 * immOiHandle**/
	SaSelectionObjectT selObj;

	HCF_CC_TRACE(AddClassImpl, "Calling 'saImmOiSelectionObjectGet' to get the selectable handle for the class:", "Multiple Class Implementers");
	if ((errorCode = saImmOiSelectionObjectGet(immOiHandle, &selObj)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(AddClassImplErr, "Call 'saImmOiSelectionObjectGet' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	p_obj->setImmOiHandle(immOiHandle);
	p_obj->setSelObj(selObj);
	_implementer_list[p_obj->getImmOiHandle()] = p_obj;
	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::removeClassImpl(object_implementer_interface * obj, const char * className) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;
	SaUint64T handle = obj->getImmOiHandle();

	/** This function informs the IMM Service that the implementer whose name is associated
	 * with the handle immOiHandle must not be anymore as the implementer of the object that
	 * are instances of the object class whose name is specified by className**/
	HCF_CC_TRACE(RemoveClassImpl, "Calling 'saImmOiClassImplementerRelease' to release the implementer for the class:", className);
	if ((errorCode = saImmOiClassImplementerRelease(handle, const_cast<char *>(className))) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(RemoveClassImplErr, "Call 'saImmOiClassImplementerRelease' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/** Remove the obj element from the map**/
	_implementer_list.erase(obj->getImmOiHandle());

	/** this method release all the handle retrieved during the initialization  **/
	HCF_CC_TRACE(RemoveClassImpl, "Finalizing the IMM OI Handle for the class:", className);
	Finalize(obj->getImmOiHandle());
	return OPERATION_SUCCESS;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::removeMultipleClassImpl(object_implementer_interface * p_obj, std::vector<std::string> p_classNameVec) {
	HCF_CC_TRACE_FUNCTION;
	if ((!p_obj) || (!p_classNameVec.size())) {
		HCF_CC_TRACE(RemoveClassImplErr, "NULL object_implementer_interface pointer or empty classes vector received: vector size is:", p_classNameVec.size(), "");
		return OPERATION_FAILURE;
	}

	SaAisErrorT errorCode;
	SaUint64T handle = p_obj->getImmOiHandle();

	for (size_t i = 0; i < p_classNameVec.size(); ++i) {
		/** This function informs the IMM Service that the implementer whose name is associated
		 * with the handle immOiHandle must not be anymore as the implementer of the object that
		 * are instances of the object class whose name is specified by className**/
		HCF_CC_TRACE(RemoveClassImpl, "Calling 'saImmOiClassImplementerRelease' to release the implementer for the class:", p_classNameVec[i].c_str());
		if ((errorCode = saImmOiClassImplementerRelease(handle, const_cast<char *>(p_classNameVec[i].c_str()))) != SA_AIS_OK) {
			setInternalError(errorCode);
			HCF_CC_TRACE(RemoveClassImplErr, "Call 'saImmOiClassImplementerRelease' failed", errorCode, getInternalLastErrorText());
			return OPERATION_FAILURE;
		}
	}

	/** Remove the obj element from the map**/
	_implementer_list.erase(p_obj->getImmOiHandle());

	/** this method release all the handle retrieved during the initialization  **/
	HCF_CC_TRACE(RemoveClassImpl, "Finalizing the IMM OI Handle for the class:", "Multiple Class Implementers");
	Finalize(p_obj->getImmOiHandle());
	return OPERATION_SUCCESS;
}

const char * __TSL_CLASS_NAME__::getInternalLastErrorText () {
	if ((_errorCode > 0) && (_errorCode < IMM_API_INTERNAL_NUMBER_OF_ERROR_MESSAGES)) {
		return IMM_API_ERROR_MESSAGES[_errorCode];
	}
	return 0;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::Finalize (SaUint64T immHandle) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT errorCode;

	/* clear the implementer name associated with the immOiHandle and unregisters
	 * the invoking process as an Object Implementer for the name previously associated
	 *
	 * immOiHandle is the handle witch was obtained by a previous invocation of the
	 * saImmOiInitialize_2 function
	 * */
	HCF_CC_TRACE(Finalize, "Calling 'saImmOiImplementerClear' function with IMM OI handle:", immHandle);
	if ((errorCode = saImmOiImplementerClear(immHandle)) != SA_AIS_OK) {
		setInternalError(errorCode);
		HCF_CC_TRACE(FinalizeErr, "Call 'saImmOiImplementerClear' failed", errorCode, getInternalLastErrorText());
		return OPERATION_FAILURE;
	}

	/* closes the association represented by the immOiHandle parameter between the
	 * invoking process and the information Model
	 *
	 * immOiHandle is the ahandle witch was obtained by a previous invocation of the
	 * saImmOiInitialize_2 and witch identifies this particular initization of the
	 * IMM service
	 * */
	HCF_CC_TRACE(Finalize, "Calling 'saImmOiFinalize' function with IMM OI handle:", immHandle);
		if ((errorCode = saImmOiFinalize(immHandle)) != SA_AIS_OK) {
			setInternalError(errorCode);
			HCF_CC_TRACE(FinalizeErr, "Call 'saImmOiFinalize' failed", errorCode, getInternalLastErrorText());
			return OPERATION_FAILURE;
	}
	return OPERATION_SUCCESS;
}

SaAisErrorT __TSL_CLASS_NAME__::CreateCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId, const SaImmClassNameT className, const SaNameT * parentName, const SaImmAttrValuesT_2 ** attr) {
	HCF_CC_TRACE_FUNCTION;

	IMM_API_ReturnTypes errorCode;
	object_implementer_interface * obj;

	HCF_CC_TRACE(CreateCallback, "Create callback received, looking for implementer!", immOiHandle, ccbId, className, reinterpret_cast<const char *>(parentName->value));
	std::map<SaUint64T, object_implementer_interface *>::iterator listIterator = _implementer_list.find(immOiHandle);
	if (listIterator == _implementer_list.end()) {
		HCF_CC_TRACE(CreateCallbackErr, "No implementer found for the class, returning BAD_OPERATION", className);
		return SA_AIS_ERR_BAD_OPERATION;
	}

	if (!(obj = listIterator->second)) {
		HCF_CC_TRACE(CreateCallbackErr, "NULL implementer found for the class, returning BAD_OPERATION", className);
		return SA_AIS_ERR_BAD_OPERATION;
	}

	char * cName = new char[::strlen(className) + 1];
	::strcpy(cName, className);
	char * pName = new char[(parentName->length) + 1];
	::memcpy(pName, parentName->value, parentName->length);
	pName[parentName->length] = '\0';

	/**Calculate the size of array of attribute descriptor**/
	int dim = 0;
	int i = 0;
	while (attr[i]) {
		++i; ++dim;
	}

	/*Prepares data to be returned to calling the create function of implementer*/
	IMM_API_AttrValues * myAttributeList[dim + 1];
	IMM_API_AttrValues oneAttr[dim];

	HCF_CC_TRACE(CreateCallback, "Implementer found: preparing attributes vector!", immOiHandle, ccbId, className, reinterpret_cast<const char *>(parentName->value));
	i = 0;
	while (attr[i]) {
		oneAttr[i].attrName = attr[i]->attrName;
		oneAttr[i].attrType = (IMM_API_AttrValueTypes)(attr[i]->attrValueType);
		oneAttr[i].attrValuesNum = attr[i]->attrValuesNumber;
		oneAttr[i].attrValues = new void * [attr[i]->attrValuesNumber];

		if (attr[i]->attrValueType == SA_IMM_ATTR_SASTRINGT) {
			for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				SaStringT stringVal = *(reinterpret_cast<SaStringT *>(attr[i]->attrValues[j]));
				oneAttr[i].attrValues[j] = reinterpret_cast<char *>(stringVal);
			}

		}
		else if (attr[i]->attrValueType == SA_IMM_ATTR_SANAMET) {
			for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				SaNameT * localSaName;
				localSaName = reinterpret_cast<SaNameT *>(attr[i]->attrValues[j]);
				oneAttr[i].attrValues[j] = localSaName->value;
			}
		}
		else {
			for (unsigned int j = 0; j <  attr[i]->attrValuesNumber; j++){
				oneAttr[i].attrValues[j] = attr[i]->attrValues[j];
			}
		}

		myAttributeList[i] = &(oneAttr[i]);
		++i;
	}

	/**the myAttributeList must be a null terminated array**/
	myAttributeList[dim] = 0;

	/**call the create function of Implementer**/
	HCF_CC_TRACE(CreateCallback, "Calling create callback on the found object implementer!", immOiHandle, ccbId, className, reinterpret_cast<const char *>(parentName->value));
	errorCode = obj->create(immOiHandle, ccbId, cName, pName, myAttributeList);

	for (int i = 0; i < dim; ++i)
		delete [] oneAttr[i].attrValues;

	delete [] cName;
	delete [] pName;

	if (errorCode != OPERATION_SUCCESS) {
		HCF_CC_TRACE(CreateCallbackErr, "Call 'obj->create' failed with error message:", obj->getExitCodeString().c_str());
		saImmOiCcbSetErrorString(immOiHandle, ccbId, const_cast<char *>((obj->getExitCodeString()).c_str()));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	return SA_AIS_OK;
}

SaAisErrorT __TSL_CLASS_NAME__::DeleteCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId, const SaNameT * objectName) {
	HCF_CC_TRACE_FUNCTION;

	IMM_API_ReturnTypes errorCode;
	object_implementer_interface * obj;

	HCF_CC_TRACE(DeleteCallback, "Delete callback received, looking for implementer!", immOiHandle, ccbId, reinterpret_cast<const char *>(objectName->value));
	std::map<SaUint64T, object_implementer_interface *>::iterator listIterator = _implementer_list.find(immOiHandle);
	if (listIterator == _implementer_list.end()) {
		HCF_CC_TRACE(DeleteCallbackErr, "No implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	if (!(obj = listIterator->second)) {
		HCF_CC_TRACE(DeleteCallbackErr, "NULL implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	char * objName = new char[(objectName->length) + 1];
	::memcpy(objName, objectName->value, objectName->length);
	objName[objectName->length] = '\0';

	/**call the deleted function of Implementer**/
	HCF_CC_TRACE(DeleteCallback, "Calling delete callback on the found object implementer!", immOiHandle, ccbId, reinterpret_cast<const char *>(objectName->value));
	errorCode = obj->deleted(immOiHandle, ccbId, objName);
	delete [] objName;

	if (errorCode != OPERATION_SUCCESS) {
		HCF_CC_TRACE(DeleteCallbackErr, "Call 'obj->deleted' failed with error message:", obj->getExitCodeString().c_str());
		saImmOiCcbSetErrorString(immOiHandle, ccbId, const_cast<char *>((obj->getExitCodeString()).c_str()));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	return SA_AIS_OK;
}

SaAisErrorT __TSL_CLASS_NAME__::ModifyCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId, const SaNameT * objectName, const SaImmAttrModificationT_2 ** attrMods) {
	HCF_CC_TRACE_FUNCTION;

	IMM_API_ReturnTypes errorCode;
	object_implementer_interface * obj;

	HCF_CC_TRACE(ModifyCallback, "Modify callback received, looking for implementer!", immOiHandle, ccbId, reinterpret_cast<const char *>(objectName->value));
	std::map<SaUint64T, object_implementer_interface *>::iterator listIterator = _implementer_list.find(immOiHandle);
	if (listIterator == _implementer_list.end()) {
		HCF_CC_TRACE(ModifyCallbackErr, "No implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	if (!(obj = listIterator->second)) {
		HCF_CC_TRACE(ModifyCallbackErr, "NULL implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	char * objName = new char[(objectName->length) + 1];
	::memcpy(objName, objectName->value, objectName->length);
	objName[objectName->length] = '\0';

	/**Calculate the size of array of attribute descriptor**/
	int dim = 0;
	int i = 0;
	while (attrMods[i]) {
		++i; ++dim;
	}

	/*Prepares data to be returned to calling the modify function of implementer*/
	IMM_API_AttrModification * myAttrMods[dim + 1];
	IMM_API_AttrModification attr[dim];

	HCF_CC_TRACE(DeleteCallback, "Implementer found: preparing modified attributes vector!", immOiHandle, ccbId, reinterpret_cast<const char *>(objectName->value));
	i = 0;
	while (attrMods[i]) {
		if (attrMods[i]->modType == SA_IMM_ATTR_VALUES_ADD) {
			attr[i].modType = ATTR_VALUES_ADD;
		}
		else if (attrMods[i]->modType == SA_IMM_ATTR_VALUES_DELETE) {
			attr[i].modType = ATTR_VALUES_DELETE;
		}
		else {
			attr[i].modType = ATTR_VALUES_REPLACE;
		}

		attr[i].modAttr.attrName = attrMods[i]->modAttr.attrName;
		attr[i].modAttr.attrType = (IMM_API_AttrValueTypes)(attrMods[i]->modAttr.attrValueType);
		attr[i].modAttr.attrValuesNum = attrMods[i]->modAttr.attrValuesNumber;
		attr[i].modAttr.attrValues = new void * [attr[i].modAttr.attrValuesNum];

		if (attrMods[i]->modAttr.attrValueType == SA_IMM_ATTR_SASTRINGT) {
			for (unsigned int j = 0; j <  attr[i].modAttr.attrValuesNum; ++j) {
				SaStringT stringVal = *(reinterpret_cast<SaStringT *>(attrMods[i]->modAttr.attrValues[j]));
				attr[i].modAttr.attrValues[j] = reinterpret_cast<char *>(stringVal);
			}
		}
		else if (attrMods[i]->modAttr.attrValueType == SA_IMM_ATTR_SANAMET) {
			for (unsigned int j = 0; j <  attr[i].modAttr.attrValuesNum; ++j) {
				SaNameT * parSaName = 0;//new SaNameT;
				parSaName = (reinterpret_cast<SaNameT*>(attrMods[i]->modAttr.attrValues[j]));
				attr[i].modAttr.attrValues[j] = reinterpret_cast<char *>(parSaName->value);
			}
		}
		else {
			//Problem Struct Password
			for (unsigned int j = 0; j < attr[i].modAttr.attrValuesNum; ++j) {
				attr[i].modAttr.attrValues[j] = attrMods[i]->modAttr.attrValues[j];
			}
		}

		myAttrMods[i] = &attr[i];
		++i;
	}

	/**the myAttrMods must be a null terminated array**/
	myAttrMods[dim] = 0;

	/**call the modify function of Implementer**/
	HCF_CC_TRACE(ModifyCallback, "Calling modify callback on the found object implementer!", immOiHandle, ccbId, reinterpret_cast<const char *>(objectName->value));
	errorCode = obj->modify(immOiHandle, ccbId, objName, myAttrMods);
	delete [] objName;

	for (int i = 0; i < dim; ++i)
		delete [] attr[i].modAttr.attrValues;

	if (errorCode != OPERATION_SUCCESS) {
		HCF_CC_TRACE(ModifyCallbackErr, "Call 'obj->modify' failed with error message:", obj->getExitCodeString().c_str());
		saImmOiCcbSetErrorString(immOiHandle, ccbId, const_cast<char*>((obj->getExitCodeString()).c_str()));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	return SA_AIS_OK;
}

SaAisErrorT __TSL_CLASS_NAME__::CompletedCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId) {
	HCF_CC_TRACE_FUNCTION;

	IMM_API_ReturnTypes errorCode;
	object_implementer_interface * obj;

	HCF_CC_TRACE(CompletedCallback, "Completed callback received, looking for implementer!", immOiHandle, ccbId);
	std::map<SaUint64T, object_implementer_interface *>::iterator listIterator = _implementer_list.find(immOiHandle);
	if (listIterator == _implementer_list.end()) {
		HCF_CC_TRACE(CompletedCallbackErr, "No implementer found for the object, returning BAD_OPERATION", "");
		return SA_AIS_ERR_BAD_OPERATION;
	}

	if (!(obj = listIterator->second)) {
		HCF_CC_TRACE(CompletedCallbackErr, "NULL implementer found for the object, returning BAD_OPERATION", "");
		return SA_AIS_ERR_BAD_OPERATION;
	}

	/**call the complete function of Implementer**/
	HCF_CC_TRACE(CompletedCallback, "Calling complete callback on the found object implementer!", immOiHandle, ccbId);
	errorCode = obj->complete(immOiHandle, ccbId);

	if (errorCode != OPERATION_SUCCESS) {
		HCF_CC_TRACE(CompletedCallbackErr, "Call 'obj->complete' failed with error message:", obj->getExitCodeString().c_str());
		saImmOiCcbSetErrorString(immOiHandle, ccbId, const_cast<char*>((obj->getExitCodeString()).c_str()));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	return SA_AIS_OK;
}

void __TSL_CLASS_NAME__::AbortCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId) {
	HCF_CC_TRACE_FUNCTION;

	HCF_CC_TRACE(AbortCallback, "Abort callback received, looking for implementer!", immOiHandle, ccbId);
	std::map<SaUint64T, object_implementer_interface *>::iterator listIterator = _implementer_list.find(immOiHandle);
	if (listIterator == _implementer_list.end()) {
		HCF_CC_TRACE(AbortCallbackErr, "No implementer found for the object, returning BAD_OPERATION", "");
		return;
	}

	object_implementer_interface * obj;
	if (!(obj = listIterator->second)) {
		HCF_CC_TRACE(AbortCallbackErr, "NULL implementer found for the object, returning BAD_OPERATION", "");
		return;
	}

	/**call the abort function of Implementer**/
	HCF_CC_TRACE(AbortCallback, "Calling abort callback on the found object implementer!", immOiHandle, ccbId);
	obj->abort(immOiHandle, ccbId);
}

void __TSL_CLASS_NAME__::ApplyCallback(SaImmOiHandleT immOiHandle, SaImmOiCcbIdT ccbId) {
	HCF_CC_TRACE_FUNCTION;

	HCF_CC_TRACE(ApplyCallback, "Apply callback received, looking for implementer!", immOiHandle, ccbId);
	std::map<SaUint64T, object_implementer_interface *>::iterator listIterator = _implementer_list.find(immOiHandle);
	if (listIterator == _implementer_list.end()) {
		HCF_CC_TRACE(ApplyCallbackErr, "No implementer found for the object, returning BAD_OPERATION", "");
		return;
	}

	object_implementer_interface * obj;
	if (!(obj = listIterator->second)) {
		HCF_CC_TRACE(ApplyCallbackErr, "NULL implementer found for the object, returning BAD_OPERATION", "");
		return;
	}

	/**call the apply function of Implementer**/
	HCF_CC_TRACE(ApplyCallback, "Calling apply callback on the found object implementer!", immOiHandle, ccbId);
	obj->apply(immOiHandle, ccbId);
}

SaAisErrorT __TSL_CLASS_NAME__::OiRtAttUpdateCallback(SaImmOiHandleT immOiHandle, const SaNameT * objectName, const SaImmAttrNameT * attributeNames) {
	HCF_CC_TRACE_FUNCTION;

	IMM_API_ReturnTypes errorCode;
	object_implementer_interface * obj;

	HCF_CC_TRACE(UpdateRuntimeCallback, "Update runtime callback received, looking for implementer!", immOiHandle, reinterpret_cast<const char *>(objectName->value));
	std::map<SaUint64T, object_implementer_interface *>::iterator listIterator = _implementer_list.find(immOiHandle);
	if (listIterator == _implementer_list.end()) {
		HCF_CC_TRACE(UpdateRuntimeCallbackErr, "No implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	if (!(obj = listIterator->second)) {
		HCF_CC_TRACE(UpdateRuntimeCallbackErr, "NULL implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return SA_AIS_ERR_BAD_OPERATION;
	}

	HCF_CC_TRACE(UpdateRuntimeCallback, "Implementer found: preparing attributes-to-update vector!", immOiHandle, reinterpret_cast<const char *>(objectName->value));
	char * nameObj = new char[(objectName->length) + 1];
	::memset( nameObj, '\0', (objectName->length) +1);
	::memcpy(nameObj, objectName->value, objectName->length);

	//save a local copy of pointer of attribute name
	const SaImmAttrNameT * myPtr = attributeNames;

	//number of attribute to update
	int numAttr = 0;
	while (*attributeNames != 0) {
		++numAttr;
		++attributeNames;
	}

	char * nameAttrs[numAttr + 1];
	attributeNames = myPtr;

	int i = 0;
	while (*attributeNames != 0) {
		nameAttrs[i] = *attributeNames;
		++i;
		++attributeNames;
	}

	//close the vector with a null pointer
	nameAttrs[numAttr] = 0;

	//call the updateCallback defined by application
	HCF_CC_TRACE(UpdateRuntimeCallback, "Calling update runtime callback on the found object implementer!", immOiHandle, reinterpret_cast<const char *>(objectName->value));
	errorCode = obj->updateRuntime(nameObj, (const char **)(nameAttrs));
	delete [] nameObj;

	if(errorCode != OPERATION_SUCCESS) {
		HCF_CC_TRACE(UpdateRuntimeCallbackErr, "Call 'obj->updateRuntime' failed with error message:", obj->getExitCodeString().c_str());
		return SA_AIS_ERR_BAD_OPERATION;
	}

	return SA_AIS_OK;
}

void __TSL_CLASS_NAME__::adminOperationCallback(SaImmOiHandleT immOiHandle, SaInvocationT invocation, const SaNameT * objectName,	SaImmAdminOperationIdT operationId, const SaImmAdminOperationParamsT_2 ** params) {
	HCF_CC_TRACE_FUNCTION;

	HCF_CC_TRACE(AdminCallback, "Administrative callback received, looking for implementer!", immOiHandle, invocation, operationId, reinterpret_cast<const char *>(objectName->value));
	std::map<SaUint64T, object_implementer_interface *>::iterator listIterator = _implementer_list.find(immOiHandle);
	if (listIterator == _implementer_list.end()) {
		HCF_CC_TRACE(AdminCallbackErr, "No implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return;
	}

	object_implementer_interface * obj;
	if (!(obj = listIterator->second)) {
		HCF_CC_TRACE(AdminCallbackErr, "NULL implementer found for the object, returning BAD_OPERATION", reinterpret_cast<const char *>(objectName->value));
		return;
	}

	HCF_CC_TRACE(AdminCallback, "Implementer found: preparing administrative operation arguments vector!", immOiHandle, invocation, operationId, reinterpret_cast<const char *>(objectName->value));
	/*set the object name to be passed*/
	char * nameObj = new char[(objectName->length) + 1];
	::memset( nameObj, '\0', (objectName->length) +1);
	::memcpy(nameObj, objectName->value, objectName->length );

	/**Calculate the size of array of attribute descriptor**/
	int dim = 0;
	int i = 0;
	while (params[i]) {
		++i; ++dim;
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
			localSaName = reinterpret_cast<SaNameT *>(params[i]->paramBuffer);
			oneAttr[i].attrValues = localSaName->value;
		}
		else {
			oneAttr[i].attrValues = params[i]->paramBuffer;
		}

		myAttributeList[i] = &(oneAttr[i]);
		++i;
	}

	/**the myAttributeList must be a null terminated array**/
	myAttributeList[dim] = 0;

	HCF_CC_TRACE(AdminCallback, "Calling administrative callback on the found object implementer!", immOiHandle, invocation, operationId, reinterpret_cast<const char *>(objectName->value));
	obj->adminOperationCallback(immOiHandle, (IMM_API_InvocationType) invocation, nameObj, (IMM_API_AdminOperationIdType) operationId, myAttributeList);
	delete []nameObj;
}

TSL_NS_HCF_IMM_END
