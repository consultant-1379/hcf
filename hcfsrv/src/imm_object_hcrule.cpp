#include <string.h>

#include "hcf/core/logger.h"
#include "hcf/core/guard.h"
#include "hcf/imm/imm_config_class.h"

#include "configurator.h"
#include "srv_utils.h"
#include "imm_object_healthcheckm.h"
#include "imm_object_hcrule_inputparameter.h"
#include "imm_object_hcrule.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_healthcheckm> healthcheckm_class_handler;
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule> hcrule_class_handler;
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule_inputparameter> hcrule_input_parameter_class_handler;

HCF_NS_HCF_SRV_BEGIN

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_CLASS_NAME                   = "HCMHcRule";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_IMPLEMENTER_NAME             = "HcRule_Implementer";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_RULE_ID_ATTR_NAME            = "hcRuleId";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_ADM_STATE_ATTR_NAME          = "administrativeState";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_CATEGORIES_ATTR_NAME         = "categories";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_INPUT_PARAMS_ATTR_NAME       = "inputParameters";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_DESCRIPTION_ATTR_NAME        = "description";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_NAME_ATTR_NAME               = "name";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_RECOMMENDED_ACTION_ATTR_NAME = "recommendedAction";
const char * const __TSL_CLASS_NAME__::HCF_HCRULE_SEVERITY_ATTR_NAME           = "severity";
std::set<std::string> __TSL_CLASS_NAME__::_rules_set_by_category[HCF_MAX_NUMBER_OF_CATEGORIES];
TSL_NS_HCF_CORE::thread_mutex_recursive __TSL_CLASS_NAME__::_rules_set_by_category_mutex;

