#include "hcf/core/logger.h"
#include "hcf/imm/imm_config_class.h"

#include "imm_object_hcjob.h"
#include "imm_object_hcrule.h"
#include "imm_object_hcrule_parameter.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE_PARAMETER__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule> hcrule_class_handler;

HCF_NS_HCF_SRV_BEGIN

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_HCRULEPARAMETER_CLASS_NAME         = "HCMHcRuleParameter";
const char * const __TSL_CLASS_NAME__::HCF_HCRULEPARAMETER_IMPLEMENTER_NAME   = "HcRuleParameter_Implementer";
const char * const __TSL_CLASS_NAME__::HCF_HCRULEPARAMETER_ID_ATTR_NAME       = "id";
const char * const __TSL_CLASS_NAME__::HCF_HCRULEPARAMETER_HCRULE_ATTR_NAME   = "hcRule";
const char * const __TSL_CLASS_NAME__::HCF_HCRULEPARAMETER_NAME_ATTR_NAME     = "name";
const char * const __TSL_CLASS_NAME__::HCF_HCRULEPARAMETER_VALUE_ATTR_NAME    = "value";
const char * const __TSL_CLASS_NAME__::HCF_HCRULEPARAMETER_HCRULE_RDN_PREFIX  = "HcRule=";
const char * const __TSL_CLASS_NAME__::HCF_HCRULEPARAMETER_ID_SEPARATOR_VALUE = "##";

