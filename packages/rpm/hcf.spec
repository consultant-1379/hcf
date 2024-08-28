##
# ------------------------------------------------------------------------
# Copyright (C) 2014 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
# hcf.spec
##
# Description:
# Spec file for HCF rpm.
##
# Note:
# -
##
# Changelog:
# - 2015-07-14 Alessio Cascone (XCASALE)
# Added libhcftrace library deployment.
# - 2015-07-09 Alessio Cascone (XCASALE)
# Added libssh2 3PP library deployment.
# - 2015-04-22 Alessio Cascone (XCASALE)
# Added creation and deployment with hcf-srv user/group id.
# - 2015-01-13: Alessio Cascone (XCASALE)
# Improvements.
# - 2014-12-23: Alessio Cascone (XCASALE)
# First version.
##

# Define the functionalities to be activated/deactivated
%define HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS 1

# Allow the ability to override defaults with -D enable_no_root_permissions=1
%{?enable_no_root_permissions:%define enable_no_root_permissions 1}

# Define the packer name
%define PACKER %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)

# Some useful macros storing information about the HCF folder structure 
%define HCF_OPT_DIR /opt
%define HCF_OPT_HCF_DIR %{HCF_OPT_DIR}/hcf
%define HCF_BIN_DIR %{HCF_OPT_HCF_DIR}/bin
%define HCF_LIB_DIR %{HCF_OPT_HCF_DIR}/lib
%define HCF_PRIVATE_FOLDER_NAME /hcf-apr9010574
%define HCF_TMP_DIR /tmp%{HCF_PRIVATE_FOLDER_NAME}
%define HCF_NOBACKUP_CLUSTER_DIR /cluster/storage/no-backup%{HCF_PRIVATE_FOLDER_NAME}
%define HCF_CLUSTER_DIR_INSTALLING /installing
%define HCF_CLUSTER_DIR_INSTALLED /installed
%define HCF_CLUSTER_DIR_DELETING /deleting
%define HCF_SOURCE_ROOT_DIR %(echo $HOME)/hcf
%define HCF_OUTPUT_FOLDER_NAME /health_check
%define HCF_SERVER_ADDRESS_FILE_NAME .srv_addr
%define PSO_CONFIG_BACKUP_FOLDER_CONFIG_FILE_PATH /usr/share/pso/storage-paths/config
%define PSO_CONFIG_BACKUP_FOLDER_CONFIG_FILE_2_PATH /usr/share/ericsson/cba/pso/storage-paths/config

# Some useful macros storing information about the FileM configuration
%define NBI_PATH_CONFIG_FILE_PATH /usr/share/ericsson/cba/nbi-root-dir
%define FILEM_PATH_CONFIG_FILE_PATH /usr/share/filem/internal_filem_root.conf
%define FILEM_PATH_DEFAULT_VALUE /var/filem/internal_root

# Macros storing information about the HCF uid/gid(s)
%define HCF_SRV_USER_NAME hcf-srv
%define HCF_SRV_USER_ID 250
%define HCF_SRV_GROUP_NAME hcf-srv
%define HCF_SRV_GROUP_ID 250
%define SYSTEM_NBI_DATA_GROUP_NAME system-nbi-data
%define SYSTEM_NBI_DATA_GROUP_ID 302
%define CMW_IMM_USERS_GROUP_NAME cmw-imm-users
%define CMW_IMM_USERS_GROUP_ID 315
%define PSO_GROUP_NAME pso

# Macros to be used in order to avoid to use the phases magic numbers
%define HCF_INSTALL_DURING_POST_PHASE 1
%define HCF_UPGRADE_DURING_POST_PHASE 2
%define HCF_UPGRADE_DURING_POSTUN_PHASE 1
%define HCF_UNINSTALL_DURING_POSTUN_PHASE 0

# Macros storing libraries revisions
%define HCF_3PP_LIBSSH2_LIBRARY_NAME libssh2.so.1.0.1
%define HCF_3PP_LIBSSH2_LINK_NAME libssh2.so.1
%define HCF_3PP_LIBSSH2_SO_NAME libssh2.so
%define HCF_TRACE_LIBRARY_NAME libhcftrace.so.1.1.0
%define HCF_TRACE_LINK_NAME libhcftrace.so.1
%define HCF_TRACE_SO_NAME libhcftrace.so

# Macros used to remove dependencies from LTTnG libraries
%global _use_internal_dependency_generator 0
%global __find_requires_orig %{__find_requires}
%global __find_requires %{HCF_SOURCE_ROOT_DIR}/scripts/find_requires.sh %{__find_requires_orig}