int __TSL_CLASS_NAME__::set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	// Save the old values before modifying them, in order to rollback in case of errors
	_adm_state_old               = _adm_state;
	_categories_old              = _categories;
	_rule_default_parameters_old = _rule_default_parameters;
	_description_old             = _description;
	_name_old                    = _name;
	_recommended_action_old      = _recommended_action;
	_severity_old                = _severity;

	// For each of the provided attributes, understand the attribute name and set the class field accordingly
	for (size_t i = 0; i < attributes.size(); ++i) {
		// Extract from the given attributes vector the attribute name and its value
		std::string attr_name = attributes[i].first;
		std::vector<void *> attr_values = attributes[i].second;
		HCF_CC_TRACE(SetObjectAttributes, "Received a request to set/change the value of the following attribute:", attr_name.c_str());

		if (!attr_name.compare(HCF_HCRULE_RULE_ID_ATTR_NAME)) {
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
		else if (!attr_name.compare(HCF_HCRULE_ADM_STATE_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_adm_state = *(reinterpret_cast<administrative_state_t *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the administrative state attribute is:", std::to_string(_adm_state).c_str());
			}
			else {
				// No value provided, using the default value
				_adm_state = ADMINISTRATIVE_STATE_UNLOCKED;
				HCF_CC_TRACE(SetObjectAttributes, "No value for the administrative state attribute was provided, using the default value:", std::to_string(_adm_state).c_str());
			}
		}
		else if (!attr_name.compare(HCF_HCRULE_CATEGORIES_ATTR_NAME)) {
			// Erase the old categories, if present
			_categories.clear();

			// Fill the categories set with the provided values
			for (size_t j = 0; j < attr_values.size(); ++j) {
				const category_t cat = *(reinterpret_cast<category_t *>(attr_values[j]));
				HCF_CC_TRACE(SetObjectAttributes, "The new value for the category is:", utils::map_category_value_to_category_string(cat));
				_categories.insert(cat);
			}
		}
		else if (!attr_name.compare(HCF_HCRULE_INPUT_PARAMS_ATTR_NAME)) {
			// Extract from the input parameters the HcRule default input parameters
			std::map<std::string, std::pair<std::string, std::string>> tmp_parameters_map;

			for (size_t j = 0; j < attr_values.size(); ++j) {

				// Extract the object ID from its DN
				std::string attr_dn = reinterpret_cast<char *>(attr_values[j]);
				std::string attr_id = imm_object_hcrule_inputparameter::get_obj_id_from_dn(attr_dn.c_str());
				if (attr_id.empty()) {
					HCF_CC_TRACE(SetObjectAttributesErr, "The DN of the HcRuleInputParameter object is not correctly formed!", ERR_NULL_INSTANCE_RETRIEVED);
					HCF_CC_LOG(LEVEL_ERROR, "The DN (%s) of the HcRuleInputParameter object is not correctly formed!", attr_dn.c_str());
					set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
					return ERR_NULL_INSTANCE_RETRIEVED;
				}

				// Get the HcRuleInputParameter instance from its ID
				HCF_CC_TRACE(SetObjectAttributes, "Retrieving the instance of HcRuleInputParameter with the following ID:", attr_id.c_str());
				imm_object_hcrule_inputparameter * parameter = hcrule_input_parameter_class_handler.get_instance(attr_id);
				if (!parameter) {
					HCF_CC_TRACE(SetObjectAttributesErr, "Call 'hcrule_input_parameter_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
					HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_input_parameter_class_handler.get_instance' failed with ID == '%s'!", attr_id.c_str());
					set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
					return ERR_NULL_INSTANCE_RETRIEVED;
				}

				// Store the retrieved information into the temporary map
				HCF_CC_TRACE(SetObjectAttributes, "Adding the following HcRuleInputParameter to the internal data structures:", attr_id.c_str());
				std::pair<std::string, std::string> type_value_pair(parameter->get_type(), parameter->get_value());
				tmp_parameters_map[parameter->get_name()] = type_value_pair;
			}

			// Copy the information stored into the temporary map into the class member
			HCF_CC_TRACE(SetObjectAttributes, "Copying the HcRuleInputParameter information in the internal data structures:", "");
			TSL_CRITICAL_SECTION_BEGIN(sync_t, _rule_parameters_mutex);
				_rule_default_parameters.clear();
				_rule_default_parameters = tmp_parameters_map;
			TSL_CRITICAL_SECTION_END;
		}
		else if (!attr_name.compare(HCF_HCRULE_DESCRIPTION_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_description = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the description attribute is:", _description.c_str());
			}
			else {	// ERROR: this field cannot be empty!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HCRULE_NAME_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_name = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the name attribute is:", _name.c_str());
			}
			else {	// ERROR: this field cannot be empty!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HCRULE_RECOMMENDED_ACTION_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_recommended_action = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the recommended action attribute is:", _recommended_action.c_str());
			}
			else {	// ERROR: this field cannot be empty!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HCRULE_SEVERITY_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_severity = *(reinterpret_cast<severity_t *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the recommended action attribute is:", std::to_string(_severity).c_str());
			}
			else {	// ERROR: this field cannot be empty!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
	}
	// Add the current rule into the array of rules per category
	std::string rule_id = _obj_rdn.substr(_obj_rdn.find_first_of('=') + 1);
	HCF_CC_TRACE(SetObjectAttributes, "Adding the following HcRule instance to the global list of HcRules:", rule_id.c_str());

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _rules_set_by_category_mutex);
		for (std::set<category_t>::const_iterator category = _categories.begin(); category != _categories.end(); ++category) {
			_rules_set_by_category[*category].insert(rule_id);
		}
	TSL_CRITICAL_SECTION_END;

	size_t rule_default_parameters_size = 0;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _rule_parameters_mutex);
		rule_default_parameters_size = _rule_default_parameters.size();
	TSL_CRITICAL_SECTION_END;

	HCF_CC_LOG(LEVEL_NOTICE, "Object attributes correctly modified: RDN == '%s', _adm_state == %d, _categories(count) == %lu,"
			"_rule_default_parameters(count) == %lu, _description == '%s', _name == '%s', _recommended_action == '%s', _severity == %d",
			_obj_rdn.c_str(), _adm_state, _categories.size(), rule_default_parameters_size, _description.c_str(),
			_name.c_str(), _recommended_action.c_str(), _severity);
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: removing the HcRule from each category global list!", _obj_id.c_str());

	// Before finalizing the HcRule object, remove from the rules per category the current rule
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _rules_set_by_category_mutex);
		for (std::set<category_t>::const_iterator category = _categories.begin(); category != _categories.end(); ++category) {
			_rules_set_by_category[*category].erase(_obj_id);
		}
	TSL_CRITICAL_SECTION_END;

	HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: the HcRule was removed from each category global list!", _obj_id.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	std::string rule_id = _obj_rdn.substr(_obj_rdn.find_first_of('=') + 1);
	HCF_CC_TRACE(FinalizeObject, "Received a request to abort all the modifications to this object: removing the HcRule from each new category global list!", _obj_id.c_str());

	/*
	 * The callback has been aborted, restore the original values.
	 * First, remove from the rules per category the old categories.
	 * Then, restore the original values (adding the old categories to the array).
	 */
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _rules_set_by_category_mutex);
		for (std::set<category_t>::const_iterator category = _categories.begin(); category != _categories.end(); ++category) {
			_rules_set_by_category[*category].erase(rule_id);
		}
	TSL_CRITICAL_SECTION_END;

	HCF_CC_TRACE(FinalizeObject, "Received a request to abort all the modifications to this object: the HcRule was removed from each new category global list!", _obj_id.c_str());
	_adm_state               = _adm_state_old;
	_categories              = _categories_old;
	_rule_default_parameters = _rule_default_parameters_old;
	_description             = _description_old;
	_name                    = _name_old;
	_recommended_action      = _recommended_action_old;
	_severity                = _severity_old;

	HCF_CC_TRACE(FinalizeObject, "Received a request to abort all the modifications to this object: adding the HcRule from each old category global list!", _obj_id.c_str());
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _rules_set_by_category_mutex);
		for (std::set<category_t>::const_iterator category = _categories.begin(); category != _categories.end(); ++category) {
			_rules_set_by_category[*category].insert(rule_id);
		}
	TSL_CRITICAL_SECTION_END;
	HCF_CC_TRACE(FinalizeObject, "Received a request to abort all the modifications to this object: the HcRule was removed from each old category global list!", _obj_id.c_str());

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::create_hcrule (const char * rule_id, std::list<category_t> & categories, severity_t severity, const char * recomm_action, const char * rule_name, const char * description, std::vector<ruleset_file_parser::rule_attribute> & default_attributes, bool error_if_exists) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CreateHcRule, "Received a request to create a new instance of HcRule class: checking if all the categories are valid!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);

	// First, check if the given categories exist
	for (std::list<category_t>::const_iterator it = categories.begin(); it != categories.end(); ++it) {
		if ((*it < CATEGORY_BACKOFFICE) || (*it > CATEGORY_OTHER)) {
			HCF_CC_TRACE(CreateHcRuleErr, "Received an invalid category!", ERR_UNDEFINED_CATEGORY);
			HCF_CC_LOG(LEVEL_ERROR, "The provided category (%d) doesn't exist!", *it);
			return ERR_UNDEFINED_CATEGORY;
		}
	}

	// Get the HCM root pointer to know the DN under which the object must be created
	HCF_CC_TRACE(CreateHcRule, "Received a request to create a new instance of HcRule class: retrieving the root class instance!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);
	imm_object_healthcheckm * hcm_root = healthcheckm_class_handler.get_instance(imm_object_healthcheckm::HCF_HEALTHCHECKM_ID_DEFAULT_VALUE);
	if (!hcm_root) {
		HCF_CC_TRACE(CreateHcRuleErr, "Call 'healthcheckm_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'healthcheckm_class_handler.get_instance' failed!");
		return ERR_NULL_INSTANCE_RETRIEVED;
	}
	const char * parent_dn = hcm_root->get_dn().c_str();
	std::vector<TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes> attr_list;
	HCF_CC_TRACE(CreateHcRule, "Received a request to create a new instance of HcRule class: preparing all the needed data structures!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);

	// 1) prepare the ID attribute structure
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes id_attr;
	id_attr.attrName = const_cast<char *>(HCF_HCRULE_RULE_ID_ATTR_NAME);
	id_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	id_attr.attrValuesNum = 1;
	char rule_rdn[2048];
	::snprintf(rule_rdn, TSL_ARRAY_SIZE(rule_rdn), "%s=%s", HCF_HCRULE_RULE_ID_ATTR_NAME, rule_id);
	void * tmp_id[1] = { const_cast<char *>(rule_rdn) };
	id_attr.attrValues = tmp_id;
	attr_list.push_back(id_attr);

	// 2) prepare the severity attribute structure
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes severity_attr;
	severity_attr.attrName = const_cast<char *>(HCF_HCRULE_SEVERITY_ATTR_NAME);
	severity_attr.attrType = TSL_NS_HCF_IMM::ATTR_INT32T;
	severity_attr.attrValuesNum = 1;
	void * tmp_sev[1] = { &severity };
	severity_attr.attrValues = tmp_sev;
	attr_list.push_back(severity_attr);

	// 3) prepare the recommended action attribute structure
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes action_attr;
	action_attr.attrName = const_cast<char *>(HCF_HCRULE_RECOMMENDED_ACTION_ATTR_NAME);
	action_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	action_attr.attrValuesNum = 1;
	void * tmp_action[1] = { const_cast<char *>(recomm_action) };
	action_attr.attrValues = tmp_action;
	attr_list.push_back(action_attr);

	// 4) prepare the name attribute structure
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes name_attr;
	name_attr.attrName = const_cast<char *>(HCF_HCRULE_NAME_ATTR_NAME);
	name_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	name_attr.attrValuesNum = 1;
	void * tmp_name[1] = { const_cast<char *>(rule_name) };
	name_attr.attrValues = tmp_name;
	attr_list.push_back(name_attr);

	// 5) prepare the description attribute structure
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes descr_attr;
	descr_attr.attrName = const_cast<char *>(HCF_HCRULE_DESCRIPTION_ATTR_NAME);
	descr_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	descr_attr.attrValuesNum = 1;
	void * tmp_desc[1] = { const_cast<char *>(description) };
	descr_attr.attrValues = tmp_desc;
	attr_list.push_back(descr_attr);

	// 6) prepare the description attribute structure
	int i = 0;
	const size_t num_of_categories = categories.size();
	void * tmp_categories[num_of_categories];

	for (std::list<category_t>::iterator it = categories.begin(); it != categories.end(); ++it)
		tmp_categories[i++] = &(*it);

	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes categories_attr;
	categories_attr.attrName = const_cast<char *>(HCF_HCRULE_CATEGORIES_ATTR_NAME);
	categories_attr.attrType = TSL_NS_HCF_IMM::ATTR_INT32T;
	categories_attr.attrValuesNum = num_of_categories;
	categories_attr.attrValues = tmp_categories;
	attr_list.push_back(categories_attr);

	// Create the HcRule object under HcM root MOC
	HCF_CC_TRACE(CreateHcRule, "Received a request to create a new instance of HcRule class: creating the object instance!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);
	int call_result = 0;
	if ((call_result = create_object(HCF_HCRULE_CLASS_NAME, parent_dn, attr_list))) {
		if ((!error_if_exists) && (call_result == TSL_NS_HCF_IMM::ERR_OBJECT_ALREADY_EXISTS)) {
			HCF_CC_TRACE(CreateHcRule, "Call 'create_object' returned already exists, nothing to do!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);
			HCF_CC_LOG(LEVEL_NOTICE, "The object having RDN == '%s' already exist, nothing to do!", rule_rdn);
		}
		else {
			HCF_CC_TRACE(CreateHcRuleErr, "Call 'create_object' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'create_object' failed for class %s under object having "
					"dn == '%s', call_result == %d", HCF_HCRULE_CLASS_NAME, parent_dn, call_result);
			return call_result;
		}
	}

	// In case of not customizable rule (with no input parameters) no other operations are needed
	const size_t number_of_rule_attributes = default_attributes.size();
	if (number_of_rule_attributes == 0) {
		HCF_CC_TRACE(CreateHcRule, "The created HcRule is a simple rule: no other operations are needed!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);
		HCF_CC_LOG(LEVEL_NOTICE, "The rule is a NOT CUSTOMIZABLE rule, no other operations are needed!");
		return ERR_NO_ERROR;
	}

	/*
	 * In case of customizable rule (with at least one input parameter), create the
	 * HcRuleInputParameter objects under the HcRule object previously created.
	 */
	HCF_CC_TRACE(CreateHcRule, "The created HcRule is a customizable rule: retrieving the HcRule instance!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);
	imm_object_hcrule * hcrule_ptr = hcrule_class_handler.get_instance(rule_id);
	if (!hcrule_ptr) {
		HCF_CC_TRACE(CreateHcRuleErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_class_handler.get_instance' failed with ID == '%s'!", rule_id);
		return ERR_NULL_INSTANCE_RETRIEVED;
	}

	int attribute_number = 0;
	char tmp_dns[number_of_rule_attributes][2048];
	char * attributes_dns[number_of_rule_attributes];

	for (std::vector<ruleset_file_parser::rule_attribute>::const_iterator it = default_attributes.begin(); it != default_attributes.end(); ++it) {
		// Create RDN value for the HcRuleInputParameter object to be created and create it
		char parameter_rdn[2048];
		::snprintf(parameter_rdn, TSL_ARRAY_SIZE(parameter_rdn), imm_object_hcrule_inputparameter::HCF_HCRULE_INPUTPARAMETER_DEFAULT_ATTR_ID_FORMAT, rule_id, attribute_number);

		HCF_CC_TRACE(CreateHcRule, "The created HcRule is a customizable rule: creating the HcRuleInputParameter object!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);
		if ((call_result = imm_object_hcrule_inputparameter::create_hcrule_input_parameter(parameter_rdn, hcrule_ptr->get_dn().c_str(), it->description.c_str(), it->name.c_str(), it->value.c_str(), error_if_exists))) {
			HCF_CC_TRACE(CreateHcRuleErr, "Call 'create_hcrule_input_parameter' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'create_hcrule_input_parameter' failed under object having DN == '%s', call_result == %d", parent_dn, call_result);

			// In case of error the HcRule object is not complete, so delete it
			HCF_CC_TRACE(CreateHcRuleErr, "Call 'create_hcrule_input_parameter' was failed, removing also the HcRule object!", call_result);
			hcrule_ptr->delete_object();
			return call_result;
		}
		HCF_CC_TRACE(CreateHcRule, "The created HcRule is a customizable rule: storing the HcRuleInputParameter object into the internal data structures!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);

		// Store the DN of the created object into a proper array
		::snprintf(tmp_dns[attribute_number], TSL_ARRAY_SIZE(tmp_dns[attribute_number]), "%s,%s", parameter_rdn, hcrule_ptr->get_dn().c_str());
		attributes_dns[attribute_number] = tmp_dns[attribute_number];

		// Store the type of rule attribute
		std::string rule_input_parameter_id = get_obj_id_from_dn(attributes_dns[attribute_number]);
		imm_object_hcrule_inputparameter * rule_input_parameter_ptr = hcrule_input_parameter_class_handler.get_instance(rule_input_parameter_id);
		rule_input_parameter_ptr->set_type(it->type);

		++attribute_number;
	}

	// After creating the HcRuleInputParameter objects modify the HcRule object adding the references to them
	HCF_CC_TRACE(CreateHcRule, "The created HcRule is a customizable rule: updating the HcRule object!", rule_id, categories.size(), severity, recomm_action, rule_name, description, error_if_exists);
	if ((call_result = hcrule_ptr->modify_object_attribute(HCF_HCRULE_INPUT_PARAMS_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_NAMET, number_of_rule_attributes, reinterpret_cast<void **>(attributes_dns)))) {
		HCF_CC_TRACE(CreateHcRuleErr, "Call 'hcrule_ptr->modify_object_attribute' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'modify_object_attribute' failed for object having dn == '%s', call_result == %d", hcrule_ptr->get_dn().c_str(), call_result);

		// In case of error the HcRule object is not complete, so delete it
		HCF_CC_TRACE(CreateHcRuleErr, "Call 'hcrule_ptr->modify_object_attribute' was failed, removing also the HcRule object!", call_result);
		hcrule_ptr->delete_object();
		return call_result;
	}
	return ERR_NO_ERROR;
}

std::set<std::string> __TSL_CLASS_NAME__::get_rules_by_categories (std::set<category_t> & categories) {
	HCF_CC_TRACE_FUNCTION;
	std::set<std::string> rules;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _rules_set_by_category_mutex);
		for (std::set<category_t>::const_iterator category = categories.begin(); category != categories.end(); ++category) {
			rules.insert(_rules_set_by_category[*category].begin(), _rules_set_by_category[*category].end());
		}
	TSL_CRITICAL_SECTION_END;
	return rules;
}

int __TSL_CLASS_NAME__::add_rule_attribute (const std::string & job, const std::string & name, const std::string & value) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(AddRuleAttribute, "Received a request to add a new rule attribute!", job.c_str(), name.c_str(), value.c_str());

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _rule_parameters_mutex);
		// FIRST: Check if the pair <name, value> exists in the default attributes map
		if (_rule_default_parameters.find(name) == _rule_default_parameters.end()) {
			HCF_CC_TRACE(AddRuleAttributeErr, "The provided rule attribute doesn't exist!", ERR_UNDEFINED_RULE_ATTRIBUTE);
			HCF_CC_LOG(LEVEL_ERROR, "The attribute having name == '%s' is not present into the default attributes of the current rule!", name.c_str());
			 return ERR_UNDEFINED_RULE_ATTRIBUTE;
		}

		// SECOND: Get the HcRule parameters map related to the provided job
		std::map<std::string, std::string> & parameters_per_job = _rule_input_parameters[job];

		// THIRD: Check if the attribute having the provided name has been previously added
		HCF_CC_TRACE(AddRuleAttribute, "Checking if the same attribute is already associated to the current job!", job.c_str(), name.c_str(), value.c_str());
		if (parameters_per_job.find(name) != parameters_per_job.end()) {
			HCF_CC_TRACE(AddRuleAttributeErr, "The provided rule attribute was already associated to the current job!", ERR_DUPLICATED_RULE_ATTRIBUTE);
			HCF_CC_LOG(LEVEL_ERROR, "The parameters_per_job map already contains an element with name == '%s'", name.c_str());
			 return ERR_DUPLICATED_RULE_ATTRIBUTE;
		}

		// After each check, add the pair <name, value> to the internal structures
		parameters_per_job[name] = value;
	TSL_CRITICAL_SECTION_END;

	HCF_CC_TRACE(AddRuleAttribute, "The rule attribute was correctly added!", job.c_str(), name.c_str(), value.c_str());
	HCF_CC_LOG(LEVEL_NOTICE, "Attribute <%s, %s> (for job == '%s') added for the HcRule having id == '%s'",
  		name.c_str(), value.c_str(), job.c_str(), _obj_id.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::modify_rule_attribute (const std::string & job, const std::string & name, const std::string & value) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ModifyRuleAttribute, "Received a request to modify a rule attribute!", job.c_str(), name.c_str(), value.c_str());
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _rule_parameters_mutex);
		// FIRST: Get the HcRule parameters map related to the provided job
		std::map<std::string, std::string> & parameters_per_job = _rule_input_parameters[job];

		// SECOND: Modify the provided attribute
		parameters_per_job[name] = value;
  TSL_CRITICAL_SECTION_END;

	HCF_CC_TRACE(ModifyRuleAttribute, "The rule attribute was correctly modified!", job.c_str(), name.c_str(), value.c_str());
  HCF_CC_LOG(LEVEL_NOTICE, "Attribute <%s, %s> (for job == '%s') modified for the HcRule having id == '%s'",
  		name.c_str(), value.c_str(), job.c_str(), _obj_id.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::remove_rule_attribute (const std::string & job, const std::string & name) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(RemoveRuleAttribute, "Received a request to remove a rule attribute!", job.c_str(), name.c_str());
  size_t elements_erased = 0;

  TSL_CRITICAL_SECTION_BEGIN(sync_t, _rule_parameters_mutex);
		// FIRST: Get the HcRule parameters map related to the provided job
		std::map<std::string, std::string> & parameters_per_job = _rule_input_parameters[job];

		// SECOND: Erase the provided attribute
		elements_erased = parameters_per_job.erase(name);
  TSL_CRITICAL_SECTION_END;

 	HCF_CC_TRACE(RemoveRuleAttribute, ((elements_erased > 0) ? "The rule attribute was correctly removed!" : "The rule attribute wasn't removed!"), job.c_str(), name.c_str());
  HCF_CC_LOG(LEVEL_NOTICE, "Attribute having name == '%s' %sREMOVED (for job == '%s') for the HcRule having id == '%s'!",
  		name.c_str(), ((elements_erased > 0) ? "" : "NOT "), job.c_str(), _obj_id.c_str());
  return (elements_erased) ? ERR_NO_ERROR : ERR_UNDEFINED_RULE_ATTRIBUTE;
}

std::map<std::string, std::pair<std::string, std::string>> __TSL_CLASS_NAME__::get_rule_attributes (const std::string & job) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(GetRuleAttributes, "Received a request to retrieve all the HcRule parameters for the following job:", job.c_str(), "");
	std::map<std::string, std::pair<std::string, std::string>> rule_attributes;

  /*
   * Create a vector of pairs where the first item is the attribute name, and the second is the value.
   * If for an attribute a user-defined value has been provided, use it.
   * Otherwise, use the default one.
   */
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _rule_parameters_mutex);
		// FIRST: Get the HcRule parameters map related to the provided job
		std::map<std::string, std::string> & parameters_per_job = _rule_input_parameters[job];

		// SECOND: Create the rule parameters vector
		for (std::map<std::string, std::pair<std::string, std::string>>::const_iterator item = _rule_default_parameters.begin(); item != _rule_default_parameters.end(); ++item) {

			// Check if the rule attribute has been customized, otherwise the default one is to be considered
			std::map<std::string, std::string>::const_iterator user_defined_attr = parameters_per_job.find(item->first);

			std::pair<std::string, std::pair<std::string, std::string>> attribute;
			attribute.first = item->first;
			attribute.second.first = item->second.first;
			attribute.second.second = (user_defined_attr != parameters_per_job.end()) ? user_defined_attr->second : item->second.second;

			HCF_CC_TRACE(GetRuleAttributes, "The HcRule has associated the following couple <name, value>:", attribute.second.first.c_str(), attribute.second.second.c_str());
			rule_attributes.insert(attribute);
		}
  TSL_CRITICAL_SECTION_END;
	return rule_attributes;
}

int __TSL_CLASS_NAME__::calculate_targets () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CalculateTargets, "Received a request to calculate all the targets associated to the following HcRule:", _obj_id.c_str());

	// First, get from the configuration handler the list of the configured nodes and delete any previously loaded configuration
	int call_result = 0;
	std::set<std::string> sc_set;
	HCF_CC_TRACE(CalculateTargets, "Retrieving the list of all the SC nodes in the cluster.", "");
	if ((call_result = configurator::get_system_controllers_set(sc_set))) {
		HCF_CC_TRACE(CalculateTargetsErr, "Call 'configurator::get_system_controllers_set' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'configurator::get_system_controllers_set' failed, error code == %d", call_result);
		return call_result;
	}

	std::set<std::string> pl_set;
	HCF_CC_TRACE(CalculateTargets, "Retrieving the list of all the PL nodes in the cluster.", "");
	if ((call_result = configurator::get_payloads_set(pl_set))) {
		HCF_CC_TRACE(CalculateTargetsErr, "Call 'configurator::get_payloads_set' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'configurator::get_payloads_set' failed, error code == %d", call_result);
		return call_result;
	}
	_targets_set.clear();

	/*
	 * Then, add to the current rule targets the list of the targets to be included.
	 * For each provided value, check if it represents a precise target (i.e. SC-1)
	 * or if it represents a group of targets (i.e. PL-ALL).
	 * In this second case, transform the group into the list of single targets.
	 */
	HCF_CC_TRACE(CalculateTargets, "Retrieving the list of the target to be included.", "");
	for (std::list<std::string>::const_iterator it = _targets_to_include.begin(); it != _targets_to_include.end(); ++it) {
		const std::string & target = *it;
		HCF_CC_TRACE(CalculateTargets, "Translating the following target:", target.c_str());

		if (!target.compare(configurator::HCF_ALL_NODES_TARGET_VALUE)) {
			// The target is ALL: add to the rule targets the SC and PL nodes
			_targets_set.insert(sc_set.begin(), sc_set.end());
			_targets_set.insert(pl_set.begin(), pl_set.end());
		}
		else if (!target.compare(configurator::HCF_ALL_CONTROLLER_NODES_TARGET_VALUE)) {
			// The target is SC-ALL: add to the rule targets the SC nodes
			_targets_set.insert(sc_set.begin(), sc_set.end());
		}
		else if (!target.compare(configurator::HCF_ALL_PAYLOAD_NODES_TARGET_VALUE)) {
			// The target is PL-ALL: add to the rule targets the PL nodes
			_targets_set.insert(pl_set.begin(), pl_set.end());
		}
		else if (target.find(configurator::HCF_SYSTEM_CONTROLLER_NODE_PREFIX) != std::string::npos) {
			// The target is SC-xx, check if it exists and add it to the rule targets set
			if (sc_set.find(target) != sc_set.end())
				_targets_set.insert(target);
			else {
				HCF_CC_TRACE(CalculateTargetsErr, "Undefined target provided, skipping it!", 0);
				HCF_CC_LOG(LEVEL_WARNING, "The target '%s' wasn't found into system configuration, skipping it!", target.c_str());
			}
		}
		else if (target.find(configurator::HCF_PAYLOAD_NODE_PREFIX) != std::string::npos) {
			// The target is PL-xx, check if it exists and add it to the rule targets set
			if (pl_set.find(target) != pl_set.end())
				_targets_set.insert(target);
			else {
				HCF_CC_TRACE(CalculateTargetsErr, "Undefined target provided, skipping it!", 0);
				HCF_CC_LOG(LEVEL_WARNING, "The target '%s' wasn't found into system configuration, skipping it!", target.c_str());
			}
		}
		else {
			HCF_CC_TRACE(CalculateTargetsErr, "Undefined target provided, skipping it!", 0);
			HCF_CC_LOG(LEVEL_WARNING, "The target '%s' is undefined, skipping it!", target.c_str());
		}
	}

	/*
	 * Then, from the rule targets list, remove the list of the targets to be excluded.
	 * For each provided value, check if it represents a precise target (i.e. SC-1)
	 * or if it represents a group of targets (i.e. PL-ALL).
	 * In this second case, transform the group into the list of single targets.
	 */
	HCF_CC_TRACE(CalculateTargets, "Retrieving the list of the target to be excluded.", "");
	for (std::list<std::string>::const_iterator it = _targets_to_exclude.begin(); it != _targets_to_exclude.end(); ++it) {
		const std::string & target = *it;
		HCF_CC_TRACE(CalculateTargets, "Translating the following target:", target.c_str());

		if (!target.compare(configurator::HCF_ALL_NODES_TARGET_VALUE)) {
			// The target is ALL: remove each inserted tag
			_targets_set.clear();
		}
		else if (!target.compare(configurator::HCF_ALL_CONTROLLER_NODES_TARGET_VALUE)) {
			// The target is SC-ALL: remove from the rule targets the SC nodes
			for (std::set<std::string>::const_iterator node = sc_set.begin(); node != sc_set.end(); ++node) {
				_targets_set.erase(*node);
			}
		}
		else if (!target.compare(configurator::HCF_ALL_PAYLOAD_NODES_TARGET_VALUE)) {
			// The target is PL-ALL: remove from the rule targets the PL nodes
			for (std::set<std::string>::const_iterator node = pl_set.begin(); node != pl_set.end(); ++node) {
				_targets_set.erase(*node);
			}
		}
		else if (target.find(configurator::HCF_SYSTEM_CONTROLLER_NODE_PREFIX) != std::string::npos) {
			// The target is SC-xx, check if it exists and remove it from the rule targets set
			if (sc_set.find(target) != sc_set.end())
				_targets_set.erase(target);
			else {
				HCF_CC_TRACE(CalculateTargetsErr, "Undefined target provided, skipping it!", 0);
				HCF_CC_LOG(LEVEL_WARNING, "The target '%s' wasn't found into system configuration, skipping it!", target.c_str());
			}
		}
		else if (target.find(configurator::HCF_PAYLOAD_NODE_PREFIX) != std::string::npos) {
			// The target is PL-xx, check if it exists and remove it from the rule targets set
			if (pl_set.find(target) != pl_set.end())
				_targets_set.erase(target);
			else {
				HCF_CC_TRACE(CalculateTargetsErr, "Undefined target provided, skipping it!", 0);
				HCF_CC_LOG(LEVEL_WARNING, "The target '%s' wasn't found into system configuration, skipping it!", target.c_str());
			}
		}
		else {
			HCF_CC_TRACE(CalculateTargetsErr, "Undefined target provided, skipping it!", 0);
			HCF_CC_LOG(LEVEL_WARNING, "The target '%s' is undefined, skipping it!", target.c_str());
		}
	}
	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
