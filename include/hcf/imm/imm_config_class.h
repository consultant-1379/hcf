#ifndef __INCLUDE_GUARD_CLASS_IMM_IMM_CONFIG_CLASS__
#define __INCLUDE_GUARD_CLASS_IMM_IMM_CONFIG_CLASS__ imm_config_class

/**
 *  @file imm_config_class.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-11-17
 *
 *	COPYRIGHT Ericsson AB, 2014
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+==========+============+==============+=====================================+
 *	| REV      | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+==========+============+==============+=====================================+
 *	| 2014-001 | 2014-11-17 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *	| 2015-002 | 2015-03-06 | xcasale      | Event handling adaptation.          |
 *	+==========+============+==============+=====================================+
 */

#include <map>
#include <list>

#include "hcf/core/thread_mutex.h"
#include "hcf/core/guard.h"
#include "hcf/core/event_handler.h"
#include "hcf/core/logger.h"

#include "imm_macros.h"
#include "object_implementer_interface.h"
#include "object_implementer_handler.h"
#include "om_handler.h"
#include "constants.h"

#include "hcf/trace/com_ericsson_common_hcf_imm_immconfigclass.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_IMM_CONFIG_CLASS__

TSL_NS_HCF_IMM_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

template <typename obj_type>
class __TSL_CLASS_NAME__ : public object_implementer_interface, public TSL_NS_HCF_CORE::event_handler {

	//==============//
	// Enumerations //
	//==============//
private:
	enum operation_type_t {
		OPERATION_CREATE
		, OPERATION_MODIFY
		, OPERATION_DELETE
	};

