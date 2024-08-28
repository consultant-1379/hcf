#include "hcf/core/logger.h"
#include "hcf/core/guard.h"
#include "hcf/imm/imm_config_class.h"

#include "imm_object_hcrule.h"
#include "imm_object_hcrule_inputparameter.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE_INPUTPARAMETER__

HCF_NS_HCF_SRV_BEGIN

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_INPUTPARAMETER_CLASS_NAME             = "HCMHcRuleInputParameter";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_INPUTPARAMETER_IMPLEMENTER_NAME       = "HcRuleInputParameter_Implementer";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_INPUTPARAMETER_ID_ATTR_NAME           = "id";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_INPUTPARAMETER_DESCRIPTION_ATTR_NAME  = "description";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_INPUTPARAMETER_NAME_ATTR_NAME         = "name";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_INPUTPARAMETER_VALUE_ATTR_NAME        = "value";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_INPUTPARAMETER_DEFAULT_ATTR_ID_FORMAT = "id=%s-%d";

int __TSL_CLASS_NAME__::set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	// Save the old values before modifying them, in order to rollback in case of errors
	_description_old = _description;
	_name_old        = _name;
	_value_old       = _value;

	// For each of the provided attributes, understand the attribute name and set the class field accordingly
	for (size_t i = 0; i < attributes.size(); ++i) {

		// Extract from the given attributes vector the attribute name and its value
		std::string attr_name = attributes[i].first;
		std::vector<void *> attr_values = attributes[i].second;
		HCF_CC_TRACE(SetObjectAttributes, "Received a request to set/change the value of the following attribute:", attr_name.c_str());

		if (!attr_name.compare(HCF_HCRULE_INPUTPARAMETER_ID_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_obj_rdn = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The value of the object RDN is:", _obj_rdn.c_str());
			}
			else {	// ERROR: the ID field cannot be empty!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HCRULE_INPUTPARAMETER_DESCRIPTION_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_description = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the description attribute is:", _description.c_str());
			}
			else {	// ERROR: No value provided!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HCRULE_INPUTPARAMETER_NAME_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_name = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the name attribute is:", _name.c_str());
			}
			else {	// ERROR: No value provided!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HCRULE_INPUTPARAMETER_VALUE_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_value = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the value attribute is:", _value.c_str());
			}
			else {	// ERROR: No value provided!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
	}

	HCF_CC_LOG(LEVEL_NOTICE, "Object attributes correctly modified: RDN == '%s', _description == '%s', _name == '%s',"
			" _value == '%s'", _obj_rdn.c_str(), _description.c_str(), _name.c_str(), _value.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::create_hcrule_input_parameter (const char * attribute_rdn, const char * rule_dn, const char * description, const char * name, const char * value, bool error_if_exists) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CreateHcRuleInputParameter, "Received a request to create a new instance of HcRuleInputParameter class: "
			"preparing all the needed data structures!", attribute_rdn, rule_dn, description, name, value, error_if_exists);

	std::vector<TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes> attr_list;

	// FIRST: prepare the ID attribute structure
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes id_attr;
	id_attr.attrName = const_cast<char *>(HCF_HCRULE_INPUTPARAMETER_ID_ATTR_NAME);
	id_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	id_attr.attrValuesNum = 1;
	void * tmp_id[1] = { const_cast<char *>(attribute_rdn) };
	id_attr.attrValues = tmp_id;
	attr_list.push_back(id_attr);

	// SECOND: prepare the name attribute structure
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes descr_attr;
	descr_attr.attrName = const_cast<char *>(HCF_HCRULE_INPUTPARAMETER_DESCRIPTION_ATTR_NAME);
	descr_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	descr_attr.attrValuesNum = 1;
	void * tmp_descr[1] = { const_cast<char *>(description) };
	descr_attr.attrValues = tmp_descr;
	attr_list.push_back(descr_attr);

	// THIRD: prepare the name attribute structure
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes name_attr;
	name_attr.attrName = const_cast<char *>(HCF_HCRULE_INPUTPARAMETER_NAME_ATTR_NAME);
	name_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	name_attr.attrValuesNum = 1;
	void * tmp_name[1] = { const_cast<char *>(name) };
	name_attr.attrValues = tmp_name;
	attr_list.push_back(name_attr);

	// FOURTH: prepare the value attribute structure
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes value_attr;
	value_attr.attrName = const_cast<char *>(HCF_HCRULE_INPUTPARAMETER_VALUE_ATTR_NAME);
	value_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	value_attr.attrValuesNum = 1;
	void * tmp_value[1] = { const_cast<char *>(value) };
	value_attr.attrValues = tmp_value;
	attr_list.push_back(value_attr);

	HCF_CC_TRACE(CreateHcRuleInputParameter, "Received a request to create a new instance of HcRuleInputParameter class: creating the object instance!", attribute_rdn, rule_dn, description, name, value, error_if_exists);
	if (const int call_result = create_object(HCF_HCRULE_INPUTPARAMETER_CLASS_NAME, rule_dn, attr_list)) {
		if ((!error_if_exists) && (call_result == TSL_NS_HCF_IMM::ERR_OBJECT_ALREADY_EXISTS)) {
			HCF_CC_TRACE(CreateHcRuleInputParameter, "Call 'create_object' returned already exists, nothing to do!", attribute_rdn, rule_dn, description, name, value, error_if_exists);
			HCF_CC_LOG(LEVEL_NOTICE, "The object having RDN == '%s' already exist, nothing to do!", attribute_rdn);
		}
		else {
			HCF_CC_TRACE(CreateHcRuleInputParameterErr, "Call 'create_object' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'create_object' failed for class %s under object having "
					"dn == '%s', call_result == %d", HCF_HCRULE_INPUTPARAMETER_CLASS_NAME, rule_dn, call_result);
			return call_result;
		}
	}
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	// The callback has been aborted, restore the original values
	_description = _description_old;
	_name        = _name_old;
	_value       = _value_old;

	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