Name:         HCF
License:      Other License(s), see package
Group:        Applications/HCF
Version:      %{PROD_NUM}
Release:      %{PROD_VER}
Summary:      Package containing HCF executables
Vendor:       Ericsson
Packager:     %PACKER

%description
Contains HCF - Health Check Framework

%install
# Create directories to be deployed on the target
if [ ! -d $RPM_BUILD_ROOT%{HCF_BIN_DIR} ]; then
	mkdir -p $RPM_BUILD_ROOT%{HCF_BIN_DIR}
fi

if [ ! -d $RPM_BUILD_ROOT%{HCF_LIB_DIR} ]; then
	mkdir -p $RPM_BUILD_ROOT%{HCF_LIB_DIR}
fi


# Copy the files to be included into the RPM package, assigning the right permissions
install --mode=755 %{HCF_SOURCE_ROOT_DIR}/hcfsrv/%{BUILD_CONF_NAME}/hcfd $RPM_BUILD_ROOT%{HCF_BIN_DIR}
install --mode=755 %{HCF_SOURCE_ROOT_DIR}/hcfcmd/%{BUILD_CONF_NAME}/hcrsfm $RPM_BUILD_ROOT%{HCF_BIN_DIR}
install --mode=755 %{HCF_SOURCE_ROOT_DIR}/3pp/libssh2/lib64/%{HCF_3PP_LIBSSH2_LIBRARY_NAME} $RPM_BUILD_ROOT%{HCF_LIB_DIR}
install --mode=755 %{HCF_SOURCE_ROOT_DIR}/hcftraceapi/%{BUILD_CONF_NAME}/%{HCF_TRACE_LIBRARY_NAME} $RPM_BUILD_ROOT%{HCF_LIB_DIR}
install --mode=755 %{HCF_SOURCE_ROOT_DIR}/scripts/hcf.sh $RPM_BUILD_ROOT%{HCF_BIN_DIR}
install --mode=755 %{HCF_SOURCE_ROOT_DIR}/scripts/hcrsfm.sh $RPM_BUILD_ROOT%{HCF_BIN_DIR}
install --mode=640 %{HCF_SOURCE_ROOT_DIR}/schemas/hcf_rsf_schema.xsd $RPM_BUILD_ROOT%{HCF_BIN_DIR}/hcf_rsf_schema.xsd

%pre
%if %{HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS}
# First, create the HCF secondary groups (used to access NBI and IMM)
/usr/sbin/groupadd -g %{SYSTEM_NBI_DATA_GROUP_ID} -r %{SYSTEM_NBI_DATA_GROUP_NAME} 2> /dev/null || :
/usr/sbin/groupadd -g %{CMW_IMM_USERS_GROUP_ID} -r %{CMW_IMM_USERS_GROUP_NAME} 2> /dev/null || :
 
# Then, create the HCF user and group, using the specified UID/GID
/usr/sbin/groupadd -g %{HCF_SRV_GROUP_ID} -r %{HCF_SRV_GROUP_NAME} 2> /dev/null || :
/usr/sbin/useradd  -d / -g %{HCF_SRV_GROUP_NAME} -G %{SYSTEM_NBI_DATA_GROUP_NAME},%{CMW_IMM_USERS_GROUP_NAME},%{PSO_GROUP_NAME} -r -s /bin/false -u %{HCF_SRV_USER_ID} %{HCF_SRV_USER_NAME} 2> /dev/null || :
%endif

%post
# Create some symbolic links to the HCF command executable, to libSSH2 library and to libhcftrace library
ln -sf %{HCF_BIN_DIR}/hcrsfm.sh /usr/bin/hcrsfm
ln -sf %{HCF_LIB_DIR}/%{HCF_3PP_LIBSSH2_LIBRARY_NAME} %{HCF_LIB_DIR}/%{HCF_3PP_LIBSSH2_LINK_NAME}
ln -sf %{HCF_LIB_DIR}/%{HCF_3PP_LIBSSH2_LINK_NAME} %{HCF_LIB_DIR}/%{HCF_3PP_LIBSSH2_SO_NAME}
ln -sf %{HCF_LIB_DIR}/%{HCF_TRACE_LIBRARY_NAME} %{HCF_LIB_DIR}/%{HCF_TRACE_LINK_NAME}
ln -sf %{HCF_LIB_DIR}/%{HCF_TRACE_LINK_NAME} %{HCF_LIB_DIR}/%{HCF_TRACE_SO_NAME}

