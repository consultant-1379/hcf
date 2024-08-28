#include <string.h>

#include "hcf/core/logger.h"
#include "hcf/core/guard.h"

#include "hcf/imm/imm_error_constants.h"
#include "hcf/imm/constants.h"

#include "hcf/trace/com_ericsson_common_hcf_imm_immobject.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/imm/imm_object.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_IMM_OBJECT__

TSL_NS_HCF_IMM_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::set_object_attributes_and_dn (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, const char * parent_dn, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;

	// FIRST: Call the overload method to modify the object attributes
	HCF_CC_TRACE(SetObjectAttributes, "Setting the attributes for the object under the parent: ", parent_dn, "");
	int call_result = 0;
	if ((call_result = set_object_attributes(attributes, callback_info))) {
		HCF_CC_LOG(LEVEL_ERROR, "Call 'set_object_attributes' failed, return_code == %d", call_result);
		HCF_CC_TRACE(SetObjectAttributesErr, "Call 'set_object_attributes' failed.", call_result);
		return call_result;
	}

	// SECOND: calculate an appropriate value for the object DN and ID
	_obj_dn = ((!parent_dn) || (!::strcmp(parent_dn, ""))) ? _obj_rdn : (_obj_rdn + "," + std::string(parent_dn));
	_obj_id = _obj_rdn.substr(_obj_rdn.find_first_of('=') + 1);
	HCF_CC_TRACE(SetObjectAttributes, "The value of the object DN and ID are:", _obj_dn.c_str(), _obj_id.c_str());
	return ERR_NO_ERROR;
}

std::string __TSL_CLASS_NAME__::get_obj_id_from_dn (const char * obj_dn) {
	HCF_CC_TRACE_FUNCTION;

	/*
	 * Basically, if the object has the following DN: "objId=<ID>,parentId=<SOME_VALUE>",
	 * then the object ID will be "<ID>".
	 */
	std::string obj_id(obj_dn);
	size_t equal_pos = obj_id.find_first_of('=');
	size_t comma_pos = obj_id.find_first_of(',');

	if (equal_pos == std::string::npos) {
		HCF_CC_LOG(LEVEL_WARNING, "DN %s is not correctly formed, exiting", obj_dn);
		return "";
	}

	obj_id = obj_id.substr(equal_pos + 1, (comma_pos != std::string::npos) ? (comma_pos - equal_pos - 1) : std::string::npos);

	HCF_CC_TRACE(GetObjIdFromDN, "The object identifier was calculated starting from the DN as follow:", obj_dn, obj_id.c_str());
	return obj_id;
}

int __TSL_CLASS_NAME__::create_object (const char * class_name, const char * parent_name, std::vector<IMM_API_ValuesDefinitionTypes> & attr_values) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CreateObject, "Received a request to create an object: initializing the OM handler!", class_name, parent_name);

	om_handler om_handler;
	if (om_handler.Init() != OPERATION_SUCCESS) {
		HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.Init' failed, return_code == %d, error_message == '%s'!",
				om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
		HCF_CC_TRACE(CreateObjectErr, "Call 'om_handler.Init' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
		return ERR_OM_HANDLER_INIT_FAILED;
	}

	// Create the object instance with the provided info
	int imm_error_code = IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
	for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
		HCF_CC_TRACE(CreateObject, "Creating the object instance!", class_name, parent_name);

		if (om_handler.createObject(class_name, parent_name, attr_values) != OPERATION_SUCCESS) {
			imm_error_code = om_handler.getInternalLastError();
			HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.createObject' failed (RETRY == %d): class_name == '%s', parent_name == '%s' "
					"call_result == %d, error_message == '%s'!", (HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), class_name,
					parent_name, imm_error_code, om_handler.getInternalLastErrorText());
			HCF_CC_TRACE(CreateObjectErr, "Call 'om_handler.createObject' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());

			// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
			if (imm_error_code == IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
		}
		else imm_error_code = IMM_API_ERROR_CODE_NO_ERR;
	}

	HCF_CC_TRACE(CreateObject, "Object correctly created: finalizing OM handler!", class_name, parent_name);
	om_handler.Finalize();
	return ((imm_error_code == IMM_API_ERROR_CODE_NO_ERR) ? ERR_NO_ERROR : (imm_error_code == IMM_API_ERROR_CODE_IMM_EXIST) ? ERR_OBJECT_ALREADY_EXISTS : ERR_OM_HANDLER_CREATE_OBJECT_FAILED);
}

