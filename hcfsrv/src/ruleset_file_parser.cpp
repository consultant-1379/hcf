#include <string.h>
#include <string>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include "hcf/core/logger.h"
#include "hcf/imm/imm_config_class.h"

#include "constants.h"
#include "srv_utils.h"
#include "imm_object_hcrule.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_rulesetfileparser.h"
#include "hcf/trace/tracing_macros.h"

#include "ruleset_file_parser.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLASS_RULESET_FILE_PARSER__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule> hcrule_class_handler;

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
int __TSL_CLASS_NAME__::_parser_initialized = 0;

void __TSL_CLASS_NAME__::initialize () {
	HCF_CC_TRACE_FUNCTION;
	if (_parser_initialized) {
		HCF_CC_TRACE(Initialize, "The RSF parser object has been already initialized, nothing to do!");
		return;
	}

	// Register and handler for the generic XML errors
	HCF_CC_TRACE(Initialize, "Received a request to initialize the RSF parser: registering the error message callback!");
	xmlSetGenericErrorFunc(0, log_xml_error);

	_parser_initialized = 1;
}

void __TSL_CLASS_NAME__::finalize () {
	HCF_CC_TRACE_FUNCTION;
	if (!_parser_initialized) {
		HCF_CC_TRACE(Finalize, "The RSF parser object has been already finalized, nothing to do!");
		return;
	}
	HCF_CC_TRACE(Finalize, "Finalizing the RSF parser object!");
	_parser_initialized = 0;
}

int __TSL_CLASS_NAME__::parse (const char * ruleset_file_path, const char * schema_file_path, std::pair<std::string, std::string> * rule_set_file_info, bool error_on_duplicate_rule) {
	HCF_CC_TRACE_FUNCTION;

	// First, parse the XML file into a tree structure in memory
	HCF_CC_TRACE(Parse, "Received a request to parse a RSF: first, try to open the following file:", ruleset_file_path);
	xmlDoc * ruleset_file = 0;
	if (!(ruleset_file = xmlReadFile(ruleset_file_path, 0, 0))) {
		HCF_CC_TRACE(ParseErr, "Call 'xmlReadFile' failed.", ERR_RSF_XML_PARSING_FAILED);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_PARSING_FAILED, "Failed to parse the XML file having path == '%s'", ruleset_file_path);
	}
	HCF_CC_LOG(LEVEL_NOTICE, "XML File having path == '%s' correctly parsed!", ruleset_file_path);

	// Then, if required, validate the parsed file against the XSD schema
	HCF_CC_TRACE(Parse, "After a successful file opening operation, let's validate it against the provided XSD schema (if needed):", schema_file_path);
	if (schema_file_path) {
		const int validation_result = validate(ruleset_file, schema_file_path);
		if (validation_result < 0) {
			// The validation is failed, log the error and return -1 (the error message has been already set)
			HCF_CC_LOG(LEVEL_ERROR, "Call 'validate' failed for XML file having path == '%s' and schema having "
					"path == '%s', validation_result == %d!", ruleset_file_path, schema_file_path, validation_result);
			HCF_CC_TRACE(ParseErr, "Call 'validate' failed.", validation_result);
			xmlFreeDoc(ruleset_file);
			return -1;
		}

		// Validation operation correctly performed. Check if the file is valid
		if (validation_result != 0) {
			// The file is not valid
			xmlFreeDoc(ruleset_file);
			HCF_CC_TRACE(Parse, "The provided RSF is NOT valid for the provided schema, validation returned:", std::to_string(validation_result).c_str());
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_FILE_NOT_VALID, "The XML file having path == '%s' is not valid for "
					"the XSD schema having path == '%s'", ruleset_file_path, schema_file_path);
		}

		// The XML file is valid!
		HCF_CC_LOG(LEVEL_NOTICE, "The XML file having path == '%s' is valid for the schema having path == '%s'!", ruleset_file_path, schema_file_path);
		HCF_CC_TRACE(Parse, "The provided RSF is valid for the provided schema!", "");
	}

	// Begin the parsing operation: get the root (RuleSetFile) element
	HCF_CC_TRACE(Parse, "Parsing the document root tag!", "<RuleSetFile>");
	xmlNode * rulesetfile_element = 0;
	if (!(rulesetfile_element = xmlDocGetRootElement(ruleset_file))) {
		// NULL root element provided, the XML file is empty!
		xmlFreeDoc(ruleset_file);
		HCF_CC_TRACE(ParseErr, "Call 'xmlDocGetRootElement' failed.", ERR_RSF_XML_FILE_EMPTY);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_FILE_EMPTY, "The XML file having path == '%s' is empty!", ruleset_file_path);
	}

	// Extract from the element the RuleSetFile ID and Revision
	std::string rsf_id;
	std::string rsf_rev;
	HCF_CC_TRACE(Parse, "Extracting the attributes of the following element:", "<RuleSetFile>");
	for (xmlAttr * rulesetfile_attr = rulesetfile_element->properties; rulesetfile_attr; rulesetfile_attr = rulesetfile_attr->next)
	{
		// Extract the rule attribute information according to the attribute name
		if (xmlStrEqual(rulesetfile_attr->name, reinterpret_cast<const unsigned char *>("id"))) {
			rsf_id = UTF8_TO_STRING(rulesetfile_attr->children->content);
			HCF_CC_TRACE(Parse, "The value of the id element is:", rsf_id.c_str());
		}
		else if (xmlStrEqual(rulesetfile_attr->name, reinterpret_cast<const unsigned char *>("revision"))) {
			rsf_rev = UTF8_TO_STRING(rulesetfile_attr->children->content);
			HCF_CC_TRACE(Parse, "The value of the revision element is:", rsf_rev.c_str());
		}
		else {
			// ERROR: found an unknown attribute into the rule set file, very strange!
			HCF_CC_TRACE(ParseErr, "Unexpected attribute found in <RuleSetFile> element!", ERR_RSF_XML_BAD_ELEMENT);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <RuleSetFile>!", rulesetfile_attr->name);
		}
	}
	HCF_CC_LOG(LEVEL_NOTICE, "Rule Set File properties: id == %s, revision == %s", rsf_id.c_str(), rsf_rev.c_str());

	if (rule_set_file_info) {
		rule_set_file_info->first = rsf_id;
		rule_set_file_info->second = rsf_rev;
	}

	std::set<std::string> installed_rules;

	// A rule set file is built by a group of rules, parse them
	HCF_CC_TRACE(Parse, "Extracting all the children objects for the following one:", "<RuleSetFile>");
	for (xmlNode * rule_element = rulesetfile_element->children; rule_element; rule_element = rule_element->next) {
		/*
		 * Take only rules, that are ELEMENT nodes: ignore all the other
		 * (TEXT) nodes (since are only not printable characters).
		 */
		if (rule_element->type != XML_ELEMENT_NODE)	continue;

		HCF_CC_TRACE(Parse, "Parsing the following element:", "<Rule>");
		rule_info rule_info;
		if (parse_rule(rule_element, rule_info) < 0) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'parse_rule' failed, error_code == %d!", error());
			HCF_CC_TRACE(ParseErr, "Call 'parse_rule' failed.", error());
			xmlFreeDoc(ruleset_file);
			return -1;
		}

		// After parsing the <rule> element, create the HcRule object with the parsed info
		std::string rule_id = rule_info.provider + "_" + rule_info.id;
		HCF_CC_TRACE(Parse, "Rule object with the following ID correctly parsed, creating the HcRule object!", rule_id.c_str());
		if (imm_object_hcrule::create_hcrule(rule_id.c_str(), rule_info.categories, rule_info.severity, rule_info.recommendedAction.c_str(), rule_info.name.c_str(), rule_info.description.c_str(), rule_info.attributes, error_on_duplicate_rule)) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'create_hcrule' failed, for rule having id == '%s'", rule_id.c_str());
			xmlFreeDoc(ruleset_file);
			HCF_CC_TRACE(ParseErr, "Call 'create_hcrule' failed.", ERR_RSF_IMM_RULE_CREATE_FAILED);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_IMM_RULE_CREATE_FAILED, "Failed to create the HcRule object with id == '%s'", rule_id.c_str());
		}

		// Insert the element into the installed rules set. If a rule with the same ID already exists, return error
		HCF_CC_TRACE(Parse, "Adding the created rule to the set of the installed ones!", rule_id.c_str());
		if (!installed_rules.insert(rule_id).second) {
			HCF_CC_LOG(LEVEL_WARNING, "The rule having ID == '%s' was already installed!!!", rule_id.c_str());
			xmlFreeDoc(ruleset_file);
			HCF_CC_TRACE(ParseErr, "The provided rule has been already installed!", ERR_RSF_IMM_RULE_DUPLICATED);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_IMM_RULE_DUPLICATED, "The rule having ID == '%s' was already installed!!!", rule_id.c_str());
		}

		// Get the a reference to the rule previously created, in order to add other info not included in IMM
		HCF_CC_TRACE(Parse, "Retrieving the instance of the rule previously created!", rule_id.c_str());
		imm_object_hcrule * rule_ptr = hcrule_class_handler.get_instance(rule_id);
		if (!rule_ptr) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'get_instance' failed for HcRule having id == '%s', very strange!", rule_id.c_str());
			xmlFreeDoc(ruleset_file);
			HCF_CC_TRACE(ParseErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_RSF_IMM_RULE_GET_INSTANCE_FAILED);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_IMM_RULE_GET_INSTANCE_FAILED, "Got NULL pointer for the HcRule having id == '%s'!", rule_id.c_str());
		}

		/*
		 * Set into the HcRule object all the info not present into IMM but needed for rule evaluation:
		 * 	- The targets of the rule execution.
		 * 	- The command to be executed.
		 * 	- The error message to be showed when the rule execution fails.
		 */
		HCF_CC_TRACE(Parse, "Adding all the needed targets to the created rule!", rule_id.c_str());
		int call_result = 0;
		if ((call_result = rule_ptr->set_targets(rule_info.include_targets, rule_info.exclude_targets))) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'rule_ptr->set_targets' failed, error_code == %d", call_result);
			HCF_CC_TRACE(ParseErr, "Call 'rule_ptr->set_targets' failed: deleting the HcRule object!", call_result);

			// Failed to set the targets, remove the rule object previously created
			if ((call_result = rule_ptr->delete_object())) {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'rule_ptr->delete_object' failed, error_code == %d", call_result);
				HCF_CC_TRACE(ParseErr, "Call 'rule_ptr->delete_object' failed.", call_result);
				TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, "Failed to remove the HcRule object after setting targets failure!");
			}
			TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, "Failed to set the HcRule targets!");
		}

		HCF_CC_TRACE(Parse, "Adding to the rule object all the information not stored into IMM!", rule_id.c_str());
		rule_ptr->set_rule_set_file_info(rsf_id, rsf_rev);
		rule_ptr->set_reason(rule_info.reason);
		rule_ptr->set_skip_log(rule_info.skip_log);
		rule_ptr->set_parameter_container(rule_info.parameters);
		rule_ptr->set_env_variables(rule_info.env_variables);
	}

	// After terminate the parsing operation, free the allocated memory and reset the error
	xmlFreeDoc(ruleset_file);
	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::validate (const char * ruleset_file_path, const char * schema_file_path) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Validate, "Received a request to validate a RSF with its XSD schema: first, opening the file!", ruleset_file_path);

	// First, read and parse the XML rule set file
	xmlDoc * ruleset_file = 0;
	if (!(ruleset_file = xmlReadFile(ruleset_file_path, 0, 0))) {
		HCF_CC_TRACE(ValidateErr, "Call 'xmlReadFile' failed.", ERR_RSF_XML_PARSING_FAILED);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_PARSING_FAILED,
				"Failed to parse the XML file having path == '%s'", ruleset_file_path);
	}
	HCF_CC_LOG(LEVEL_NOTICE, "XML File having path == '%s' correctly parsed, checking if "
			"it's valid for the schema having path == '%s'!", ruleset_file_path, schema_file_path);
	HCF_CC_TRACE(Validate, "RSF file correctly opened, checking if it's valid for the XSD schema!", schema_file_path);

	// Then, delegate the private method to check the XML file validity
	const int call_result = validate(ruleset_file, schema_file_path);
	HCF_CC_TRACE(Validate, "The validation result is:", std::to_string(call_result).c_str());

	// After checking the file validity, free the allocated memory
	xmlFreeDoc(ruleset_file);
	return call_result;
}

