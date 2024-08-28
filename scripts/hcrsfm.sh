##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# hcrsfm.sh
##
# Description:
# Script file to launch hcrsfm command, used to enable (if available) the TraceCC support.
##
# Note:
# -
##
# Changelog:
# - 2015-07-21: Alessio Cascone (XCASALE)
# First version.
##

# Check if TraceEa has been installed on the node:  if yes, also LTTnG has been 
# installed and the HCF Trace Library can be preloaded before executing hcrsfm command.
if [ -r /opt/trace_ea/bin/TraceEa ]; then
	export LD_PRELOAD=/opt/hcf/lib/libhcftrace.so
fi

/opt/hcf/bin/hcrsfm "$@"

exit $?

