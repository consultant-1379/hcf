#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# create_symlinks.sh
##
# Description:
# This script is used to create symbolic links to the libraries present into HCF source code.
##
# Note:
# -
##
# Changelog:
# - 2015-05-18: Alessio Cascone (XCASALE)
# First version.
##

### BEGIN: Helper functions
print_usage()
{
	echo "Incorrect usage"
	echo "Usage:"
	echo "create_symlinks.sh <build_conf_name>"
	echo
}
### END: Helper functions

# Check input parameters
if [ $# -ne 1 ] ; then
	print_usage
	exit 2
fi

# Some useful variables, needed for each link type
BUILD_CONF_NAME=$1
HCF_ROOT_DIR=$HOME/hcf
HCF_LINK_TARGET_PATTERN=$(date +"%Y%m")
HCF_LINK_NAME_PREFIX="lib"

# FIRST: Create the links for the static libraries
HCF_STATIC_LIBS_LINK_NAME_SUFFIX=".a"
HCF_STATIC_LIBS_DIRS_LIST=(hcfcoreapi hcfimmapi hcfinternalapi)
HCF_STATIC_LIBS_LINK_NAMES_LIST=(hcfcore hcfimmapi hcfinternal)

for ((idx=0; idx<${#HCF_STATIC_LIBS_DIRS_LIST[*]}; ++idx)); do
	cd $HCF_ROOT_DIR/${HCF_STATIC_LIBS_DIRS_LIST[$idx]}/$BUILD_CONF_NAME/
	LINK_TARGET=$HCF_LINK_NAME_PREFIX${HCF_STATIC_LIBS_LINK_NAMES_LIST[$idx]}-0.1.$HCF_LINK_TARGET_PATTERN$HCF_STATIC_LIBS_LINK_NAME_SUFFIX
	LINK_NAME=$HCF_LINK_NAME_PREFIX${HCF_STATIC_LIBS_LINK_NAMES_LIST[$idx]}$HCF_STATIC_LIBS_LINK_NAME_SUFFIX
	echo "Creating symbolic link for '$LINK_TARGET' with name '$LINK_NAME'..."
	/bin/ln -sf $LINK_TARGET $LINK_NAME
	cd - &> /dev/null
done

# SECOND: Create the links for the shared libraries
HCF_STATIC_LIBS_LINK_NAME_SUFFIX=".so"
HCF_SHARED_LIBS_DIRS_LIST=(hcftraceapi)
HCF_SHARED_LIBS_LINK_NAMES_LIST=(hcftrace)
HCF_SHARED_LIBS_VERSIONS_LIST=(1.1.0)

for ((idx=0; idx<${#HCF_SHARED_LIBS_DIRS_LIST[*]}; ++idx)); do
	cd $HCF_ROOT_DIR/${HCF_SHARED_LIBS_DIRS_LIST[$idx]}/$BUILD_CONF_NAME/
	LINK_TARGET=$HCF_LINK_NAME_PREFIX${HCF_SHARED_LIBS_LINK_NAMES_LIST[$idx]}$HCF_STATIC_LIBS_LINK_NAME_SUFFIX.${HCF_SHARED_LIBS_VERSIONS_LIST[$idx]}
	LINK_NAME=$HCF_LINK_NAME_PREFIX${HCF_SHARED_LIBS_LINK_NAMES_LIST[$idx]}$HCF_STATIC_LIBS_LINK_NAME_SUFFIX
	echo "Creating symbolic link for '$LINK_TARGET' with name '$LINK_NAME'..."
	/bin/ln -sf $LINK_TARGET $LINK_NAME
	cd - &> /dev/null
done

