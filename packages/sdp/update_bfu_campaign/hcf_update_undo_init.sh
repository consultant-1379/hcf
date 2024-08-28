#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# hcf_update_undo_init.sh
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
	echo "hcf_update_undo_init.sh <old_sdp_dn> <new_sdp_name>"
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

# Extract the HCF major revision number for the package to be removed
OLD_SDP_MAJOR_NUMBER=`echo $OLD_SDP_NAME | cut -d _ -f 2 | cut -d - -f 1` 

# FIRST: Remove the COM model associated to the new SDP package
execute cmw-model-delete ${NEW_SDP_NAME} --mt COM_R1

# SECOND: Install the COM model associated to the old SDP package
if [ $OLD_SDP_MAJOR_NUMBER -eq 1 ]; then
	execute comsa-mim-tool add ${OLD_SDP_NAME}
	execute comsa-mim-tool commit
else
	execute cmw-model-add ${OLD_SDP_NAME} --mt COM_R1
fi

execute cmw-model-done

# THIRD: Execute a COM switchover to apply the operation
comsa-mim-tool com_switchover

