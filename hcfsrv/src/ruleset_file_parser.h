#ifndef __INCLUDE_GUARD_CLASS_SRV_CLASS_RULESET_FILE_PARSER__
#define __INCLUDE_GUARD_CLASS_SRV_CLASS_RULESET_FILE_PARSER__ ruleset_file_parser

/*
 *	@file ruleset_file_parser.h
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

#include <string>
#include <vector>
#include <list>
#include <libxml/tree.h>

#include "hcf/core/error_info_provider.h"

#include "srv_macros.h"
#include "app_types.h"
#include "ruleset_file_containers.h"
#include "rule_callbacks_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLASS_RULESET_FILE_PARSER__

#ifndef UTF8_TO_STRING_DEFINED
# define UTF8_TO_STRING_DEFINED
# define UTF8_TO_STRING(s) reinterpret_cast<char *>(s)
#endif

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::error_info_provider {

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
		TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::error_info_provider())
	{}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===============//
	// Nested Classes//
	//===============//
public:
	struct rule_attribute {
		std::string description;
		std::string name;
		std::string value;
		std::string type;
	};

private:
	struct rule_info {
		// Common rule info
		std::string id;
		std::string provider;
		std::string name;
		std::string description;
		std::string recommendedAction;
		std::string reason;
		severity_t severity;
		skip_log_t skip_log;
		std::list<category_t> categories;
		std::list<std::string> exclude_targets;
		std::list<std::string> include_targets;

		// Customizable rule attributes
		std::vector<rule_attribute> attributes;
		std::vector<std::pair<std::string, std::string> > env_variables;

		SUFFIX_CONCAT(parameter,SUFFIX_C) * parameters;
	};

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This static method initializes the parser class.
	 * It's needed to call it before any other type of operation on the parser (also declarations).
	 */
	static void initialize ();

	/**
	 * This static method finalizes the parser class.
	 * It's needed to call it after any other type of operation on the parser (also implicit destructors).
	 */
	static void finalize ();

	/**
	 * This method performs the parsing of the rule set file stored in the given path.
	 * Before parsing, also the validation against the provided XSD schema is performed.
	 *
	 * @param[in]  ruleset_file_path - The path where the ruleset file is located.
	 * @param[in]  schema_file_path - The path where the XSD schema file is located.
	 * @param[out] rule_set_file_info - A pair containing the RSF info (name, revision).
	 * @param[in]  error_on_duplicate_rule - A flag indicating if the parser must return an error in case of duplicate HcRule objects.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int parse (const char * ruleset_file_path, const char * schema_file_path = 0, std::pair<std::string, std::string> * rule_set_file_info = 0, bool error_on_duplicate_rule = false);

	/**
	 * This method performs the validation of the XML file stored in the given path
	 * against the provided XSD schema file.
	 *
	 * @param[in]  ruleset_file_path - The path where the ruleset file is located.
	 * @param[in]  schema_file_path - The path where the XSD schema file is located.
	 * @return an integer value, according to the following logic:
	 * 				0   if the XML file is valid.
	 * 				> 0 if the XML file is not valid.
	 * 				-1  in case of validation failure, for internal errors.
	 */
	int validate (const char * ruleset_file_path, const char * schema_file_path);

	/**
	 * This method extracts from the rule set file some useful info (as its number and revision).
	 *
	 * @param[in]  ruleset_file_path - The path where the ruleset file is located.
	 * @param[in]  schema_file_path - The path where the XSD schema file is located.
	 * @param[out] rule_set_file_info - A pair containing the RSF info (name, revision).
	 * @return 0 on success, -1 on failure.
	 */
	int get_rsf_info (const char * ruleset_file_path, const char * schema_file_path, std::pair<std::string,std::string> * rule_set_file_info);

private:
	/**
	 * This method performs the validation against the provided XSD schema file.
	 * The input XML file to be validated is an already parsed file.
	 *
	 * @param[in]  ruleset_file - The already parsed ruleset XML file.
	 * @param[in]  schema_file_path - The path where the XSD schema file is located.
	 * @return an integer value, according to the following logic:
	 * 				0   if the XML file is valid.
	 * 				> 0 if the XML file is not valid.
	 * 				-1 	 in case of validation failure, for internal errors.
	 */
	int validate (xmlDoc * ruleset_file, const char * schema_file_path);

	/**
	 * This method performs the parsing of <rule> tag present into the rule set file.
	 *
	 * @param[in]  rule - The tree with <rule> as root.
	 * @param[in]  rule_info - The info extracted from the rule set file.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int parse_rule (xmlNode * rule, struct rule_info & rule_info);

	/**
	 * This method performs the parsing of <attributes> tag present into each rule in a rule set file.
	 *
	 * @param[in]  rule_attributes - The tree with <attributes> as root.
	 * @param[in]  rule_info - The info extracted from the rule set file.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int parse_rule_attributes (xmlNode * rule_attributes, struct rule_info & rule_info);

	/**
	 * This method performs the parsing of <check> tag present into each rule in a rule set file.
	 *
	 * @param[in]  rule_check - The tree with <check> as root.
	 * @param[in]  rule_info - The info extracted from the rule set file.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int parse_rule_check (xmlNode * rule_check, struct rule_info & rule_info);

	/**
	 * This method performs the parsing of <printout> tag present into each rule in a rule set file.
	 *
	 * @param[in]  rule_check_printout - The printout element node with <os> as root.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int parse_rule_check_printout (xmlNode * rule_check_printout, SUFFIX_CONCAT(os,SUFFIX_C) & os_c);

	/**
	 * This method performs the parsing of <printout> tag present into each rule in a rule set file.
	 *
	 * @param[in]  rule_check_filter - The filter element node with <os> as root.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int parse_rule_check_filter (xmlNode * rule_check_filter, SUFFIX_CONCAT(filter,SUFFIX_C) & filter_c, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) & filter_parameter_c);

	/**
	 * This method performs the parsing of <printout> tag present into each rule in a rule set file.
	 *
	 * @param[in]  rule_check_eval - The eval element node with <check> as root.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int parse_rule_check_eval (xmlNode * rule_check_eval, SUFFIX_CONCAT(evaluation,SUFFIX_C) & eval_c, SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) & eval_parameter_c);

	/**
	 * This method implements the callback function to be called by libXML2 library when an error occurs.
	 *
	 * @param[in]  context - The context in which the error occurred.
	 * @param[in]  format - The error message to be logged.
	 */
	static void log_xml_error (void * context, const char * format, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * This method implements the callback function to be called by libXML2 library when a warning occurs.
	 *
	 * @param[in]  context - The context in which the error occurred.
	 * @param[in]  format - The error message to be logged.
	 */
	static void log_xml_warning (void * context, const char * format, ...) __attribute__ ((format (printf, 2, 3)));

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	static int _parser_initialized;
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_CLASS_RULESET_FILE_PARSER__ */
