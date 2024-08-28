#include <string.h>

#include "hcf/core/logger.h"
#include "hcf/imm/om_handler.h"
#include "hcf/imm/constants.h"
#include "hcf/imm/imm_error_constants.h"
#include "hcf/imm/imm_config_class.h"

#include "imm_object_hcrule.h"
#include "configurator.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_configurationhandler.h"
#include "hcf/trace/tracing_macros.h"

#include "configuration_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CONFIGURATION_HANDLER__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule> hcrule_class_handler;

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_CONFIGURATION_HANDLER_IMPLEMENTER_NAME = "HCF_ConfigImplementer";
const char * const __TSL_CLASS_NAME__::HCF_MANAGED_ELEMENT_CLASS_NAME             = "ManagedElement";
const char * const __TSL_CLASS_NAME__::HCF_AMF_NODE_CLASS_NAME                    = "SaAmfNode";
const char * const __TSL_CLASS_NAME__::HCF_MANAGED_ELEMENT_DEFAULT_DN             = "managedElementId=1";
const char * const __TSL_CLASS_NAME__::HCF_NETWORK_MEID_ATTR_NAME                 = "networkManagedElementId";
const char * const __TSL_CLASS_NAME__::HCF_MANAGED_ELEMENT_ID_DEFAULT_VALUE       = "1";
const char * const __TSL_CLASS_NAME__::HCF_AMF_NODE_DN_PATTERN_VALUE              = "safAmfNode=";
const int __TSL_CLASS_NAME__::HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES            = 3;
std::string __TSL_CLASS_NAME__::_configuration_class_names[]                      = {HCF_MANAGED_ELEMENT_CLASS_NAME, HCF_AMF_NODE_CLASS_NAME};
int __TSL_CLASS_NAME__::_number_of_configuration_classes                          = 2;

int __TSL_CLASS_NAME__::initialize () {
	HCF_CC_TRACE_FUNCTION;

	if (_object_initialized) {
		HCF_CC_TRACE(Initialize, "The Configuration Handler object was already initialized, nothing to do!");
		return ERR_NO_ERROR;
	}

	HCF_CC_TRACE(Initialize, "Loading the value of the ManagedElementId attribute!");
	if (const int call_result = load_managed_element_id_value()) {
		HCF_CC_TRACE(InitializeErr, "Call 'load_managed_element_id_value' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'load_managed_element_id_value' failed, call_result == %d", call_result);
		return call_result;
	}

	HCF_CC_TRACE(Initialize, "Initializing the father Observer class!");
	return TSL_NS_HCF_IMM::imm_observer::initialize();
}

