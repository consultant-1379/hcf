#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# hcf_remove.sh
##
# Description:
# This script is used during the HCF removal
##
# Note:
# -
##
# Changelog:
# - 2015-04-14: Alessio Cascone (XCASALE)
# Updated with security rules deployment.
# - 2015-01-13: Alessio Cascone (XCASALE)
# First version.
##

### BEGIN: Helper functions
print_usage()
{
	echo "Incorrect usage"
	echo "Usage:"
	echo "hcf_remove.sh <sdp_name>"
	echo
}

execute ()
{
	# Check the number of arguments
	if [ $# == 0 ] ; then
		echo "Usage: $0 cmd [arguments]"
		exit 2
	fi

	# Execute the command and check its exit code
	$@

	if [ $? != 0 ] ; then 
        echo "Failed to execute '$@'!"
        exit 1
	fi
}
### END: Helper functions

# Check input parameters
if [ $# != 1 ] ; then
	print_usage
	exit 2
fi

SDP_NAME=$1

# FIRST: Remove the root MO of the installed configuration objects from IMM and the HCF security roles
/usr/bin/immcfg -d -u HCMhealthCheckMId=1
/usr/bin/immcfg -d -u ruleId=HealthCheckManagement_1,roleId=SystemAdministrator,localAuthorizationMethodId=1
OUTPUT=`immfind roleId=SystemReadOnly,localAuthorizationMethodId=1 2> /dev/null`
if [ "$OUTPUT" != "" ] ; then
	# Remove also the Rule#2 under SystemReadOnly rule
	/usr/bin/immcfg -d -u ruleId=HealthCheckManagement_2,roleId=SystemReadOnly,localAuthorizationMethodId=1
fi

# SECOND: Remove each defined IMM class
for CLASS in `grep '<class name=' $(dirname $(dirname $(readlink -f $0)))/$SDP_NAME/models/*imm_classes.xml | awk 'BEGIN {FS = "\""} { print $2 }' | sort | uniq`; do
	execute cmw-immclass-delete $CLASS
done

# THIRD: Remove the COM model file (MP file)
execute cmw-model-delete $SDP_NAME --mt COM_R1
execute cmw-model-done

# FOURTH: Remove the RPMs packages (if present)
rm -rf /cluster/rpms/HCF-*

# FIFTH: Execute a COM switchover to apply the operation
comsa-mim-tool com_switchover