if [ ! -d %{HCF_NOBACKUP_CLUSTER_DIR} ]; then
	mkdir --mode=750 -p %{HCF_NOBACKUP_CLUSTER_DIR}
fi

# Retrieve the path of the folder where the data that needs to participate to the backup must be stored
if [ -e %{PSO_CONFIG_BACKUP_FOLDER_CONFIG_FILE_PATH} ]; then
	HCF_BACKUP_CLUSTER_DIR=`cat %{PSO_CONFIG_BACKUP_FOLDER_CONFIG_FILE_PATH}`%{HCF_PRIVATE_FOLDER_NAME}
else
	HCF_BACKUP_CLUSTER_DIR=`cat %{PSO_CONFIG_BACKUP_FOLDER_CONFIG_FILE_2_PATH}`%{HCF_PRIVATE_FOLDER_NAME}
fi

# Create, if not yet done, the HCF command folders
if [ ! -d $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLING} ]; then
	mkdir --mode=750 -p $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLING}
else
	chmod 750 $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLING}
fi

if [ ! -d $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLED} ]; then
	mkdir --mode=750 -p $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLED}
else
	chmod 750 $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLED}
fi

if [ ! -d $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_DELETING} ]; then
	mkdir --mode=750 -p $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_DELETING}
else
	chmod 750 $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_DELETING}
fi