int __TSL_CLASS_NAME__::modify_object_attribute (const char * attribute_name, IMM_API_AttrValueTypes attribute_type, void * attribute_value) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ModifyObject, "Received a request to modify an object attribute: initializing the OM handler!", _obj_dn.c_str(), attribute_name, attribute_type);

	int call_result = 0;
	if ((call_result = initialize_om_handler())) {
		HCF_CC_LOG(LEVEL_ERROR, "Call 'initialize_om_handler' failed, return_code == %d!", call_result);
		HCF_CC_TRACE(ModifyObjectErr, "Call 'initialize_om_handler' failed: return code is:", call_result, "");
		return call_result;
	}

	IMM_API_ImmParameter parameter;
	parameter.attrName = const_cast<char *> (attribute_name);
	parameter.attrType = attribute_type;
	parameter.attrValuesNum = 1;
	void * value[1] = {attribute_value};
	parameter.attrValues = value;

	int imm_error_code = IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		// Modify the current object with the provided info
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(ModifyObject, "Modifying the object attribute value!", _obj_dn.c_str(), attribute_name, attribute_type);

			if (_om_handler.modifyAttribute(_obj_dn.c_str(), &parameter) != OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d), for the attribute '%s' "
						"of the object having dn == '%s'return_code == %d, error_message == '%s'!", (HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1),
						attribute_name, _obj_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_TRACE(ModifyObjectErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = IMM_API_ERROR_CODE_NO_ERR;
		}
	TSL_CRITICAL_SECTION_END;
	return (imm_error_code == IMM_API_ERROR_CODE_NO_ERR) ? ERR_NO_ERROR : ERR_OM_HANDLER_MODIFY_OBJECT_FAILED;
}

int __TSL_CLASS_NAME__::modify_object_attribute (const char * attribute_name, IMM_API_AttrValueTypes attribute_type, int num_of_attr, void ** attribute_values) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ModifyObject, "Received a request to modify an object MULTI-VALUED attribute: initializing the OM handler!", _obj_dn.c_str(), attribute_name, attribute_type);

	int call_result = 0;
	if ((call_result = initialize_om_handler())) {
		HCF_CC_LOG(LEVEL_ERROR, "Call 'initialize_om_handler' failed, return_code == %d!", call_result);
		HCF_CC_TRACE(ModifyObjectErr, "Call 'initialize_om_handler' failed: return code is:", call_result, "");
		return call_result;
	}

	IMM_API_ImmParameter parameter;
	parameter.attrName = const_cast<char *> (attribute_name);
	parameter.attrType = attribute_type;
	parameter.attrValuesNum = num_of_attr;
	parameter.attrValues = attribute_values;

	int imm_error_code = IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		// Modify the current object with the provided info
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(ModifyObject, "Modifying the object MULTI-VALUED attribute values!", _obj_dn.c_str(), attribute_name, attribute_type);

			if (_om_handler.modifyAttribute(_obj_dn.c_str(), &parameter) != OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d), for the attribute '%s' "
						"of the object having dn == '%s'return_code == %d, error_message == '%s'!", (HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1),
						attribute_name, _obj_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_TRACE(ModifyObjectErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = IMM_API_ERROR_CODE_NO_ERR;
		}
	TSL_CRITICAL_SECTION_END;
	return (imm_error_code == IMM_API_ERROR_CODE_NO_ERR) ? ERR_NO_ERROR : ERR_OM_HANDLER_MODIFY_OBJECT_FAILED;
}

