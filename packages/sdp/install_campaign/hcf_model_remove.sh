#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# hcf_model_remove.sh
##
# Description:
# This script is used during the HCF installation to remove the HCM model files.
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
	echo "hcf_model_remove.sh <sdp_name>"
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

# FIRST: Remove the COM model file (MP file)
execute cmw-model-delete $SDP_NAME --mt COM_R1
execute cmw-model-done

# SECOND: Remove each object defined into the IMM objects file
for FILE in `/bin/ls $(dirname $(dirname $(readlink -f $0)))/$SDP_NAME/models/*Instances_imm_objects.xml`; do
	for OBJ in `grep "<dn>" $FILE | sort -r`; do
		OBJ=${OBJ#<dn>}
		OBJ=${OBJ%</dn>}
		immcfg -d $OBJ 2> /dev/null
	done
done

# THIRD: Remove each class defined into the IMM classes file
for CLASS in `grep '<class name=' $(dirname $(dirname $(readlink -f $0)))/$SDP_NAME/models/*imm_classes.xml | awk 'BEGIN {FS = "\""} { print $2 }' | sort | uniq`; do
	execute cmw-immclass-delete $CLASS
done

# FOURTH: Execute a COM switchover to apply the operation
comsa-mim-tool com_switchover