int __TSL_CLASS_NAME__::validate (xmlDoc * ruleset_file, const char * schema_file_path) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Validate, "Creating the schema parser context for the following schema:", schema_file_path);

	// First, create the schema parser context
	xmlSchemaParserCtxt * schema_parser_context = 0;
	if (!(schema_parser_context = xmlSchemaNewParserCtxt(schema_file_path))) {
		// Failed to initialize the schema parser context, set the error and return
		HCF_CC_TRACE(ValidateErr, "Call 'xmlSchemaNewParserCtxt' failed.", ERR_RSF_XSD_CTX_INIT_FAILED);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XSD_CTX_INIT_FAILED, "Failed to initialize the parser "
				"context for the XSD schema having path == '%s'", schema_file_path);
	}

	// Register an handler for the schema parsing error/warning messages
	HCF_CC_TRACE(Validate, "Registering a callback function for the parsing messages!", schema_file_path);
	xmlSchemaSetParserErrors(schema_parser_context, log_xml_error, log_xml_warning, 0);

	// Then, using the previously created context, parse the XSD schema file and free the context
	HCF_CC_TRACE(Validate, "Parsing the following XSD schema file:", schema_file_path);
	xmlSchema * schema_file = xmlSchemaParse(schema_parser_context);
	xmlSchemaFreeParserCtxt(schema_parser_context);
	if (!schema_file) {
		// Failed to parse the schema file, probably it contains some errors
		HCF_CC_TRACE(ValidateErr, "Call 'xmlSchemaParse' failed.", ERR_RSF_XSD_PARSING_FAILED);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XSD_PARSING_FAILED,
				"Failed to parse the XSD schema having path == '%s'", schema_file_path);
	}

	// After parsing the schema, create a validation context
	HCF_CC_TRACE(Validate, "Creating the schema validation context for the following schema:", schema_file_path);
	xmlSchemaValidCtxt * validation_context = 0;
	if (!(validation_context = xmlSchemaNewValidCtxt(schema_file))) {
		// Failed to initialize the validation context, set the error, free memory and return
		xmlSchemaFree(schema_file);
		HCF_CC_TRACE(ValidateErr, "Call 'xmlSchemaNewValidCtxt' failed.", ERR_RSF_XSD_CTX_INIT_FAILED);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XSD_CTX_INIT_FAILED, "Failed to initialize the validation "
				"context for the XSD schema having path == '%s'", schema_file_path);
	}

	// Register an handler for the validation error/warning messages
	HCF_CC_TRACE(Validate, "Registering a callback function for the validation messages!", schema_file_path);
	xmlSchemaSetValidErrors(validation_context, log_xml_error, log_xml_warning, 0);

	// Perform the validation operation for the provided XML document
	HCF_CC_TRACE(Validate, "Validating the RSF against the following XSD schema file:", schema_file_path);
	const int validation_result = xmlSchemaValidateDoc(validation_context, ruleset_file);
	if (validation_result < 0) {
		// Error occurred while trying to validate the file, set the error message
		HCF_CC_TRACE(ValidateErr, "Call 'xmlSchemaValidateDoc' failed.", validation_result);
		set_error_info(ERR_RSF_XSD_VALIDATION_FAILED, "Failed to perform the validation "
				"operation for the XSD schema having path == '%s'", schema_file_path);
	}
	else {
		// Validation operation successful, check if the file it's valid or not
		HCF_CC_LOG(LEVEL_NOTICE, "Call 'xmlSchemaValidateDoc' returned '%s', call_result == %d",
				(validation_result ? "NOT VALID" : "VALID"), validation_result);
		HCF_CC_TRACE(Validate, "The RSF validation result is:", (validation_result ? "NOT VALID" : "VALID"));
	}

	// After performing the validation operation, free the allocated memory
	xmlSchemaFreeValidCtxt(validation_context);
	xmlSchemaFree(schema_file);
	return validation_result;
}

