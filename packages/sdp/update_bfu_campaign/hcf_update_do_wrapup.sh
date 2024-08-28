#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# hcf_update_do_wrapup.sh
##
# Description:
# This script is used during the HCF upgrade, when the campaign is committed.
##
# Note:
# -
##
# Changelog:
# - 2015-10-30: Alessio Cascone (XCASALE)
# First version.
##

PSO_NOBACKUP_FOLDER_CONFIG_FILE_PATH="/usr/share/pso/storage-paths/no-backup"
PSO_NOBACKUP_FOLDER_CONFIG_FILE_2_PATH="/usr/share/ericsson/cba/pso/storage-paths/no-backup"
HCF_PRIVATE_FOLDER_NAME="/hcf-apr9010574"
HCF_RSF_INSTALLED_FOLDER_NAME="/installed"
HCF_RSF_INSTALLING_FOLDER_NAME="/installing"
HCF_RSF_DELETING_FOLDER_NAME="/deleting"

# FIRST: Calculate the HCF no-backup folder name
if [ -e $PSO_NOBACKUP_FOLDER_CONFIG_FILE_PATH ]; then
	HCF_NOBACKUP_FOLDER=`cat $PSO_NOBACKUP_FOLDER_CONFIG_FILE_PATH`$HCF_PRIVATE_FOLDER_NAME
else
	HCF_NOBACKUP_FOLDER=`cat $PSO_NOBACKUP_FOLDER_CONFIG_FILE_2_PATH`$HCF_PRIVATE_FOLDER_NAME
fi

# SECOND: If still existing, remove the HCF private folders under the no-backup area where the RSFs were stored 
rm -rf $HCF_NOBACKUP_FOLDER{$HCF_RSF_INSTALLED_FOLDER_NAME,$HCF_RSF_INSTALLING_FOLDER_NAME,$HCF_RSF_DELETING_FOLDER_NAME}
