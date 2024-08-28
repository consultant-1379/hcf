#!/bin/bash

##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# find_requires.sh
##
# Description:
# This script is used to remove any dependency from LTTnG libraries from the RPM package.
##
# Note:
# -
##
# Changelog:
# - 2015-08-11: Alessio Cascone (XCASALE)
# First version.
##

### BEGIN: Helper functions
print_usage()
{
	echo "Incorrect usage"
	echo "Usage:"
	echo "find_requires.sh <find_requres_cmd> <package_name>"
	echo
}
### END: Helper functions

# Check input parameters
if [ $# != 2 ] ; then
	print_usage
	exit 2
fi

# Some useful variables
FIND_REQUIRES_CMD=$1
PACKAGE_NAME=$2

# For each calculated dependency, print it only if not related to LTTnG
for DEP in `$FIND_REQUIRES_CMD $PACKAGE_NAME`; do
	if ! [[ $DEP =~ "lttng" ]] && ! [[ $DEP =~ "urcu" ]]; then
		echo $DEP
	fi
done