	//==========//
	// Typedefs //
	//==========//
private:
	typedef TSL_NS_HCF_CORE::thread_mutex_recursive sync_t;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * class_name, const char * impl_name)
		TSL_CTOR_INIT_LIST(object_implementer_interface(class_name, impl_name, SCOPE_ONE), TSL_NS_HCF_CORE::event_handler(),
				_object_to_be_finalized(0), _class_name(class_name), _impl_name(impl_name),
				_mutex(), _class_instances(), _oi_handler(), _ongoing_callbacks_per_ccb()) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () { (_object_to_be_finalized) && finalize(); }

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method initializes the object previously created.
	 * It must be called only once.
	 *
	 * @return On success, 0 is returned. In the other case a negative value is returned.
	 */
	int initialize () {
		HCF_CC_TRACE_FUNCTION;

		if (_object_to_be_finalized) {
			HCF_CC_TRACE(Initialize, "The Configuration Class Handler object was already "
					"initialized: nothing to do (VALUE == CLASS_NAME)!", _class_name.c_str(), _object_to_be_finalized);
			return ERR_NO_ERROR;
		}

		int call_result = 0;
		HCF_CC_TRACE(Initialize, "Initializing the OM Handler object (VALUE == CLASS_NAME)!", _class_name.c_str(), _object_to_be_finalized);

		om_handler om_handler;
		if (om_handler.Init() != OPERATION_SUCCESS) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.Init' failed, return_code == %d, error_message == '%s'!",
					om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
			HCF_CC_TRACE(InitializeErr, "Call 'om_handler.Init' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
			return ERR_OM_HANDLER_INIT_FAILED;
		}
		HCF_CC_TRACE(Initialize, "Retrieving all the instances of this class (VALUE == CLASS_NAME)!", _class_name.c_str(), _object_to_be_finalized);

		// First, load from IMM the already created instances of the current class (if any)
		std::vector<std::string> class_instances_dn;
		if (om_handler.getClassInstances(_class_name.c_str(), class_instances_dn) != OPERATION_SUCCESS) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.getClassInstances' for class '%s' failed, return_code == %d, error_message == '%s'!",
					_class_name.c_str(), om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
			HCF_CC_TRACE(InitializeErr, "Call 'om_handler.getClassInstances' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
			om_handler.Finalize();
			return ERR_OM_HANDLER_GET_INSTANCES_FAILED;
		}

		/*
		 *  Then, for each retrieved instance (if any), retrieve the attributes
		 *  values from IMM and add the object to the internal data structure.
		 */
		for (size_t i = 0; i < class_instances_dn.size(); ++i) {
			IMM_API_ImmObject imm_obj;
			imm_obj.objName = class_instances_dn[i];

			HCF_CC_TRACE(Initialize, "Retrieving the entire object (VALUE == OBJECT)!", class_instances_dn[i].c_str(), _object_to_be_finalized);
			if (om_handler.getObject(&imm_obj) != OPERATION_SUCCESS) {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.getObject' for object '%s' failed, return_code == %d, error_message == '%s'!",
						class_instances_dn[i].c_str(), om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
				HCF_CC_TRACE(InitializeErr, "Call 'om_handler.getObject' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
				om_handler.Finalize();
				return ERR_OM_HANDLER_GET_OBJECT_FAILED;
			}
			HCF_CC_TRACE(Initialize, "Object correctly retrieved: preparing the attributes vector "
					"and the parent object DN (VALUE == OBJECT)!", class_instances_dn[i].c_str(), _object_to_be_finalized);

			std::vector<IMM_API_ImmAttribute> attributes = imm_obj.attributes;
			std::vector<std::pair<std::string, std::vector<void *> > > obj_attributes;

			for (size_t j = 0; j < attributes.size(); ++j) {
				// FIRST: get all the values for the current attribute
				std::vector<void *> attr_values;
				for (unsigned int k = 0; k < attributes[j].attrValuesNum; ++k)	attr_values.push_back(attributes[j].attrValues[k]);

				// SECOND: create a pair with the attribute name and the retrieved value(s)
				std::pair<std::string, std::vector<void *> > attribute;
				attribute.first = attributes[j].attrName;
				attribute.second = attr_values;

				// THIRD: add the pair to the array of attributes
				obj_attributes.push_back(attribute);
			}

			// Extract the parent dn for the current object (if present)
			std::string parent_dn;
			size_t comma_pos = class_instances_dn[i].find_first_of(',');
			if (comma_pos != std::string::npos) {
				parent_dn = class_instances_dn[i].substr(comma_pos + 1);
			}

			HCF_CC_TRACE(Initialize, "Adding the retrieved object and its attributes to the internal "
					"data structures (VALUE == OBJECT)!", class_instances_dn[i].c_str(), _object_to_be_finalized);

			// Add the object into the internal data structure, passing the parent dn
			obj_type * created_obj;
			std::string error_message;
			if ((call_result = add_object_to_internals(obj_attributes, parent_dn.c_str(), error_message, &created_obj))) {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'add_object_to_internals' failed for object having "
						"dn == '%s' (parent_dn == '%s')! with error_code == %d",
						class_instances_dn[i].c_str(), parent_dn.c_str(), call_result);
				HCF_CC_TRACE(InitializeErr, "Call 'add_object_to_internals' failed: error code is:", call_result, "");
				om_handler.Finalize();
				return call_result;
			}
		}
		om_handler.Finalize();
		HCF_CC_TRACE(Initialize, "Internal data structures correctly updated: registering the "
				"class implementer (VALUE == IMPLEMENTER)!", _impl_name.c_str(), _object_to_be_finalized);

		// After updating the internal data structures, set the class implementer
		if ((call_result = register_class_implementer())) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'register_class_implementer' failed, return_code = %d", call_result);
			HCF_CC_TRACE(InitializeErr, "Call 'register_class_implementer' failed: error code is:", call_result, "");
			return call_result;
		}

		_object_to_be_finalized = 1;
		HCF_CC_TRACE(Initialize, "Initialization correctly executed for the class (VALUE == CLASS_NAME)!", _class_name.c_str(), _object_to_be_finalized);
		return ERR_NO_ERROR;
	}

	/**
	 * This method finalizes the object before destroying it.
	 * It must be called only once.
	 *
	 * @return On success, 0 is returned. In the other case a negative value is returned.
	 */
	int finalize () {
		HCF_CC_TRACE_FUNCTION;

		if (!_object_to_be_finalized) {
			HCF_CC_TRACE(Finalize, "The Configuration Class Handler object was already finalized: nothing to do!", _class_name.c_str(), _object_to_be_finalized);
			return ERR_NO_ERROR;
		}
		HCF_CC_TRACE(Finalize, "Unregistering the class implementer!", _class_name.c_str(), _object_to_be_finalized);

		// FIRST: remove the class implementer
		int call_result = 0;
		if ((call_result = remove_class_implementer())) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'remove_class_implementer' failed, return_code = %d", call_result);
			HCF_CC_TRACE(FinalizeErr, "Call 'remove_class_implementer' failed: error code is:", call_result, "");
			return call_result;
		}
		HCF_CC_TRACE(Finalize, "Class implementer correctly removed: erasing the content of the internal data structures!", _class_name.c_str(), _object_to_be_finalized);

		// SECOND: remove from the internal data structure all the allocated objects
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			for (typename std::map<std::string, obj_type *>::iterator it = _class_instances.begin(); it != _class_instances.end(); ++it) {
				delete it->second;
			}
			_class_instances.clear();
		TSL_CRITICAL_SECTION_END;

		_object_to_be_finalized = 0;
		HCF_CC_TRACE(Initialize, "Finalization correctly executed for the class!", _class_name.c_str(), _object_to_be_finalized);
		return ERR_NO_ERROR;
	}

	/**
	 * This method returns the class instance identified by the ID provided as input.
	 *
	 * @param[in]  object_name - The ID of the object to be returned.
	 * @return If the object is contained into the map, a pointer to the class instance, 0 otherwise.
	 */
	obj_type * get_instance (const std::string & object_name) {
		HCF_CC_TRACE_FUNCTION;
		obj_type * instance = 0;

		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			typename std::map<std::string, obj_type *>::iterator it = _class_instances.find(object_name);
			if (it != _class_instances.end()) {
				instance = _class_instances[object_name];
			}
		TSL_CRITICAL_SECTION_END;

		return instance;
	}

	/**
	 * This method returns the list of all the created objects.
	 *
	 * @return a list containing the pointer to each created object instance.
	 */
	std::list<obj_type *> get_all_instances () {
		HCF_CC_TRACE_FUNCTION;
		std::list<obj_type *> instances;

		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			for (typename std::map<std::string, obj_type *>::iterator it = _class_instances.begin(); it != _class_instances.end(); ++it) {
				instances.push_back(it->second);
			}
		TSL_CRITICAL_SECTION_END;
		return instances;
	}

