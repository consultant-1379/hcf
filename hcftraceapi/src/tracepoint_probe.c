#include "hcf/trace/warning_macros.h"

#define TRACEPOINT_DEFINE
#define TRACEPOINT_PROBE_DYNAMIC_LINKAGE
#include "hcf/trace/com_ericsson_common_hcf_core_alarmtimeplan.h"
#include "hcf/trace/com_ericsson_common_hcf_core_calendaralarmtimeplan.h"
#include "hcf/trace/com_ericsson_common_hcf_core_cmdoptionparser.h"
#include "hcf/trace/com_ericsson_common_hcf_core_cmwlogger.h"
#include "hcf/trace/com_ericsson_common_hcf_core_epolleventreactor.h"
#include "hcf/trace/com_ericsson_common_hcf_core_eventreactor.h"
#include "hcf/trace/com_ericsson_common_hcf_core_oneshotalarmtimeplan.h"
#include "hcf/trace/com_ericsson_common_hcf_core_periodicalarmtimeplan.h"
#include "hcf/trace/com_ericsson_common_hcf_core_processdaemonizer.h"
#include "hcf/trace/com_ericsson_common_hcf_core_thread.h"
#include "hcf/trace/com_ericsson_common_hcf_core_threadset.h"
#include "hcf/trace/com_ericsson_common_hcf_core_utils.h"
#include "hcf/trace/com_ericsson_common_hcf_hcrsfm.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_immconfigclass.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_immobject.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_immobserver.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_oihandler.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_oiinterface.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_omhandler.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_runtimeowner.h"
#include "hcf/trace/com_ericsson_common_hcf_internal_applicationmanager.h"
#include "hcf/trace/com_ericsson_common_hcf_internal_internalapiconstants.h"
#include "hcf/trace/com_ericsson_common_hcf_internal_protocolhandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_clockalarmnotifier.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_cmdhandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_cmdoptionagent.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_configurationhandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_configurator.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_datetime.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_datetimeparser.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hccalendarevent.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcfapplicationmanager.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcfd.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcjob.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcjobscheduler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcrule.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcperiodicevent.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcruleinputparameter.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcruleparameter.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcsingleevent.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_healthcheckm.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_jobexecutor.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_jobschedulingpolicy.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_outputfilewriter.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_rootworkerthread.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_rulecallbackshandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_ruleexecutor.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_rulesetfileparser.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_sftphandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_workerthreadset.h"
#include "hcf/trace/com_ericsson_common_hcf.h"

#define TRACEPOINT_CREATE_PROBES

/*
 * Start from this point onwards to ignore all the warnings generated by LTTnG
 * library (sigh!) due to the usage of -Wall and -Wextra compiler flags.
 * We'll restore the default behavior as soon LTTnG library header files are not used anymore!
 */
TURN_OFF_WARNINGS_INTO_LTTNG_LIBRARY_BEGIN

#include "hcf/trace/com_ericsson_common_hcf_core_alarmtimeplan.h"
#include "hcf/trace/com_ericsson_common_hcf_core_calendaralarmtimeplan.h"
#include "hcf/trace/com_ericsson_common_hcf_core_cmdoptionparser.h"
#include "hcf/trace/com_ericsson_common_hcf_core_cmwlogger.h"
#include "hcf/trace/com_ericsson_common_hcf_core_epolleventreactor.h"
#include "hcf/trace/com_ericsson_common_hcf_core_oneshotalarmtimeplan.h"
#include "hcf/trace/com_ericsson_common_hcf_core_eventreactor.h"
#include "hcf/trace/com_ericsson_common_hcf_core_periodicalarmtimeplan.h"
#include "hcf/trace/com_ericsson_common_hcf_core_processdaemonizer.h"
#include "hcf/trace/com_ericsson_common_hcf_core_thread.h"
#include "hcf/trace/com_ericsson_common_hcf_core_threadset.h"
#include "hcf/trace/com_ericsson_common_hcf_core_utils.h"
#include "hcf/trace/com_ericsson_common_hcf_hcrsfm.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_immconfigclass.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_immobject.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_immobserver.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_oihandler.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_oiinterface.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_omhandler.h"
#include "hcf/trace/com_ericsson_common_hcf_imm_runtimeowner.h"
#include "hcf/trace/com_ericsson_common_hcf_internal_applicationmanager.h"
#include "hcf/trace/com_ericsson_common_hcf_internal_internalapiconstants.h"
#include "hcf/trace/com_ericsson_common_hcf_internal_protocolhandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_clockalarmnotifier.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_cmdhandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_cmdoptionagent.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_configurationhandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_configurator.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_datetime.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_datetimeparser.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hccalendarevent.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcfapplicationmanager.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcfd.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcjob.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcjobscheduler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcperiodicevent.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcrule.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcruleinputparameter.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcruleparameter.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_hcsingleevent.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_healthcheckm.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_jobexecutor.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_jobschedulingpolicy.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_outputfilewriter.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_rootworkerthread.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_rulecallbackshandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_ruleexecutor.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_rulesetfileparser.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_sftphandler.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_srvutils.h"
#include "hcf/trace/com_ericsson_common_hcf_srv_workerthreadset.h"
#include "hcf/trace/com_ericsson_common_hcf.h"

/*
 * All the LTTnG library files have been included now (hurrah!), we can use our compiler flags!
 */
TURN_OFF_WARNINGS_INTO_LTTNG_LIBRARY_END
