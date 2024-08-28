#ifndef __INCLUDE_GUARD_CLASS_SRV_CLASS_OUTPUT_FILE_WRITER__
#define __INCLUDE_GUARD_CLASS_SRV_CLASS_OUTPUT_FILE_WRITER__ output_file_writer

/*
 *	@file output_file_writer.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-02-02
 *
 *	COPYRIGHT Ericsson AB, 2015
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
 *	| 2015-001 | 2015-02-02 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <string>
#include <set>
#include <libxml/xmlwriter.h>

#include "hcf/core/error_info_provider.h"

#include "srv_macros.h"
#include "app_types.h"
#include "srv_utils.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLASS_OUTPUT_FILE_WRITER__

#ifndef STRING_TO_UTF8_DEFINED
# define STRING_TO_UTF8_DEFINED
# define STRING_TO_UTF8(s) reinterpret_cast<xmlChar *>(const_cast<char *>(s))
#endif

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::error_info_provider {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
		TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::error_info_provider(), _job_id(), _categories(),
				_date(), _status(HEALTH_STATUS_NOT_AVAILABLE), _failed_rules(), _success_rules()) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method creates the output XML file with the information provided by the caller with the set_* and add_*.
	 *
	 * @param[in]  output_file_path - The path where the output file must be created.
	 * @return 0 on success, -1 on failure.
	 */
	int write (const char * output_file_path);

	/**
	 * This method sets the value to be used for the <HcJobId> element of the output file.
	 *
	 * @param[in]  job_id - The ID of the executed instance of HcJob.
	 */
	inline void set_job_id (const std::string & job_id) { _job_id = job_id; }

	/**
	 * This method adds a value to the list of values to be used to generate the <RulesCategory> elements of the output file.
	 *
	 * @param[in]  category - One of the categories of the executed instance of HcJob.
	 */
	inline void add_category (category_t category) { _categories.insert(utils::map_category_value_to_category_string(category)); }

	/**
	 * This method sets the value to be used for the <Date> element of the output file.
	 *
	 * @param[in]  date - The timestamp of the HcJob execution end.
	 */
	inline void set_date (const std::string & date) { _date = date; }

	/**
	 * This method sets the value to be used for the <Status> element of the output file.
	 *
	 * @param[in]  status - The node healthiness status.
	 */
	inline void set_status (health_status_t status) { _status = status; }

	/**
	 * This method adds a value to the list of failed rules to be used to generate the <FailedRules> element of the output file.
	 *
	 * @param[in]  rule - The ID of the failed HcRule.
	 */
	inline void add_failed_rule (const std::string & rule) { _failed_rules.push_back(rule); }

	/**
	 * This method adds a value to the list of success rules to be used to generate the <SuccessRules> element of the output file.
	 *
	 * @param[in]  rule - The ID of the success HcRule.
	 */
	inline void add_success_rule (const std::string & rule) { _success_rules.push_back(rule); }

private:
	/**
	 * This writes the <FailedRules> element of the output file.
	 *
	 * @param[in]  xml_writer - The xmlTextWriter object to be used to write to the output file.
	 */
	int write_failed_rules (xmlTextWriter * xml_writer);

	/**
	 * This writes the <SuccessRules> element of the output file.
	 *
	 * @param[in]  xml_writer - The xmlTextWriter object to be used to write to the output file.
	 */
	int write_success_rules (xmlTextWriter * xml_writer);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	static const char * const HCF_OUTPUT_FILE_DEFAULT_XML_VERSION;
	static const char * const HCF_OUTPUT_FILE_DEFAULT_XML_ENCODING;

	std::string _job_id;
	std::set<const char *> _categories;
	std::string _date;
	health_status_t _status;
	std::list<std::string> _failed_rules;
	std::list<std::string> _success_rules;
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_CLASS_OUTPUT_FILE_WRITER__ */