private:
	/**
	 * This method registers the current object as class implementer for the class specified by _class_name field.
	 * The implementer name used is the one specified by _impl_name field.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int register_class_implementer () {
		HCF_CC_TRACE_FUNCTION;
		HCF_CC_LOG(LEVEL_NOTICE, "Setting class implementer for '%s' with implementer name '%s'", _class_name.c_str(), _impl_name.c_str());

		// Try to set the class implementer max for MAX_NUM_OF_RETRIES
		for (int i = 0; i < MAX_NUM_OF_RETRIES; ++i) {
			HCF_CC_TRACE(RegisterImplementer, "Registering the class implementer!", _class_name.c_str(), _impl_name.c_str());

			if (_oi_handler.addClassImpl(this, _class_name.c_str()) != OPERATION_SUCCESS) {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'addClassImpl' failed for class '%s', waiting before retrying... error_code == %d, error_message == '%s'",
						_class_name.c_str(), _oi_handler.getInternalLastError(), _oi_handler.getInternalLastErrorText());
				HCF_CC_TRACE(RegisterImplementerErr, "Call '_oi_handler.addClassImpl' failed.", _oi_handler.getInternalLastError(), _oi_handler.getInternalLastErrorText());
				::sleep(1);
			}
			else {
				HCF_CC_TRACE(RegisterImplementer, "Class implementer correctly registered!", _class_name.c_str(), _impl_name.c_str());
				return ERR_NO_ERROR;
			}
		}
		return ERR_OM_HANDLER_SET_IMPLEMENTER_FAILED;
	}

	/**
	 * This method unregisters the current object as class implementer for the class specified by _class_name field.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int remove_class_implementer () {
		HCF_CC_TRACE_FUNCTION;
		HCF_CC_LOG(LEVEL_NOTICE, "Removing class implementer for '%s'", _class_name.c_str());

		// Try to remove the class implementer max for MAX_NUM_OF_RETRIES
		for (int i = 0; i < MAX_NUM_OF_RETRIES; ++i) {
			HCF_CC_TRACE(RemoveImplementer, "Removing the class implementer!", _class_name.c_str(), _impl_name.c_str());

			if (_oi_handler.removeClassImpl(this, _class_name.c_str()) != OPERATION_SUCCESS) {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'removeClassImpl' failed for class '%s', waiting before retrying... error_code == %d, error_message == '%s'",
						_class_name.c_str(), _oi_handler.getInternalLastError(), _oi_handler.getInternalLastErrorText());
				HCF_CC_TRACE(RemoveImplementerErr, "Call '_oi_handler.removeClassImpl' failed.", _oi_handler.getInternalLastError(), _oi_handler.getInternalLastErrorText());
				::sleep(1);
			}
			else {
				HCF_CC_TRACE(RemoveImplementer, "Class implementer correctly unregistered!", _class_name.c_str(), _impl_name.c_str());
				return ERR_NO_ERROR;
			}
		}
		return ERR_OM_HANDLER_REMOVE_IMPLEMENTER_FAILED;
	}

	/**
	 * This method adds a new object to the internal data structure containing all the created objects.
	 * The value of each object attribute is specified by the obj_attributes parameter.
	 *
	 * @param[in]  obj_attributes - A vector of pairs representing the new object attributes.
	 *             Each pair has as first item a string indicating the attribute name and second item
	 *             a pointer to the attribute value.
	 * @param[in]  parent_dn - The parent DN value for the object to be created.
	 * @param[out] error_message - The reason for which the operation failed.
	 * @param[out] obj_ptr - The pointer to the created object.
	 * @param[in] callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int add_object_to_internals (std::vector<std::pair<std::string, std::vector<void *> > > & obj_attributes, const char * parent_dn, std::string & error_message, obj_type ** obj_ptr, IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0) {
		HCF_CC_TRACE_FUNCTION;

		int call_result = 0;

		// Create the new IMM object instance, providing the received attributes
		HCF_CC_TRACE(AddObjectToInternals, "Received a request to add a new class instance (VALUE == CLASS)!", _class_name.c_str());
		obj_type * created_object = new obj_type(_class_name.c_str());

		HCF_CC_TRACE(AddObjectToInternals, "Adding the attributes for the created object (VALUE == PARENT)!", parent_dn);
		if ((call_result = created_object->set_object_attributes_and_dn(obj_attributes, parent_dn, callback_info))) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'set_object_attributes_and_dn' failed, return_code == %d", call_result);
			error_message = created_object->get_error_message();
			delete created_object;
			HCF_CC_TRACE(AddObjectToInternalsErr, "Call 'created_object->set_object_attributes_and_dn' failed.", call_result, error_message.c_str());
			return ERR_SET_OBJECT_ATTRIBUTES_FAILED;
		}

		// After successful object creation, add it to the internal data structure
		HCF_CC_LOG(LEVEL_NOTICE, "Object having dn == '%s' (id == '%s') correctly created, adding it to the internal data structure...",
				created_object->get_dn().c_str(), created_object->get_id().c_str());
		HCF_CC_TRACE(AddObjectToInternals, "Object correctly created, adding it to the internal data structures (VALUE == ID)!", created_object->get_id().c_str());

		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			_class_instances[created_object->get_id()] = created_object;
			*obj_ptr = created_object;
		TSL_CRITICAL_SECTION_END;
		return ERR_NO_ERROR;
	}

	//=====================================================================//
	// object_implementer_interface class Virtual Functions Implementation //
	//=====================================================================//
public:
	/**
	 * This method implements the callback method to be called when a new instance of the current class is created.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes create (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, const char * class_name, const char * parent_dn, IMM_API_AttrValues ** attr) {
		HCF_CC_TRACE_FUNCTION;
		HCF_CC_LOG(LEVEL_NOTICE, "Received request to create a new instance of '%s', parent_dn = '%s', ccb == %llu", class_name, parent_dn, ccbId);
		HCF_CC_TRACE(CreateCallback, "Received a request to create a new class instance: preparing attributes vector!", oiHandle, ccbId, class_name, parent_dn);
		std::vector<std::pair<std::string, std::vector<void *> > > obj_attributes;

		int i = 0;
		while (attr[i]) {
			// FIRST: get all the values for the current attribute
			std::vector<void *> attr_values;
			for (unsigned int j = 0; j < attr[i]->attrValuesNum; ++j)	attr_values.push_back(attr[i]->attrValues[j]);

			// SECOND: create a pair with the attribute name and the retrieved value(s)
			std::pair<std::string, std::vector<void *> > attribute;
			attribute.first = attr[i]->attrName;
			attribute.second = attr_values;

			// THIRD: add the pair to the array of attributes
			obj_attributes.push_back(attribute);
			++i;
		}

		// Store the callback info
		IMM_API_ObjectImplementer_CallbackInfo callback_info;
		callback_info.oi_handle = oiHandle;
		callback_info.ccb_id = ccbId;
		callback_info.object_implementer = this;
		callback_info.initialized = false;

		HCF_CC_TRACE(CreateCallback, "Received a request to create a new class instance: updating the internal data structures!", oiHandle, ccbId, class_name, parent_dn);

		// Create the object into the internal data structure and return success or failure accordingly
		obj_type * created_obj;
		std::string error_message;
		int call_result = 0;
		if ((call_result = add_object_to_internals(obj_attributes, parent_dn, error_message, &created_obj, &callback_info))) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'add_object_to_internals' failed, error_code == %d, error_message == '%s'", call_result, error_message.c_str());
			setExitCode(call_result, error_message);
			HCF_CC_TRACE(CreateCallbackErr, "Call 'add_object_to_internals' failed.", call_result, error_message.c_str());
			return OPERATION_FAILURE;
		}

		// After the callback has been correctly dispatched, save the created object into the ongoing callbacks map
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			std::list<std::pair<operation_type_t, std::string> > & ccb_ongoing_callbacks = _ongoing_callbacks_per_ccb[ccbId];
			ccb_ongoing_callbacks.push_back(std::pair<operation_type_t, std::string>(OPERATION_CREATE, created_obj->get_id()));
		TSL_CRITICAL_SECTION_END;
		return OPERATION_SUCCESS;
	}

	/**
	 * This method implements the callback method to be called when an instance of the current class is deleted.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes deleted (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, const char * obj_dn) {
		HCF_CC_TRACE_FUNCTION;

		int call_result = 0;
		std::string obj_id = obj_type::get_obj_id_from_dn(obj_dn);
		HCF_CC_LOG(LEVEL_NOTICE, "Received request to delete the object having dn == '%s' and id == '%s', ccb == %llu", obj_dn, obj_id.c_str(), ccbId);
		HCF_CC_TRACE(DeleteCallback, "Received a request to delete this object instance: looking for it into the internal structures!", oiHandle, ccbId, obj_dn);

		// Search into the internal data structure the IMM object instance having the calculated id
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			typename std::map<std::string, obj_type *>::iterator map_item = _class_instances.find(obj_id.c_str());
			if (map_item == _class_instances.end()) {
				HCF_CC_LOG(LEVEL_ERROR, "No entry into the internal data structure found for the IMM object having id == '%s'", obj_id.c_str());
				setExitCode(ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
				HCF_CC_TRACE(DeleteCallbackErr, "No instance found for the requested object.", ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
				return OPERATION_FAILURE;
			}

			// Store the callback info
			IMM_API_ObjectImplementer_CallbackInfo callback_info;
			callback_info.oi_handle = oiHandle;
			callback_info.ccb_id = ccbId;
			callback_info.object_implementer = this;
			callback_info.initialized = false;

			// Finalize the IMM object previously retrieved from the internal data structure
			HCF_CC_TRACE(DeleteCallback, "Object instance found: finalizing it!", oiHandle, ccbId, obj_dn);
			obj_type * obj_to_delete = map_item->second;
			if ((call_result = obj_to_delete->finalize_object(&callback_info))) {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'finalize_object' failed, return_code == %d", call_result);
				setExitCode(call_result, obj_to_delete->get_error_message());
				HCF_CC_TRACE(DeleteCallbackErr, "Call 'obj_to_delete->finalize_object' failed.", call_result, obj_to_delete->get_error_message().c_str());
				return OPERATION_FAILURE;
			}

			// After the callback has been correctly dispatched, save the created object into the ongoing callbacks map
			std::list<std::pair<operation_type_t, std::string> > & ccb_ongoing_callbacks = _ongoing_callbacks_per_ccb[ccbId];
			ccb_ongoing_callbacks.push_back(std::pair<operation_type_t, std::string>(OPERATION_DELETE, obj_id));
		TSL_CRITICAL_SECTION_END;
		return OPERATION_SUCCESS;
	}

	/**
	 * This method implements the callback method to be called when an instance of the current class is modified.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes modify (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, const char * obj_dn, IMM_API_AttrModification ** attrMods) {
		HCF_CC_TRACE_FUNCTION;

		int call_result = 0;
		std::string obj_id = obj_type::get_obj_id_from_dn(obj_dn);
		HCF_CC_LOG(LEVEL_NOTICE, "Received request to modify the object having dn == '%s' and id == '%s', ccb == %llu", obj_dn, obj_id.c_str(), ccbId);
		HCF_CC_TRACE(ModifyCallback, "Received a request to modify this object instance: looking for it into the internal structures!", oiHandle, ccbId, obj_dn);

		// Search into the internal data structure the IMM object instance having the calculated id
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			typename std::map<std::string, obj_type *>::iterator map_item = _class_instances.find(obj_id.c_str());
			if (map_item == _class_instances.end()) {
				HCF_CC_LOG(LEVEL_ERROR, "No entry into the internal data structure found for the IMM object having id == '%s'", obj_id.c_str());
				setExitCode(ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
				HCF_CC_TRACE(ModifyCallbackErr, "No instance found for the requested object.", ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
				return OPERATION_FAILURE;
			}

			// Fill the attributes vector before creating the object
			std::vector<std::pair<std::string, std::vector<void *> > > obj_attributes;
			HCF_CC_TRACE(ModifyCallback, "Object instance found: preparing the modified attributes vector!", oiHandle, ccbId, obj_dn);

			int i = 0;
			while (attrMods[i]) {
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
				++i;
			}

			// Store the callback info
			IMM_API_ObjectImplementer_CallbackInfo callback_info;
			callback_info.oi_handle = oiHandle;
			callback_info.ccb_id = ccbId;
			callback_info.object_implementer = this;
			callback_info.initialized = false;

			// Modify the IMM object previously retrieved from the internal data structure
			obj_type * obj_to_modify = map_item->second;

			// Before dispatching the callback, save the created object into the ongoing callbacks map
			std::list<std::pair<operation_type_t, std::string> > & ccb_ongoing_callbacks = _ongoing_callbacks_per_ccb[ccbId];
			ccb_ongoing_callbacks.push_back(std::pair<operation_type_t, std::string>(OPERATION_MODIFY, obj_id));

			// Dispatch the callback on the retrieved object
			HCF_CC_TRACE(ModifyCallback, "Object instance found: modifying it!", oiHandle, ccbId, obj_dn);
			if ((call_result = obj_to_modify->set_object_attributes(obj_attributes, &callback_info))) {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'obj_to_modify->set_object_attributes' failed, return_code == %d", call_result);
				setExitCode(call_result, obj_to_modify->get_error_message());
				HCF_CC_TRACE(ModifyCallbackErr, "Call 'obj_to_modify->set_object_attributes' failed.", call_result, obj_to_modify->get_error_message().c_str());
				return OPERATION_FAILURE;
			}
		TSL_CRITICAL_SECTION_END;
		return OPERATION_SUCCESS;
	}

	/**
	 * This method implements the callback method to be called when the object
	 * creation/modification/deletion has been correctly executed.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes complete (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId) {
		HCF_CC_TRACE_FUNCTION;
		HCF_CC_LOG(LEVEL_NOTICE, "Received a complete callback for the ccb == %llu", ccbId);
		int call_result = 0;

		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			std::list<std::pair<operation_type_t, std::string> > & ccb_ongoing_callbacks = _ongoing_callbacks_per_ccb[ccbId];
			HCF_CC_TRACE(CompleteCallback, "Received a request to complete the CCB: signaling the information to each object involved!", oiHandle, ccbId, "");

			// Signal the receiving of the complete callback to each object involved into the current CCB
			for (typename std::list<std::pair<operation_type_t, std::string> >::iterator it = ccb_ongoing_callbacks.begin(); it != ccb_ongoing_callbacks.end(); ++it) {
				typename std::map<std::string, obj_type *>::iterator map_item = _class_instances.find(it->second);
				if (map_item == _class_instances.end()) {
					HCF_CC_LOG(LEVEL_ERROR, "No entry into the internal data structure found for the IMM object having id == '%s'", it->second.c_str());
					setExitCode(ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
					HCF_CC_TRACE(CompleteCallbackErr, "No instance found for an object involved into the callback.", ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
					return OPERATION_FAILURE;
				}
				HCF_CC_LOG(LEVEL_NOTICE, "Signaling the complete callback to the object having id == '%s'", map_item->first.c_str());

				// Store the callback info
				IMM_API_ObjectImplementer_CallbackInfo callback_info;
				callback_info.oi_handle = oiHandle;
				callback_info.ccb_id = ccbId;
				callback_info.object_implementer = this;
				callback_info.initialized = false;

				HCF_CC_TRACE(CompleteCallback, "Signaling the CCB completion to the object!", oiHandle, ccbId, map_item->first.c_str());
				if ((call_result = map_item->second->complete_object_modification(&callback_info))) {
					HCF_CC_LOG(LEVEL_ERROR, "Call 'complete_object_modification' failed, return_code == %d", call_result);
					setExitCode(call_result, map_item->second->get_error_message());
					HCF_CC_TRACE(CompleteCallbackErr, "Call 'map_item->second->complete_object_modification' failed.", call_result, map_item->second->get_error_message().c_str());
					return OPERATION_FAILURE;
				}
			}
		TSL_CRITICAL_SECTION_END;
		return OPERATION_SUCCESS;
	}

	/**
	 * This method implements the callback method to be called when the object creation/modification/deletion is failed.
	 * @see object_implementer_interface class for further info.
	 */
	virtual void abort (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId) {
		HCF_CC_TRACE_FUNCTION;
		HCF_CC_LOG(LEVEL_NOTICE, "Received an abort callback for the ccb == %llu", ccbId);

		// Store the callback info
		IMM_API_ObjectImplementer_CallbackInfo callback_info;
		callback_info.oi_handle = oiHandle;
		callback_info.ccb_id = ccbId;
		callback_info.object_implementer = this;
		callback_info.initialized = false;

		int call_result = 0;

		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			std::list<std::pair<operation_type_t, std::string> > & ccb_ongoing_callbacks = _ongoing_callbacks_per_ccb[ccbId];
			HCF_CC_TRACE(AbortCallback, "Received a request to abort the CCB: signaling the information to each object involved!", oiHandle, ccbId, "");

			// Signal the receiving of the abort callback to each object involved into the current CCB
			for (typename std::list<std::pair<operation_type_t, std::string> >::iterator it = ccb_ongoing_callbacks.begin(); it != ccb_ongoing_callbacks.end(); ++it) {
				typename std::map<std::string, obj_type *>::iterator map_item = _class_instances.find(it->second);
				if (map_item == _class_instances.end()) {
					HCF_CC_LOG(LEVEL_ERROR, "No entry into the internal data structure found for the IMM object having id == '%s'", it->second.c_str());
					HCF_CC_TRACE(AbortCallbackErr, "No instance found for an object involved into the callback.", ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
					continue;
				}
				HCF_CC_LOG(LEVEL_NOTICE, "Signaling the abort callback to the object having id == '%s'", map_item->first.c_str());

				HCF_CC_TRACE(AbortCallback, "Signaling the CCB aborting to the object!", oiHandle, ccbId, map_item->first.c_str());
				if ((call_result = map_item->second->abort_object_modification(&callback_info))) {
					HCF_CC_LOG(LEVEL_ERROR, "Call 'abort_object_modification' failed, return_code == %d", call_result);
					HCF_CC_TRACE(AbortCallbackErr, "Call 'map_item->second->abort_object_modification' failed.", call_result, "");
					continue;
				}
			}

			/*
			 * The abort callback is the last one for this CCB: remove the entry from the ongoing CCB map
			 * Before doing this, align the _class_instances map: remove each new object, freeing its memory.
			 */
			HCF_CC_TRACE(AbortCallback, "This callback is the last for this CCB: removing the allocated memory for the objects created in this CCB!", oiHandle, ccbId, "");
			for (typename std::list<std::pair<operation_type_t, std::string> >::iterator it = ccb_ongoing_callbacks.begin(); it != ccb_ongoing_callbacks.end(); ++it) {
				if (it->first == OPERATION_CREATE) {
					typename std::map<std::string, obj_type *>::iterator map_item = _class_instances.find(it->second);
					if (map_item == _class_instances.end()) {
						HCF_CC_LOG(LEVEL_ERROR, "No entry into the internal data structure found for the IMM object having id == '%s'", it->second.c_str());
						HCF_CC_TRACE(AbortCallbackErr, "No instance found for an object involved into the callback.", ERR_NULL_INSTANCE_RETRIEVED, "");
						continue;
					}

					// Before deleting the object, finalize it
					HCF_CC_TRACE(AbortCallback, "Finalizing the object created in this CCB!", oiHandle, ccbId, map_item->first.c_str());
					if ((call_result = map_item->second->finalize_object(&callback_info))) {
						HCF_CC_LOG(LEVEL_ERROR, "Call 'finalize_object' failed, return_code == %d", call_result);
						HCF_CC_TRACE(AbortCallbackErr, "No instance found for an object involved into the callback.", call_result, map_item->second->get_error_message().c_str());
					}

					HCF_CC_LOG(LEVEL_NOTICE, "Deleting from internal data structure the object having ID == '%s'", map_item->first.c_str());
					HCF_CC_TRACE(AbortCallback, "Deleting the object created in this CCB!", oiHandle, ccbId, map_item->first.c_str());
					_class_instances.erase(map_item);
					delete map_item->second;
				}
			}
			_ongoing_callbacks_per_ccb.erase(ccbId);
		TSL_CRITICAL_SECTION_END;
	}

	/**
	 * This method implements the callback method to be called when the object
	 * creation/modification/deletion has been correctly executed and the complete method called.
	 * @see object_implementer_interface class for further info.
	 */
	virtual void apply (IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId) {
		HCF_CC_TRACE_FUNCTION;
		HCF_CC_LOG(LEVEL_NOTICE, "Received an apply callback for the ccb == %llu", ccbId);
		int call_result = 0;

		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			std::list<std::pair<operation_type_t, std::string> > & ccb_ongoing_callbacks = _ongoing_callbacks_per_ccb[ccbId];
			HCF_CC_TRACE(ApplyCallback, "Received a request to APPLY the CCB: signaling the information to each object involved!", oiHandle, ccbId, "");

			// Signal the receiving of the complete callback to each object involved into the current CCB
			for (typename std::list<std::pair<operation_type_t, std::string> >::iterator it = ccb_ongoing_callbacks.begin(); it != ccb_ongoing_callbacks.end(); ++it) {
				typename std::map<std::string, obj_type *>::iterator map_item = _class_instances.find(it->second);
				if (map_item == _class_instances.end()) {
					HCF_CC_LOG(LEVEL_ERROR, "No entry into the internal data structure found for the IMM object having id == '%s'", it->second.c_str());
					HCF_CC_TRACE(ApplyCallbackErr, "No instance found for an object involved into the callback.", ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
					continue;
				}

				// Store the callback info
				IMM_API_ObjectImplementer_CallbackInfo callback_info;
				callback_info.oi_handle = oiHandle;
				callback_info.ccb_id = ccbId;
				callback_info.object_implementer = this;
				callback_info.initialized = false;

				HCF_CC_TRACE(ApplyCallback, "Signaling the CCB apply to the object!", oiHandle, ccbId, map_item->first.c_str());
				if ((call_result = map_item->second->apply_object_modification(&callback_info))) {
					HCF_CC_LOG(LEVEL_ERROR, "Call 'apply_object_modification' failed, return_code == %d", call_result);
					HCF_CC_TRACE(ApplyCallbackErr, "Call 'map_item->second->abort_object_modification' failed.", call_result, "");
					continue;
				}
			}

			/*
			 * The apply callback is the last one for this CCB: remove the entry from the ongoing CCB map.
			 * Before doing this, align the _class_instances map: remove each deleted object, freeing its memory.
			 */
			HCF_CC_TRACE(ApplyCallback, "This callback is the last for this CCB: removing the allocated memory for the objects deleted in this CCB!", oiHandle, ccbId, "");
			for (typename std::list<std::pair<operation_type_t, std::string> >::iterator it = ccb_ongoing_callbacks.begin(); it != ccb_ongoing_callbacks.end(); ++it) {
				if (it->first == OPERATION_DELETE) {
					typename std::map<std::string, obj_type *>::iterator map_item = _class_instances.find(it->second);
					if (map_item == _class_instances.end()) {
						HCF_CC_LOG(LEVEL_ERROR, "No entry into the internal data structure found for the IMM object having id == '%s'", it->second.c_str());
						HCF_CC_TRACE(ApplyCallbackErr, "No instance found for an object involved into the callback.", ERR_NULL_INSTANCE_RETRIEVED, "");
						continue;
					}

					HCF_CC_LOG(LEVEL_NOTICE, "Deleting from internal data structure the object having ID == '%s'", map_item->first.c_str());
					HCF_CC_TRACE(ApplyCallback, "Deleting the object created in this CCB!", oiHandle, ccbId, map_item->first.c_str());
					_class_instances.erase(map_item);
					delete map_item->second;
				}
			}
			_ongoing_callbacks_per_ccb.erase(ccbId);
		TSL_CRITICAL_SECTION_END;
	}

	/**
	 * This method implements the callback method to be called when trying to modify
	 * a runtime not-cached attribute inside a configuration class.
	 * @see object_implementer_interface class for further info.
	 */
	virtual IMM_API_ReturnTypes updateRuntime (const char * obj_dn, const char ** p_attrName) {
		HCF_CC_TRACE_FUNCTION;

		int i = 0;
		std::string obj_id = obj_type::get_obj_id_from_dn(obj_dn);
		HCF_CC_LOG(LEVEL_NOTICE, "Received request to update one or more runtime parameters for the object having dn == '%s' and id == '%s'", obj_dn, obj_id.c_str());
		HCF_CC_TRACE(UpdateRuntimeCallback, "Received a request to update some object runtime attributes: looking for object instance!", obj_dn, "");

		// Search into the internal data structure the IMM object instance having the calculated id
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			typename std::map<std::string, obj_type *>::iterator map_item = _class_instances.find(obj_id.c_str());
			if (map_item == _class_instances.end()) {
				HCF_CC_LOG(LEVEL_ERROR, "No entry into the internal data structure found for the IMM object having id == '%s'", obj_id.c_str());
				setExitCode(ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
				HCF_CC_TRACE(UpdateRuntimeCallbackErr, "No instance found for the object.", ERR_NULL_INSTANCE_RETRIEVED, "Generic Error");
				return OPERATION_FAILURE;
			}

			// After retrieving the object, update each of the received parameters
			obj_type * obj_to_update = map_item->second;

			while (p_attrName[i]) {
				HCF_CC_LOG(LEVEL_NOTICE, "Trying to modify the attribute '%s'", p_attrName[i]);
				HCF_CC_TRACE(UpdateRuntimeCallback, "Requesting to update the value for an attribute!", obj_dn, p_attrName[i]);

				IMM_API_ImmParameter param_to_update;
				int call_result = 0;
				if ((call_result = obj_to_update->update_runtime_attribute(p_attrName[i], param_to_update))) {
					HCF_CC_LOG(LEVEL_ERROR, "Call 'update_runtime_attribute' failed, return_code = %d", call_result);
					setExitCode(call_result, obj_to_update->get_error_message());
					HCF_CC_TRACE(UpdateRuntimeCallbackErr, "Call 'obj_to_update->update_runtime_attribute' failed.", call_result, obj_to_update->get_error_message().c_str());
					return OPERATION_FAILURE;
				}

				HCF_CC_TRACE(UpdateRuntimeCallback, "A value for the attribute was provided: updating it!!", obj_dn, p_attrName[i]);
				if (modifyRuntimeObj(obj_dn, &param_to_update) != OPERATION_SUCCESS) {
					HCF_CC_LOG(LEVEL_ERROR, "Call 'modifyRuntimeObj' failed for object having dn == '%s', "
							"error_code = %d, error_message = '%s'", obj_dn, getInternalLastError(), getInternalLastErrorText());
					setExitCode(OPERATION_FAILURE, "Generic Error");
					HCF_CC_TRACE(UpdateRuntimeCallbackErr, "Call 'modifyRuntimeObj' failed.", OPERATION_FAILURE, "Generic Error");
					return OPERATION_FAILURE;
				}

				delete [] param_to_update.attrValues;
				++i;
			}
		TSL_CRITICAL_SECTION_END;
		return OPERATION_SUCCESS;
	}

	/**
	 * This method implements the callback method to be called when
	 * an administrative operation is invoked on an object of the current class.
	 * @see object_implementer_interface class for further info.
	 */
	virtual void adminOperationCallback (IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, const char * obj_dn, IMM_API_AdminOperationIdType operationId, IMM_API_AdminOperationParamType ** paramList) {
		HCF_CC_TRACE_FUNCTION;

		uint64_t op_id = operationId;
		std::string obj_id = obj_type::get_obj_id_from_dn(obj_dn);
		HCF_CC_LOG(LEVEL_NOTICE, "Administrative operation (with id == %lu) invoked on the "
				"object having dn == '%s' and id == '%s'", op_id, obj_dn, obj_id.c_str());
		HCF_CC_TRACE(AdminOperationCallback, "Received a request to execute an administrative operation!", oiHandle, invocation, obj_dn, operationId);

		// Prepare the return values to be used in case of failed operation
		IMM_API_AdminOperationParamType param;
		std::vector<IMM_API_AdminOperationParamType> out_params;
		param.attrName = const_cast<char*>("error_code");
		param.attrType = ATTR_STRINGT;

		// Search into the internal data structure the IMM object instance having the calculated id
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			HCF_CC_TRACE(AdminOperationCallback, "Received a request to execute an administrative operation: looking for object instance!", oiHandle, invocation, obj_dn, operationId);
			typename std::map<std::string, obj_type *>::iterator map_item = _class_instances.find(obj_id.c_str());
			if (map_item == _class_instances.end()) {		// ERROR: element not found!
				HCF_CC_LOG(LEVEL_ERROR, "No entry into the internal data structure found for the IMM object having id == '%s'", obj_id.c_str());

				// Prepare the error message and set accordingly the return values
				setExitCode(SA_AIS_ERR_FAILED_OPERATION, "Generic Error");
				param.attrValues = reinterpret_cast<void *>(const_cast<char *>(getExitCodeString().c_str()));
				out_params.push_back(param);

				HCF_CC_TRACE(AdminOperationCallbackErr, "No instance found for the requested object: returning error to the caller.", SA_AIS_ERR_FAILED_OPERATION, "Generic Error");

				if (adminOperationResult(oiHandle, invocation, SA_AIS_ERR_FAILED_OPERATION, out_params) != OPERATION_SUCCESS) {
					HCF_CC_LOG(LEVEL_ERROR, "Call 'adminOperationResult' failed!");
					HCF_CC_TRACE(AdminOperationCallbackErr, "Call 'adminOperationResult' failed: not possible to return a proper error message.", OPERATION_FAILURE, "Generic Error");
				}
				return;
			}

			// After retrieving the object, update execute the operation on it
			obj_type * obj = map_item->second;
			int call_result = 0;

			HCF_CC_TRACE(AdminOperationCallback, "Object instance found: invoking administrative operation!", oiHandle, invocation, obj_dn, operationId);
			if ((call_result = obj->execute_admin_operation(op_id, paramList))) {
				// In case the operation failed, setup the error message and return it accordingly
				HCF_CC_LOG(LEVEL_ERROR, "Call 'execute_admin_operation' failed, return_code == %d", call_result);
				HCF_CC_TRACE(AdminOperationCallbackErr, "Call 'obj->execute_admin_operation' failed: returning error to the caller.", SA_AIS_ERR_FAILED_OPERATION, obj->get_error_message().c_str());

				setExitCode(SA_AIS_ERR_FAILED_OPERATION, obj->get_error_message());
				param.attrValues = reinterpret_cast<void *>(const_cast<char *>(getExitCodeString().c_str()));
				out_params.push_back(param);

				if (adminOperationResult(oiHandle, invocation, SA_AIS_ERR_FAILED_OPERATION, out_params) != OPERATION_SUCCESS) {
					HCF_CC_LOG(LEVEL_ERROR, "Call 'adminOperationResult' failed!");
					HCF_CC_TRACE(AdminOperationCallbackErr, "Call 'adminOperationResult' failed: not possible to return a proper error message.", OPERATION_FAILURE, "Generic Error");
				}
			}
			else {
				// Operation correctly dispatched, return OK
				HCF_CC_LOG(LEVEL_NOTICE, "Administrative operation correctly executed!");
				HCF_CC_TRACE(AdminOperationCallback, "Administrative operation correctly executed: signaling the success to the caller!", oiHandle, invocation, obj_dn, operationId);
				adminOperationResult(oiHandle, invocation, SA_AIS_OK);
			}
		TSL_CRITICAL_SECTION_END;

		return;
	}

	//======================================================//
	// event_handler class Virtual Functions Implementation //
	//======================================================//
public:
	/**
	 * This method returns the I/O handle associated to the Event Handler object.
	 *
	 * @return the I/O handle associated to the class implementer.
	 */
	inline virtual int get_handle () { return getSelObj(); }

	/**
	 * This method is called when an input operation is signaled on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_input () {
		HCF_CC_TRACE_FUNCTION;

		HCF_CC_LOG(LEVEL_NOTICE, "Received a callback for the class '%s', dispatching it!", _class_name.c_str());
		HCF_CC_TRACE(HandleInput, "Received an input event (callback to be dispatched) on the following file descriptor:", get_handle());

		int call_result = 0;
		if ((call_result = dispatch(TSL_NS_HCF_IMM::DISPATCH_ALL))) {
			HCF_CC_LOG(LEVEL_ERROR, "Dispatch failed for the class '%s', call_result == %d", _class_name.c_str(), call_result);
			HCF_CC_TRACE(HandleInput, "Failed to dispatch the IMM callback, the call returned the following error code:", call_result);
		}
		return 0;
	}

	/**
	 * This method is called when an output operation is signaled on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_output () {
		HCF_CC_TRACE_FUNCTION;
		HCF_CC_LOG(LEVEL_WARNING, "Received an output I/O event for the class '%s', it shouldn't occur, "
				"removing the Event Handler from the reactor", _class_name.c_str());
		HCF_CC_TRACE(HandleOutput, "Received an output event (very strange)!", get_handle());
		return -1;
	}

	/**
	 * This method is called when an error occurs on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_exception (bool error_flag, bool hungup_flag, bool read_hungup_flag) {
		HCF_CC_TRACE_FUNCTION;
		HCF_CC_LOG(LEVEL_WARNING, "Received an error on the I/O handle for the class '%s', removing the Event Handler "
				"from the reactor: error_flag == %s, hungup_flag == %s, read_hungup_flag == %s", _class_name.c_str(),
				(error_flag ? "TRUE" : "FALSE"), (hungup_flag ? "TRUE" : "FALSE"), (read_hungup_flag ? "TRUE" : "FALSE"));
		HCF_CC_TRACE(HandleException, "Received an error event!", get_handle(), error_flag, hungup_flag, read_hungup_flag);
		return -1;
	}

	/**
	 * This method is called before removing the Event Handler from the reactor.
	 *
	 * @return 0 on success, -1 on failure.
	 */
	virtual int handle_close () {
		HCF_CC_TRACE_FUNCTION;
		HCF_CC_LOG(LEVEL_NOTICE, "Removing the class handler for the class '%s' from the reactor!", _class_name.c_str());
		HCF_CC_TRACE(HandleClose, "Received a request to remove the event handler from the reactor!", get_handle());
		return 0;
	}

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	static const int MAX_NUM_OF_RETRIES = 10;

	int _object_to_be_finalized;
	std::string _class_name;
	std::string _impl_name;
	TSL_NS_HCF_CORE::thread_mutex_recursive _mutex;
	std::map<std::string, obj_type *> _class_instances;
	object_implementer_handler _oi_handler;
	/*
	 * This map contains all the needed info about the not completed callbacks ongoing.
	 * In particular:
	 * - The key element is the CCB identifier.
	 * - The value element is list of pairs, where:
	 *   - The first element identifies the type of operation ongoing.
	 *   - The second element is the ID of the object instance which the operation refers.
	 */
	std::map<IMM_API_CcbId, std::list<std::pair<operation_type_t, std::string> > > _ongoing_callbacks_per_ccb;
};

TSL_NS_HCF_IMM_END

#endif	// __INCLUDE_GUARD_CLASS_IMM_IMM_CONFIG_CLASS__