int __TSL_CLASS_NAME__::set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;

	/*
	 * Before doing any create/modify operation on the rule input parameter,
	 * check if the job which the parameter belongs to is currently running.
	 * In that case, return a proper error message.
	 */
	HCF_CC_TRACE(SetObjectAttributes, "Checking if the job which the parameter belongs to is running!", "");
	std::string running_job_dn;
	if (!(running_job_dn = imm_object_hcjob::get_running_job_dn()).empty()) {
		HCF_CC_TRACE(SetObjectAttributes, "There is some job running, checking if it's the job which this parameter belongs to!", "");
		const std::string running_job_id = imm_object_hcjob::get_obj_id_from_dn(running_job_dn.c_str());
		if (running_job_id == _job) {
			HCF_CC_LOG(LEVEL_ERROR, "Cannot execute the create/modify operation, since the job with DN == '%s' (and ID == '%s') is currently running!", running_job_dn.c_str(), _job.c_str());
			HCF_CC_TRACE(SetObjectAttributesErr, "Cannot execute the create/modify operation, since the job is running!", ERR_OPERATION_NOT_PERMITTED);
			set_error_message(HCF_INPUTPARAMETER_OP_NOT_PERMITTED_ERROR_MSG);
			return ERR_OPERATION_NOT_PERMITTED;
		}
	}

	// Save the old values before modifying them, in order to rollback in case of errors
	_hcrule_id_old = _hcrule_id;
	_name_old      = _name;
	_value_old     = _value;

	std::string new_name;
	std::string new_value;
	std::string new_hcrule_id;

	// For each of the provided attributes, understand the attribute name and set the class field accordingly
	for (size_t i = 0; i < attributes.size(); ++i) {

		// Extract from the given attributes vector the attribute name and its value
		std::string attr_name = attributes[i].first;
		std::vector<void *> attr_values = attributes[i].second;
		HCF_CC_TRACE(SetObjectAttributes, "Received a request to set/change the value of the following attribute:", attr_name.c_str());

		if (!attr_name.compare(HCF_HCRULEPARAMETER_ID_ATTR_NAME)) {
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
		else if (!attr_name.compare(HCF_HCRULEPARAMETER_HCRULE_ATTR_NAME)) {
			if (attr_values.size() > 0)	{
				// Parse the HcRule RDN and extract the HcRule ID
				std::string hcrule_rdn = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "Checking if the provided value for the hcRule attribute is valid.", hcrule_rdn.c_str());

				// FIRST: Check if the value is correctly formed
				if (hcrule_rdn.find(HCF_HCRULEPARAMETER_HCRULE_RDN_PREFIX) == std::string::npos) {
					HCF_CC_LOG(LEVEL_ERROR, "The HcRule RDN (%s) is not correctly formed!", hcrule_rdn.c_str());
					set_error_message(HCF_INVALID_RULE_RDN_ERROR_MSG);
					return ERR_NULL_INSTANCE_RETRIEVED;
				}

				// SECOND: Extract the HcRule ID
				new_hcrule_id = hcrule_rdn.substr(hcrule_rdn.find_first_of('=') + 1);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the hcRule attribute is:", new_hcrule_id.c_str());
			}
			else {	// ERROR: No value provided!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HCRULEPARAMETER_NAME_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				new_name = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the name attribute is:", new_name.c_str());
			}
			else {	// ERROR: No value provided!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HCRULEPARAMETER_VALUE_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				new_value = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the value attribute is:", new_value.c_str());
			}
			else {	// ERROR: No value provided!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
	}

	/*
	 * Apply the creation/change to the attribute to the internal
	 * data structures, according to the following algorithm:
	 * 1. If the HcRule RDN is changed (and if the old RDN is not empty),
	 *    the attribute is removed from the old rule and added to the new one.
	 * 2. Otherwise, the name value is considered:
	 * 		2.1. If its value isn't modified, only a modification if performed.
	 * 		2.2. Otherwise, the old couple <name1,value> is removed (only if the old
	 *         attribute name is not empty) and the new couple <name2, value> is added.
	 */
  int call_result = 0;
	if (!(_hcrule_id.empty()) && (_hcrule_id.compare(new_hcrule_id))) {		// RDN changed and old one is not empty
		// Retrieve the old HcRule RDN and delete the rule attribute
		HCF_CC_TRACE(SetObjectAttributes, "The value of the HcRule RDN was changed and the old one is not empty: retrieving the old rule!", _hcrule_id.c_str());
		imm_object_hcrule * old_rule = hcrule_class_handler.get_instance(_hcrule_id);
		if (!old_rule) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "The HcRule object having id == '%s' doesn't exist!", _hcrule_id.c_str());
			set_error_message(HCF_UNDEFINED_RULE_RDN_ERROR_MSG);
	  	return ERR_NULL_INSTANCE_RETRIEVED;
		}

		HCF_CC_TRACE(SetObjectAttributes, "The value of the HcRule RDN was changed and the old one is not empty: removing this parameter from the old rule!", new_hcrule_id.c_str());
  	if ((call_result = old_rule->remove_rule_attribute(_job, _name))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'old_rule->remove_rule_attribute' failed.", call_result);
  		HCF_CC_LOG(LEVEL_ERROR, "Call 'remove_rule_attribute' failed for the triple <%s, %s, %s>, call_result == %d",
  				_hcrule_id.c_str(), _name.c_str(), _value.c_str(), call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
  		return call_result;
  	}

  	// Retrieve the new HcRule RDN and add the rule attribute
		HCF_CC_TRACE(SetObjectAttributes, "The value of the HcRule RDN was changed and the old one is not empty: retrieving the new rule!", new_hcrule_id.c_str());
		imm_object_hcrule * new_rule = hcrule_class_handler.get_instance(new_hcrule_id);
		if (!new_rule) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "The HcRule object having id == '%s' doesn't exist!", new_hcrule_id.c_str());
			set_error_message(HCF_UNDEFINED_RULE_RDN_ERROR_MSG);
	  	return ERR_NULL_INSTANCE_RETRIEVED;
		}

		HCF_CC_TRACE(SetObjectAttributes, "The value of the HcRule RDN was changed and the old one is not empty: adding this parameter to the new rule!", new_hcrule_id.c_str());
		if ((call_result = new_rule->add_rule_attribute(_job, new_name, new_value))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'new_rule->add_rule_attribute' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'add_rule_attribute' failed for the triple <%s, %s, %s>, call_result == %d",
					new_hcrule_id.c_str(), new_name.c_str(), new_value.c_str(), call_result);

			// Provide the correct error message to the caller according to the return code
			const char * error_msg;
			if (call_result == ERR_UNDEFINED_RULE_ATTRIBUTE)
				error_msg = HCF_UNDEFINED_RULE_ATTRIBUTE_ERROR_MSG;
			else if (call_result == ERR_DUPLICATED_RULE_ATTRIBUTE)
				error_msg = HCF_DUPLICATED_RULE_ATTRIBUTE_ERROR_MSG;
			else
				error_msg = HCF_GENERIC_ERROR_ERROR_MSG;

			set_error_message(error_msg);
			return call_result;
		}
	}
	else {		// RDN not changed or old one is empty
		// Retrieve the HcRule RDN to execute the correct operation
		HCF_CC_TRACE(SetObjectAttributes, "The value of the HcRule RDN wasn't changed, so the current rule must be modified!", new_hcrule_id.c_str());
		imm_object_hcrule * rule = hcrule_class_handler.get_instance(new_hcrule_id);
		if (!rule) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "The HcRule object having id == '%s' doesn't exist!", new_hcrule_id.c_str());
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return ERR_NULL_INSTANCE_RETRIEVED;
		}

		// Check the action to be executed according to the attribute name value
		if (!_name.compare(new_name)) {
			// Attribute name isn't changed: modify the attribute value
			HCF_CC_TRACE(SetObjectAttributes, "The attribute name was not changed, modifying the attribute value!", new_value.c_str());
			if ((call_result = rule->modify_rule_attribute(_job, new_name, new_value))) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call 'rule->modify_rule_attribute' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'modify_rule_attribute' failed for the triple <%s, %s, %s>, call_result == %d",
						new_hcrule_id.c_str(), new_name.c_str(), new_value.c_str(), call_result);

				// Provide the correct error message to the caller according to the return code
				const char * error_msg;
				if (call_result == ERR_UNDEFINED_RULE_ATTRIBUTE)
					error_msg = HCF_UNDEFINED_RULE_ATTRIBUTE_ERROR_MSG;
				else if (call_result == ERR_DUPLICATED_RULE_ATTRIBUTE)
					error_msg = HCF_DUPLICATED_RULE_ATTRIBUTE_ERROR_MSG;
				else
					error_msg = HCF_GENERIC_ERROR_ERROR_MSG;

				set_error_message(error_msg);
				return call_result;
			}
		}
		else {
			/*
			 * Attribute name is changed: remove the old couple <name1, value> and add
			 * the new couple <name2, value>, only if the attribute name is not empty.
			 */
			HCF_CC_TRACE(SetObjectAttributes, "The attribute name was changed, removing the old attribute!", _name.c_str());
			if (!(_name.empty()) && (call_result = rule->remove_rule_attribute(_job, _name))) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call 'rule->remove_rule_attribute' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'remove_rule_attribute' failed for the triple <%s, %s, %s>, call_result == %d",
						new_hcrule_id.c_str(), _name.c_str(), _value.c_str(), call_result);
				set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
				return call_result;
			}

			HCF_CC_TRACE(SetObjectAttributes, "The attribute name was changed, adding the new attribute!", new_name.c_str());
			if ((call_result = rule->add_rule_attribute(_job, new_name, new_value))) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call 'rule->add_rule_attribute' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'add_rule_attribute' failed for the triple <%s, %s, %s>, call_result == %d",
						new_hcrule_id.c_str(), new_name.c_str(), new_value.c_str(), call_result);

				// Provide the correct error message to the caller according to the return code
				const char * error_msg;
				if (call_result == ERR_UNDEFINED_RULE_ATTRIBUTE)
					error_msg = HCF_UNDEFINED_RULE_ATTRIBUTE_ERROR_MSG;
				else if (call_result == ERR_DUPLICATED_RULE_ATTRIBUTE)
					error_msg = HCF_DUPLICATED_RULE_ATTRIBUTE_ERROR_MSG;
				else
					error_msg = HCF_GENERIC_ERROR_ERROR_MSG;

				set_error_message(error_msg);
				return call_result;
			}
		}
	}
	HCF_CC_TRACE(SetObjectAttributes, "The internal data structures were correctly updated, storing the new values!", "");

	// After a successful update of the internal data structures, store the new values into the class members
	_name = new_name;
  _value = new_value;
  _hcrule_id = new_hcrule_id;

  HCF_CC_LOG(LEVEL_NOTICE, "Object attributes correctly modified: RDN == '%s', _hcrule_id == '%s', _name == '%s',"
			" _value == '%s'", _obj_rdn.c_str(), _hcrule_id.c_str(), _name.c_str(), _value.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::set_object_attributes_and_dn (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, const char * parent_dn, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	 // FIRST: Calculate the ID of the father (HcRule) object
	_job = imm_object_hcjob::get_obj_id_from_dn(parent_dn);
	HCF_CC_TRACE(SetObjectAttributes, "The father HcJob object has the following ID:", _job.c_str());

	// SECOND: Call the same method in the base class
	HCF_CC_TRACE(SetObjectAttributes, "Setting the object attributes and its DN.", "");
	if (const int call_result = TSL_NS_HCF_IMM::imm_object::set_object_attributes_and_dn(attributes, parent_dn, callback_info)) {
		HCF_CC_TRACE(SetObjectAttributesErr, "Call 'imm_object::set_object_attributes_and_dn' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'set_object_attributes_and_dn' failed, return_code == %d", call_result);
		return call_result;
	}

	// THIRD: Calculate the correct value for the ID field (that is different from the other classes)
	_obj_id = get_obj_id_from_dn(_obj_dn.c_str());
	HCF_CC_TRACE(SetObjectAttributes, "The ID of this object is:", _obj_id.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	/*
	 * Before executing the finalization operation of the rule input parameter,
	 * check if the job which the parameter belongs to is currently running.
	 * In that case, return a proper error message.
	 */
	HCF_CC_TRACE(FinalizeObject, "Checking if the job which the parameter belongs to is running!", _obj_id.c_str(), _job.c_str());
	std::string running_job_dn;
	if (!(running_job_dn = imm_object_hcjob::get_running_job_dn()).empty()) {
		HCF_CC_TRACE(FinalizeObject, "There is some job running, checking if it's the job which this parameter belongs to!", _obj_id.c_str(), running_job_dn.c_str());
		const std::string running_job_id = imm_object_hcjob::get_obj_id_from_dn(running_job_dn.c_str());
		if (running_job_id == _job) {
			HCF_CC_LOG(LEVEL_ERROR, "Cannot execute the finalization operation, since the job with DN == '%s' (and ID == '%s') is currently running!", running_job_dn.c_str(), _job.c_str());
			HCF_CC_TRACE(FinalizeObjectErr, "Cannot execute the finalization operation, since the job is running!", ERR_OPERATION_NOT_PERMITTED);
			set_error_message(HCF_INPUTPARAMETER_OP_NOT_PERMITTED_ERROR_MSG);
			return ERR_OPERATION_NOT_PERMITTED;
		}
	}

	// When this object must be finalized, get the HcRule father instance and notify the removal of the current attribute.
	HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: retrieving its HcRule instance!", _obj_id.c_str(), _hcrule_id.c_str());
  imm_object_hcrule * rule = hcrule_class_handler.get_instance(_hcrule_id);
  if (!rule) {
		HCF_CC_TRACE(FinalizeObjectErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
  	HCF_CC_LOG(LEVEL_WARNING, "The HcRule object having id == '%s' doesn't exist, probably it was deleted!", _hcrule_id.c_str());
  	return ERR_NO_ERROR;
  }

	HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: removing the HcRule attribute!", _obj_id.c_str(), _name.c_str());
  if (const int call_result = rule->remove_rule_attribute(_job, _name)) {
		HCF_CC_TRACE(FinalizeObjectErr, "Call 'rule->remove_rule_attribute' failed.", call_result);
  	HCF_CC_LOG(LEVEL_ERROR, "Call 'remove_rule_attribute' failed for the attribute having name == '%s', call_result == %d", _name.c_str(), call_result);
		set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
		return call_result;
  }

	HCF_CC_TRACE(FinalizeObject, "The HcRule attribute was correctly deleted!", _obj_id.c_str(), _name.c_str());
  HCF_CC_LOG(LEVEL_NOTICE, "The attribute <%s, %s> belonging to the HcRule %s has been correctly deleted!", _name.c_str(), _value.c_str(), _hcrule_id.c_str());
  return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	/*
	 * The callback has been aborted, restore the original values.
	 * First, remove the attribute (_name, _value) from the _hcRule object.
	 * Second, add the attribute (_name_old, _value_old) to the _hcRule object.
	 * Then, restore the original values.
	 */
  int call_result = 0;
  imm_object_hcrule * rule = 0;

	HCF_CC_TRACE(AbortObjectModification, "Received a request to abort all the modifications to this object: retrieving its HcRule instance!", _obj_id.c_str(), _hcrule_id.c_str());
  if ((rule = hcrule_class_handler.get_instance(_hcrule_id))) {
  	HCF_CC_TRACE(AbortObjectModification, "Removing the attribute from the old HcRule", _obj_id.c_str(), _name.c_str());
  	if ((call_result = rule->remove_rule_attribute(_job, _name))) {
    	HCF_CC_TRACE(AbortObjectModificationErr, "Call 'rule->remove_rule_attribute' failed.", call_result);
    	HCF_CC_LOG(LEVEL_ERROR, "Call 'remove_rule_attribute' failed for the attribute having name == '%s', call_result == %d", _name.c_str(), call_result);
  	}
    else
    	HCF_CC_LOG(LEVEL_NOTICE, "The attribute <%s, %s> belonging to the HcRule %s has been correctly deleted!", _name.c_str(), _value.c_str(), _hcrule_id.c_str());
  }
  else {
  	HCF_CC_TRACE(AbortObjectModificationErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
  	HCF_CC_LOG(LEVEL_ERROR, "The HcRule object having id == '%s' doesn't exist!", _hcrule_id.c_str());
  }

	HCF_CC_TRACE(AbortObjectModification, "Received a request to abort all the modifications to this object: retrieving the old HcRule instance!", _obj_id.c_str(), _hcrule_id.c_str());
  if ((rule = hcrule_class_handler.get_instance(_hcrule_id_old))) {
  	HCF_CC_TRACE(AbortObjectModification, "Adding the attribute to the new HcRule", _obj_id.c_str(), _name_old.c_str());
    if ((call_result = rule->add_rule_attribute(_job, _name_old, _value_old))) {
    	HCF_CC_TRACE(AbortObjectModificationErr, "Call 'rule->add_rule_attribute' failed.", call_result);
    	HCF_CC_LOG(LEVEL_ERROR, "Call 'add_rule_attribute' failed for the couple <%s, %s>, call_result == %d", _name_old.c_str(), _value_old.c_str(), call_result);
    }
    else
    	HCF_CC_LOG(LEVEL_NOTICE, "The attribute <%s, %s> belonging to the HcRule %s has been correctly deleted!", _name_old.c_str(), _value_old.c_str(), _hcrule_id_old.c_str());
  }
  else {
  	HCF_CC_TRACE(AbortObjectModificationErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
  	HCF_CC_LOG(LEVEL_ERROR, "The HcRule object having id == '%s' doesn't exist!", _hcrule_id_old.c_str());
  }

	_hcrule_id = _hcrule_id_old;
	_name      = _name_old;
	_value     = _value_old;
  return ERR_NO_ERROR;
}

std::string __TSL_CLASS_NAME__::get_obj_id_from_dn (const char * obj_dn) {
	HCF_CC_TRACE_FUNCTION;
	/*
   * In HcRuleParameter class, the object ID must be calculated in a different way.
   * If the object has the following DN: "id=<ID>,hcJobId=<JOB_ID>,HCMhealthCheckMId=1",
   * then the object ID will be "<ID>##<JOB_ID>".
   */
  std::string dn(obj_dn);
  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcRuleParameter ID starting from the DN: extracting the first part!", obj_dn);

  size_t comma_pos = dn.find_first_of(",");
  size_t equal_pos = dn.find_first_of("=");
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcRuleParameter DN is not correctly formed!", obj_dn);
  	HCF_CC_LOG(LEVEL_WARNING, "The HcRuleParameter DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }
  std::string parameter_id = dn.substr((equal_pos + 1), (comma_pos - equal_pos - 1));
  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcRuleParameter ID starting from the DN: extracting the second part!", obj_dn);

  comma_pos = dn.find_first_of(",", (comma_pos + 1));
  equal_pos = dn.find_first_of("=", (equal_pos + 1));
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcRuleParameter DN is not correctly formed!", obj_dn);
  	HCF_CC_LOG(LEVEL_WARNING, "The HcRuleParameter DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }
  std::string job_id = dn.substr((equal_pos + 1), (comma_pos - equal_pos - 1));
  return parameter_id + HCF_HCRULEPARAMETER_ID_SEPARATOR_VALUE + job_id;
}

HCF_NS_HCF_SRV_END