int __TSL_CLASS_NAME__::load_managed_element_id_value () {
	HCF_CC_TRACE_FUNCTION;

	// Initialize the OM Handler object
	HCF_CC_TRACE(LoadManagedElementIdValue, "Initializing the internal OM Handler object!", HCF_NETWORK_MEID_ATTR_NAME);
	TSL_NS_HCF_IMM::om_handler om_handler;
	if (om_handler.Init() != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
		HCF_CC_TRACE(LoadManagedElementIdValueErr, "Call 'om_handler.Init' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.Init()' failed, error_code == %d, error_message == '%s'",
				om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
		return TSL_NS_HCF_IMM::ERR_OM_HANDLER_INIT_FAILED;
	}
	HCF_CC_TRACE(LoadManagedElementIdValue, "Preparing the data structure for the MEId attribute!", HCF_NETWORK_MEID_ATTR_NAME);

	// Prepare the structure to retrieve the Network Managed Element
	std::vector<TSL_NS_HCF_IMM::IMM_API_ImmAttribute *> attributes;
	TSL_NS_HCF_IMM::IMM_API_ImmAttribute network_meid;
	network_meid.attrName = const_cast<char *>(HCF_NETWORK_MEID_ATTR_NAME);
	attributes.push_back(&network_meid);

	// Retrieve the Managed Element ID into ManagedElement MOC
	int imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
	for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
		HCF_CC_TRACE(LoadManagedElementIdValue, "Trying to retrieve the MEId attribute from the following object:", HCF_MANAGED_ELEMENT_DEFAULT_DN);
		if (om_handler.getAttribute(HCF_MANAGED_ELEMENT_DEFAULT_DN, attributes) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
			imm_error_code = om_handler.getInternalLastError();
			HCF_CC_TRACE(LoadManagedElementIdValueErr, "Call 'om_handler.getAttribute' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.getAttribute()' failed (RETRY == %d) for attribute '%s', "
					"error_code == %d, error_message == '%s'", (HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1),
					HCF_NETWORK_MEID_ATTR_NAME, om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());

			// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
			if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
		}
		else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
	}
	om_handler.Finalize();

	if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
		HCF_CC_TRACE(LoadManagedElementIdValueErr, "Failed to retrieve the value of ManagedElementId attribute!", imm_error_code, "");
		return TSL_NS_HCF_IMM::ERR_OM_HANDLER_GET_OBJECT_FAILED;
	}

	/*
	 * Check if a value for the Network Managed Element ID has been provided.
	 * Otherwise, use the default one.
	 */
	_managed_element_id = ((network_meid.attrValuesNum > 0) ? reinterpret_cast<char *>(network_meid.attrValues[0]) : HCF_MANAGED_ELEMENT_ID_DEFAULT_VALUE);
	HCF_CC_TRACE(LoadManagedElementIdValue, "The retrieved value for the ManagedElementId is:", _managed_element_id.c_str());
	HCF_CC_LOG(LEVEL_NOTICE, "The retrieved value for the Managed Element ID is '%s'", _managed_element_id.c_str());
	configurator::set_managed_element_id(_managed_element_id.c_str());
	return ERR_NO_ERROR;
}

void __TSL_CLASS_NAME__::handle_managed_element_class_notification (std::vector<std::pair<std::string, std::vector<void *> > > & attributes) {
	HCF_CC_TRACE_FUNCTION;
	/*
	 * Received a notification about the ManagedElement MOC.
	 * Extract from the attributes vector only the needed information
	 * about the networkManagedElementId attribute.
	 */
	for (size_t i = 0; i < attributes.size(); ++i) {
		// Extract from the given attributes vector the attribute name and its value
		const std::string & attr_name = attributes[i].first;
		const std::vector<void *> & attr_values = attributes[i].second;
		HCF_CC_TRACE(HandleManagedElementClassNotification, "Received a notification for the following attribute:", attr_name.c_str());

		if (!attr_name.compare(HCF_NETWORK_MEID_ATTR_NAME)) {
			_managed_element_id_old = _managed_element_id;
			_managed_element_id = ((attr_values.size() > 0) ? reinterpret_cast<char *>(attr_values[0]) : HCF_MANAGED_ELEMENT_ID_DEFAULT_VALUE);
			HCF_CC_TRACE(HandleManagedElementClassNotification, "The ManagedElementId attribute value was changed to:", _managed_element_id.c_str());
			HCF_CC_LOG(LEVEL_NOTICE, "Received a notification for the '%s' attribute: old_value == '%s', new_value == '%s'",
					attr_name.c_str(), _managed_element_id_old.c_str(), _managed_element_id.c_str());
		}
	}
}

