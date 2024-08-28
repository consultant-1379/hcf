#include <sys/stat.h>

#include "hcf/core/logger.h"
#include "hcf/imm/imm_config_class.h"

#include "constants.h"
#include "imm_object_hcrule.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_outputfilewriter.h"
#include "hcf/trace/tracing_macros.h"

#include "output_file_writer.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLASS_OUTPUT_FILE_WRITER__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule> hcrule_class_handler;

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_OUTPUT_FILE_DEFAULT_XML_VERSION  = "1.0";
const char * const __TSL_CLASS_NAME__::HCF_OUTPUT_FILE_DEFAULT_XML_ENCODING = "UTF-8";

int __TSL_CLASS_NAME__::write (const char * output_file_path) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Write, "Received a request to produce the output XML file: checking the provided data.", output_file_path);

	// First of all, check that all the needed info has been provided
	if ((_job_id.empty()) || (_categories.size() == 0) || (_date.empty()) || (_status == HEALTH_STATUS_NOT_AVAILABLE)) {
		HCF_CC_TRACE(WriteErr, "Some of the required information to produce the XML output file is missing!", ERR_OUT_XML_FILE_CREATE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "All the needed info to produce the output file has not been provided! _job_id == '%s', "
				"_categories(count) == %lu, _date == '%s', _status == %d, _failed_rules(count) == %lu, _success_rules(count) == %lu",
				_job_id.c_str(), _categories.size(), _date.c_str(), _status, _failed_rules.size(), _success_rules.size());
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_CREATE_FAILED, "All the needed info to produce the output file has not been provided!");
	}

	// FIRST: Create the XML buffer that will contain the XML output file
	HCF_CC_TRACE(Write, "Creating the XML buffer.", output_file_path);
	xmlBuffer * xml_buffer = 0;
	if (!(xml_buffer = xmlBufferCreate())) {
		HCF_CC_TRACE(WriteErr, "Call 'xmlBufferCreate' failed.", ERR_OUT_XML_FILE_CREATE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlBufferCreate' failed!");
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_CREATE_FAILED, "Failed to allocate memory for the output XML file");
	}

	// SECOND: create the XMLWriter object to create the XML output file
	HCF_CC_TRACE(Write, "Creating the XML writer object.", output_file_path);
	xmlTextWriter * xml_writer = 0;
	if (!(xml_writer = xmlNewTextWriterMemory(xml_buffer, 0))) {
		xmlBufferFree(xml_buffer);
		HCF_CC_TRACE(WriteErr, "Call 'xmlNewTextWriterMemory' failed.", ERR_OUT_XML_FILE_CREATE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlNewTextWriterMemory' failed!");
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_CREATE_FAILED, "Failed to create the output XML file");
	}

	// THIRD: set the XMLWriter to produce a formatted output
	HCF_CC_TRACE(Write, "Setting the indentation for the output XML.", output_file_path);
	if (xmlTextWriterSetIndent(xml_writer, 1) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'xmlTextWriterSetIndent' failed.", ERR_OUT_XML_FILE_INDENT_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterSetIndent' failed");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_INDENT_FAILED, "Failed to set the indentation for the output XML file");
	}

	// FOURTH: start the document with XML version and encoding info
	HCF_CC_TRACE(Write, "Setting XML version and encoding.", output_file_path);
	if (xmlTextWriterStartDocument(xml_writer, HCF_OUTPUT_FILE_DEFAULT_XML_VERSION, HCF_OUTPUT_FILE_DEFAULT_XML_ENCODING, 0) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'xmlTextWriterStartDocument' failed.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterStartDocument' failed!");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to start the document for the output XML file");
	}

	/*
	 * Then, begin to fill the XML document with all the needed info, according to the schema file:
	 * - Report element (root element)
	 * 		- HcJobId
	 * 		- RulesCategory
	 * 		- Date
	 * 		- Status
	 * 		- FailedRules
	 * 		- SuccessRules
	 */
	// BEGIN: Report element (root element)
	HCF_CC_TRACE(Write, "Opening Report element.", output_file_path);
	if (xmlTextWriterStartElement(xml_writer, STRING_TO_UTF8("Report")) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'xmlTextWriterStartElement' failed for 'Report' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterStartElement' failed for 'Report' element!");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the Report element");
	}

	// Write the HcJobId element with the value of the class field
	HCF_CC_TRACE(Write, "Creating HcJobId element.", output_file_path);
	if (xmlTextWriterWriteElement(xml_writer, STRING_TO_UTF8("HcJobId"), STRING_TO_UTF8(_job_id.c_str())) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'xmlTextWriterWriteElement' failed for 'HcJobId' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteElement' failed for 'HcJobId' element!");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the HcJobId element");
	}

	// Write a RulesCategory element for each provided category
	for (std::set<const char *>::const_iterator category = _categories.begin(); category != _categories.end(); ++category) {
		HCF_CC_TRACE(Write, "Creating RulesCategory element.", output_file_path);
		if (xmlTextWriterWriteElement(xml_writer, STRING_TO_UTF8("RulesCategory"), STRING_TO_UTF8(*category)) < 0) {
			HCF_CC_TRACE(WriteErr, "Call 'xmlTextWriterStartElement' failed for 'RulesCategory' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterStartElement' failed for 'RulesCategory' element!");
			xmlFreeTextWriter(xml_writer);
			xmlBufferFree(xml_buffer);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the RulesCategory element");
		}
	}

	// Write the Date element with the value of the class field
	HCF_CC_TRACE(Write, "Creating Date element.", output_file_path);
	if (xmlTextWriterWriteElement(xml_writer, STRING_TO_UTF8("Date"), STRING_TO_UTF8(_date.c_str())) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'xmlTextWriterWriteElement' failed for 'Date' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteElement' failed for 'Date' element!");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the Date element");
	}

	// Write the Date element with the value of the class field
	HCF_CC_TRACE(Write, "Creating Status element.", output_file_path);
	if (xmlTextWriterWriteElement(xml_writer, STRING_TO_UTF8("Status"), STRING_TO_UTF8(utils::map_health_status_to_string(_status))) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'xmlTextWriterWriteElement' failed for 'Status' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteElement' failed for 'Status' element!");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the Status element");
	}

	// Write the FailedRules element, with one child for each failed rule
	HCF_CC_TRACE(Write, "Adding information about failed rules.", output_file_path);
	if (write_failed_rules(xml_writer) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'write_failed_rules' failed.", error());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'write_failed_rules' failed for 'FailedRules' element!");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		return -1;
	}

	// Write the SuccessRules element, with one child for each rule executed with success
	HCF_CC_TRACE(Write, "Adding information about success rules.", output_file_path);
	if (write_success_rules(xml_writer) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'write_success_rules' failed.", error());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'write_success_rules' failed for 'SuccessRules' element!");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		return -1;
	}

	// END: Report element (root element)
	HCF_CC_TRACE(Write, "Closing Report element.", output_file_path);
	if (xmlTextWriterEndElement(xml_writer) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'xmlTextWriterEndElement' failed for 'Report' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterEndElement' failed for 'Report' element!");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the Report element");
	}

	// When all the document data has been written, end the XML document
	HCF_CC_TRACE(Write, "Closing XML document.", output_file_path);
	if (xmlTextWriterEndDocument(xml_writer) < 0) {
		HCF_CC_TRACE(WriteErr, "Call 'xmlTextWriterEndDocument' failed.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterEndDocument' failed!");
		xmlFreeTextWriter(xml_writer);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to end the document for the output XML file");
	}
	xmlFreeTextWriter(xml_writer);

	// After preparing the buffer with the file content, create the file
	HCF_CC_TRACE(Write, "Opening the output file!", output_file_path);
	FILE * output_file_ptr = 0;
	if (!(output_file_ptr = ::fopen(output_file_path, "w"))) {
		const int errno_save = errno;
		HCF_CC_TRACE(WriteErr, "Call 'fopen' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'fopen' failed for file having path == '%s', errno == %d", output_file_path, errno_save);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_CREATE_FAILED, "Failed to create the output file");
	}

	// After creating the file, change its permissions in order to drop read permissions for all users
	HCF_CC_TRACE(Write, "Setting the right permissions set to the output file!", output_file_path);
	if (::chmod(output_file_path, 0640)) {
		const int errno_save = errno;
		HCF_CC_TRACE(WriteErr, "Call 'chmod' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'chmod' failed for file having path == '%s', errno == %d", output_file_path, errno_save);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_CREATE_FAILED, "Failed to change permissions to the output file");
	}

	// After creating the output file, write its content
	HCF_CC_TRACE(Write, "Writing the XML file on the file system!", output_file_path);
	const int chars_to_write = xml_buffer->use;
	int chars_written = 0;
	if ((chars_written = ::fprintf(output_file_ptr, "%s", xml_buffer->content)) < chars_to_write) {
		HCF_CC_TRACE(WriteErr, "Call 'chmod' failed: bytes written are:", chars_written);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'chmod' failed for file having path == '%s', chars_to_write == %u, "
				"chars_written == %d", output_file_path, chars_to_write, chars_written);
		::fclose(output_file_ptr);
		xmlBufferFree(xml_buffer);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_CREATE_FAILED, "Failed to write to the output file");
	}

	// After a successful write operation, free the allocated resources and return
	HCF_CC_TRACE(Write, "Output file correctly written to file system!", output_file_path);
	::fclose(output_file_ptr);
	xmlBufferFree(xml_buffer);
	TSL_SET_ERROR_INFO_OK_AND_RETURN(ERR_NO_ERROR);
}

