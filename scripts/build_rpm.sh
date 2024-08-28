#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2014 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# build_rpm.sh
##
# Description:
# This script is used to create the rpm package for HCF common component.
##
# Note:
# -
##
# Changelog:
# - 2014-12-23: Alessio Cascone (XCASALE)
# First version.
# - 2016-03-10: Alessio Cascone (XCASALE)
# Updated with new product numbers.
##

### BEGIN: Helper functions
print_usage()
{
	echo "Incorrect usage"
	echo "Usage:"
	echo "build_rpm.sh <prod_number> <prod_version> <build_conf_name> <lde_sdk_path>"
	echo
}
### END: Helper functions

echo "LDE SDK PATH: $4"

# Check input parameters
if [ $# != 4 ] ; then
	print_usage
	exit 2
fi

# Some useful variables
PROD_NUM=$1
PROD_VER=$2
BUILD_CONF_NAME=$3
HCF_LDE_SDK_ROOT_DIR=$4
HCF_ROOT_DIR=$HOME/hcf
HCF_RPM_DIR=$HCF_ROOT_DIR/packages/rpm
HCF_RPM_BUILD_ROOT=$HCF_RPM_DIR/tmp
HCF_SYSTEM_RPM_OUTPUT_DIR=$HOME/rpmbuild/RPMS/x86_64/
HCF_SYSTEM_RPM_SRPMS_DIR=$HOME/rpmbuild/SRPMS
HCF_RPM_FILENAME=HCF-$PROD_NUM-$PROD_VER.x86_64.rpm

RPMBUILDER=$HCF_LDE_SDK_ROOT_DIR/usr/bin/rpmbuild

# Remove the old packages
echo "Removing old RPM packages (if any):"
/bin/rm -f $HCF_RPM_DIR/*.rpm
echo

# Create (if not existing) the temporary RPM build root directory
if [ ! -d $HCF_RPM_BUILD_ROOT ] ; then
   /bin/mkdir $HCF_RPM_BUILD_ROOT
fi

# Execute the command to create the RPM package
echo "Building RPM package (Product: $PROD_NUM Revision: $PROD_VER CONF_NAME: $BUILD_CONF_NAME):"
$RPMBUILDER -vv -ba $HCF_RPM_DIR/hcf.spec --target x86_64 --buildroot $HCF_RPM_BUILD_ROOT "--define=PROD_NUM $PROD_NUM" "--define=PROD_VER $PROD_VER" "--define=BUILD_CONF_NAME $BUILD_CONF_NAME"

# Remove the temporary RPM build root directory
rm -rf $HCF_RPM_BUILD_ROOT

# Move the created RPM package under the predefined output directory
mv -f $HCF_SYSTEM_RPM_OUTPUT_DIR/$HCF_RPM_FILENAME $HCF_RPM_DIR

# Cleanup the SRPM system folder to remove old packages
rm -rf $HCF_SYSTEM_RPM_SRPMS_DIR/*

