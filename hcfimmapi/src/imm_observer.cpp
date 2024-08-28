#include "hcf/core/logger.h"
#include "hcf/imm/constants.h"

#include "hcf/trace/com_ericsson_common_hcf_imm_immobserver.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/imm/imm_observer.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_IMM_OBSERVER__

TSL_NS_HCF_IMM_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_SHADOW_IMPLEMENTER_NAME_PREFIX = "@";

__TSL_CLASS_NAME__::TSL_CTOR (std::vector<std::string> classes, const std::string & implementer_name)
	TSL_CTOR_INIT_LIST(object_implementer_interface(), event_handler(), _object_initialized(false), _oi_handler(), _classes(classes)) {
	HCF_CC_TRACE_FUNCTION;

	const std::string shadow_implementer_name = ((!implementer_name.size()) || (!implementer_name.find(HCF_SHADOW_IMPLEMENTER_NAME_PREFIX)))
		? implementer_name : (HCF_SHADOW_IMPLEMENTER_NAME_PREFIX + implementer_name);
	setImpName(shadow_implementer_name);
}

int __TSL_CLASS_NAME__::initialize () {
	HCF_CC_TRACE_FUNCTION;

	if (_object_initialized) {
		HCF_CC_TRACE(Initialize, "The observer object was already initialized: nothing to do!", getImpName().c_str(), _classes.size());
		return ERR_NO_ERROR;
	}

	HCF_CC_LOG(LEVEL_NOTICE, "Setting the hidden class implementer (with name '%s') for %zd classes!", getImpName().c_str(), _classes.size());

	// Try to set the class implementers max for MAX_NUM_OF_RETRIES
	for (int i = 0; i < MAX_NUM_OF_RETRIES; ++i) {
		HCF_CC_TRACE(Initialize, "Calling 'addMultipleClassImpl' function to set the class implementer!", getImpName().c_str(), _classes.size());

		if (_oi_handler.addMultipleClassImpl(this, _classes) != OPERATION_SUCCESS) {
			HCF_CC_TRACE(InitializeErr, "Call 'addMultipleClassImpl' failed.", _oi_handler.getInternalLastError(), _oi_handler.getInternalLastErrorText());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'addMultipleClassImpl' failed for implementer name '%s', waiting before retrying... "
					"error_code == %d, error_message == '%s'", getImpName().c_str(),
					_oi_handler.getInternalLastError(), _oi_handler.getInternalLastErrorText());
			::sleep(1);
		}
		else {
			HCF_CC_TRACE(Initialize, "Class implementer correctly set for all the classes: the object is now initialized!", getImpName().c_str(), _classes.size());
			_object_initialized = true;
			return ERR_NO_ERROR;
		}
	}

	return ERR_OM_HANDLER_SET_IMPLEMENTER_FAILED;
}

int __TSL_CLASS_NAME__::finalize () {
	HCF_CC_TRACE_FUNCTION;

	if (!_object_initialized) {
		HCF_CC_TRACE(Finalize, "The observer object was already finalized: nothing to do!", getImpName().c_str(), _classes.size());
		return ERR_NO_ERROR;
	}

	HCF_CC_LOG(LEVEL_NOTICE, "Removing the hidden class implementer for %zd classes!", _classes.size());

	// Try to remove the class implementers max for MAX_NUM_OF_RETRIES
	for (int i = 0; i < MAX_NUM_OF_RETRIES; ++i) {
		HCF_CC_TRACE(Finalize, "Calling 'removeMultipleClassImpl' function to remove the class implementer!", getImpName().c_str(), _classes.size());

		if (_oi_handler.removeMultipleClassImpl(this, _classes) != OPERATION_SUCCESS) {
			HCF_CC_TRACE(FinalizeErr, "Call 'removeMultipleClassImpl' failed.", _oi_handler.getInternalLastError(), _oi_handler.getInternalLastErrorText());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'removeMultipleClassImpl' failed for implementer name '%s', waiting before retrying... "
					"error_code == %d, error_message == '%s'", getImpName().c_str(),
					_oi_handler.getInternalLastError(), _oi_handler.getInternalLastErrorText());
			::sleep(1);
		}
		else {
			HCF_CC_TRACE(Finalize, "Class implementer correctly removed for all the classes: the object is now finalized!", getImpName().c_str(), _classes.size());
			_object_initialized = false;
			return ERR_NO_ERROR;
		}
	}

	return ERR_OM_HANDLER_REMOVE_IMPLEMENTER_FAILED;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::create (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, const char * class_name, const char * parent_dn, IMM_API_AttrValues ** attr) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CreateCallback, "The observer object received a request to validate an object creation: "
			"preparing the attributes vector!", oiHandle, ccbId, class_name, parent_dn);

	std::vector<std::pair<std::string, std::vector<void *> > > obj_attributes;

	for (int i = 0; attr[i]; ++i) {
		// FIRST: get all the values for the current attribute
		std::vector<void *> attr_values;
		for (unsigned int j = 0; j < attr[i]->attrValuesNum; ++j)	attr_values.push_back(attr[i]->attrValues[j]);

		// SECOND: create a pair with the attribute name and the retrieved value(s)
		std::pair<std::string, std::vector<void *> > attribute;
		attribute.first = attr[i]->attrName;
		attribute.second = attr_values;

		// THIRD: add the pair to the array of attributes
		obj_attributes.push_back(attribute);
	}

	HCF_CC_TRACE(CreateCallback, "Attributes vector was correctly created: notifying about the object creation!", oiHandle, ccbId, class_name, parent_dn);
	return (notify_object_creation(class_name, parent_dn, obj_attributes) ? OPERATION_FAILURE : OPERATION_SUCCESS);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleted (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, const char * obj_dn) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(DeleteCallback, "The observer object received a request to validate an object deletion: "
			"notifying about the object deletion!", oiHandle, ccbId, obj_dn);

	return (notify_object_deletion(obj_dn) ? OPERATION_FAILURE : OPERATION_SUCCESS);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::modify (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, const char * obj_dn, IMM_API_AttrModification ** attrMods) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ModifyCallback, "The observer object received a request to validate an object modification: "
			"preparing the attributes vector!", oiHandle, ccbId, obj_dn);

	std::vector<std::pair<std::string, std::vector<void *> > > obj_attributes;

	for (int i = 0; attrMods[i]; ++i) {
		// FIRST: get all the values for the current attribute
		IMM_API_AttrValues modified_attr = attrMods[i]->modAttr;
		std::vector<void *> attr_values;
		for (unsigned int j = 0; j < modified_attr.attrValuesNum; ++j)	attr_values.push_back(modified_attr.attrValues[j]);

		// SECOND: create a pair with the attribute name and the retrieved value(s)
		std::pair<std::string, std::vector<void *> > attribute;
		attribute.first = modified_attr.attrName;
		attribute.second = attr_values;

		// THIRD: add the pair to the array of attributes
		obj_attributes.push_back(attribute);
	}

	HCF_CC_TRACE(ModifyCallback, "Attributes vector was correctly created: notifying about the object modification!", oiHandle, ccbId, obj_dn);
	return (notify_object_modification(obj_dn, obj_attributes) ? OPERATION_FAILURE : OPERATION_SUCCESS);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::complete (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CompleteCallback, "The observer object received a request to complete the CCB: notifying the about the callback!", oiHandle, ccbId);
	return (notify_object_modification_completion() ? OPERATION_FAILURE : OPERATION_SUCCESS);
}