int __TSL_CLASS_NAME__::write_failed_rules (xmlTextWriter * xml_writer) {
	HCF_CC_TRACE_FUNCTION;
	// BEGIN: FailedRules element
	HCF_CC_TRACE(WriteFailedRules, "Opening the following element:", "FailedRules");
	if (xmlTextWriterStartElement(xml_writer, STRING_TO_UTF8("FailedRules")) < 0) {
		HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterStartElement' failed for 'FailedRules' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterStartElement' failed for 'FailedRules' element!");
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the FailedRules element");
	}

	/*
	 * For each failed rule, the following elements must be added:
	 * 	1. HcRule (with rule name and ID)
	 * 	1.1. InputParameter (one for each input parameter of a customizable rule)
	 * 	1.2. Description
	 * 	1.3. Reason
	 * 	1.4. Severity
	 * 	1.5. RecommendedAction
	 */
	for (std::list<std::string>::const_iterator rule = _failed_rules.begin(); rule != _failed_rules.end(); ++rule) {
		HCF_CC_TRACE(WriteFailedRules, "Retrieving the instance for the following HcRule:", rule->c_str());
		imm_object_hcrule * rule_ptr = hcrule_class_handler.get_instance(*rule);
		if (!rule_ptr) {
			HCF_CC_TRACE(WriteFailedRulesErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_class_handler.get_instance' for id == '%s' returned a NULL pointer!", rule->c_str());
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NULL_INSTANCE_RETRIEVED, "Failed to get the HcRule instance having id == '%s'", rule->c_str());
		}

		// BEGIN: HcRule element
		HCF_CC_TRACE(WriteFailedRules, "Opening the HcRule element for the following HcRule:", rule->c_str());
		if (xmlTextWriterStartElement(xml_writer, STRING_TO_UTF8("HcRule")) < 0) {
			HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterStartElement' failed for 'HcRule' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterStartElement' failed for 'HcRule' element!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the HcRule element");
		}

		// Add the attribute 'id' to the HcRule element
		HCF_CC_TRACE(WriteFailedRules, "Writing the id attribute in the HcRule element for the following HcRule:", rule->c_str());
		if (xmlTextWriterWriteAttribute(xml_writer, STRING_TO_UTF8("id"), STRING_TO_UTF8(rule_ptr->get_id().c_str())) < 0) {
			HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterWriteAttribute' failed for 'id' attribute.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteAttribute' failed for 'id' attribute!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the 'id' attribute");
		}

		// Add the attribute 'name' to the InputParameters element
		HCF_CC_TRACE(WriteFailedRules, "Writing the name attribute in the HcRule element for the following HcRule:", rule->c_str());
		if (xmlTextWriterWriteAttribute(xml_writer, STRING_TO_UTF8("name"), STRING_TO_UTF8(rule_ptr->get_name().c_str())) < 0) {
			HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterWriteAttribute' failed for 'name' attribute.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteAttribute' failed for 'name' attribute!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the 'name' attribute");
		}

		// Add an InputParameters element for each parameter of a customizable rule
		HCF_CC_TRACE(WriteFailedRules, "Checking if the following HcRule has some input parameters.", rule->c_str());
		std::map<std::string, std::pair<std::string, std::string>> rule_attributes = rule_ptr->get_rule_attributes(_job_id);
		for (std::map<std::string, std::pair<std::string, std::string>>::const_iterator it = rule_attributes.begin(); it != rule_attributes.end(); ++it) {
			// BEGIN: InputParameters element
			HCF_CC_TRACE(WriteFailedRules, "Opening the InputParameters element for the following InputParameters:", it->first.c_str());
			if (xmlTextWriterStartElement(xml_writer, STRING_TO_UTF8("InputParameters")) < 0) {
				HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterStartElement' failed for 'InputParameters' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterStartElement' failed for 'InputParameters' element!");
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the InputParameters element");
			}

			// Add the attribute 'name' to the InputParameters element
			HCF_CC_TRACE(WriteFailedRules, "Writing the name attribute in the InputParameters element for the following InputParameter:", it->first.c_str());
			if (xmlTextWriterWriteAttribute(xml_writer, STRING_TO_UTF8("name"), STRING_TO_UTF8(it->first.c_str())) < 0) {
				HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterWriteAttribute' failed for 'name' attribute.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteAttribute' failed for 'name' attribute!");
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the 'name' attribute");
			}

			// Add the attribute 'value' to the InputParameters element
			HCF_CC_TRACE(WriteFailedRules, "Writing the value attribute in the InputParameters element for the following InputParameter:", it->first.c_str());
			if (xmlTextWriterWriteAttribute(xml_writer, STRING_TO_UTF8("value"), STRING_TO_UTF8(it->second.second.c_str())) < 0) {
				HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterWriteAttribute' failed for 'value' attribute.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteAttribute' failed for 'value' attribute!");
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the 'value' attribute");
			}

			// END: InputParameters element
			HCF_CC_TRACE(WriteFailedRules, "Closing the InputParameters element for the following InputParameters:", it->first.c_str());
			if (xmlTextWriterEndElement(xml_writer) < 0) {
				HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterEndElement' failed for 'InputParameters' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterEndElement' failed for 'InputParameters' element!");
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the InputParameters element");
			}
		}

		// Write the Description element
		HCF_CC_TRACE(WriteFailedRules, "Creating Description element for the following HcRule:", rule->c_str());
		if (xmlTextWriterWriteElement(xml_writer, STRING_TO_UTF8("Description"), STRING_TO_UTF8(rule_ptr->get_description().c_str())) < 0) {
			HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterWriteElement' failed for 'Description' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteElement' failed for 'Description' element!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the Description element");
		}

		// Write the Reason element
		HCF_CC_TRACE(WriteFailedRules, "Creating Reason element for the following HcRule:", rule->c_str());
		if (xmlTextWriterWriteElement(xml_writer, STRING_TO_UTF8("Reason"), STRING_TO_UTF8(rule_ptr->get_reason().c_str())) < 0) {
			HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterWriteElement' failed for 'Reason' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteElement' failed for 'Reason' element!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the Reason element");
		}

		// Write the Severity element
		HCF_CC_TRACE(WriteFailedRules, "Creating Severity element for the following HcRule:", rule->c_str());
		if (xmlTextWriterWriteElement(xml_writer, STRING_TO_UTF8("Severity"), STRING_TO_UTF8((rule_ptr->is_critical()) ? "CRITICAL" : "WARNING")) < 0) {
			HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterWriteElement' failed for 'Severity' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteElement' failed for 'Severity' element!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the Severity element");
		}

		// Write the RecommendedAction element
		HCF_CC_TRACE(WriteFailedRules, "Creating RecommendedAction element for the following HcRule:", rule->c_str());
		if (xmlTextWriterWriteElement(xml_writer, STRING_TO_UTF8("RecommendedAction"), STRING_TO_UTF8(rule_ptr->get_recommended_action().c_str())) < 0) {
			HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterWriteElement' failed for 'RecommendedAction' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteElement' failed for 'RecommendedAction' element!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the RecommendedAction element");
		}

		// END: HcRule element
		HCF_CC_TRACE(WriteFailedRules, "Closing the HcRule element for the following HcRule:", rule->c_str());
		if (xmlTextWriterEndElement(xml_writer) < 0) {
			HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterEndElement' failed for 'HcRule' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterEndElement' failed for 'HcRule' element!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the HcRule element");
		}
	}

	// END: FailedRules element
	HCF_CC_TRACE(WriteFailedRules, "Closing the following element:", "FailedRules");
	if (xmlTextWriterEndElement(xml_writer) < 0) {
		HCF_CC_TRACE(WriteFailedRulesErr, "Call 'xmlTextWriterEndElement' failed for 'FailedRules' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterEndElement' failed for 'FailedRules' element!");
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the FailedRules element");
	}
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::write_success_rules (xmlTextWriter * xml_writer) {
	HCF_CC_TRACE_FUNCTION;
	// BEGIN: SuccessRules element
	HCF_CC_TRACE(WriteSuccessRules, "Opening the following element:", "SuccessRules");
	if (xmlTextWriterStartElement(xml_writer, STRING_TO_UTF8("SuccessRules")) < 0) {
		HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterStartElement' failed for 'SuccessRules' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterStartElement' failed for 'SuccessRules' element!");
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the SuccessRules element");
	}

	/*
	 * For each rule executed with success, the following elements must be added:
	 * 	1. HcRule (with rule name and ID)
	 * 	1.1. InputParameter (one for each input parameter of a customizable rule)
	 */
	for (std::list<std::string>::const_iterator rule = _success_rules.begin(); rule != _success_rules.end(); ++rule) {
		HCF_CC_TRACE(WriteSuccessRules, "Retrieving the instance for the following HcRule:", rule->c_str());
		imm_object_hcrule * rule_ptr = hcrule_class_handler.get_instance(*rule);
		if (!rule_ptr) {
			HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_class_handler.get_instance' for id == '%s' returned a NULL pointer!", rule->c_str());
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NULL_INSTANCE_RETRIEVED, "Failed to get the HcRule instance having id == '%s'", rule->c_str());
		}

		// BEGIN: HcRule element
		HCF_CC_TRACE(WriteSuccessRules, "Opening the HcRule element for the following HcRule:", rule->c_str());
		if (xmlTextWriterStartElement(xml_writer, STRING_TO_UTF8("HcRule")) < 0) {
			HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterStartElement' failed for 'HcRule' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterStartElement' failed for 'HcRule' element!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the HcRule element");
		}

		// Add the attribute 'id' to the HcRule element
		HCF_CC_TRACE(WriteSuccessRules, "Writing the id attribute in the HcRule element for the following HcRule:", rule->c_str());
		if (xmlTextWriterWriteAttribute(xml_writer, STRING_TO_UTF8("id"), STRING_TO_UTF8(rule_ptr->get_id().c_str())) < 0) {
			HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterWriteAttribute' failed for 'id' attribute.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteAttribute' failed for 'id' attribute!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the 'id' attribute");
		}

		// Add the attribute 'name' to the InputParameters element
		HCF_CC_TRACE(WriteSuccessRules, "Writing the name attribute in the HcRule element for the following HcRule:", rule->c_str());
		if (xmlTextWriterWriteAttribute(xml_writer, STRING_TO_UTF8("name"), STRING_TO_UTF8(rule_ptr->get_name().c_str())) < 0) {
			HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterWriteAttribute' failed for 'name' attribute.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteAttribute' failed for 'name' attribute!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the 'name' attribute");
		}

		// Add an InputParameters element for each parameter of a customizable rule
		HCF_CC_TRACE(WriteSuccessRules, "Checking if the following HcRule has some input parameters.", rule->c_str());
		std::map<std::string, std::pair<std::string, std::string>> rule_attributes = rule_ptr->get_rule_attributes(_job_id);
		for (std::map<std::string, std::pair<std::string, std::string>>::const_iterator it = rule_attributes.begin(); it != rule_attributes.end(); ++it) {
			// BEGIN: InputParameters element
			HCF_CC_TRACE(WriteSuccessRules, "Opening the InputParameters element for the following InputParameter:", it->first.c_str());
			if (xmlTextWriterStartElement(xml_writer, STRING_TO_UTF8("InputParameters")) < 0) {
				HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterStartElement' failed for 'InputParameters' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterStartElement' failed for 'InputParameters' element!");
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the InputParameters element");
			}

			// Add the attribute 'name' to the InputParameters element
			HCF_CC_TRACE(WriteSuccessRules, "Writing the name attribute in the InputParameters element for the following InputParameter:", it->first.c_str());
			if (xmlTextWriterWriteAttribute(xml_writer, STRING_TO_UTF8("name"), STRING_TO_UTF8(it->first.c_str())) < 0) {
				HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterWriteAttribute' failed for 'name' attribute.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteAttribute' failed for 'name' attribute!");
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the 'name' attribute");
			}

			// Add the attribute 'value' to the InputParameters element
			HCF_CC_TRACE(WriteSuccessRules, "Writing the value attribute in the InputParameters element for the following InputParameter:", it->first.c_str());
			if (xmlTextWriterWriteAttribute(xml_writer, STRING_TO_UTF8("value"), STRING_TO_UTF8(it->second.second.c_str())) < 0) {
				HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterWriteAttribute' failed for 'value' attribute.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterWriteAttribute' failed for 'value' attribute!");
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the 'value' attribute");
			}

			// END: InputParameters element
			HCF_CC_TRACE(WriteSuccessRules, "Closing the InputParameters element for the following InputParameter:", it->first.c_str());
			if (xmlTextWriterEndElement(xml_writer) < 0) {
				HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterEndElement' failed for 'InputParameters' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterEndElement' failed for 'InputParameters' element!");
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the InputParameters element");
			}
		}

		// END: HcRule element
		HCF_CC_TRACE(WriteSuccessRules, "Closing the HcRule element for the following HcRule:", rule->c_str());
		if (xmlTextWriterEndElement(xml_writer) < 0) {
			HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterEndElement' failed for 'HcRule' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterEndElement' failed for 'HcRule' element!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the HcRule element");
		}
	}

	// END: SuccessRules element
	HCF_CC_TRACE(WriteSuccessRules, "Closing the following element:", "SuccessRules");
	if (xmlTextWriterEndElement(xml_writer) < 0) {
		HCF_CC_TRACE(WriteSuccessRulesErr, "Call 'xmlTextWriterEndElement' failed for 'SuccessRules' element.", ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'xmlTextWriterEndElement' failed for 'SuccessRules' element!");
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED, "Failed to create the SuccessRules element");
	}
	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