int __TSL_CLASS_NAME__::parse_rule (xmlNode * rule, struct rule_info & rule_info) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ParseRule, "Extracting the attributes of the following element:", "<Rule>");

	// Extract from the element the Rule ID, the Rule provider and the Rule name
	for (xmlAttr * rule_attr = rule->properties; rule_attr; rule_attr = rule_attr->next)
	{
		// Extract the rule attribute information according to the attribute name
		if (xmlStrEqual(rule_attr->name, reinterpret_cast<const unsigned char *>("id"))) {
			rule_info.id = UTF8_TO_STRING(rule_attr->children->content);
			HCF_CC_TRACE(ParseRule, "The value of id attribute is:", rule_info.id.c_str());
		}
		else if (xmlStrEqual(rule_attr->name, reinterpret_cast<const unsigned char *>("provider"))) {
			rule_info.provider = UTF8_TO_STRING(rule_attr->children->content);
			HCF_CC_TRACE(ParseRule, "The value of provider attribute is:", rule_info.provider.c_str());
		}
		else if (xmlStrEqual(rule_attr->name, reinterpret_cast<const unsigned char *>("name"))) {
			rule_info.name = UTF8_TO_STRING(rule_attr->children->content);
			HCF_CC_TRACE(ParseRule, "The value of name attribute is:", rule_info.name.c_str());
		}
    else if (xmlStrEqual(rule_attr->name, reinterpret_cast<const unsigned char *>("skipLog"))) {
      rule_info.skip_log = (utils::map_skip_log_string_to_skip_log_value(UTF8_TO_STRING(rule_attr->children->content)));
			HCF_CC_TRACE(ParseRule, "The value of skipLog attribute is:", utils::map_skip_log_value_to_skip_log_string(rule_info.skip_log));
    }
		else {
			// ERROR: found an unknown attribute into the rule set file, very strange!
			HCF_CC_TRACE(ParseRuleErr, "Unexpected attribute found in <Rule> element!", ERR_RSF_XML_BAD_ELEMENT);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Rule>!", rule_attr->name);
		}
	}

	// A rule is build by a group of informations, that are its direct children: extract them
	HCF_CC_TRACE(ParseRule, "Extracting all the children objects for the following one:", "<Rule>");
	for (xmlNode * rule_data = rule->children; rule_data; rule_data = rule_data->next) {
		/*
		 * Take only rules, that are ELEMENT nodes: ignore all the other
		 * (TEXT) nodes (since are only not printable characters).
		 */
		if (rule_data->type != XML_ELEMENT_NODE)	continue;

		/*
		 * Extract from the current node its information and store it
		 * in the proper structure field according to the node name.
		 */
		if (xmlStrEqual(rule_data->name, reinterpret_cast<const unsigned char *>("Description"))) {
			rule_info.description = UTF8_TO_STRING(rule_data->children->content);
			HCF_CC_TRACE(ParseRule, "The value of Description element is:", rule_info.description.c_str());
		}
		else if (xmlStrEqual(rule_data->name, reinterpret_cast<const unsigned char *>("RecommendedAction"))) {
			rule_info.recommendedAction = UTF8_TO_STRING(rule_data->children->content);
			HCF_CC_TRACE(ParseRule, "The value of RecommendedAction element is:", rule_info.recommendedAction.c_str());
		}
		else if (xmlStrEqual(rule_data->name, reinterpret_cast<const unsigned char *>("Reason"))) {
			rule_info.reason = UTF8_TO_STRING(rule_data->children->content);
			HCF_CC_TRACE(ParseRule, "The value of Reason element is:", rule_info.reason.c_str());
		}
		else if (xmlStrEqual(rule_data->name, reinterpret_cast<const unsigned char *>("Severity"))) {
			const char * severity_string = UTF8_TO_STRING(rule_data->children->content);
			rule_info.severity = (!::strcmp(severity_string, "WARNING")) ? SEVERITY_WARNING : SEVERITY_CRITICAL;
			HCF_CC_TRACE(ParseRule, "The value of Severity element is:", severity_string);
		}
		else if (xmlStrEqual(rule_data->name, reinterpret_cast<const unsigned char *>("Attributes"))) {
			HCF_CC_TRACE(ParseRule, "Parsing the following element:", "<Attributes>");
			if (parse_rule_attributes(rule_data, rule_info) < 0) {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'parse_rule_attributes' failed, return_code == %d", error());
				HCF_CC_TRACE(ParseRuleErr, "Call 'parse_rule_attributes' failed.", error());
				return -1;
			}
		}
		else if (xmlStrEqual(rule_data->name, reinterpret_cast<const unsigned char *>("Category"))) {
			rule_info.categories.push_back(utils::map_category_string_to_category_value(UTF8_TO_STRING(rule_data->children->content)));
			HCF_CC_TRACE(ParseRule, "The value of Category element is:", UTF8_TO_STRING(rule_data->children->content));
		}
		else if (xmlStrEqual(rule_data->name, reinterpret_cast<const unsigned char *>("Check"))) {
			HCF_CC_TRACE(ParseRule, "Parsing the following element:", "<Check>");
			if (parse_rule_check(rule_data, rule_info) < 0) {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'parse_rule_check' failed, return_code == %d", error());
				HCF_CC_TRACE(ParseRuleErr, "Call 'parse_rule_check' failed.", error());
				return -1;
			}
		}
		else {
			// ERROR: found an unknown node into the rule set file, very strange!
			HCF_CC_TRACE(ParseRuleErr, "Unexpected child element found in <Rule> element!", ERR_RSF_XML_BAD_ELEMENT);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Rule>!", rule_data->name);
		}
	}
	HCF_CC_LOG(LEVEL_INFO, "Rule[%s, %s]: Description == '%s', RecommendedAction == '%s', Reason == '%s', Severity == '%d', "
			"Category list size == '%lu'\n", rule_info.id.c_str(), rule_info.name.c_str(), rule_info.description.c_str(),
			rule_info.recommendedAction.c_str(), rule_info.reason.c_str(), rule_info.severity, rule_info.categories.size());
	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::parse_rule_attributes (xmlNode * rule_attributes, struct rule_info & rule_info) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ParseRuleAttributes, "Extracting all the children objects for the following one:", "<Attributes>");

	// A rule can contain three types of information: optional arguments, target to be excluded and target to be included
	for (xmlNode * attr = rule_attributes->children; attr; attr = attr->next) {
		/*
		 * Take only attributes, that are ELEMENT nodes: ignore all the other
		 * (TEXT) nodes (since are only not printable characters).
		 */
		if (attr->type != XML_ELEMENT_NODE)	continue;

		/*
		 * Extract from the current node its information and store it
		 * in the proper structure field according to the node name.
		 */
		if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Params"))) {
			HCF_CC_TRACE(ParseRuleAttributes, "Extracting all the children objects for the following one:", "<Params>");

			// After understanding the node type, extract the attribute info from its children
			for (xmlNode * child = attr->children; child; child = child->next) {
				if (child->type != XML_ELEMENT_NODE)	continue;

				if (xmlStrEqual(child->name, reinterpret_cast<const unsigned char *>("Param"))) {
					// The param info are attributes of <param> element: fetch them an store into the rule info structure
					HCF_CC_TRACE(ParseRuleAttributes, "Extracting the attributes of the following element:", "<Param>");
					rule_attribute rule_attribute;
					for (xmlAttr * param_attr = child->properties; param_attr; param_attr = param_attr->next) {

						// Extract the rule attribute information according to the attribute name
						if (xmlStrEqual(param_attr->name, reinterpret_cast<const unsigned char *>("description"))) {
							rule_attribute.description = UTF8_TO_STRING(param_attr->children->content);
							HCF_CC_TRACE(ParseRuleAttributes, "The value of description attribute is:", rule_attribute.description.c_str());
						}
						else if (xmlStrEqual(param_attr->name, reinterpret_cast<const unsigned char *>("name"))) {
							rule_attribute.name = UTF8_TO_STRING(param_attr->children->content);
							HCF_CC_TRACE(ParseRuleAttributes, "The value of name attribute is:", rule_attribute.name.c_str());
						}
						else if (xmlStrEqual(param_attr->name, reinterpret_cast<const unsigned char *>("defaultValue"))) {
							rule_attribute.value = UTF8_TO_STRING(param_attr->children->content);
							HCF_CC_TRACE(ParseRuleAttributes, "The value of defaultValue attribute is:", rule_attribute.value.c_str());
						}
						else if (xmlStrEqual(param_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
							rule_attribute.type = UTF8_TO_STRING(param_attr->children->content);
							HCF_CC_TRACE(ParseRuleAttributes, "The value of type attribute is:", rule_attribute.type.c_str());
						}
						else {
							// ERROR: found an unknown attribute into the rule set file, very strange!
							HCF_CC_TRACE(ParseRuleAttributesErr, "Unexpected attribute found in <Param> element!", ERR_RSF_XML_BAD_ELEMENT);
							TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Param>!", param_attr->name);
						}
					}
					rule_info.attributes.push_back(rule_attribute);
				}
				else if (xmlStrEqual(child->name, reinterpret_cast<const unsigned char *>("EnvVariable"))) {
					// The environment variables info are attributes of <EnvVariable> element: fetch them an store into the rule info structure
					HCF_CC_TRACE(ParseRuleAttributes, "Extracting the attributes of the following element:", "<EnvVariable>");
					std::pair<std::string, std::string> env_variable;

					for (xmlAttr * env_attr = child->properties; env_attr; env_attr = env_attr->next) {
						// Extract the environment variable information according to the attribute name
						if (xmlStrEqual(env_attr->name, reinterpret_cast<const unsigned char *>("name"))) {
							env_variable.first = UTF8_TO_STRING(env_attr->children->content);
							HCF_CC_TRACE(ParseRuleAttributes, "The value of name attribute is:", env_variable.first.c_str());
						}
						else if (xmlStrEqual(env_attr->name, reinterpret_cast<const unsigned char *>("value"))) {
							env_variable.second = UTF8_TO_STRING(env_attr->children->content);
							HCF_CC_TRACE(ParseRuleAttributes, "The value of value attribute is:", env_variable.second.c_str());
						}
						else {
							// ERROR: found an unknown attribute into the rule set file, very strange!
							HCF_CC_TRACE(ParseRuleAttributesErr, "Unexpected attribute found in <EnvVariable> element!", ERR_RSF_XML_BAD_ELEMENT);
							TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <EnvVariable>!", env_attr->name);
						}
					}
					rule_info.env_variables.push_back(env_variable);
				}
				else {
					// ERROR: found an unknown node into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleAttributesErr, "Unexpected attribute found in <Params> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Params>!", child->name);
				}
			}
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Include"))) {
			HCF_CC_TRACE(ParseRuleAttributes, "Extracting all the children objects for the following one:", "<Include>");

			// After understanding the node type, extract the attribute info from its children
			for (xmlNode * child = attr->children; child; child = child->next) {
				if (child->type != XML_ELEMENT_NODE)	continue;

				if (xmlStrEqual(child->name, reinterpret_cast<const unsigned char *>("Target"))) {
					rule_info.include_targets.push_back(UTF8_TO_STRING(child->children->content));
					HCF_CC_TRACE(ParseRuleAttributes, "The value of Target element is:", UTF8_TO_STRING(child->children->content));
				}
				else {
					// ERROR: found an unknown node into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleAttributesErr, "Unexpected attribute found in <Include> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <c>!", child->name);
				}
			}
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Exclude"))) {
			HCF_CC_TRACE(ParseRuleAttributes, "Extracting all the children objects for the following one:", "<Exclude>");

			// After understanding the node type, extract the attribute info from its children
			for (xmlNode * child = attr->children; child; child = child->next) {
				if (child->type != XML_ELEMENT_NODE)	continue;

				if (xmlStrEqual(child->name, reinterpret_cast<const unsigned char *>("Target"))) {
					rule_info.exclude_targets.push_back(UTF8_TO_STRING(child->children->content));
					HCF_CC_TRACE(ParseRuleAttributes, "The value of Target element is:", UTF8_TO_STRING(child->children->content));
				}
				else {
					// ERROR: found an unknown node into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleAttributesErr, "Unexpected attribute found in <Exclude> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Exclude>!", child->name);
				}
			}
		}
		else {
			// ERROR: found an unknown node into the rule set file, very strange!
			HCF_CC_TRACE(ParseRuleAttributesErr, "Unexpected attribute found in <Attributes> element!", ERR_RSF_XML_BAD_ELEMENT);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Attributes>!", attr->name);
		}
	}
	HCF_CC_LOG(LEVEL_INFO, "Rule[%s, %s]: include_targets(count) == %lu, exclude_targets(count) == %lu, "
			"attributes(count) == %lu", rule_info.id.c_str(), rule_info.name.c_str(),
			rule_info.include_targets.size(), rule_info.exclude_targets.size(), rule_info.attributes.size());
	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::parse_rule_check (xmlNode * rule_check, struct rule_info & rule_info) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ParseRuleCheck, "Extracting all the children objects for the following one:", "<Check>");

	rule_info.parameters = new SUFFIX_CONCAT(parameter,SUFFIX_C);

	for (xmlNode * attr = rule_check->children; attr; attr = attr->next) {
		if (attr->type != XML_ELEMENT_NODE)	continue;

		/*
		 * Extract from the current node its information and store it
		 * in the proper structure field according to the node name.
		 */
		if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("ExecEnvironment"))) {
			HCF_CC_TRACE(ParseRulePrintout, "Extracting the attributes of the following element:", "<ExecEnvironment>");

			SUFFIX_CONCAT(execEnv,SUFFIX_C) execEnv_c;

			// Extract from the element the executeOnConditionMatch attribute
			for (xmlAttr * exec_env_attr = attr->properties; exec_env_attr; exec_env_attr = exec_env_attr->next) {

				// Extract the attribute information according to the attribute name
				if (xmlStrEqual(exec_env_attr->name, reinterpret_cast<const unsigned char *>("executeOnConditionMatch"))) {
					execEnv_c._execOnCondMatch = UTF8_TO_STRING(exec_env_attr->children->content);
					HCF_CC_TRACE(ParseRuleCheck, "The value of executeOnConditionMatch attribute is:", execEnv_c._execOnCondMatch.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleCheckErr, "Unexpected attribute found in <ExecEnvironment> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <ExecEnvironment>!", exec_env_attr->name);
				}
			}

			HCF_CC_TRACE(ParseRuleCheck, "Extracting all the children objects for the following one:", "<ExecEnvironment>");
			xmlNode * execEnv_node = attr;
			for (xmlNode * attr2 = execEnv_node->children; attr2; attr2 = attr2->next) {
				if (attr2->type != XML_ELEMENT_NODE)	continue;

				if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("ExecEnvCheck"))) {
					execEnv_c._execEnvCheck = UTF8_TO_STRING(attr2->children->content);
					HCF_CC_TRACE(ParseRuleCheck, "The value of ExecEnvCheck element is:", execEnv_c._execEnvCheck.c_str());
				}
				else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("ExecEnvCondition"))) {
					std::string execEnvCond = UTF8_TO_STRING(attr2->children->content);
					HCF_CC_TRACE(ParseRuleCheck, "The value of ExecEnvCondition element is:", execEnvCond.c_str());
					execEnv_c._execEnvCondition_list.push_back(execEnvCond);
				}
				else {
					// ERROR: found an unknown node into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleCheckErr, "Unexpected attribute found in <ExecEnvironment> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <ExecEnvironment>!", attr2->name);
				}
			}
			rule_info.parameters->execEnv_list.push_back(execEnv_c);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Os"))) {
			HCF_CC_TRACE(ParseRuleCheck, "Extracting all the children objects for the following one:", "<Os>");
			SUFFIX_CONCAT(os,SUFFIX_C) os_c;

			xmlNode * os_node = attr;
			for (xmlNode * attr2 = os_node->children; attr2; attr2 = attr2->next) {
				if (attr2->type != XML_ELEMENT_NODE)	continue;

				if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Command"))) {
					os_c.command = UTF8_TO_STRING(attr2->children->content);
					//////////////////////////////////////////////////////////////////////////////////////////////////////
					// XCASALE: Command could store sensitive data: uncomment the following line only for debug purposes//
					//////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0	// Uncomment for debug
					HCF_CC_TRACE(ParseRuleCheck, "The value of Command element is:", os_c.command.c_str());
#endif
				}
				else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Printout"))) {
					HCF_CC_TRACE(ParseRuleCheck, "Parsing the following element:", "<Printout>");
					if(parse_rule_check_printout(attr2, os_c) < 0)
					{
						HCF_CC_LOG(LEVEL_ERROR, "Error while parsing <%s> tag", attr2->name);
						HCF_CC_TRACE(ParseRuleCheckErr, "Call 'parse_rule_check_printout' failed.", error());
						return -1;
					}
				}
				else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Filter"))) {
					HCF_CC_TRACE(ParseRuleCheck, "Parsing the following element:", "<Filter>");
					SUFFIX_CONCAT(filter,SUFFIX_C) filter_c;

					if(parse_rule_check_filter(attr2, filter_c, filter_c.filter_parameter) < 0)
					{
						HCF_CC_LOG(LEVEL_ERROR, "Error while parsing <%s> tag", attr2->name);
						HCF_CC_TRACE(ParseRuleCheckErr, "Call 'parse_rule_check_filter' failed.", error());
						return -1;
					}

					os_c.filter_list.push_back(filter_c);
				}
				else {
					// ERROR: found an unknown node into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleCheckErr, "Unexpected attribute found in <Os> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Os>!", attr2->name);
				}
			}
			rule_info.parameters->os_list.push_back(os_c);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Eval"))) {
			HCF_CC_TRACE(ParseRuleCheck, "Parsing the following element:", "<Eval>");

			if(parse_rule_check_eval(attr, rule_info.parameters->evaluation, rule_info.parameters->evaluation.evaluation_parmater) < 0)
			{
				HCF_CC_LOG(LEVEL_ERROR, "Error while parsing <%s> tag", attr->name);
				HCF_CC_TRACE(ParseRuleCheckErr, "Call 'parse_rule_check_eval' failed.", error());
				return -1;
			}
		}
		else {
			// ERROR: found an unknown node into the rule set file, very strange!
			HCF_CC_TRACE(ParseRuleCheckErr, "Unexpected attribute found in <Check> element!", ERR_RSF_XML_BAD_ELEMENT);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Check>!", attr->name);
		}
	}

	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::parse_rule_check_printout (xmlNode * rule_check_printout, SUFFIX_CONCAT(os,SUFFIX_C) & os_c) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ParseRulePrintout, "Extracting all the children objects for the following one:", "<Printout>");

	// Extract from the element the result attribute
	xmlAttr * printout_result_attr = rule_check_printout->properties;
	os_c.printout.result = UTF8_TO_STRING(printout_result_attr->children->content);

	for (xmlNode * attr = rule_check_printout->children; attr; attr = attr->next) {
		if (attr->type != XML_ELEMENT_NODE)	continue;

		/*
		 * Extract from the current node its information and store it
		 * in the proper structure field according to the node name.
		 */
		if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Begin"))) {
			os_c.printout.begin_c._value = UTF8_TO_STRING(attr->children->content);
			HCF_CC_TRACE(ParseRulePrintout, "The value of Begin element is:", os_c.printout.begin_c._value.c_str());

			HCF_CC_TRACE(ParseRulePrintout, "Extracting the attributes of the following element:", "<Begin>");
      for (xmlAttr * begin_attr = attr->properties; begin_attr; begin_attr = begin_attr->next) {

        // Extract the rule attribute information according to the attribute name
        if (xmlStrEqual(begin_attr->name, reinterpret_cast<const unsigned char *>("skipFullLine"))) {
            os_c.printout.begin_c._skip_full_line = UTF8_TO_STRING(begin_attr->children->content);
            HCF_CC_TRACE(ParseRulePrintout, "The value of skipFullLine attribute is:", os_c.printout.begin_c._skip_full_line.c_str());
       }
        else {
          // ERROR: found an unknown attribute into the rule set file, very strange!
        	HCF_CC_TRACE(ParseRulePrintoutErr, "Unexpected attribute found in <Begin> element!", ERR_RSF_XML_BAD_ELEMENT);
          TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Begin>!", begin_attr->name);
        }
      }
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("End"))) {
			HCF_CC_TRACE(ParseRulePrintout, "Extracting all the children objects for the following one:", "<End>");

			xmlNode * endNode = attr;
			for (xmlNode * attr2 = endNode->children; attr2; attr2 = attr2->next) {
				if (attr2->type != XML_ELEMENT_NODE)	continue;

				/*
				 * Extract from the current node its information and store it
				 * in the proper structure field according to the node name.
				 */
				if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Normal"))) {
					os_c.printout.end_c._normal = UTF8_TO_STRING(attr2->children->content);
					HCF_CC_TRACE(ParseRulePrintout, "The value of Normal element is:", os_c.printout.end_c._normal.c_str());

					HCF_CC_TRACE(ParseRulePrintout, "Extracting the attributes of the following element:", "<Normal>");
		      for (xmlAttr * normal_attr = attr2->properties; normal_attr; normal_attr = normal_attr->next) {

		        // Extract the rule attribute information according to the attribute name
		        if (xmlStrEqual(normal_attr->name, reinterpret_cast<const unsigned char *>("skipFullLine"))) {
		            os_c.printout.end_c._skip_full_line = UTF8_TO_STRING(normal_attr->children->content);
		            HCF_CC_TRACE(ParseRulePrintout, "The value of skipFullLine attribute is:", os_c.printout.end_c._skip_full_line.c_str());
		        }
		        else {
		          // ERROR: found an unknown attribute into the rule set file, very strange!
		        	HCF_CC_TRACE(ParseRulePrintoutErr, "Unexpected attribute found in <Normal> element!", ERR_RSF_XML_BAD_ELEMENT);
		          TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Normal>!", normal_attr->name);
		        }
		      }
				}
				else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Error"))) {

					SUFFIX_CONCAT(end,SUFFIX_C)::error error;
					error._value = UTF8_TO_STRING(attr2->children->content);
					HCF_CC_TRACE(ParseRulePrintout, "The value of Error element is:", error._value.c_str());

					HCF_CC_TRACE(ParseRulePrintout, "Extracting the attributes of the following element:", "<Error>");
					for (xmlAttr * error_attr = attr2->properties; error_attr; error_attr = error_attr->next) {

						// Extract the rule attribute information according to the attribute name
						if (xmlStrEqual(error_attr->name, reinterpret_cast<const unsigned char *>("intext"))) {
							error._intext = UTF8_TO_STRING(error_attr->children->content);
	            HCF_CC_TRACE(ParseRulePrintout, "The value of intext attribute is:", error._intext.c_str());
						}
						else if (xmlStrEqual(error_attr->name, reinterpret_cast<const unsigned char *>("maxlines"))) {
							error._maxlines = UTF8_TO_STRING(error_attr->children->content);
	            HCF_CC_TRACE(ParseRulePrintout, "The value of maxlines attribute is:", error._maxlines.c_str());
						}
						else {
							// ERROR: found an unknown attribute into the rule set file, very strange!
		        	HCF_CC_TRACE(ParseRulePrintoutErr, "Unexpected attribute found in <Error> element!", ERR_RSF_XML_BAD_ELEMENT);
							TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Error>!", error_attr->name);
						}
					}
					os_c.printout.end_c._error_list.push_back(error);
				}
				else {
					// ERROR: found an unknown node into the rule set file, very strange!
        	HCF_CC_TRACE(ParseRulePrintoutErr, "Unexpected attribute found in <End> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <End>!", attr2->name);
				}
			}
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Ignore"))) {

			SUFFIX_CONCAT(ignore,SUFFIX_C) ignoreContainer;
			ignoreContainer._value = UTF8_TO_STRING(attr->children->content);
			HCF_CC_TRACE(ParseRulePrintout, "The value of Ignore element is:", ignoreContainer._value.c_str());

			HCF_CC_TRACE(ParseRulePrintout, "Extracting the attributes of the following element:", "<Ignore>");
			for (xmlAttr * ignore_attr = attr->properties; ignore_attr; ignore_attr = ignore_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(ignore_attr->name, reinterpret_cast<const unsigned char *>("intext"))) {
					ignoreContainer._intext = UTF8_TO_STRING(ignore_attr->children->content);
          HCF_CC_TRACE(ParseRulePrintout, "The value of intext attribute is:", ignoreContainer._intext.c_str());
				}
				else if (xmlStrEqual(ignore_attr->name, reinterpret_cast<const unsigned char *>("lines"))) {
					ignoreContainer._lines = UTF8_TO_STRING(ignore_attr->children->content);
          HCF_CC_TRACE(ParseRulePrintout, "The value of lines attribute is:", ignoreContainer._lines.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
        	HCF_CC_TRACE(ParseRulePrintoutErr, "Unexpected attribute found in <Ignore> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Ignore>!", ignore_attr->name);
				}
			}
			os_c.printout.ignore_list.push_back(ignoreContainer);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Header"))) {

			SUFFIX_CONCAT(header,SUFFIX_C) printoutHeader;

			HCF_CC_TRACE(ParseRulePrintout, "Extracting the attributes of the following element:", "<Header>");
			for (xmlAttr * header_attr = attr->properties; header_attr; header_attr = header_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(header_attr->name, reinterpret_cast<const unsigned char *>("level"))) {
					printoutHeader._level = UTF8_TO_STRING(header_attr->children->content);
          HCF_CC_TRACE(ParseRulePrintout, "The value of level attribute is:", printoutHeader._level.c_str());
				}
				else if (xmlStrEqual(header_attr->name, reinterpret_cast<const unsigned char *>("maxlines"))) {
					printoutHeader._maxlines = UTF8_TO_STRING(header_attr->children->content);
          HCF_CC_TRACE(ParseRulePrintout, "The value of maxlines attribute is:", printoutHeader._maxlines.c_str());
				}
				else if (xmlStrEqual(header_attr->name, reinterpret_cast<const unsigned char *>("colSeparator"))) {
					printoutHeader._colSeparator = UTF8_TO_STRING(header_attr->children->content);
          HCF_CC_TRACE(ParseRulePrintout, "The value of colSeparator attribute is:", printoutHeader._colSeparator.c_str());
				}
				else if (xmlStrEqual(header_attr->name, reinterpret_cast<const unsigned char *>("headerPresence"))) {
					printoutHeader._headerPresence = UTF8_TO_STRING(header_attr->children->content);
          HCF_CC_TRACE(ParseRulePrintout, "The value of headerPresence attribute is:", printoutHeader._headerPresence.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
        	HCF_CC_TRACE(ParseRulePrintoutErr, "Unexpected attribute found in <Header> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Header>!", header_attr->name);
				}
			}

			HCF_CC_TRACE(ParseRulePrintout, "Extracting all the children objects for the following one:", "<Header>");
			xmlNode * headerNode = attr;
			for (xmlNode * attr2 = headerNode->children; attr2; attr2 = attr2->next) {
				if (attr2->type != XML_ELEMENT_NODE)	continue;

				if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Column"))) {

					SUFFIX_CONCAT(header,SUFFIX_C)::column column_properties;
					xmlAttr * column_required_attr = attr2->properties;
					column_properties._required = UTF8_TO_STRING(column_required_attr->children->content);
					column_properties._value = UTF8_TO_STRING(attr2->children->content);
					HCF_CC_TRACE(ParseRulePrintout, "The value of Column element is:", column_properties._value.c_str());
          HCF_CC_TRACE(ParseRulePrintout, "The value of required attribute is:", column_properties._required.c_str());

					printoutHeader._columns.push_back(column_properties);
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
        	HCF_CC_TRACE(ParseRulePrintoutErr, "Unexpected attribute found in <Column> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Column>!", attr2->name);
				}
			}
			os_c.printout.header_list.push_back(printoutHeader);
		}
		else {
			// ERROR: found an unknown node into the rule set file, very strange!
    	HCF_CC_TRACE(ParseRulePrintoutErr, "Unexpected attribute found in <Printout> element!", ERR_RSF_XML_BAD_ELEMENT);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Printout>!", attr->name);
		}
	}
	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::parse_rule_check_filter (xmlNode * rule_check_filter, SUFFIX_CONCAT(filter,SUFFIX_C) & filter_c, SUFFIX_CONCAT(filter_parameter,SUFFIX_C) & filter_parameter_c) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ParseRuleFilter, "Extracting the attributes of the following element:", "<Filter>");

	// Extract from the element the result attribute
	xmlAttr * filter_result_attr = rule_check_filter->properties;
	if(filter_result_attr && xmlStrEqual(filter_result_attr->name, reinterpret_cast<const unsigned char *>("result")))
	{
		filter_c.result = UTF8_TO_STRING(filter_result_attr->children->content);
		HCF_CC_TRACE(ParseRuleFilter, "The value of result attribute is:", filter_c.result.c_str());
	}

	for (xmlNode * attr = rule_check_filter->children; attr; attr = attr->next) {
		if (attr->type != XML_ELEMENT_NODE)	continue;

		/*
		 * Extract from the current node its information and store it
		 * in the proper structure field according to the node name.
		 */
		if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Equal"))) {
			HCF_CC_TRACE(ParseRuleFilter, "Extracting the attributes of the following element:", "<Equal>");

			SUFFIX_CONCAT(equal,SUFFIX_C) equalContainer;

			for (xmlAttr * equal_attr = attr->properties; equal_attr; equal_attr = equal_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(equal_attr->name, reinterpret_cast<const unsigned char *>("name"))) {
					equalContainer._name = UTF8_TO_STRING(equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of name attribute is:", equalContainer._name.c_str());
				}
				else if (xmlStrEqual(equal_attr->name, reinterpret_cast<const unsigned char *>("value"))) {
					equalContainer._value = UTF8_TO_STRING(equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of value attribute is:", equalContainer._value.c_str());
				}
				else if (xmlStrEqual(equal_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
					equalContainer._type = UTF8_TO_STRING(equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of type attribute is:", equalContainer._type.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleFilterErr, "Unexpected attribute found in <Equal> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Equal>!", equal_attr->name);
				}
			}
			filter_parameter_c.equal_list.push_back(equalContainer);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Greater"))) {
			HCF_CC_TRACE(ParseRuleFilter, "Extracting the attributes of the following element:", "<Greater>");

			SUFFIX_CONCAT(greater,SUFFIX_C) greaterContainer;

			for (xmlAttr * greater_attr = attr->properties; greater_attr; greater_attr = greater_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(greater_attr->name, reinterpret_cast<const unsigned char *>("name"))) {
					greaterContainer._name = UTF8_TO_STRING(greater_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of name attribute is:", greaterContainer._name.c_str());
				}
				else if (xmlStrEqual(greater_attr->name, reinterpret_cast<const unsigned char *>("value"))) {
					greaterContainer._value = UTF8_TO_STRING(greater_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of value attribute is:", greaterContainer._value.c_str());
				}
				else if (xmlStrEqual(greater_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
					greaterContainer._type = UTF8_TO_STRING(greater_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of type attribute is:", greaterContainer._type.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleFilterErr, "Unexpected attribute found in <Greater> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Greater>!", greater_attr->name);
				}
			}
			filter_parameter_c.greater_list.push_back(greaterContainer);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Intersect"))) {
			HCF_CC_TRACE(ParseRuleFilter, "Parsing the following element:", "<Intersect>");

			SUFFIX_CONCAT(filter_parameter,SUFFIX_C) intersect_c;

			if (parse_rule_check_filter(attr, filter_c, intersect_c) < 0)
			{
				HCF_CC_TRACE(ParseRuleFilterErr, "Call 'parse_rule_check_filter' failed.", error());
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Error while parsing <%s> tag", attr->name);
			}

			filter_c.intersect_list.push_back(intersect_c);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Less"))) {
			HCF_CC_TRACE(ParseRuleFilter, "Extracting the attributes of the following element:", "<Less>");

			SUFFIX_CONCAT(less,SUFFIX_C) lessContainer;

			for (xmlAttr * less_attr = attr->properties; less_attr; less_attr = less_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(less_attr->name, reinterpret_cast<const unsigned char *>("name"))) {
					lessContainer._name = UTF8_TO_STRING(less_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of name attribute is:", lessContainer._name.c_str());
				}
				else if (xmlStrEqual(less_attr->name, reinterpret_cast<const unsigned char *>("value"))) {
					lessContainer._value = UTF8_TO_STRING(less_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of value attribute is:", lessContainer._value.c_str());
				}
				else if (xmlStrEqual(less_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
					lessContainer._type = UTF8_TO_STRING(less_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of _value attribute is:", lessContainer._type.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleFilterErr, "Unexpected attribute found in <Less> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Less>!", less_attr->name);
				}
			}
			filter_parameter_c.less_list.push_back(lessContainer);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Match"))) {
			HCF_CC_TRACE(ParseRuleFilter, "Extracting the attributes of the following element:", "<Match>");

			SUFFIX_CONCAT(match,SUFFIX_C) matchContainer;

			for (xmlAttr * match_attr = attr->properties; match_attr; match_attr = match_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(match_attr->name, reinterpret_cast<const unsigned char *>("name"))) {
					matchContainer._name = UTF8_TO_STRING(match_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of name attribute is:", matchContainer._name.c_str());
				}
				else if (xmlStrEqual(match_attr->name, reinterpret_cast<const unsigned char *>("value"))) {
					matchContainer._value = UTF8_TO_STRING(match_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of value attribute is:", matchContainer._value.c_str());
				}
				else if (xmlStrEqual(match_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
					matchContainer._type = UTF8_TO_STRING(match_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of type attribute is:", matchContainer._type.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleFilterErr, "Unexpected attribute found in <Match> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Match>!", match_attr->name);
				}
			}
			filter_parameter_c.match_list.push_back(matchContainer);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Not-Equal"))) {
			HCF_CC_TRACE(ParseRuleFilter, "Extracting the attributes of the following element:", "<Not-Equal>");

			SUFFIX_CONCAT(not_equal,SUFFIX_C) notEqualContainer;

			for (xmlAttr * notEqual_attr = attr->properties; notEqual_attr; notEqual_attr = notEqual_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(notEqual_attr->name, reinterpret_cast<const unsigned char *>("name"))) {
					notEqualContainer._name = UTF8_TO_STRING(notEqual_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of name attribute is:", notEqualContainer._name.c_str());
				}
				else if (xmlStrEqual(notEqual_attr->name, reinterpret_cast<const unsigned char *>("value"))) {
					notEqualContainer._value = UTF8_TO_STRING(notEqual_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of value attribute is:", notEqualContainer._value.c_str());
				}
				else if (xmlStrEqual(notEqual_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
					notEqualContainer._type = UTF8_TO_STRING(notEqual_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of type attribute is:", notEqualContainer._type.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleFilterErr, "Unexpected attribute found in <Not-Equal> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Not-Equal>!", notEqual_attr->name);
				}
			}
			filter_parameter_c.not_equal_list.push_back(notEqualContainer);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Range"))) {
			HCF_CC_TRACE(ParseRuleFilter, "Extracting the attributes of the following element:", "<Range>");

			SUFFIX_CONCAT(range,SUFFIX_C) rangeContainer;

			for (xmlAttr * range_attr = attr->properties; range_attr; range_attr = range_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(range_attr->name, reinterpret_cast<const unsigned char *>("name"))) {
					rangeContainer._name = UTF8_TO_STRING(range_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of name attribute is:", rangeContainer._name.c_str());
				}
				else if (xmlStrEqual(range_attr->name, reinterpret_cast<const unsigned char *>("min"))) {
					rangeContainer._min = UTF8_TO_STRING(range_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of min attribute is:", rangeContainer._min.c_str());
				}
				else if (xmlStrEqual(range_attr->name, reinterpret_cast<const unsigned char *>("max"))) {
					rangeContainer._max = UTF8_TO_STRING(range_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of max attribute is:", rangeContainer._max.c_str());
				}
				else if (xmlStrEqual(range_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
					rangeContainer._type = UTF8_TO_STRING(range_attr->children->content);
					HCF_CC_TRACE(ParseRuleFilter, "The value of type attribute is:", rangeContainer._type.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleFilterErr, "Unexpected attribute found in <Range> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Range>!", range_attr->name);
				}
			}
			filter_parameter_c.range_list.push_back(rangeContainer);
		}
		else if (xmlStrEqual(attr->name, reinterpret_cast<const unsigned char *>("Unite"))) {
			HCF_CC_TRACE(ParseRuleFilter, "Parsing the following element:", "<Unite>");

			SUFFIX_CONCAT(filter_parameter,SUFFIX_C) unite_c;

			if (parse_rule_check_filter(attr, filter_c, unite_c) < 0)
			{
				HCF_CC_TRACE(ParseRuleFilterErr, "Call 'parse_rule_check_filter' failed.", error());
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Error while parsing <%s> tag", attr->name);
			}

			filter_c.unite_list.push_back(unite_c);
		}
		else {
			// ERROR: found an unknown node into the rule set file, very strange!
			HCF_CC_TRACE(ParseRuleFilterErr, "Unexpected attribute found in <Filter> element!", ERR_RSF_XML_BAD_ELEMENT);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Filter>!", attr->name);
		}
	}
	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::parse_rule_check_eval (xmlNode * rule_check_eval, SUFFIX_CONCAT(evaluation,SUFFIX_C) & eval_c, SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) & eval_parameter_c) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Eval>");

	for (xmlNode * attr2 = rule_check_eval->children; attr2; attr2 = attr2->next) {
		if (attr2->type != XML_ELEMENT_NODE)	continue;

		if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("And"))) {
			HCF_CC_TRACE(ParseRuleEval, "Parsing the following element:", "<And>");

			SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) and_c;

			if (parse_rule_check_eval(attr2, eval_c, and_c) < 0)
			{
				HCF_CC_LOG(LEVEL_ERROR, "Error while parsing <%s> tag", attr2->name);
				HCF_CC_TRACE(ParseRuleEvalErr, "Call 'parse_rule_check_eval' failed.", error());
				return -1;
			}

			eval_c.and_list.push_back(and_c);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Or"))) {
			HCF_CC_TRACE(ParseRuleEval, "Parsing the following element:", "<Or>");

			SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) or_c;

			if (parse_rule_check_eval(attr2, eval_c, or_c) < 0)
			{
				HCF_CC_LOG(LEVEL_ERROR, "Error while parsing <%s> tag", attr2->name);
				HCF_CC_TRACE(ParseRuleEvalErr, "Call 'parse_rule_check_eval' failed.", error());
				return -1;
			}

			eval_c.or_list.push_back(or_c);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Not"))) {
			HCF_CC_TRACE(ParseRuleEval, "Parsing the following element:", "<Not>");

			SUFFIX_CONCAT(evaluation_parameter,SUFFIX_C) not_c;

			if (parse_rule_check_eval(attr2, eval_c, not_c) < 0)
			{
				HCF_CC_LOG(LEVEL_ERROR, "Error while parsing <%s> tag", attr2->name);
				HCF_CC_TRACE(ParseRuleEvalErr, "Call 'parse_rule_check_eval' failed.", error());
				return -1;
			}

			eval_c.not_list.push_back(not_c);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("All-In-Period"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<All-In-Period>");

			SUFFIX_CONCAT(all_in_period,SUFFIX_C) allInPeriodContainer;

			for (xmlAttr * all_in_period_attr = attr2->properties; all_in_period_attr; all_in_period_attr = all_in_period_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(all_in_period_attr->name, reinterpret_cast<const unsigned char *>("set"))) {
					allInPeriodContainer._set = UTF8_TO_STRING(all_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set attribute is:", allInPeriodContainer._set.c_str());
				}
				else if (xmlStrEqual(all_in_period_attr->name, reinterpret_cast<const unsigned char *>("start-time"))) {
					allInPeriodContainer._start_time = UTF8_TO_STRING(all_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of start-time attribute is:", allInPeriodContainer._start_time.c_str());
				}
				else if (xmlStrEqual(all_in_period_attr->name, reinterpret_cast<const unsigned char *>("end-time"))) {
					allInPeriodContainer._end_time = UTF8_TO_STRING(all_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of end-time attribute is:", allInPeriodContainer._end_time.c_str());
				}
				else if (xmlStrEqual(all_in_period_attr->name, reinterpret_cast<const unsigned char *>("format"))) {
					allInPeriodContainer._format = UTF8_TO_STRING(all_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of format attribute is:", allInPeriodContainer._format.c_str());
				}
				else if (xmlStrEqual(all_in_period_attr->name, reinterpret_cast<const unsigned char *>("col-date"))) {
					allInPeriodContainer._col_date = UTF8_TO_STRING(all_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-date attribute is:", allInPeriodContainer._col_date.c_str());
				}
				else if (xmlStrEqual(all_in_period_attr->name, reinterpret_cast<const unsigned char *>("col-time"))) {
					allInPeriodContainer._col_time = UTF8_TO_STRING(all_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-time attribute is:", allInPeriodContainer._col_time.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <All-In-Period> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <All-In-Period>!", all_in_period_attr->name);
				}
			}
			eval_parameter_c.all_in_period_list.push_back(allInPeriodContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("All-In-Range"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<All-In-Range>");

			SUFFIX_CONCAT(all_in_range,SUFFIX_C) allInRangeContainer;

			for (xmlAttr * all_in_range_attr = attr2->properties; all_in_range_attr; all_in_range_attr = all_in_range_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(all_in_range_attr->name, reinterpret_cast<const unsigned char *>("set"))) {
					allInRangeContainer._set = UTF8_TO_STRING(all_in_range_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set attribute is:", allInRangeContainer._set.c_str());
				}
				else if (xmlStrEqual(all_in_range_attr->name, reinterpret_cast<const unsigned char *>("col"))) {
					allInRangeContainer._col = UTF8_TO_STRING(all_in_range_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col attribute is:", allInRangeContainer._col.c_str());
				}
				else if (xmlStrEqual(all_in_range_attr->name, reinterpret_cast<const unsigned char *>("min"))) {
					allInRangeContainer._min = UTF8_TO_STRING(all_in_range_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of min attribute is:", allInRangeContainer._min.c_str());
				}
				else if (xmlStrEqual(all_in_range_attr->name, reinterpret_cast<const unsigned char *>("max"))) {
					allInRangeContainer._max = UTF8_TO_STRING(all_in_range_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of max attribute is:", allInRangeContainer._max.c_str());
				}
				else if (xmlStrEqual(all_in_range_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
					allInRangeContainer._type = UTF8_TO_STRING(all_in_range_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of type attribute is:", allInRangeContainer._type.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <All-In-Range> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <All-In-Range>!", all_in_range_attr->name);
				}
			}
			eval_parameter_c.all_in_range_list.push_back(allInRangeContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Columns-Equal"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Columns-Equal>");

			SUFFIX_CONCAT(columns_equal,SUFFIX_C) columnsEqualContainer;

			for (xmlAttr * columns_equal_attr = attr2->properties; columns_equal_attr; columns_equal_attr = columns_equal_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(columns_equal_attr->name, reinterpret_cast<const unsigned char *>("set"))) {
					columnsEqualContainer._set = UTF8_TO_STRING(columns_equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set attribute is:", columnsEqualContainer._set.c_str());
				}
				else if (xmlStrEqual(columns_equal_attr->name, reinterpret_cast<const unsigned char *>("col-left"))) {
					columnsEqualContainer._col_left = UTF8_TO_STRING(columns_equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-left attribute is:", columnsEqualContainer._col_left.c_str());
				}
				else if (xmlStrEqual(columns_equal_attr->name, reinterpret_cast<const unsigned char *>("col-right"))) {
					columnsEqualContainer._col_right = UTF8_TO_STRING(columns_equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-right attribute is:", columnsEqualContainer._col_right.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Columns-Equal> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Columns-Equal>!", columns_equal_attr->name);
				}
			}
			eval_parameter_c.columns_equal_list.push_back(columnsEqualContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Columns-Greater"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Columns-Greater>");

			SUFFIX_CONCAT(columns_greater,SUFFIX_C) columnsGreaterContainer;

			for (xmlAttr * columns_greater_attr = attr2->properties; columns_greater_attr; columns_greater_attr = columns_greater_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(columns_greater_attr->name, reinterpret_cast<const unsigned char *>("set"))) {
					columnsGreaterContainer._set = UTF8_TO_STRING(columns_greater_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set attribute is:", columnsGreaterContainer._set.c_str());
				}
				else if (xmlStrEqual(columns_greater_attr->name, reinterpret_cast<const unsigned char *>("col-left"))) {
					columnsGreaterContainer._col_left = UTF8_TO_STRING(columns_greater_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-left attribute is:", columnsGreaterContainer._col_left.c_str());
				}
				else if (xmlStrEqual(columns_greater_attr->name, reinterpret_cast<const unsigned char *>("col-right"))) {
					columnsGreaterContainer._col_right = UTF8_TO_STRING(columns_greater_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-right attribute is:", columnsGreaterContainer._col_right.c_str());
				}
				else if (xmlStrEqual(columns_greater_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
					columnsGreaterContainer._type = UTF8_TO_STRING(columns_greater_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of type attribute is:", columnsGreaterContainer._type.c_str());
				}
				else if (xmlStrEqual(columns_greater_attr->name, reinterpret_cast<const unsigned char *>("percent"))) {
					columnsGreaterContainer._percent = UTF8_TO_STRING(columns_greater_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of percent attribute is:", columnsGreaterContainer._percent.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Columns-Greater> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Columns-Greater>!", columns_greater_attr->name);
				}
			}
			eval_parameter_c.columns_greater_list.push_back(columnsGreaterContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Columns-Less"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Columns-Less>");

			SUFFIX_CONCAT(columns_less,SUFFIX_C) columnsLessContainer;

			for (xmlAttr * columns_less_attr = attr2->properties; columns_less_attr; columns_less_attr = columns_less_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(columns_less_attr->name, reinterpret_cast<const unsigned char *>("set"))) {
					columnsLessContainer._set = UTF8_TO_STRING(columns_less_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set attribute is:", columnsLessContainer._set.c_str());
				}
				else if (xmlStrEqual(columns_less_attr->name, reinterpret_cast<const unsigned char *>("col-left"))) {
					columnsLessContainer._col_left = UTF8_TO_STRING(columns_less_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-left attribute is:", columnsLessContainer._col_left.c_str());
				}
				else if (xmlStrEqual(columns_less_attr->name, reinterpret_cast<const unsigned char *>("col-right"))) {
					columnsLessContainer._col_right = UTF8_TO_STRING(columns_less_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-right attribute is:", columnsLessContainer._col_right.c_str());
				}
				else if (xmlStrEqual(columns_less_attr->name, reinterpret_cast<const unsigned char *>("type"))) {
					columnsLessContainer._type = UTF8_TO_STRING(columns_less_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of type attribute is:", columnsLessContainer._type.c_str());
				}
				else if (xmlStrEqual(columns_less_attr->name, reinterpret_cast<const unsigned char *>("percent"))) {
					columnsLessContainer._percent = UTF8_TO_STRING(columns_less_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of percent attribute is:", columnsLessContainer._percent.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Columns-Less> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Columns-Less>!", columns_less_attr->name);
				}
			}
			eval_parameter_c.columns_less_list.push_back(columnsLessContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Is-Empty"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Is-Empty>");

			SUFFIX_CONCAT(is_empty,SUFFIX_C) isEmptyContainer;

			for (xmlAttr * is_empty_attr = attr2->properties; is_empty_attr; is_empty_attr = is_empty_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(is_empty_attr->name, reinterpret_cast<const unsigned char *>("set"))) {
					isEmptyContainer._set = UTF8_TO_STRING(is_empty_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set attribute is:", isEmptyContainer._set.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Is-Empty> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Is-Empty>!", is_empty_attr->name);
				}
			}
			eval_parameter_c.is_empty_list.push_back(isEmptyContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Is-Equal"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Is-Equal>");

			SUFFIX_CONCAT(is_equal,SUFFIX_C) isEqualContainer;

			for (xmlAttr * is_equal_attr = attr2->properties; is_equal_attr; is_equal_attr = is_equal_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(is_equal_attr->name, reinterpret_cast<const unsigned char *>("set-left"))) {
					isEqualContainer._set_left = UTF8_TO_STRING(is_equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set-left attribute is:", isEqualContainer._set_left.c_str());
				}
				else if (xmlStrEqual(is_equal_attr->name, reinterpret_cast<const unsigned char *>("set-right"))) {
					isEqualContainer._set_right = UTF8_TO_STRING(is_equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set-right attribute is:", isEqualContainer._set_right.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Is-Equal> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Is-Equal>!", is_equal_attr->name);
				}
			}
			eval_parameter_c.is_equal_list.push_back(isEqualContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Is-In-Period"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Is-In-Period>");

			SUFFIX_CONCAT(is_in_period,SUFFIX_C) isInPeriodContainer;

			for (xmlAttr * is_in_period_attr = attr2->properties; is_in_period_attr; is_in_period_attr = is_in_period_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(is_in_period_attr->name, reinterpret_cast<const unsigned char *>("set"))) {
					isInPeriodContainer._set = UTF8_TO_STRING(is_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set attribute is:", isInPeriodContainer._set.c_str());
				}
				else if (xmlStrEqual(is_in_period_attr->name, reinterpret_cast<const unsigned char *>("start-time"))) {
					isInPeriodContainer._start_time = UTF8_TO_STRING(is_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of start-time attribute is:", isInPeriodContainer._start_time.c_str());
				}
				else if (xmlStrEqual(is_in_period_attr->name, reinterpret_cast<const unsigned char *>("end-time"))) {
					isInPeriodContainer._end_time = UTF8_TO_STRING(is_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of end-time attribute is:", isInPeriodContainer._end_time.c_str());
				}
				else if (xmlStrEqual(is_in_period_attr->name, reinterpret_cast<const unsigned char *>("format"))) {
					isInPeriodContainer._format = UTF8_TO_STRING(is_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of format attribute is:", isInPeriodContainer._format.c_str());
				}
				else if (xmlStrEqual(is_in_period_attr->name, reinterpret_cast<const unsigned char *>("col-date"))) {
					isInPeriodContainer._col_date = UTF8_TO_STRING(is_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-date attribute is:", isInPeriodContainer._col_date.c_str());
				}
				else if (xmlStrEqual(is_in_period_attr->name, reinterpret_cast<const unsigned char *>("col-time"))) {
					isInPeriodContainer._col_time = UTF8_TO_STRING(is_in_period_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of col-time attribute is:", isInPeriodContainer._col_time.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Is-In-Period> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Is-In-Period>!", is_in_period_attr->name);
				}
			}
			eval_parameter_c.is_in_period_list.push_back(isInPeriodContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Is-Not-Empty"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Is-Not-Empty>");

			SUFFIX_CONCAT(is_not_empty,SUFFIX_C) isNotEmptyContainer;

			for (xmlAttr * is_not_empty_attr = attr2->properties; is_not_empty_attr; is_not_empty_attr = is_not_empty_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(is_not_empty_attr->name, reinterpret_cast<const unsigned char *>("set"))) {
					isNotEmptyContainer._set = UTF8_TO_STRING(is_not_empty_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set attribute is:", isNotEmptyContainer._set.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Is-Not-Empty> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Is-Not-Empty>!", is_not_empty_attr->name);
				}
			}
			eval_parameter_c.is_not_empty_list.push_back(isNotEmptyContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Is-Not-Equal"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Is-Not-Equal>");

			SUFFIX_CONCAT(is_not_equal,SUFFIX_C) isNotEqualContainer;

			for (xmlAttr * is_not_equal_attr = attr2->properties; is_not_equal_attr; is_not_equal_attr = is_not_equal_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(is_not_equal_attr->name, reinterpret_cast<const unsigned char *>("set-left"))) {
					isNotEqualContainer._set_left = UTF8_TO_STRING(is_not_equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set-left attribute is:", isNotEqualContainer._set_left.c_str());
				}
				else if (xmlStrEqual(is_not_equal_attr->name, reinterpret_cast<const unsigned char *>("set-right"))) {
					isNotEqualContainer._set_right = UTF8_TO_STRING(is_not_equal_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of set-right attribute is:", isNotEqualContainer._set_right.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Is-Not-Equal> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Is-Not-Equal>!", is_not_equal_attr->name);
				}
			}
			eval_parameter_c.is_not_equal_list.push_back(isNotEqualContainer);
		}
		else if (xmlStrEqual(attr2->name, reinterpret_cast<const unsigned char *>("Is-Subset"))) {
			HCF_CC_TRACE(ParseRuleEval, "Extracting the attributes of the following element:", "<Is-Subset>");

			SUFFIX_CONCAT(is_subset,SUFFIX_C) isSubsetContainer;

			for (xmlAttr * is_subset_attr = attr2->properties; is_subset_attr; is_subset_attr = is_subset_attr->next) {

				// Extract the rule attribute information according to the attribute name
				if (xmlStrEqual(is_subset_attr->name, reinterpret_cast<const unsigned char *>("subset"))) {
					isSubsetContainer._subset = UTF8_TO_STRING(is_subset_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of subset attribute is:", isSubsetContainer._subset.c_str());
				}
				else if (xmlStrEqual(is_subset_attr->name, reinterpret_cast<const unsigned char *>("superset"))) {
					isSubsetContainer._superset = UTF8_TO_STRING(is_subset_attr->children->content);
					HCF_CC_TRACE(ParseRuleEval, "The value of superset attribute is:", isSubsetContainer._superset.c_str());
				}
				else {
					// ERROR: found an unknown attribute into the rule set file, very strange!
					HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Is-Subset> element!", ERR_RSF_XML_BAD_ELEMENT);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Is-Subset>!", is_subset_attr->name);
				}
			}
			eval_parameter_c.is_subset_list.push_back(isSubsetContainer);
		}
		else {
			// ERROR: found an unknown node into the rule set file, very strange!
			HCF_CC_TRACE(ParseRuleEvalErr, "Unexpected attribute found in <Eval> element!", ERR_RSF_XML_BAD_ELEMENT);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <Eval>!", attr2->name);
		}
	}
	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

void __TSL_CLASS_NAME__::log_xml_error (void * TSL_UNUSED_PAR(context), const char * format, ...) {
	HCF_CC_TRACE_FUNCTION;
	va_list ap;
	va_start(ap, format);

	char log_buffer[8 * 1024];
	size_t written = ::snprintf(log_buffer, TSL_ARRAY_SIZE(log_buffer), "XML2_LIBRARY_ERROR: ");
	::vsnprintf((log_buffer + written), (TSL_ARRAY_SIZE(log_buffer) - written), format, ap);

	HCF_CC_LOG(LEVEL_ERROR, log_buffer);
	HCF_CC_TRACE(LogXMLMessage, log_buffer);
	va_end(ap);
}

void __TSL_CLASS_NAME__::log_xml_warning (void * TSL_UNUSED_PAR(context), const char * format, ...) {
	HCF_CC_TRACE_FUNCTION;
	va_list ap;
	va_start(ap, format);

	char log_buffer[8 * 1024];
	size_t written = ::snprintf(log_buffer, TSL_ARRAY_SIZE(log_buffer), "XML2_LIBRARY_WARNING: ");
	::vsnprintf((log_buffer + written), (TSL_ARRAY_SIZE(log_buffer) - written), format, ap);

	HCF_CC_LOG(LEVEL_ERROR, log_buffer);
	HCF_CC_TRACE(LogXMLMessage, log_buffer);
	va_end(ap);
}

int __TSL_CLASS_NAME__::get_rsf_info ( const char * ruleset_file_path, const char * schema_file_path, std::pair<std::string,std::string>* rule_set_file_info ){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(GetRSFInfo, "Received a request to extract the RSF info: first, try to open the following file:", ruleset_file_path);

	xmlDoc * ruleset_file = 0;
	if (!(ruleset_file = xmlReadFile(ruleset_file_path, 0, 0))) {
		HCF_CC_TRACE(GetRSFInfoErr, "Call 'xmlReadFile' failed.", ERR_RSF_XML_PARSING_FAILED);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_PARSING_FAILED, "Failed to parse the XML file having path == '%s'", ruleset_file_path);
	}
	HCF_CC_LOG(LEVEL_NOTICE, "XML File having path == '%s' correctly parsed!", ruleset_file_path);

	// Then, if required, validate the parsed file against the XSD schema
	HCF_CC_TRACE(GetRSFInfo, "After a successful file opening operation, let's validate it against the provided XSD schema (if needed):", schema_file_path);
	if (schema_file_path) {
		const int validation_result = validate(ruleset_file, schema_file_path);
		if (validation_result < 0) {
			// The validation is failed, log the error and return -1 (the error message has been already set)
			HCF_CC_LOG(LEVEL_ERROR, "Call 'validate' failed for XML file having path == '%s' and schema having "
					"path == '%s', validation_result == %d!", ruleset_file_path, schema_file_path, validation_result);
			HCF_CC_TRACE(GetRSFInfoErr, "Call 'validate' failed.", validation_result);
			xmlFreeDoc(ruleset_file);
			return -1;
		}

		// Validation operation correctly performed. Check if the file is valid
		if (validation_result != 0) {
			// The file is not valid
			xmlFreeDoc(ruleset_file);
			HCF_CC_TRACE(GetRSFInfo, "The provided RSF is NOT valid for the provided schema, validation returned:", std::to_string(validation_result).c_str());
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_FILE_NOT_VALID, "The XML file having path == '%s' is not valid for "
					"the XSD schema having path == '%s'", ruleset_file_path, schema_file_path);
		}

		// The XML file is valid!
		HCF_CC_LOG(LEVEL_NOTICE, "The XML file having path == '%s' is valid for the schema having path == '%s'!", ruleset_file_path, schema_file_path);
		HCF_CC_TRACE(GetRSFInfo, "The provided RSF is valid for the provided schema!", "");
	}

	// Begin the parsing operation: get the root (RuleSetFile) element
	HCF_CC_TRACE(GetRSFInfo, "Parsing the document root tag!", "<RuleSetFile>");
	xmlNode * rulesetfile_element = 0;
	if (!(rulesetfile_element = xmlDocGetRootElement(ruleset_file))) {
		// NULL root element provided, the XML file is empty!
		xmlFreeDoc(ruleset_file);
		HCF_CC_TRACE(GetRSFInfoErr, "Call 'xmlDocGetRootElement' failed.", ERR_RSF_XML_FILE_EMPTY);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_FILE_EMPTY, "The XML file having path == '%s' is empty!", ruleset_file_path);
	}
	HCF_CC_TRACE(GetRSFInfo, "Extracting the attributes of the following element:", "<RuleSetFile>");

	// Extract from the element the RuleSetFile ID and Revision
	for (xmlAttr * rulesetfile_attr = rulesetfile_element->properties; rulesetfile_attr; rulesetfile_attr = rulesetfile_attr->next)
	{
		// Extract the rule attribute information according to the attribute name
		if (xmlStrEqual(rulesetfile_attr->name, reinterpret_cast<const unsigned char *>("id"))) {
			rule_set_file_info->first = UTF8_TO_STRING(rulesetfile_attr->children->content);
			HCF_CC_TRACE(GetRSFInfo, "The value of the id element is:", rule_set_file_info->first.c_str());
		}
		else if (xmlStrEqual(rulesetfile_attr->name, reinterpret_cast<const unsigned char *>("revision"))) {
			rule_set_file_info->second = UTF8_TO_STRING(rulesetfile_attr->children->content);
			HCF_CC_TRACE(GetRSFInfo, "The value of the revision element is:", rule_set_file_info->second.c_str());
		}
		else {
			// ERROR: found an unknown attribute into the rule set file, very strange!
			HCF_CC_TRACE(GetRSFInfoErr, "Unexpected attribute found in <RuleSetFile> element!", ERR_RSF_XML_BAD_ELEMENT);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_RSF_XML_BAD_ELEMENT, "Unexpected attribute '%s' found in <RuleSetFile>!", rulesetfile_attr->name);
		}
	}
	//Free the allocated memory
	xmlFreeDoc(ruleset_file);
	return 0;
}

HCF_NS_HCF_SRV_END
