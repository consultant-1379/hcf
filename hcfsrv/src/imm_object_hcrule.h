#ifndef __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE__
#define __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE__ imm_object_hcrule

/**
 *  @file imm_object_hcrule.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-12-02
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
 *	| 2014-001 | 2014-12-02 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <map>
#include <set>
#include <list>

#include "hcf/core/thread_mutex.h"
#include "hcf/imm/imm_object.h"

#include "srv_macros.h"
#include "app_types.h"
#include "constants.h"
#include "ruleset_file_parser.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_hcrule.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_IMM::imm_object {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * class_name)
		TSL_CTOR_INIT_LIST(imm_object(class_name), _adm_state(), _categories(), _rule_default_parameters(),
				_description(), _name(), _recommended_action(), _severity(), _rule_parameters_mutex(), _rule_input_parameters(),
				_rule_set_file_info(), _targets_to_include(), _targets_to_exclude(), _targets_set(), _reason(), _skip_log(),
				_parameter_container(0), _env_variables(), _adm_state_old(), _categories_old(), _rule_default_parameters_old(),
				_description_old(), _name_old(), _recommended_action_old(), _severity_old())
	{}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () { delete _parameter_container; _parameter_container = 0;	}

	//====================================================//
	// imm_object class Virtual Functions Implementation //
	//===================================================//
public:
	/**
	 * This method sets the attributes values for the current instance of the current class.
	 * The value of each class field is set according to the values present in attributes argument.
	 *
	 * @param[in]  attributes - A vector of pairs representing the new object attributes.
	 *             Each pair has as first item a string indicating the attribute name and second item
	 *             a pointer to the attribute value.
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0);

	/**
	 * This method finalizes the current instance of current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0);

	/**
	 * This method completes the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int complete_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0) {
		HCF_CC_TRACE_FUNCTION;
		return 0;
	}

	/**
	 * This method applies the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int apply_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0) {
		HCF_CC_TRACE_FUNCTION;
		return 0;
	}

	/**
	 * This method aborts the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0);

	/**
	 * This method updates the value of the runtime attribute with the provided name.
	 *
	 * @param[in]  attr_name - The name of the attribute to be updated.
	 * @param[out] param_to_update - A structure containing all the info about the parameter.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual inline int update_runtime_attribute (const char * TSL_UNUSED_PAR(attr_name), TSL_NS_HCF_IMM::IMM_API_ImmParameter & TSL_UNUSED_PAR(param_to_update)) {
		HCF_CC_TRACE_FUNCTION;
		// No runtime attributes for now!
		return ERR_OPERATION_NOT_IMPLEMENTED;
	}

	/**
	 * This method executes the given administrative operation on the current object.
	 *
	 * @param[in]  operation_id - The administrative operation id.
	 * @param[in]  arguments - The administrative operation optional arguments.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual inline int execute_admin_operation (uint64_t TSL_UNUSED_PAR(operation_id), TSL_NS_HCF_IMM::IMM_API_AdminOperationParamType ** TSL_UNUSED_PAR(arguments)) {
		HCF_CC_TRACE_FUNCTION;
		// No administrative operations for now!
		return ERR_OPERATION_NOT_IMPLEMENTED;
	}

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method creates a new HcRule instance with the provided info.
	 *
	 * @param[in]  rule_id - The ID of the object to be created.
	 * @param[in]  categories - The categories which the rule belongs to.
	 * @param[in]  severity - The rule severity.
	 * @param[in]  recomm_action - The action to be executed when the rule evaluation fails.
	 * @param[in]  rule_name - The name of the rule.
	 * @param[in]  description - The description to be associated to the rule.
	 * @param[in]  default_attributes - The default attributes of a customizable rule.
	 * @param[in]  error_if_exists - An error is returned if the object already exists.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	static int create_hcrule (const char * rule_id, std::list<category_t> & categories, severity_t severity, const char * recomm_action, const char * rule_name, const char * description, std::vector<ruleset_file_parser::rule_attribute> & default_attributes, bool error_if_exists = true);

	/**
	 * This method returns the ordered set of the rules belonging to the provided categories.
	 *
	 * @param[in]  categories - The categories of rules.
	 * @return the set of rules belonging to the provided categories.
	 */
	static std::set<std::string> get_rules_by_categories (std::set<category_t> & categories);

  /**
   * This method adds one attribute to the rule internal collection.
   *
   * @param[in]  job - The job which the rule customization belongs to.
   * @param[in]  name - The attribute name.
   * @param[in]  value - The attribute value.
   * @return 0 is returned on success, a negative value on failure.
   */
  int add_rule_attribute (const std::string & job, const std::string & name, const std::string & value);

  /**
   * This method modifies one attribute of the rule internal collection.
   *
   * @param[in]  job - The job which the rule customization belongs to.
   * @param[in]  name - The attribute name.
   * @param[in]  value - The attribute value.
   * @return 0 is returned on success, a negative value on failure.
   */
  int modify_rule_attribute (const std::string & job, const std::string & name, const std::string & value);

  /**
   * This method removes the attribute with the provided name from the rule internal collections.
   *
   * @param[in]  job - The job which the rule customization belongs to.
   * @param[in]  name - The attribute name.
   * @return 0 is returned on success, a negative value on failure.
   */
  int remove_rule_attribute (const std::string & job, const std::string & name);

	//==================//
	// Fields Modifiers //
	//==================//