# We are installing the RPM: let's check if the RSFs must be moved from no-backup area to backup area 
if [ -d %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_INSTALLING} ]; then
	mv %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_INSTALLING}/* $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLING} || :
fi

if [ -d %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_INSTALLED} ]; then
	mv %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_INSTALLED}/* $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLED} || :
fi

if [ -d %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_DELETING} ]; then
	mv %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_DELETING}/* $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_DELETING} || :
fi


%if %{HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS}
# Take the ownership of all the HCF private files  
/bin/chown -Rf %{HCF_SRV_USER_NAME}:%{HCF_SRV_GROUP_NAME} %{HCF_NOBACKUP_CLUSTER_DIR}
/bin/chown -Rf %{HCF_SRV_USER_NAME}:%{HCF_SRV_GROUP_NAME} $HCF_BACKUP_CLUSTER_DIR

# fix for issue described in JIRA CC-15498
/bin/chmod -f 2750 $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLING} || :
/bin/chmod -f 2750 $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLED} || :
/bin/chmod -f 2750 $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_DELETING} || :

/bin/chown -Rf %{HCF_SRV_USER_NAME}:%{HCF_SRV_GROUP_NAME} %{HCF_TMP_DIR}

if [ -e %{NBI_PATH_CONFIG_FILE_PATH} ]; then
	/bin/chown -Rf %{HCF_SRV_USER_NAME}:%{SYSTEM_NBI_DATA_GROUP_NAME} `cat %{NBI_PATH_CONFIG_FILE_PATH}`%{HCF_OUTPUT_FOLDER_NAME}/ || :
	/bin/chmod -f 2770 `cat %{NBI_PATH_CONFIG_FILE_PATH}`%{HCF_OUTPUT_FOLDER_NAME} || :
elif [ -e %{FILEM_PATH_CONFIG_FILE_PATH} ]; then
	/bin/chown -Rf %{HCF_SRV_USER_NAME}:%{SYSTEM_NBI_DATA_GROUP_NAME} `cat %{FILEM_PATH_CONFIG_FILE_PATH}`%{HCF_OUTPUT_FOLDER_NAME}/ || :
	/bin/chmod -f 2770 `cat %{FILEM_PATH_CONFIG_FILE_PATH}`%{HCF_OUTPUT_FOLDER_NAME} || :
elif [ -e %{FILEM_PATH_DEFAULT_VALUE} ]; then
	/bin/chown -Rf %{HCF_SRV_USER_NAME}:%{SYSTEM_NBI_DATA_GROUP_NAME} %{FILEM_PATH_DEFAULT_VALUE}%{HCF_OUTPUT_FOLDER_NAME}/ || :
	/bin/chmod -f 2770 %{FILEM_PATH_DEFAULT_VALUE}%{HCF_OUTPUT_FOLDER_NAME} || :
else 	
	/bin/chown -Rf %{HCF_SRV_USER_NAME}:%{SYSTEM_NBI_DATA_GROUP_NAME} %{HCF_OUTPUT_FOLDER_NAME}/ || :
	/bin/chmod -f 2770 %{HCF_OUTPUT_FOLDER_NAME} || :
fi

/bin/chown -R %{HCF_SRV_USER_NAME}:%{HCF_SRV_GROUP_NAME} %{HCF_OPT_HCF_DIR} 
/bin/chmod -f 640 %{HCF_NOBACKUP_CLUSTER_DIR}/%{HCF_SERVER_ADDRESS_FILE_NAME} || :
%endif

# fix for issue described in JIRA CC-15425
/bin/chown -f %{HCF_SRV_USER_NAME}:%{SYSTEM_NBI_DATA_GROUP_NAME} %{HCF_NOBACKUP_CLUSTER_DIR}
/bin/chmod -f 2750 %{HCF_NOBACKUP_CLUSTER_DIR}

%preun

if [ -d %{HCF_NOBACKUP_CLUSTER_DIR} ]; then
	mv %{HCF_NOBACKUP_CLUSTER_DIR}/hcf_rsf_schema.xsd %{HCF_BIN_DIR}/
fi

%postun
# Retrieve the path of the folder where the data that needs to participate to the backup must be stored
if [ -e %{PSO_CONFIG_BACKUP_FOLDER_CONFIG_FILE_PATH} ]; then
	HCF_BACKUP_CLUSTER_DIR=`cat %{PSO_CONFIG_BACKUP_FOLDER_CONFIG_FILE_PATH}`%{HCF_PRIVATE_FOLDER_NAME}
else
	HCF_BACKUP_CLUSTER_DIR=`cat %{PSO_CONFIG_BACKUP_FOLDER_CONFIG_FILE_2_PATH}`%{HCF_PRIVATE_FOLDER_NAME}
fi

# If we are in the simple removing phase, erase all HCF private folders
if [ $1 -eq %{HCF_UNINSTALL_DURING_POSTUN_PHASE} ]; then
	rm -rf %{HCF_NOBACKUP_CLUSTER_DIR}
	rm -rf $HCF_BACKUP_CLUSTER_DIR
	rm -f /usr/bin/hcrsfm
	rm -rf %{HCF_OPT_HCF_DIR}

# Delete the HCF output folder: to do this its needed to understand the complete path
	if [ -e %{NBI_PATH_CONFIG_FILE_PATH} ]; then
		rm -rf `cat %{NBI_PATH_CONFIG_FILE_PATH}`%{HCF_OUTPUT_FOLDER_NAME}
	elif [ -e %{FILEM_PATH_CONFIG_FILE_PATH} ]; then
		rm -rf `cat %{FILEM_PATH_CONFIG_FILE_PATH}`%{HCF_OUTPUT_FOLDER_NAME}
	elif [ -e %{FILEM_PATH_DEFAULT_VALUE} ]; then
		rm -rf %{FILEM_PATH_DEFAULT_VALUE}%{HCF_OUTPUT_FOLDER_NAME}
	else 	
		rm -rf %{HCF_OUTPUT_FOLDER_NAME}
	fi
else
	# Otherwise, let's check if the RSFs must be moved from backup area to no-backup area
	if [ -d %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_INSTALLING} ]; then
		mv $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLING}/* %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_INSTALLING} || :
	fi

	if [ -d %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_INSTALLED} ]; then
		mv $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_INSTALLED}/* %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_INSTALLED} || :
	fi
	
	if [ -d %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_DELETING} ]; then
		mv $HCF_BACKUP_CLUSTER_DIR%{HCF_CLUSTER_DIR_DELETING}/* %{HCF_NOBACKUP_CLUSTER_DIR}%{HCF_CLUSTER_DIR_DELETING} || :
	fi
fi

%posttrans

if [ -d %{HCF_NOBACKUP_CLUSTER_DIR} ]; then
	mv %{HCF_BIN_DIR}/hcf_rsf_schema.xsd %{HCF_NOBACKUP_CLUSTER_DIR}/
fi

%files
%defattr(-,root,root)
%{HCF_OPT_DIR}/


%changelog
* Tue Aug 22 2017 - luca.desimone  (at) itslab.it
- Fix for issues described in JIRA CC-15425 and CC-15498
* Thu Jul 09 2015 - alessio.cascone (at) itslab.it
- Added libhcftrace library deployment. 
* Thu Jul 09 2015 - alessio.cascone (at) itslab.it
- Added libssh2 3PP library deployment. 
* Tue Apr 22 2015 - alessio.cascone (at) itslab.it
- Added creation and deployment with hcf-srv user/group id 
* Thu Jan 13 2015 - alessio.cascone (at) itslab.it
- Improvements
