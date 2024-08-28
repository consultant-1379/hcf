#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# build_sdp.sh
##
# Description:
# This script is used to create the SDP packages for HCF common component.
##
# Note:
# -
##
# Changelog:
# - 2015-01-09: Alessio Cascone (XCASALE)
# First version.
# - 2015-03-10: Alessio Cascone (XCASALE)
# Updated with new product numbers.
# - 2015-03-25: Alessio Cascone (XCASALE)
# Added update package creation.
# - 2015-08-04: Alessio Cascone (XCASALE)
# Added deployment of installation and removal campaign for single-node clusters.
##

### BEGIN: Helper functions
print_usage()
{
	echo "Incorrect usage"
	echo "Usage:"
	echo "build_sdp.sh <prod_number> <prod_version>"
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

# Some useful variables
PROD_NUM=$1
PROD_VER=$2
SDP_CMD=$HOME/cba_sdk/dx_sdk/packaging/bin/sdppackage
HCF_ROOT_DIR=$HOME/hcf
HCF_RPM_DIR=$HCF_ROOT_DIR/packages/rpm
HCF_RPM_FILENAME=HCF-$PROD_NUM-$PROD_VER.x86_64.rpm
HCF_RPM_LIST_FILENAME=rpm_list
HCF_SDP_DIR=$HCF_ROOT_DIR/packages/sdp
HCF_SDP_ETF_PRODUCT_PLACEHOLDER="|PROD|"
HCF_SDP_ETF_REVISION_PLACEHOLDER="|VER|"
HCF_SDP_ETF_FILENAME=ETF.xml
HCF_SDP_ETF_TEMPLATE_FILENAME=$HCF_SDP_ETF_FILENAME.template
HCF_SDP_CAMPAIGN_PRODUCT_PLACEHOLDER=$HCF_SDP_ETF_PRODUCT_PLACEHOLDER
HCF_SDP_CAMPAIGN_REVISION_PLACEHOLDER=$HCF_SDP_ETF_REVISION_PLACEHOLDER
HCF_SDP_CAMPAIGN_FILENAME=campaign.xml
HCF_SDP_CAMPAIGN_TEMPLATE_FILENAME=$HCF_SDP_CAMPAIGN_FILENAME.template

HCF_SDP_BUNDLE_OUTPUT_FILENAME=HCF-$PROD_NUM-$PROD_VER.sdp
HCF_SDP_BUNDLE_FILENAME=ERIC-$HCF_SDP_BUNDLE_OUTPUT_FILENAME
HCF_SDP_BUNDLE_DIR=$HCF_SDP_DIR/bundle
HCF_SDP_BUNDLE_BUILD_DIR=$HCF_SDP_BUNDLE_DIR/build
HCF_SDP_BUNDLE_SCRIPT_FILES_LIST="$HCF_SDP_BUNDLE_DIR/offline-install $HCF_SDP_BUNDLE_DIR/offline-remove"
HCF_SDP_BUNDLE_MODELS_DIR=models
HCF_SDP_BUNDLE_MODEL_DIR=$HCF_SDP_BUNDLE_BUILD_DIR/$HCF_SDP_BUNDLE_MODELS_DIR

HCF_SDP_INSTCAMP_1_OUTPUT_FILENAME=HCF-I1-TEMPLATE-$PROD_NUM-$PROD_VER.sdp
HCF_SDP_INSTCAMP_1_FILENAME=ERIC-$HCF_SDP_INSTCAMP_1_OUTPUT_FILENAME
HCF_SDP_INSTCAMP_1_DIR=$HCF_SDP_DIR/install_campaign
HCF_SDP_INSTCAMP_1_BUILD_DIR=$HCF_SDP_INSTCAMP_1_DIR/build
HCF_SDP_INSTCAMP_1_SCRIPT_FILES_LIST="$HCF_SDP_INSTCAMP_1_DIR/hcf_model_install.sh $HCF_SDP_INSTCAMP_1_DIR/hcf_model_remove.sh"

HCF_SDP_INSTCAMP_2_OUTPUT_FILENAME=HCF-I2-TEMPLATE-$PROD_NUM-$PROD_VER.sdp
HCF_SDP_INSTCAMP_2_FILENAME=ERIC-$HCF_SDP_INSTCAMP_2_OUTPUT_FILENAME
HCF_SDP_INSTCAMP_2_DIR=$HCF_SDP_DIR/install_single_campaign
HCF_SDP_INSTCAMP_2_BUILD_DIR=$HCF_SDP_INSTCAMP_2_DIR/build
HCF_SDP_INSTCAMP_2_SCRIPT_FILES_LIST="$HCF_SDP_INSTCAMP_2_DIR/hcf_model_install.sh $HCF_SDP_INSTCAMP_2_DIR/hcf_model_remove.sh"

HCF_SDP_REMOVECAMP_1_OUTPUT_FILENAME=HCF-R1-TEMPLATE-$PROD_NUM-$PROD_VER.sdp
HCF_SDP_REMOVECAMP_1_FILENAME=ERIC-$HCF_SDP_REMOVECAMP_1_OUTPUT_FILENAME
HCF_SDP_REMOVECAMP_1_DIR=$HCF_SDP_DIR/remove_campaign
HCF_SDP_REMOVECAMP_1_BUILD_DIR=$HCF_SDP_REMOVECAMP_1_DIR/build
HCF_SDP_REMOVECAMP_1_SCRIPT_FILES_LIST="$HCF_SDP_REMOVECAMP_1_DIR/hcf_remove.sh"

HCF_SDP_REMOVECAMP_2_OUTPUT_FILENAME=HCF-R2-TEMPLATE-$PROD_NUM-$PROD_VER.sdp
HCF_SDP_REMOVECAMP_2_FILENAME=ERIC-$HCF_SDP_REMOVECAMP_2_OUTPUT_FILENAME
HCF_SDP_REMOVECAMP_2_DIR=$HCF_SDP_DIR/remove_single_campaign
HCF_SDP_REMOVECAMP_2_BUILD_DIR=$HCF_SDP_REMOVECAMP_2_DIR/build
HCF_SDP_REMOVECAMP_2_SCRIPT_FILES_LIST="$HCF_SDP_REMOVECAMP_2_DIR/hcf_remove.sh"

HCF_SDP_UPDATECAMP_OUTPUT_FILENAME=HCF-BFU-TEMPLATE-$PROD_NUM-$PROD_VER.sdp
HCF_SDP_UPDATECAMP_FILENAME=ERIC-$HCF_SDP_UPDATECAMP_OUTPUT_FILENAME
HCF_SDP_UPDATECAMP_DIR=$HCF_SDP_DIR/update_bfu_campaign
HCF_SDP_UPDATECAMP_BUILD_DIR=$HCF_SDP_UPDATECAMP_DIR/build
HCF_SDP_UPDATECAMP_SCRIPT_FILES_LIST="$HCF_SDP_UPDATECAMP_DIR/hcf_update_do_init.sh $HCF_SDP_UPDATECAMP_DIR/hcf_update_undo_init.sh $HCF_SDP_UPDATECAMP_DIR/hcf_update_do_wrapup.sh"
HCF_SDP_UPDATECAMP_CAMPAIGN_TEMPLATE_FILENAME=campaign.template.xml
HCF_SDP_UPDATECAMP_CAMPAIGN_TEMPLATE_TEMPLATE_FILENAME=$HCF_SDP_UPDATECAMP_CAMPAIGN_TEMPLATE_FILENAME.template

HCF_MODEL_FILES_DIR=$HCF_ROOT_DIR/model/output_files
HCF_MODEL_FILES_LIST=$(/bin/ls -r $HCF_MODEL_FILES_DIR | /bin/egrep -v 'DWAXE|\.svg')
HCF_MODEL_FILE_NAMES=$(/bin/ls -r $HCF_MODEL_FILES_DIR | /bin/egrep -v 'DWAXE|no_readonly|\.svg')
HCF_IMM_MODEL_CONFIG_FILENAME=IMM_R3-model.config
HCF_COM_MODEL_CONFIG_FILENAME=COM_R1-model.config
HCF_SECRULES_MODEL_CONFIG_FILENAME=IMM-I-Local_Authorization_R1-model.config

# Execute the cleanup operations
/bin/rm -f $HCF_SDP_DIR/*.sdp

### BEGIN: SDP-1: BUNDLE SDP package
echo "Building BUNDLE SDP (Product: $PROD_NUM Revision: $PROD_VER):"

# Create the working directory to build the bundle SDP
if [ -d $HCF_SDP_BUNDLE_BUILD_DIR ]; then 
	/bin/rm -rf $HCF_SDP_BUNDLE_BUILD_DIR/* 
else
	/bin/mkdir $HCF_SDP_BUNDLE_BUILD_DIR
fi

# Create the ETF.xml file for the bundle SDP with the provided revision
/bin/sed "s/$HCF_SDP_ETF_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_ETF_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_BUNDLE_DIR/$HCF_SDP_ETF_TEMPLATE_FILENAME > $HCF_SDP_BUNDLE_BUILD_DIR/$HCF_SDP_ETF_FILENAME

# Copy the offline installation/removal scripts in the working folder
/bin/cp -f $HCF_SDP_BUNDLE_SCRIPT_FILES_LIST $HCF_SDP_BUNDLE_BUILD_DIR

# Create the list of RPMs to be included in the bundle package
HCF_RPM_NAME=${HCF_RPM_FILENAME%.x86_64.rpm}
echo "$HCF_RPM_FILENAME $HCF_RPM_NAME" > $HCF_SDP_BUNDLE_BUILD_DIR/$HCF_RPM_LIST_FILENAME

# Move the RPM package into the destination directory
execute /bin/cp -f $HCF_RPM_DIR/$HCF_RPM_FILENAME $HCF_SDP_BUNDLE_BUILD_DIR

# Create the folder containing the model files and copy all the needed files
/bin/mkdir $HCF_SDP_BUNDLE_MODEL_DIR 
for item in $HCF_MODEL_FILES_LIST; do
	/bin/cp -f $HCF_MODEL_FILES_DIR/$item $HCF_SDP_BUNDLE_MODEL_DIR 
done

# Create the IMM, SecurityRules and COM configuration files
for item in $HCF_MODEL_FILE_NAMES; do
	if [[ $item =~ "_mp.xml" ]] ; then
		# MP file must be added to COM configuration file
		echo $HCF_SDP_BUNDLE_MODELS_DIR/$item >> $HCF_SDP_BUNDLE_BUILD_DIR/$HCF_COM_MODEL_CONFIG_FILENAME
  elif [[ $item =~ "Security" ]] ; then
    # Security rule to be added to the Local Authorization configuration file
    echo $HCF_SDP_BUNDLE_MODELS_DIR/$item >> $HCF_SDP_BUNDLE_BUILD_DIR/$HCF_SECRULES_MODEL_CONFIG_FILENAME
	else
		# Classes and objects definitions files must be added to IMM configuration file
		echo $HCF_SDP_BUNDLE_MODELS_DIR/$item >> $HCF_SDP_BUNDLE_BUILD_DIR/$HCF_IMM_MODEL_CONFIG_FILENAME
	fi
done

# Create the Bundle SDP and rename it to the right name
execute $SDP_CMD --src $HCF_SDP_BUNDLE_BUILD_DIR --dest $HCF_SDP_DIR
/bin/mv -f $HCF_SDP_DIR/$HCF_SDP_BUNDLE_FILENAME $HCF_SDP_DIR/$HCF_SDP_BUNDLE_OUTPUT_FILENAME

# Execute the cleanup operations
/bin/rm -rf $HCF_SDP_BUNDLE_BUILD_DIR
echo; echo
### END  : SDP-1: BUNDLE SDP package

### BEGIN: SDP-2: INSTALLATION CAMPAIGN SDP package
echo "Building INSTALLATION CAMPAIGN SDP (Product: $PROD_NUM Revision: $PROD_VER):"

# Create the working directory to build the installation campaign SDP
if [ -d $HCF_SDP_INSTCAMP_1_BUILD_DIR ]; then 
	/bin/rm -rf $HCF_SDP_INSTCAMP_1_BUILD_DIR/* 
else
	/bin/mkdir $HCF_SDP_INSTCAMP_1_BUILD_DIR
fi

# Create the campaign.xml file for the installation campaign SDP with the provided product revision
/bin/sed "s/$HCF_SDP_CAMPAIGN_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_CAMPAIGN_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_INSTCAMP_1_DIR/$HCF_SDP_CAMPAIGN_TEMPLATE_FILENAME > $HCF_SDP_INSTCAMP_1_BUILD_DIR/$HCF_SDP_CAMPAIGN_FILENAME

# Create the ETF.xml file for the installation campaign SDP with the provided revision
/bin/sed "s/$HCF_SDP_ETF_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_ETF_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_INSTCAMP_1_DIR/$HCF_SDP_ETF_TEMPLATE_FILENAME > $HCF_SDP_INSTCAMP_1_BUILD_DIR/$HCF_SDP_ETF_FILENAME

# Copy the model installation/removal scripts in the working folder
/bin/cp -f $HCF_SDP_INSTCAMP_1_SCRIPT_FILES_LIST $HCF_SDP_INSTCAMP_1_BUILD_DIR

# Create the Installation Campaign SDP and rename it to the right name
execute $SDP_CMD --src $HCF_SDP_INSTCAMP_1_BUILD_DIR --dest $HCF_SDP_DIR
/bin/mv -f $HCF_SDP_DIR/$HCF_SDP_INSTCAMP_1_FILENAME $HCF_SDP_DIR/$HCF_SDP_INSTCAMP_1_OUTPUT_FILENAME

# Execute the cleanup operations
/bin/rm -rf $HCF_SDP_INSTCAMP_1_BUILD_DIR
echo; echo
### END  : SDP-2: INSTALLATION CAMPAIGN SDP package

### BEGIN: SDP-3: INSTALLATION CAMPAIGN (for single-node clusters) SDP package
echo "Building INSTALLATION CAMPAIGN SDP for Single-Node Clusters (Product: $PROD_NUM Revision: $PROD_VER):"

# Create the working directory to build the installation campaign SDP
if [ -d $HCF_SDP_INSTCAMP_2_BUILD_DIR ]; then 
	/bin/rm -rf $HCF_SDP_INSTCAMP_2_BUILD_DIR/* 
else
	/bin/mkdir $HCF_SDP_INSTCAMP_2_BUILD_DIR
fi

# Create the campaign.xml file for the installation campaign SDP with the provided product revision
/bin/sed "s/$HCF_SDP_CAMPAIGN_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_CAMPAIGN_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_INSTCAMP_2_DIR/$HCF_SDP_CAMPAIGN_TEMPLATE_FILENAME > $HCF_SDP_INSTCAMP_2_BUILD_DIR/$HCF_SDP_CAMPAIGN_FILENAME

# Create the ETF.xml file for the installation campaign SDP with the provided revision
/bin/sed "s/$HCF_SDP_ETF_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_ETF_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_INSTCAMP_2_DIR/$HCF_SDP_ETF_TEMPLATE_FILENAME > $HCF_SDP_INSTCAMP_2_BUILD_DIR/$HCF_SDP_ETF_FILENAME

# Copy the model installation/removal scripts in the working folder
/bin/cp -f $HCF_SDP_INSTCAMP_2_SCRIPT_FILES_LIST $HCF_SDP_INSTCAMP_2_BUILD_DIR

# Create the Installation Campaign for Single-Node clusters SDP and rename it to the right name
execute $SDP_CMD --src $HCF_SDP_INSTCAMP_2_BUILD_DIR --dest $HCF_SDP_DIR
/bin/mv -f $HCF_SDP_DIR/$HCF_SDP_INSTCAMP_2_FILENAME $HCF_SDP_DIR/$HCF_SDP_INSTCAMP_2_OUTPUT_FILENAME

# Execute the cleanup operations
/bin/rm -rf $HCF_SDP_INSTCAMP_2_BUILD_DIR
echo; echo
### END  : SDP-3: INSTALLATION CAMPAIGN (for single-node clusters) SDP package

### BEGIN: SDP-4: REMOVAL CAMPAIGN SDP package
echo "Building REMOVAL CAMPAIGN SDP (Product: $PROD_NUM Revision: $PROD_VER):"

# Create the working directory to build the removal campaign SDP
if [ -d $HCF_SDP_REMOVECAMP_1_BUILD_DIR ]; then 
	/bin/rm -rf $HCF_SDP_REMOVECAMP_1_BUILD_DIR/* 
else
	/bin/mkdir $HCF_SDP_REMOVECAMP_1_BUILD_DIR
fi

# Create the campaign.xml file for the removal campaign SDP with the provided product revision
/bin/sed "s/$HCF_SDP_CAMPAIGN_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_CAMPAIGN_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_REMOVECAMP_1_DIR/$HCF_SDP_CAMPAIGN_TEMPLATE_FILENAME > $HCF_SDP_REMOVECAMP_1_BUILD_DIR/$HCF_SDP_CAMPAIGN_FILENAME

# Create the ETF.xml file for the removal campaign SDP with the provided revision
/bin/sed "s/$HCF_SDP_ETF_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_ETF_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_REMOVECAMP_1_DIR/$HCF_SDP_ETF_TEMPLATE_FILENAME > $HCF_SDP_REMOVECAMP_1_BUILD_DIR/$HCF_SDP_ETF_FILENAME

# Copy the removal script in the working folder
/bin/cp -f $HCF_SDP_REMOVECAMP_1_SCRIPT_FILES_LIST $HCF_SDP_REMOVECAMP_1_BUILD_DIR

# Create the Removal Campaign SDP and rename it to the right name
execute $SDP_CMD --src $HCF_SDP_REMOVECAMP_1_BUILD_DIR --dest $HCF_SDP_DIR
/bin/mv -f $HCF_SDP_DIR/$HCF_SDP_REMOVECAMP_1_FILENAME $HCF_SDP_DIR/$HCF_SDP_REMOVECAMP_1_OUTPUT_FILENAME

# Execute the cleanup operations
/bin/rm -rf $HCF_SDP_REMOVECAMP_1_BUILD_DIR
echo; echo
### END  : SDP-4: REMOVAL CAMPAIGN SDP package

### BEGIN: SDP-5: REMOVAL CAMPAIGN (for single-node clusters) SDP package
echo "Building REMOVAL CAMPAIGN SDP for Single-Node Clusters (Product: $PROD_NUM Revision: $PROD_VER):"

# Create the working directory to build the removal campaign SDP
if [ -d $HCF_SDP_REMOVECAMP_2_BUILD_DIR ]; then 
	/bin/rm -rf $HCF_SDP_REMOVECAMP_2_BUILD_DIR/* 
else
	/bin/mkdir $HCF_SDP_REMOVECAMP_2_BUILD_DIR
fi

# Create the campaign.xml file for the removal campaign SDP with the provided product revision
/bin/sed "s/$HCF_SDP_CAMPAIGN_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_CAMPAIGN_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_REMOVECAMP_2_DIR/$HCF_SDP_CAMPAIGN_TEMPLATE_FILENAME > $HCF_SDP_REMOVECAMP_2_BUILD_DIR/$HCF_SDP_CAMPAIGN_FILENAME

# Create the ETF.xml file for the removal campaign SDP with the provided revision
/bin/sed "s/$HCF_SDP_ETF_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_ETF_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_REMOVECAMP_2_DIR/$HCF_SDP_ETF_TEMPLATE_FILENAME > $HCF_SDP_REMOVECAMP_2_BUILD_DIR/$HCF_SDP_ETF_FILENAME

# Copy the removal script in the working folder
/bin/cp -f $HCF_SDP_REMOVECAMP_2_SCRIPT_FILES_LIST $HCF_SDP_REMOVECAMP_2_BUILD_DIR

# Create the Removal Campaign SDP for single-node clusters and rename it to the right name
execute $SDP_CMD --src $HCF_SDP_REMOVECAMP_2_BUILD_DIR --dest $HCF_SDP_DIR
/bin/mv -f $HCF_SDP_DIR/$HCF_SDP_REMOVECAMP_2_FILENAME $HCF_SDP_DIR/$HCF_SDP_REMOVECAMP_2_OUTPUT_FILENAME

# Execute the cleanup operations
/bin/rm -rf $HCF_SDP_REMOVECAMP_2_BUILD_DIR
echo; echo
### END  : SDP-5: REMOVAL CAMPAIGN REMOVAL CAMPAIGN (for single-node clusters) SDP package

### BEGIN: SDP-6: UPGRADE CAMPAIGN (with BFU-CL5 support) SDP package
echo "Building UPGRADE CAMPAIGN (with BFU-CL5 support) SDP (Product: $PROD_NUM Revision: $PROD_VER):"

# Create the working directory to build the update campaign SDP
if [ -d $HCF_SDP_UPDATECAMP_BUILD_DIR ]; then 
	/bin/rm -rf $HCF_SDP_UPDATECAMP_BUILD_DIR/* 
else
	/bin/mkdir $HCF_SDP_UPDATECAMP_BUILD_DIR
fi

# Create the campaign.template.xml file for the update campaign SDP with the provided product revision
/bin/sed "s/$HCF_SDP_CAMPAIGN_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_CAMPAIGN_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_UPDATECAMP_DIR/$HCF_SDP_UPDATECAMP_CAMPAIGN_TEMPLATE_TEMPLATE_FILENAME > $HCF_SDP_UPDATECAMP_BUILD_DIR/$HCF_SDP_UPDATECAMP_CAMPAIGN_TEMPLATE_FILENAME

# Create the ETF.xml file for the removal campaign SDP with the provided revision
/bin/sed "s/$HCF_SDP_ETF_PRODUCT_PLACEHOLDER/$PROD_NUM/g;s/$HCF_SDP_ETF_REVISION_PLACEHOLDER/$PROD_VER/g" $HCF_SDP_UPDATECAMP_DIR/$HCF_SDP_ETF_TEMPLATE_FILENAME > $HCF_SDP_UPDATECAMP_BUILD_DIR/$HCF_SDP_ETF_FILENAME

# Copy the update scripts in the working folder
/bin/cp -f $HCF_SDP_UPDATECAMP_SCRIPT_FILES_LIST $HCF_SDP_UPDATECAMP_BUILD_DIR

# Create the Update Campaign SDP and rename it to the right name
execute $SDP_CMD --src $HCF_SDP_UPDATECAMP_BUILD_DIR --dest $HCF_SDP_DIR
/bin/mv -f $HCF_SDP_DIR/$HCF_SDP_UPDATECAMP_FILENAME $HCF_SDP_DIR/$HCF_SDP_UPDATECAMP_OUTPUT_FILENAME

# Execute the cleanup operations
/bin/rm -rf $HCF_SDP_UPDATECAMP_BUILD_DIR
### END  : SDP-6: UPGRADE CAMPAIGN (with BFU-CL5 support) SDP package