int __TSL_CLASS_NAME__::delete_object () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(DeleteObject, "Received a request to delete this object: initializing the OM handler!", _obj_dn.c_str());

	om_handler om_handler;
	if (om_handler.Init() != OPERATION_SUCCESS) {
		HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.Init' failed, return_code == %d, error_message == '%s'!",
				om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
		HCF_CC_TRACE(DeleteObjectErr, "Call 'om_handler.Init' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
		return ERR_OM_HANDLER_INIT_FAILED;
	}

	// Delete the current object instance
	int imm_error_code = IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		// Modify the current object with the provided info
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(DeleteObject, "Deleting the object instance!", _obj_dn.c_str());

			if (om_handler.deleteObject(_obj_dn.c_str(), SCOPE_SUBTREE) != OPERATION_SUCCESS) {
				imm_error_code = om_handler.getInternalLastError();
				HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.deleteObject' failed (RETRY == %d) for the object having dn == '%s',"
						" return_code == %d, error_message == '%s'!", (HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), _obj_dn.c_str(),
						om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
				HCF_CC_TRACE(DeleteObjectErr, "Call 'om_handler.deleteObject' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = IMM_API_ERROR_CODE_NO_ERR;
		}
	TSL_CRITICAL_SECTION_END;

	HCF_CC_TRACE(DeleteObject, "Object correctly created: finalizing OM handler!", "");
	om_handler.Finalize();
	return (imm_error_code == IMM_API_ERROR_CODE_NO_ERR) ? ERR_NO_ERROR : ERR_OM_HANDLER_DELETE_OBJECT_FAILED;
}

int __TSL_CLASS_NAME__::initialize_om_handler () {
	HCF_CC_TRACE_FUNCTION;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		if (_om_handler_initialized) {
			HCF_CC_TRACE(InitializeOM, "The OM Handler object was already initialized, nothing to do!", _om_handler_initialized);
			return ERR_NO_ERROR;
		}

		// Initialize the om_handler object
		HCF_CC_TRACE(InitializeOM, "Initializing the OM Handler object!", _om_handler_initialized);
		if (_om_handler.Init() != OPERATION_SUCCESS) {
			HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.Init' failed, return_code == %d, error_message == '%s'!",
					_om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
			HCF_CC_TRACE(InitializeOMErr, "Call '_om_handler.Init' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
			return ERR_OM_HANDLER_INIT_FAILED;
		}

		_om_handler_initialized = true;
		HCF_CC_TRACE(InitializeOM, "The OM Handler object was correctly initialized!", _om_handler_initialized);
	TSL_CRITICAL_SECTION_END;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize_om_handler () {
	HCF_CC_TRACE_FUNCTION;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		if (!_om_handler_initialized) {
			HCF_CC_TRACE(FinalizeOM, "The OM Handler object was already finalized, nothing to do!", _om_handler_initialized);
			return ERR_NO_ERROR;
		}

		// Finalize the om_handler object
		HCF_CC_TRACE(FinalizeOM, "Finalizing the OM Handler object!", _om_handler_initialized);
		if (_om_handler.Finalize() != OPERATION_SUCCESS) {
			HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.Finalize' failed, return_code == %d, error_message == '%s'!",
					_om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
			HCF_CC_TRACE(FinalizeOMErr, "Call '_om_handler.Finalize' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
			return ERR_OM_HANDLER_FINALIZE_FAILED;
		}

		_om_handler_initialized = false;
		HCF_CC_TRACE(FinalizeOM, "The OM Handler object was correctly finalized!", _om_handler_initialized);
	TSL_CRITICAL_SECTION_END;
	return ERR_NO_ERROR;
}

TSL_NS_HCF_IMM_END
