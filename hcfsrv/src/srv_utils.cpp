#include <time.h>

#include "hcf/trace/com_ericsson_common_hcf_srv_srvutils.h"
#include "hcf/trace/tracing_macros.h"

#include "srv_utils.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_UTILS__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

void __TSL_CLASS_NAME__::get_current_timestamp (char * output_buffer, size_t output_buffer_size, const char * date_separator, const char * time_separator) {
	HCF_CC_TRACE_FUNCTION;
	time_t curr_time = ::time(0);
	struct tm * current_time = ::localtime(&curr_time);

	HCF_CC_TRACE(GetCurrentTimestamp, "Received a request to calculate the current timestamp. Using as date separator the following string:", date_separator);
	HCF_CC_TRACE(GetCurrentTimestamp, "Received a request to calculate the current timestamp. Using as time separator the following string:", time_separator);
	::snprintf(output_buffer, output_buffer_size, "%04d%s%02d%s%02dT%02d%s%02d%s%02d",
			(current_time->tm_year + 1900), date_separator, (current_time->tm_mon + 1),
			date_separator, current_time->tm_mday, current_time->tm_hour, time_separator,
			current_time->tm_min, time_separator, current_time->tm_sec);
	HCF_CC_TRACE(GetCurrentTimestamp, "The calculated timestamp value is:", output_buffer);
}

category_t __TSL_CLASS_NAME__::map_category_string_to_category_value (const char * category_str) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(MapCategoryStringToCategoryValue, "Received a request to translate a "
			"category string into a category value!", category_str);

	if (category_str) {
		if (!::strcmp(category_str, "BACKOFFICE"))		return CATEGORY_BACKOFFICE;
		if (!::strcmp(category_str, "DAILY"))					return CATEGORY_DAILY;
		if (!::strcmp(category_str, "MANDATORY"))			return CATEGORY_MANDATORY;
		if (!::strcmp(category_str, "POSTUPDATE"))		return CATEGORY_POSTUPDATE;
		if (!::strcmp(category_str, "POSTUPGRADE"))		return CATEGORY_POSTUPGRADE;
		if (!::strcmp(category_str, "PREINSTALL"))		return CATEGORY_PREINSTALL;
		if (!::strcmp(category_str, "PREUPDATE"))			return CATEGORY_PREUPDATE;
		if (!::strcmp(category_str, "PREUPGRADE"))		return CATEGORY_PREUPGRADE;
		if (!::strcmp(category_str, "SHORT"))					return CATEGORY_SHORT;
		if (!::strcmp(category_str, "TROUBLESHOOT"))	return CATEGORY_TROUBLESHOOT;
		if (!::strcmp(category_str, "OTHER"))					return CATEGORY_OTHER;
	}
	return static_cast<category_t>(-1);
}

const char * __TSL_CLASS_NAME__::map_category_value_to_category_string (category_t category) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(MapCategoryValueToCategoryString, "Received a request to translate a "
			"category value into a category string!", category);

	switch (category) {
	case CATEGORY_BACKOFFICE:		return "BACKOFFICE";
	case CATEGORY_DAILY:				return "DAILY";
	case CATEGORY_MANDATORY:		return "MANDATORY";
	case CATEGORY_OTHER:				return "OTHER";
	case CATEGORY_POSTUPDATE:		return "POSTUPDATE";
	case CATEGORY_POSTUPGRADE:	return "POSTUPGRADE";
	case CATEGORY_PREINSTALL:		return "PREINSTALL";
	case CATEGORY_PREUPDATE:		return "PREUPDATE";
	case CATEGORY_PREUPGRADE:		return "PREUPGRADE";
	case CATEGORY_SHORT:				return "SHORT";
	case CATEGORY_TROUBLESHOOT:	return "TROUBLESHOOT";
	}
	return "";
}

const char * __TSL_CLASS_NAME__::map_health_status_to_string (health_status_t status) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(MapHealthStatusToString, "Received a request to translate a healthiness status value into a string!", status);

	switch (status) {
	case HEALTH_STATUS_HEALTHY:				return "HEALTHY";
	case HEALTH_STATUS_NOT_HEALTHY:		return "NOT_HEALTHY";
	case HEALTH_STATUS_WARNING:				return "WARNING";
	case HEALTH_STATUS_NOT_AVAILABLE:	return "NOT_AVAILABLE";
	}
	return "";
}

bool __TSL_CLASS_NAME__::is_special_character (char c) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(IsSpecialCharacter, "Checking if the following character is special:", c);
	return ((c == '"') || (c == '$') || (c == '\\'));
}

void __TSL_CLASS_NAME__::escape_special_characters (char * string, size_t string_size) {
	HCF_CC_TRACE_FUNCTION;
	// XCASALE: Do not add tracings on entire string here... it could be passed a command with sensitive data!

	// To keep the entire input string and the escape characters
	const size_t str_len = ::strlen(string);
	const size_t new_str_size = str_len + 1024;
	char new_string[new_str_size];

	// For each character contained into the input string, if it's a special one, escape it
	size_t i = 0, j = 0;
	for (; (i < str_len) && (j < new_str_size); ++i, ++j) {
		if (is_special_character(string[i])) {
			HCF_CC_TRACE(EscapeSpecialCharacters, "Special character found, escaping it!", string[i]);
			// The current slash character already escapes a special character, nothing to escape
			if ((string[i] == '\\') && (i + 1 < str_len) && (is_special_character(string[i + 1]))) {
				HCF_CC_TRACE(EscapeSpecialCharacters, "The current character is a slash, already escaping the next special character!", string[i]);
			}
			// The current special character is already escaped, nothing to do
			else if ((i > 0) && (string[i - 1] == '\\')) {
				HCF_CC_TRACE(EscapeSpecialCharacters, "The current special character was already escaped, nothing to do!", string[i]);
			}
			else	new_string[j++] = '\\';
		}
		new_string[j] = string[i];
	}
	new_string[j] = 0;

	// After escaping the input string, copy the content of the temporary buffer into the input string
	::strncpy(string, new_string, string_size);
}

skip_log_t __TSL_CLASS_NAME__::map_skip_log_string_to_skip_log_value (const char * skip){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(MapSkipLogStringToSkipLogValue, "Received a request to translate a skipLog value into a skipLog string!", skip);

	if (skip) {
      if (!::strcmp(skip, "NO_SKIP"))  return NO_SKIP;
      if (!::strcmp(skip, "COMMAND"))  return SKIP_COMMAND;
      if (!::strcmp(skip, "PRINTOUT")) return SKIP_PRINTOUT;
      if (!::strcmp(skip, "ALL"))      return SKIP_ALL;
  }
  return static_cast<skip_log_t>(-1);
}

const char * __TSL_CLASS_NAME__::map_skip_log_value_to_skip_log_string (skip_log_t skip){
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(MapSkipLogValueToSkipLogString, "Received a request to translate a skipLog string into a skipLog value!", skip);

	switch (skip) {
    case NO_SKIP:       return "NO_SKIP";
    case SKIP_COMMAND:  return "COMMAND";
    case SKIP_PRINTOUT: return "PRINTOUT";
    case SKIP_ALL:      return "ALL";
  }
  return "";
}

HCF_NS_HCF_SRV_END