void __TSL_CLASS_NAME__::abort (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(AbortCallback, "The observer object received a request to abort the CCB: notifying the about the callback!", oiHandle, ccbId);
	notify_object_modification_abort();
}

void __TSL_CLASS_NAME__::apply (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ApplyCallback, "The observer object received a request to apply the CCB: notifying the about the callback!", oiHandle, ccbId);
	notify_object_modification_application();
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::updateRuntime (const char * TSL_UNUSED_PAR(obj_dn), const char ** TSL_UNUSED_PAR(attributes)) {
	HCF_CC_TRACE_FUNCTION;
	// Not needed for observer!
	return OPERATION_SUCCESS;
}

void __TSL_CLASS_NAME__::adminOperationCallback (IMM_API_OiHandle TSL_UNUSED_PAR(oiHandle), IMM_API_InvocationType TSL_UNUSED_PAR(invocation), const char * TSL_UNUSED_PAR(obj_dn), IMM_API_AdminOperationIdType TSL_UNUSED_PAR(operationId), IMM_API_AdminOperationParamType ** TSL_UNUSED_PAR(paramList)) {
	HCF_CC_TRACE_FUNCTION;
	// Not needed for observer!
}

int __TSL_CLASS_NAME__::handle_input () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Received a callback for the observer object, dispatching it!");
	HCF_CC_TRACE(HandleInput, "Received an input event (callback to be dispatched) on the following file descriptor:", get_handle());

	int call_result = 0;
	if ((call_result = dispatch(TSL_NS_HCF_IMM::DISPATCH_ALL))) {
		HCF_CC_LOG(LEVEL_ERROR, "Dispatch failed for the observer object', call_result == %d", call_result);
		HCF_CC_TRACE(HandleInput, "Failed to dispatch the IMM callback, the call returned the following error code:", call_result);
	}
	return 0;
}

int __TSL_CLASS_NAME__::handle_output () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_WARNING, "Received an output I/O event for the observer object, "
			"it shouldn't occur, removing the Event Handler from the reactor");
	HCF_CC_TRACE(HandleOutput, "Received an output event (very strange)!", get_handle());
	return -1;
}

int __TSL_CLASS_NAME__::handle_exception (bool error_flag, bool hungup_flag, bool read_hungup_flag) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_WARNING, "Received an error on the I/O handle for the observer object, removing the "
			"Event Handler from the reactor: error_flag == %s, hungup_flag == %s, read_hungup_flag == %s",
			(error_flag ? "TRUE" : "FALSE"), (hungup_flag ? "TRUE" : "FALSE"), (read_hungup_flag ? "TRUE" : "FALSE"));
	HCF_CC_TRACE(HandleException, "Received an error event!", get_handle(), error_flag, hungup_flag, read_hungup_flag);
	return -1;
}

int __TSL_CLASS_NAME__::handle_close () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Removing the class handler for the observer object from the reactor!");
	HCF_CC_TRACE(HandleClose, "Received a request to remove the event handler from the reactor!", get_handle());
	return 0;
}

TSL_NS_HCF_IMM_END
