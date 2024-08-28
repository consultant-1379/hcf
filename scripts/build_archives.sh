#!/bin/bash
##
# ------------------------------------------------------------------------
# Copyright (C) 2015 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# build_archives.sh
##
# Description:
# This script is used to create the rpm package for HCF common component.
##
# Note:
# -
##
# Changelog:
# - 2015-03-11: Alessio Cascone (XCASALE)
# First version.
# - 2015-10-06: Alessio Cascone (XCASALE)
# Improvements for HCF 2.0.
# - 2015-11-12: Alessio Cascone (XCASALE)
# Alignment HCF delivery to new CBA naming convention.
##

### BEGIN: Helper functions
print_usage()
{
	echo "Incorrect usage"
	echo "Usage:"
	echo "build_archives.sh <runtime_prod_num> <sdk_prod_num> <templ_prod_num> <oammodels_prod_num> <src_prod_num> <prod_rev> <prod_rev_major> <prod_r_state>"
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

to_lower ()
{
	# Check the number of arguments
	if [ $# != 1 ] ; then
		echo "Usage: $0 str"
		exit 2
	fi
	
	# Make the provided string lowercase
	echo $1 | tr '[:upper:]' '[:lower:]'
}
### END: Helper functions

# Check input parameters
if [ $# != 8 ] ; then
	print_usage
	exit 2
fi

# Some useful variables
HCF_RUNTIME_PRODUCT_NUMBER=$1
HCF_SDK_PRODUCT_NUMBER=$(to_lower $2)
HCF_DEPLOYMENT_TEMPLATE_PRODUCT_NUMBER=$(to_lower $3)
HCF_OAMMODELS_PRODUCT_NUMBER=$(to_lower $4)
HCF_SRC_PRODUCT_NUMBER=$(to_lower $5)
HCF_PRODUCT_REVISION=$6
HCF_PRODUCT_REVISION_MAJOR=$7
HCF_PRODUCT_RSTATE=$8
HCF_ROOT_DIR=$HOME/hcf
HCF_SDP_DIR=$HCF_ROOT_DIR/packages/sdp
HCF_SCHEMAS_DIR=$HCF_ROOT_DIR/schemas
HCF_MODEL_DIR=$HCF_ROOT_DIR/model
HCF_TMP_DIR=$HCF_ROOT_DIR/tmp

# Runtime archive variables
HCF_RUNTIME_ARCHIVE_FILENAME=hcf-$HCF_PRODUCT_REVISION-runtime-sle-$(to_lower $HCF_RUNTIME_PRODUCT_NUMBER).tar.gz
HCF_RUNTIME_SDP_FILENAME=HCF-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE.sdp
HCF_RUNTIME_ARCHIVE_IDENTITY_FILENAME=IDENTITY
HCF_RUNTIME_ARCHIVE_IDENTITY_CONTENT="PACKAGE_NAME=hcf\nPACKAGE_ID=$(to_lower $HCF_RUNTIME_PRODUCT_NUMBER)\nPACKAGE_VERSION=$HCF_PRODUCT_REVISION\nPACKAGE_TYPE=runtime"
HCF_RUNTIME_SLES12_FOLDER_NAME=sle12

# SDK archive variables
HCF_SDK_ARCHIVE_FILENAME=hcf-$HCF_PRODUCT_REVISION-sdk-sle-$HCF_SDK_PRODUCT_NUMBER.tar.gz
HCF_SDK_ARCHIVE_ITEMS_LIST="hcf_outputfile_schema.xsd hcf_rsf_schema.xsd"
HCF_SDK_FOLDER_NAME=sdk

# Deployment archive variables
HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_FILENAME=hcf-$HCF_PRODUCT_REVISION-deployment-sle-$HCF_DEPLOYMENT_TEMPLATE_PRODUCT_NUMBER.tar.gz
HCF_DEPLOYMENT_TEMPLATE_SDP_FORMAT=HCF-*-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE.sdp
HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_IDENTITY_FILENAME=IDENTITY
HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_IDENTITY_CONTENT="PACKAGE_NAME=hcf\nPACKAGE_ID=$HCF_DEPLOYMENT_TEMPLATE_PRODUCT_NUMBER\nPACKAGE_VERSION=$HCF_PRODUCT_REVISION\nPACKAGE_TYPE=deployment"
HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_METADATA_FILENAME=upgrade-campaigns.metadata
HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_METADATA_CONTENT="upgrade-campaigns-schema-version=1.0\ncomponent-name=HCF\nbfu-capability-level=5\nbfu-oldest-gate-release=R1A01\nC+P=HCF-BFU-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE.sdp"
HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_AIT_METADATA_FILENAME=ait.metadata
HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_AIT_METADATA_CONTENT="ait-comp-version=\"1.0.0\"\nait-plugin-type=\"SMF\"\nait-smf-plugin-version=\"1.0.0\"\ninstallation-campaign=\"./HCF-I1-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE.sdp\""
HCF_DEPLOYMENT_TEMPLATE_CSM_METADATA_FILENAME=csm.metadata
HCF_DEPLOYMENT_TEMPLATE_CSM_METADATA_CONTENT="csm-metadata-version=1.0.0\ncsm-root=./"
HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_README_FILENAME=README
HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_README_CONTENT="The HCF Deployment Templates product (${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR) contains the following campaigns:\n\n- ERIC-HCF-I1-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE\n\tThis campaign must be used to install HCF on a double SC node configuration.\n\tTo use this campaign use the file: HCF-I1-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE.sdp\n\n- ERIC-HCF-I2-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE\n\tThis campaign must be used to install HCF on a single SC node configuration.\n\tTo use this campaign use the file: HCF-I2-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE.sdp\n\n- ERIC-HCF-R1-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE\n\tThis campaign must be used to remove HCF on a double SC node configuration.\n\tTo use this campaign use the file: HCF-R1-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE.sdp\n\n- ERIC-HCF-R2-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE\n\tThis campaign must be used to remove HCF on a single SC node configuration.\n\tTo use this campaign use the file: HCF-R2-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE.sdp\n\n- ERIC-HCF-BFU-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE\n\tThis campaign must be used to update HCF starting from any revision.\n\tTo use this campaign use the file: HCF-BFU-TEMPLATE-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE.sdp\n"
HCF_DEPLOYMENT_TEMPLATE_CSM_DIR=csm
HCF_DEPLOYMENT_TEMPLATE_CSM_YML_FILENAME=hcf-csm.yml

# OAM models archive variables
HCF_OAMMODELS_ARCHIVE_FILENAME=hcf-$HCF_PRODUCT_REVISION-oammodels-sle-$HCF_OAMMODELS_PRODUCT_NUMBER.tar.gz
HCF_OAMMODELS_MODEL_ARCHIVE_FILENAME=HCF_model.tar.gz
HCF_OAMMODELS_MODEL_ARCHIVE_ITEMS_PATH_LIST="$HCF_MODEL_DIR/HCM/HCM.emx $HCF_MODEL_DIR/HCM/HCM.di $HCF_MODEL_DIR/HCM/HCM.notation $HCF_MODEL_DIR/HCM/HCM.uml $HCF_MODEL_DIR/output_files/HCM_mp.xml $HCF_MODEL_DIR/output_files/HCM_DWAXE_mp.xml"
HCF_OAMMODELS_MODEL_ARCHIVE_ITEMS_LIST="HCM.emx HCM.di HCM.notation HCM.uml HCM_mp.xml HCM_DWAXE_mp.xml"
HCF_OAMMODELS_CPI_MODEL_ARCHIVE_FILENAME=HCF_CPI_model.tar.gz
HCF_OAMMODELS_CPI_MODEL_ARCHIVE_ITEMS_PATH_LIST="$HCF_MODEL_DIR/HCM_CPI/HCM_CPI.emx $HCF_MODEL_DIR/HCM_CPI/HCM_CPI.di $HCF_MODEL_DIR/HCM_CPI/HCM_CPI.notation $HCF_MODEL_DIR/HCM_CPI/HCM_CPI.uml $HCF_MODEL_DIR/output_files/HCM_CPI_DWAXE_mp.xml"
HCF_OAMMODELS_CPI_MODEL_ARCHIVE_ITEMS_LIST="HCM_CPI.emx HCM_CPI.di HCM_CPI.notation HCM_CPI.uml HCM_CPI_DWAXE_mp.xml"
HCF_OAMMODELS_FOLDER_NAME=oam-models

# Source code archive variables
HCF_SRC_ARCHIVE_FILENAME=hcf-$HCF_PRODUCT_REVISION-src-sle-$HCF_SRC_PRODUCT_NUMBER.tar.gz
HCF_SRC_ARCHIVE_ITEMS_LIST="3pp hcfcmd hcfcoreapi hcfimmapi hcfinternalapi hcftraceapi hcfsrv include model packages schemas scripts Makefile*"

# Execute the cleanup operations (if needed)
cd $HCF_ROOT_DIR
/bin/rm -rf $HCF_TMP_DIR $HCF_RUNTIME_ARCHIVE_FILENAME $HCF_SDK_ARCHIVE_FILENAME $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_FILENAME $HCF_OAMMODELS_ARCHIVE_FILENAME $HCF_SRC_ARCHIVE_FILENAME

### BEGIN: ARCHIVE 1: HCF Runtime archive
echo "Building HCF Runtime Archive ($HCF_RUNTIME_ARCHIVE_FILENAME)..."
/bin/mkdir $HCF_TMP_DIR
cd $HCF_TMP_DIR

# Build the IDENTITY file for this package
echo -e $HCF_RUNTIME_ARCHIVE_IDENTITY_CONTENT > $HCF_RUNTIME_ARCHIVE_IDENTITY_FILENAME

# Create the folder containing runtime SDP for SLES12 environment
execute /bin/mkdir -p $HCF_RUNTIME_SLES12_FOLDER_NAME

# Copy under SLES12 environment folder the HCF runtime SDP
execute /bin/cp -f $HCF_SDP_DIR/$HCF_RUNTIME_SDP_FILENAME $HCF_RUNTIME_SLES12_FOLDER_NAME 

# Create a symbolic link into the root folder to the HCF runtime SDP into SLES12 environment folder
execute /bin/ln -s $HCF_RUNTIME_SLES12_FOLDER_NAME/$HCF_RUNTIME_SDP_FILENAME $HCF_RUNTIME_SDP_FILENAME

# Build the archive and remove the temporary folder
execute /bin/tar --format=gnu -czf $HCF_ROOT_DIR/$HCF_RUNTIME_ARCHIVE_FILENAME $HCF_RUNTIME_SDP_FILENAME $HCF_RUNTIME_ARCHIVE_IDENTITY_FILENAME $HCF_RUNTIME_SLES12_FOLDER_NAME
/bin/rm -rf $HCF_TMP_DIR
echo
### END  : ARCHIVE 1: HCF Runtime archive

### BEGIN: ARCHIVE 2: HCF SDK archive
echo "Building HCF SDK ($HCF_SDK_ARCHIVE_FILENAME)..."
/bin/mkdir -p $HCF_TMP_DIR/$HCF_SDK_FOLDER_NAME
cd $HCF_SCHEMAS_DIR
/bin/cp -f $HCF_SDK_ARCHIVE_ITEMS_LIST $HCF_TMP_DIR/$HCF_SDK_FOLDER_NAME
cd $HCF_TMP_DIR
execute /bin/tar --format=gnu -czf $HCF_ROOT_DIR/$HCF_SDK_ARCHIVE_FILENAME $HCF_SDK_FOLDER_NAME
/bin/rm -rf $HCF_TMP_DIR
echo
### END  : ARCHIVE 2: HCF SDK archive

### BEGIN: ARCHIVE 3: HCF Deployment Template archive
echo "Building HCF Deployment Template Archive ($HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_FILENAME)..."
cd $HCF_SDP_DIR
mkdir $HCF_DEPLOYMENT_TEMPLATE_CSM_DIR

# Build the IDENTITY, the README and the metadata files for this package
echo -e $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_IDENTITY_CONTENT > $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_IDENTITY_FILENAME
echo -e $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_README_CONTENT > $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_README_FILENAME 
echo -e $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_METADATA_CONTENT > $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_METADATA_FILENAME
echo -e $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_AIT_METADATA_CONTENT > $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_AIT_METADATA_FILENAME
echo -e $HCF_DEPLOYMENT_TEMPLATE_CSM_METADATA_CONTENT > $HCF_DEPLOYMENT_TEMPLATE_CSM_METADATA_FILENAME

cat > ${HCF_DEPLOYMENT_TEMPLATE_CSM_DIR}/${HCF_DEPLOYMENT_TEMPLATE_CSM_YML_FILENAME} << EOL
csm-version: 1.0
functions:
- uid: hcf.base
  version: ${HCF_PRODUCT_REVISION}
  name: hcf.base
  description: Health Check Framework
  services:
  - hcf.base
services:
- uid: hcf.base
  name: hcf
  components:
  - name: hcf.control
    instance-of: hcf.control
  availability-properties:
    monitor-period: 0 s
    max-failure-nr: 5
components:
- uid: hcf.control
  name: hcf.control
  description: Healt Check Framework
  availability-manager: amf
  software:
    sdp: HCF-${HCF_RUNTIME_PRODUCT_NUMBER}_${HCF_PRODUCT_REVISION_MAJOR}
  availability-properties:
    control-policy:
      type: advanced
    multiplicity-policy:
      node-active: ONE
      node-standby: ONE
      cluster-active: ONE
      cluster-standby: ONE
    lifecycle-control:
      prefix: /opt
      start: hcf/bin/hcf.sh start
      stop: hcf/bin/hcf.sh cleanup
      monitor-keys:
      - key: hcfd_hck
        period: 240 s
        timeout: 180 s
      start-stop-timeout: 10 s
      promote-demote-timeout: 10 s
      migrate-timeout: 10 s
      recovery-policy: COMPONENT_RESTART
  supersedes:
  - base-component: HC-Control
    software: ERIC-HCF-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR
  meta-data:
    component-version: ${HCF_PRODUCT_REVISION}
    deliverable:
      deployment-package: ${HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_FILENAME}
      runtime-package: ${HCF_RUNTIME_ARCHIVE_FILENAME}
    software:
    - file-name: ${HCF_RUNTIME_SDP_FILENAME}
      bundle-name: ERIC-HCF-${HCF_RUNTIME_PRODUCT_NUMBER}_$HCF_PRODUCT_REVISION_MAJOR-$HCF_PRODUCT_RSTATE

EOL

# Build the archive and remove the IDENTITY and metadata files
execute /bin/tar --format=gnu -czf $HCF_ROOT_DIR/$HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_FILENAME $HCF_DEPLOYMENT_TEMPLATE_SDP_FORMAT $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_IDENTITY_FILENAME $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_README_FILENAME $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_METADATA_FILENAME $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_AIT_METADATA_FILENAME $HCF_DEPLOYMENT_TEMPLATE_CSM_DIR $HCF_DEPLOYMENT_TEMPLATE_CSM_METADATA_FILENAME
/bin/rm -f $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_IDENTITY_FILENAME $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_README_FILENAME $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_METADATA_FILENAME $HCF_DEPLOYMENT_TEMPLATE_ARCHIVE_AIT_METADATA_FILENAME $HCF_DEPLOYMENT_TEMPLATE_CSM_METADATA_FILENAME
/bin/rm -rf $HCF_DEPLOYMENT_TEMPLATE_CSM_DIR
echo
### END  : ARCHIVE 3: HCF Deployment Template archive

### BEGIN: ARCHIVE 4: HCF OAM Models archive
echo "Building HCF OAM Models Archive ($HCF_OAMMODELS_ARCHIVE_FILENAME)..."
/bin/mkdir -p $HCF_TMP_DIR/$HCF_OAMMODELS_FOLDER_NAME
cd $HCF_TMP_DIR

# Build the first sub-archive
/bin/cp -f $HCF_OAMMODELS_MODEL_ARCHIVE_ITEMS_PATH_LIST .
execute /bin/tar --format=gnu -czf $HCF_OAMMODELS_FOLDER_NAME/$HCF_OAMMODELS_MODEL_ARCHIVE_FILENAME $HCF_OAMMODELS_MODEL_ARCHIVE_ITEMS_LIST

# Build the second sub-archive
/bin/cp -f $HCF_OAMMODELS_CPI_MODEL_ARCHIVE_ITEMS_PATH_LIST .
execute /bin/tar --format=gnu -czf $HCF_OAMMODELS_FOLDER_NAME/$HCF_OAMMODELS_CPI_MODEL_ARCHIVE_FILENAME $HCF_OAMMODELS_CPI_MODEL_ARCHIVE_ITEMS_LIST

# Create the final archive with the two sub-archives
execute /bin/tar --format=gnu -czf $HCF_ROOT_DIR/$HCF_OAMMODELS_ARCHIVE_FILENAME $HCF_OAMMODELS_FOLDER_NAME   
/bin/rm -rf $HCF_TMP_DIR 
echo
### END  : ARCHIVE 4: HCF OAM Models archive

### BEGIN: ARCHIVE 5: HCF SRC archive
echo "Building HCF SRC Archive ($HCF_SRC_ARCHIVE_FILENAME)..."
cd $HCF_ROOT_DIR
execute /bin/tar --format=gnu -czf $HCF_SRC_ARCHIVE_FILENAME $HCF_SRC_ARCHIVE_ITEMS_LIST
### END  : ARCHIVE 5: HCF SRC archive
