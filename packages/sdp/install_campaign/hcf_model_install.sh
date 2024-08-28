#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# hcf_model_install.sh
##
# Description:
# This script is used during the HCF installation to install the HCM model files.
##
# Note:
# -
##
# Changelog:
# - 2015-04-14: Alessio Cascone (XCASALE)
# Updated with security rules deployment.
# - 2015-01-12: Alessio Cascone (XCASALE)
# First version.
##

### BEGIN: Helper functions
print_usage()
{
	echo "Incorrect usage"
	echo "Usage:"
	echo "hcf_model_install.sh <sdp_name>"
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
ROLES_WITHOUT_SYSTEM_READ_ONLY_ROLE_FILENAME="HCM_SecurityRoles_Instances_imm_objects_no_readonly.xml"
ROLES_WITH_SYSTEM_READ_ONLY_ROLE_FILENAME="HCM_SecurityRoles_Instances_imm_objects.xml"

# FIRST: Add the IMM model files (classes and objects)
execute cmw-model-add $SDP_NAME --mt IMM_R3

# SECOND: Install the HCM security rules under SecM model. Two cases are possible:
#             1. SystemReadOnly Role is deployed on the target machine.
#             2. SystemReadOnly Role is NOT deployed on the target machine.
#         Depending on the case, a different set of rules (stored into a different XML file) must be installed.
OUTPUT=`immfind roleId=SystemReadOnly,localAuthorizationMethodId=1 2> /dev/null`
if [ "$OUTPUT" == "" ] ; then
	# No SystemReadOnly role has been found, install the rules without it!
	execute immcfg -f $(dirname $(dirname $(readlink -f $0)))/$SDP_NAME/models/$ROLES_WITHOUT_SYSTEM_READ_ONLY_ROLE_FILENAME
else
	# SystemReadOnly role has been found, install the rules with it!
	execute immcfg -f $(dirname $(dirname $(readlink -f $0)))/$SDP_NAME/models/$ROLES_WITH_SYSTEM_READ_ONLY_ROLE_FILENAME
fi

# THIRD: Add the COM model file (MP file)
execute cmw-model-add $SDP_NAME --mt COM_R1
execute cmw-model-done

# FOURTH: Execute a COM switchover to apply the operation
comsa-mim-tool com_switchover

