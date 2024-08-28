#include "constants.h"

HCF_NS_HCF_SRV_BEGIN

#ifdef HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS
const char * const HCF_SRV_USER_NAME                              = "hcf-srv";
const char * const HCF_SRV_GROUP_NAME                             = "hcf-srv";
const char * const HCF_SYSTEM_NBI_DATA_GROUP_NAME                 = "system-nbi-data";
const char * const HCF_CMW_IMM_USERS_GROUP_NAME                   = "cmw-imm-users";
const char * const HCF_PSO_GROUP_NAME                             = "pso";
const char * HCF_SRV_SECONDARY_GROUPS_NAMES[]                     = {HCF_SYSTEM_NBI_DATA_GROUP_NAME, HCF_CMW_IMM_USERS_GROUP_NAME, HCF_PSO_GROUP_NAME, 0};
#endif
const char * const HCF_APPLICATION_NAME                           = "HCF";
const char * const HCF_NBI_API_CONFIGURATION_FILE_PATH            = "/usr/share/ericsson/cba/nbi-root-dir";
const char * const HCF_FILEM_CONFIGURATION_FILE_PATH              = "/usr/share/filem/internal_filem_root.conf";
const char * const HCF_FILEM_INTERNAL_ROOT_DEFAULT_PATH           = "/var/filem/internal_root";
const char * const HCF_OUTPUT_FOLDER_NAME                         = "health_check";
const char * const HCF_TEMP_FOLDER_PATH                           = "/tmp/hcf-apr9010574";
const char * const HCF_RULE_LOG_FILE_EXTENSION                    = ".log";
const char * const HCF_OUTPUT_ARCHIVE_EXTENSION                   = ".tar.gz";
const char * const HCF_GENERIC_ERROR_ERROR_MSG                    = "Generic Error";
const char * const HCF_MANDATORY_VALUE_MISSING_ERROR_MSG          = "Mandatory attribute missing.";
const char * const HCF_UNDEFINED_CATEGORY_ERROR_MSG               = "The provided category is undefined.";
const char * const HCF_CHILDREN_OBJECTS_CREATION_FAILED_ERROR_MSG = "Failed to create the children objects.";
const char * const HCF_OPERATION_NOT_IMPLEMENTED_ERROR_MSG        = "Unsupported administrative operation.";
const char * const HCF_JOB_ALREADY_RUNNING_ERROR_MSG              = "Another job is in progress.";
const char * const HCF_INVALID_RULE_RDN_ERROR_MSG                 = "Invalid HcRule RDN value provided.";
const char * const HCF_UNDEFINED_RULE_RDN_ERROR_MSG               = "Undefined HcRule RDN value provided.";
const char * const HCF_UNDEFINED_RULE_ATTRIBUTE_ERROR_MSG         = "The provided couple <name,value> is not defined for the provided HcRule";
const char * const HCF_DUPLICATED_RULE_ATTRIBUTE_ERROR_MSG        = "The provided couple <name,value> is already defined for the provided HcRule";
const char * const HCF_INVALID_JOBTOTRIGGER_VALUE_ERROR_MSG       = "Invalid value for jobToTrigger attribute: it's equal to the current HcJob DN";
const char * const HCF_NOT_EXISTING_JOBTOTRIGGER_ERROR_MSG        = "Invalid value for jobToTrigger attribute: the provided value refers to a not existing HcJob";
const char * const HCF_JOB_LINKED_TO_ANOTHER_JOB_ERROR_MSG        = "Cannot delete the current job, since it is linked to another job. Please unlink it and retry.";
const char * const HCF_JOB_EXECUTION_ONGOING_ERROR_MSG            = "Cannot delete the current job, since it is currently running.";
const char * const HCF_INVALID_JOB_RDN_ERROR_MSG                  = "Invalid HcJob RDN value provided. Accepted values are: \"a-zA-Z0-9_-\"";
const char * const HCF_INVALID_TIME_VALUE_ERROR_MSG               = "Invalid time value provided.";
const char * const HCF_PAST_TIME_VALUE_ERROR_MSG                  = "Invalid time value provided. Time is over.";
const char * const HCF_INVALID_TIME_INTERVAL_ERROR_MSG            = "Invalid time interval provided: startTime is bigger than stopTime.";
const char * const HCF_INVALID_VALUE_PROVIDED_ERROR_MSG           = "Invalid value provided.";
const char * const HCF_INVALID_DAYOFWEEK_OCCURRENCE_ERROR_MSG     = "Incoherent dayOfWeekOccurrence value provided, since dayOfWeek has default value.";
const char * const HCF_INPUTPARAMETER_OP_NOT_PERMITTED_ERROR_MSG  = "Cannot execute the operation: this job is currently running.";

HCF_NS_HCF_SRV_END