public:
	inline bool is_locked () const { return (_adm_state == ADMINISTRATIVE_STATE_LOCKED); }

	inline std::set<category_t> get_categories () const { return _categories; }

	inline std::string get_description () const { return _description; }

	inline std::string get_name () const { return _name; }

	inline std::string get_recommended_action () const { return _recommended_action; }

	inline bool is_critical () const { return (_severity == SEVERITY_CRITICAL); }

	std::map<std::string, std::pair<std::string, std::string>> get_rule_attributes (const std::string & job);

	inline std::pair<std::string, std::string> get_rule_set_file_info ()const { return _rule_set_file_info; }
	inline void set_rule_set_file_info (const std::string & rsf_id, const std::string & rsf_rev) {
		_rule_set_file_info.first = rsf_id;
		_rule_set_file_info.second = rsf_rev;
	}

	inline std::set<std::string> get_targets () const { return _targets_set; }
	inline int set_targets (std::list<std::string> & target_to_include, std::list<std::string> & target_to_exclude) {
		/*
		 * First, set the include and exclude rule targets.
		 * Then, calculate them according to the cluster configuration.
		 */
		_targets_to_include = target_to_include; _targets_to_exclude = target_to_exclude;
		return calculate_targets();
	}
	int calculate_targets ();

	inline std::string get_reason () const { return _reason; }
	inline void set_reason (const std::string & reason) { _reason = reason; }

  inline hcf::srv::skip_log_t get_skip_log () const { return _skip_log; }
  inline void set_skip_log (const hcf::srv::skip_log_t & skip_log) { _skip_log = skip_log; }

  inline SUFFIX_CONCAT(parameter,SUFFIX_C) * get_parameter_container () const { return _parameter_container; }
  inline void set_parameter_container (SUFFIX_CONCAT(parameter,SUFFIX_C) * parameter_container) { _parameter_container = parameter_container; }

  inline std::vector<std::pair<std::string,std::string> > get_env_variables () const { return _env_variables; }
  inline void set_env_variables (const std::vector<std::pair<std::string,std::string> > & env_variables) { _env_variables = env_variables; }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
public:
	static const char * const HCF_HCRULE_CLASS_NAME;
	static const char * const HCF_HCRULE_IMPLEMENTER_NAME;
	static const char * const HCF_HCRULE_RULE_ID_ATTR_NAME;
	static const char * const HCF_HCRULE_ADM_STATE_ATTR_NAME;
	static const char * const HCF_HCRULE_CATEGORIES_ATTR_NAME;
	static const char * const HCF_HCRULE_INPUT_PARAMS_ATTR_NAME;
	static const char * const HCF_HCRULE_DESCRIPTION_ATTR_NAME;
	static const char * const HCF_HCRULE_NAME_ATTR_NAME;
	static const char * const HCF_HCRULE_RECOMMENDED_ACTION_ATTR_NAME;
	static const char * const HCF_HCRULE_SEVERITY_ATTR_NAME;

private:
	static std::set<std::string> _rules_set_by_category[HCF_MAX_NUMBER_OF_CATEGORIES];
	static TSL_NS_HCF_CORE::thread_mutex_recursive _rules_set_by_category_mutex;

	administrative_state_t _adm_state;
	std::set<category_t> _categories;
	std::map<std::string, std::pair<std::string, std::string>> _rule_default_parameters;
	std::string _description;
	std::string _name;
	std::string _recommended_action;
	severity_t _severity;
	TSL_NS_HCF_CORE::thread_mutex_recursive _rule_parameters_mutex;
	/*
	 * Input parameters view per Job: the map key is the HcJob ID,
	 * while the map value is another map where the key is the
	 * attribute name and the value is the attribute value.
	 */
	std::map<std::string, std::map<std::string, std::string>> _rule_input_parameters;

	// Information not showed in IMM
	std::pair<std::string,std::string> _rule_set_file_info;
	std::list<std::string> _targets_to_include;
	std::list<std::string> _targets_to_exclude;
	std::set<std::string> _targets_set;
	std::string _reason;
	hcf::srv::skip_log_t _skip_log;
	SUFFIX_CONCAT(parameter,SUFFIX_C) * _parameter_container;
	std::vector<std::pair<std::string,std::string> > _env_variables;

	// Class members to be used to implement the rollback mechanism for the abort callback
	administrative_state_t _adm_state_old;
	std::set<category_t> _categories_old;
	std::map<std::string, std::pair<std::string, std::string>> _rule_default_parameters_old;
	std::string _description_old;
	std::string _name_old;
	std::string _recommended_action_old;
	severity_t _severity_old;
};

HCF_NS_HCF_SRV_END

#endif	// __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCRULE__