int __TSL_CLASS_NAME__::notify_object_creation (const char * class_name, const char * parent_dn, std::vector<std::pair<std::string, std::vector<void *> > > & attributes) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(NotifyObjectCreation, "Received a notification about the creation of a class instance!", class_name, parent_dn);
	HCF_CC_LOG(LEVEL_NOTICE, "Received a notification about the creation of an instance of '%s'!", class_name);

	if (!::strcmp(class_name, HCF_MANAGED_ELEMENT_CLASS_NAME))
		handle_managed_element_class_notification(attributes);
	else if (!::strcmp(class_name, HCF_AMF_NODE_CLASS_NAME))
		handle_amf_node_class_notification();
	else {
		HCF_CC_TRACE(NotifyObjectCreation, "Unexpected notification received, skipping it!", class_name, parent_dn);
		HCF_CC_LOG(LEVEL_NOTICE, "This notification is regarding an unexpected class (%s), skipping it!", class_name);
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::notify_object_modification (const char * obj_dn, std::vector<std::pair<std::string, std::vector<void *> > > & attributes) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(NotifyObjectModification, "Received a notification about an object modification!", obj_dn);
	HCF_CC_LOG(LEVEL_NOTICE, "Received a notification about the modification of the object with DN == '%s'!", obj_dn);
	std::string dn (obj_dn);

	if (dn.find(HCF_MANAGED_ELEMENT_DEFAULT_DN) != std::string::npos)
		handle_managed_element_class_notification(attributes);
	else if (dn.find(HCF_AMF_NODE_DN_PATTERN_VALUE) != std::string::npos) {	// Nothing to do for now!
		HCF_CC_TRACE(NotifyObjectModification, "The received notification is about a SaAmfNode class instance: nothing to do!", obj_dn);
	}
	else {
		HCF_CC_TRACE(NotifyObjectModification, "Unexpected notification received, skipping it", obj_dn);
		HCF_CC_LOG(LEVEL_NOTICE, "This notification is regarding an unexpected object (DN == '%s'), skipping it!", obj_dn);
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::notify_object_deletion (const char * obj_dn) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(NotifyObjectDeletion, "Received a notification about an object deletion!", obj_dn);
	HCF_CC_LOG(LEVEL_NOTICE, "Received a notification about the deletion of the object with DN == '%s'!", obj_dn);
	std::string dn (obj_dn);

	if (dn.find(HCF_MANAGED_ELEMENT_DEFAULT_DN) != std::string::npos) {	// Nothing to do for now!
		HCF_CC_TRACE(NotifyObjectDeletion, "The received notification is about the ManagedElement class instance: nothing to do!", obj_dn);
	}
	else if (dn.find(HCF_AMF_NODE_DN_PATTERN_VALUE) != std::string::npos)
		handle_amf_node_class_notification();
	else {
		HCF_CC_TRACE(NotifyObjectDeletion, "Unexpected notification received, skipping it", obj_dn);
		HCF_CC_LOG(LEVEL_NOTICE, "This notification is regarding an unexpected object (DN == '%s'), skipping it!", obj_dn);
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::notify_object_modification_abort () {
	HCF_CC_TRACE_FUNCTION;
	// The object creation/modification/deletion has been aborted: restore the original situation!
	_managed_element_id = _managed_element_id_old;
	_amf_nodes_added_or_removed = false;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::notify_object_modification_application () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(NotifyObjectModificationApplication, "Received a request to apply all the performed changes!");
	/*
	 * During this notification, all the changes must be applied.
	 * First, check if the MEID value has been changed.
	 * In that case, notify that change to the configurator class.
	 */
	if (_managed_element_id != _managed_element_id_old) {
		HCF_CC_TRACE(NotifyObjectModificationApplication, "The value of the ManagedElementId attribute was changed, updating the internal structures!");
		HCF_CC_LOG(LEVEL_NOTICE, "ManagedElementID value was changed, updating the configuration!");
		configurator::set_managed_element_id(_managed_element_id.c_str());
		_managed_element_id_old.clear();
	}

	/*
	 * Then, check if a change regarding the nodes belonging to the
	 * cluster has been performed (new nodes have been added or removed).
	 * In that case, first remove the loaded configuration and then
	 * calculate again the targets for each installed health check rule.
	 */
	if (_amf_nodes_added_or_removed) {
		HCF_CC_TRACE(NotifyObjectModificationApplication, "One or more nodes have been added or removed from the cluster, recalculating the cluster configuration!");
		HCF_CC_LOG(LEVEL_NOTICE, "A node has been added or removed from the cluster, calculate again the configuration!");
		configurator::clear_loaded_configuration(configurator::CONFIGURATION_TYPE_NODES);

		std::list<imm_object_hcrule *> rules_list;
		std::list<imm_object_hcrule *>::iterator rule;
		for (rules_list = hcrule_class_handler.get_all_instances(),
				rule = rules_list.begin();
				rule != rules_list.end();
				++rule)	(*rule)->calculate_targets();

		_amf_nodes_added_or_removed = false;
	}

	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
