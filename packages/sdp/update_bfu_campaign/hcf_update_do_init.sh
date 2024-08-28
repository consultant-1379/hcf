#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# hcf_update_do_init.sh
##
# Description:
# This script is used during the HCF upgrade.
##
# Note:
# -
##
# Changelog:
# - 2015-03-30: Alessio Cascone (XCASALE)
# First version.
##

### BEGIN: Helper functions
print_usage()
{
	echo "Incorrect usage"
	echo "Usage:"
	echo "hcf_update_do_init.sh <old_sdp_dn> <new_sdp_name>"
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
if [ $# != 2 ] ; then
	print_usage
	exit 2
fi

OLD_SDP_NAME=`echo "$1" | sed 's/safSmfBundle=//'`
NEW_SDP_NAME=$2
ROLES_WITH_SYSTEM_READ_ONLY_ROLE_FILENAME="HCM_SecurityRoles_Instances_imm_objects.xml"
NBI_FILESYSTEM_API_CONFIGURATION_FILENAME="/usr/share/ericsson/cba/nbi-root-dir"

# Extract the HCF major revision number for the package to be removed
OLD_SDP_MAJOR_NUMBER=`echo $OLD_SDP_NAME | cut -d _ -f 2 | cut -d - -f 1` 

# FIRST: Remove the COM model associated to the old SDP package
#    This step must be different according to the old package major revision:
#        - Major == 1: The model must be removed with comsa-mim-tool XYZ commands 
#        - Major > 1 : The model must be removed with cmw-model-XYZ commands
if [ $OLD_SDP_MAJOR_NUMBER -eq 1 ]; then
	execute comsa-mim-tool remove ${OLD_SDP_NAME}
	execute comsa-mim-tool commit
else
	execute cmw-model-delete ${OLD_SDP_NAME} --mt COM_R1
fi

# SECOND: Install the COM model associated to the new SDP package
execute cmw-model-add ${NEW_SDP_NAME} --mt IMM_R3
execute cmw-model-add ${NEW_SDP_NAME} --mt COM_R1
execute cmw-model-done

# THIRD: Check if the SystemReadOnly role has been deployed: if yes, install the relative rule
OUTPUT=`immfind roleId=SystemReadOnly,localAuthorizationMethodId=1 2> /dev/null`
if [ "$OUTPUT" != "" ] ; then
	# SystemReadOnly role has been found, install the rules with it!
	execute immcfg -f $(dirname $(dirname $(readlink -f $0)))/$NEW_SDP_NAME/models/$ROLES_WITH_SYSTEM_READ_ONLY_ROLE_FILENAME
fi

# FOURTH: Execute a COM switchover to apply the operation
comsa-mim-tool com_switchover

# FIFTH: In case the system on which we are running supports the NBI Filesystem API, change the localFileStorePath attribute value to the new one 
if [ -e $NBI_FILESYSTEM_API_CONFIGURATION_FILENAME ] ; then
	NBI_PATH=`cat $NBI_FILESYSTEM_API_CONFIGURATION_FILENAME`
	for JOB in `immfind -c HCMHcJob`; do
		execute immcfg -a localFileStorePath=$NBI_PATH/health_check $JOB 
	done
fi
