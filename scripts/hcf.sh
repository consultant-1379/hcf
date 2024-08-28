##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# hcf.sh
##
# Description:
# Script file to instantiate HCF common component
##
# Note:
# -
##
# Changelog:
# - 2015-11-25: Alessio Cascone (XCASALE)
# Change for exporting HOME environment variable in SLES12 environment.
# - 2015-01-208: Alessio Cascone (XCASALE)
# First version.
##


log () {
  /bin/logger -t $@
}

PID_FILE=/var/run/hcf.pid

case "$1" in

  start) 
    log "Starting HCF..."

    # Check if TraceEa has been installed on the node: 
    # if yes, also LTTnG has been installed and the HCF Trace Library
    # and the LTTnG ones can be preloaded before executing HCF daemon.
    if [ -r /opt/trace_ea/bin/TraceEa ]; then
        export LD_PRELOAD=/opt/hcf/lib/libhcftrace.so:/usr/lib64/liblttng-ust-fork.so.0.0.0
        log "TraceEa & LTTnG are installed, libraries preloaded!"
    fi

	# Check if the HOME variable is empty or not.
	# In the first case, set it with the default value /.
	# Otherwise, keep it as it is.
	if [ -z "$HOME" ]; then
		export HOME=/
	fi

    nohup /opt/hcf/bin/hcfd > /dev/null 2>&1 &
    
    EXIT_CODE=`echo $?`
    echo $! > ${PID_FILE}
    
    if [ ${EXIT_CODE} -eq 0 ]; then
        log "HCF successfully started with PID: $!"
    else
        log "HCF starting failed with exit code: ${EXIT_CODE}"
    fi
    
    exit $EXIT_CODE
   ;;
   
  stop)
    log "HCF stopped!"
    ;;
  
  cleanup)
    log "HCF cleaning-up!"
    
    PID=`cat $PID_FILE`
    kill -9 $PID > /dev/null 2>&1 || true
    
    EXIT_CODE=`echo $?`   
    if [ ${EXIT_CODE} -eq 0 ]; then
        log "HCF with PID $PID successfully cleaned-up!"
    else
        log "HCF with PID $PID clean-up failed with exit code: ${EXIT_CODE}"
    fi

	for PID in `ps -eaf | grep -w "/opt/hcf/bin/hcfd" | grep -v grep | awk '{print $2}'`; do
		log "Killing HCF with PID $PID (multiple instance)"
		kill -9 $PID
	done
    
    exit $EXIT_CODE
    ;;

  *)
  	echo "Usage: $0 {start|stop|cleanup}"
  	log "Usage: $0 {start|stop|cleanup}"
    exit 2
    ;;
esac

exit 0

