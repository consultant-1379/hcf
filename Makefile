sinclude Makefile.stage-01.mk

#=============================================================================#
#                            HCF source code paths                            #
#=============================================================================#
HCF_ROOT_DIR              = $(HOME)/hcf
HCF_SCRIPTS_DIR           = $(HCF_ROOT_DIR)/scripts
HCF_CREATE_LINKS_SCRIPT   = $(HCF_SCRIPTS_DIR)/create_symlinks.sh
HCF_BUILD_RPM_SCRIPT      = $(HCF_SCRIPTS_DIR)/build_rpm.sh
HCF_BUILD_SDP_SCRIPT      = $(HCF_SCRIPTS_DIR)/build_sdp.sh
HCF_BUILD_ARCHIVES_SCRIPT = $(HCF_SCRIPTS_DIR)/build_archives.sh
DIRECTORY_LIST := hcfcoreapi hcfimmapi hcfinternalapi hcftraceapi hcfsrv hcfcmd

#=============================================================================#
#                           HCF product information                           #
#=============================================================================#
# The following variables store the information about the HCF product numbers # 
# The variable values don't need to be updated, neither for any major release #
#=============================================================================#
RUNTIME_CXP_NUM    = CXP9050113
SDK_CXP_NUM        = CXP9050114
D_TEMPLATE_CXP_NUM = CXP9050115
OAMMODELS_CXP_NUM  = CXP9028731
SRC_CXP_NUM        = CAA9013249
#=============================================================================#
# The following variables store the information about the HCF revision that   #
# will be built and deployed. Every time a new revision must be produced, the #
# following variables must be updated:                                        #
#    - PROD_REV_MAJOR (for each backward incompatible product release)        #
#    - PROD_REV_MINOR (for each backward compatible product release)          #
#    - PROD_REV_CORR  (the value '0' must be used for each PRA package,       # 
#                      otherwise it indicates the CP number)                  #
#    - PROD_REV_INFO  (represents an additional information for the produced  #
#                      product revision. It could also represent the candidate#
#                      identifier for a release. It MUST be removed when the  #
#                      official package is released).                         #
#    - PROD_R_STATE   (it must be ALWAYS be changed)                          #
#=============================================================================#
PROD_REV_MAJOR     = 2
PROD_REV_MINOR     = 0
PROD_REV_CORR      = 7
PROD_REV_INFO      =  
PROD_R_STATE       = R1H01

# Now, all the needed user-provided information is available.
# Let's calculate all the remaining information needed for the building 
# and packaging process.
# First, calculate the HCF complete product numbers, taking into account 
# both the product numbers and the major revision value.   
RUNTIME_PROD_NUM    = $(RUNTIME_CXP_NUM)_$(PROD_REV_MAJOR)
SDK_PROD_NUM        = $(SDK_CXP_NUM)_$(PROD_REV_MAJOR)
D_TEMPLATE_PROD_NUM = $(D_TEMPLATE_CXP_NUM)_$(PROD_REV_MAJOR)
OAMMODELS_PROD_NUM  = $(OAMMODELS_CXP_NUM)_$(PROD_REV_MAJOR)
SRC_PROD_NUM        = $(SRC_CXP_NUM)_$(PROD_REV_MAJOR)

# Second, calculate the revision identifier following the Major.Minor.Correction format.
# In case also some additional data has been provided, add also that information.
ifneq ($(PROD_REV_INFO),)
	PROD_REV_XYZ_FORMAT = $(PROD_REV_MAJOR).$(PROD_REV_MINOR).$(PROD_REV_CORR)-$(PROD_REV_INFO)
else
	PROD_REV_XYZ_FORMAT = $(PROD_REV_MAJOR).$(PROD_REV_MINOR).$(PROD_REV_CORR)
endif

# Then, calculate the string to be passed to each sub-makefile that could be useful for logging purposes.
PROD_REV_XYZ_AND_RSTATE=$(PROD_REV_XYZ_FORMAT)__$(PROD_R_STATE)

.PHONY: all
all: 
	$(SILENT)for d in $(DIRECTORY_LIST) ; do \
		echo "------------------------------" ; \
		$(MAKE) $(MFLAGS) BUILD_CONF_NAME=$(BUILD_CONF_NAME) PRODUCT_REVISION=$(PROD_REV_XYZ_AND_RSTATE) -C $$d; \
		if [ $$? != 0 ]; then \
			echo "Build failed for $$d"; \
			exit $$?; \
		fi; \
		echo "------------------------------" ; \
		echo; echo; \
	done

.PHONY: clean
clean: 
	$(SILENT)for d in $(DIRECTORY_LIST) ; do \
		echo "------------------------------" ; \
		$(MAKE) $(MFLAGS) BUILD_CONF_NAME=$(BUILD_CONF_NAME) PRODUCT_REVISION=$(PROD_REV_XYZ_AND_RSTATE) -C $$d clean; \
		if [ $$? != 0 ]; then \
			echo "Clean failed for $$d"; \
			exit $$?; \
		fi; \
		echo "------------------------------" ; \
		echo; echo; \
	done

.PHONY: veryclean
veryclean: 
	$(SILENT)for d in $(DIRECTORY_LIST) ; do \
		echo "------------------------------" ; \
		$(MAKE) $(MFLAGS) BUILD_CONF_NAME=$(BUILD_CONF_NAME) PRODUCT_REVISION=$(PROD_REV_XYZ_AND_RSTATE) -C $$d veryclean; \
		if [ $$? != 0 ]; then \
			echo "Very-Clean failed for $$d"; \
			exit $$?; \
		fi; \
		echo "------------------------------" ; \
		echo; echo; \
	done

.PHONY: rpm_package
rpm_package:  
	$(ECHO) "+------------------------------------------------+"
	$(ECHO) "|                  RPM BUILDING                  |"
	$(ECHO) "+------------------------------------------------+"
	$(SILENT) $(HCF_BUILD_RPM_SCRIPT) $(RUNTIME_PROD_NUM) $(PROD_R_STATE) $(BUILD_CONF_NAME) $(HCF_SDK_LDE_SDK_ROOT_DIR)
	$(SILENT)if [ $$? != 0 ]; then \
		$(ECHO) "RPM building failed!"; \
		exit $$?; \
	fi
	$(ECHO) "------------------------------"
	$(NEWLINE)
	$(NEWLINE)
	
.PHONY: sdp_packages
sdp_packages:
	$(ECHO) "+------------------------------------------------+"
	$(ECHO) "|                  SDP BUILDING                  |"
	$(ECHO) "+------------------------------------------------+"
	$(SILENT) $(HCF_BUILD_SDP_SCRIPT) $(RUNTIME_PROD_NUM) $(PROD_R_STATE)
	$(SILENT)if [ $$? != 0 ]; then \
		$(ECHO) "SDP building failed!"; \
		exit $$?; \
	fi
	$(ECHO) "------------------------------"
	$(NEWLINE)
	$(NEWLINE)

.PHONY: packages
packages: rpm_package sdp_packages ; 

.PHONY: archives
archives: packages
	$(ECHO) "+------------------------------------------------+"
	$(ECHO) "|                ARCHIVES BUILDING               |"
	$(ECHO) "+------------------------------------------------+"
	$(SILENT) $(HCF_BUILD_ARCHIVES_SCRIPT) $(RUNTIME_CXP_NUM) $(SDK_CXP_NUM) $(D_TEMPLATE_CXP_NUM) $(OAMMODELS_CXP_NUM) $(SRC_CXP_NUM) $(PROD_REV_XYZ_FORMAT) $(PROD_REV_MAJOR) $(PROD_R_STATE)
	$(SILENT)if [ $$? != 0 ]; then \
		$(ECHO) "Archives building failed!"; \
		exit $$?; \
	fi
	$(ECHO) "------------------------------"
	$(NEWLINE)
	$(NEWLINE)
	
.PHONY: links
links:
	$(ECHO) "+------------------------------------------------+"
	$(ECHO) "|                  LINKS BUILDING                |"
	$(ECHO) "+------------------------------------------------+"
	$(SILENT) $(HCF_CREATE_LINKS_SCRIPT) $(BUILD_CONF_NAME)   
	$(SILENT)if [ $$? != 0 ]; then \
		$(ECHO) "Links creation failed!"; \
		exit $$?; \
	fi
	$(ECHO) "------------------------------"
	$(NEWLINE)
	$(NEWLINE)

.PHONY: release
release: veryclean links all archives ; 

#=============================================================================#
#                                ONLY FOR DEBUG                               #
#=============================================================================#
# This makefile target is only for debug purposes.                            #
# It is useful for printing all the variables used in this makefile and to    #
# check their values.                                                         #
#=============================================================================#
#.PHONY: print_vars
#print_vars:
#	$(ECHO)
#	$(ECHO) "=============================="
#	$(ECHO)	"HCF_ROOT_DIR              == '$(HCF_ROOT_DIR)'"
#	$(ECHO)	"HCF_SCRIPTS_DIR           == '$(HCF_SCRIPTS_DIR)'"
#	$(ECHO)	"HCF_CREATE_LINKS_SCRIPT   == '$(HCF_CREATE_LINKS_SCRIPT)'"
#	$(ECHO)	"HCF_BUILD_RPM_SCRIPT      == '$(HCF_BUILD_RPM_SCRIPT)'"
#	$(ECHO)	"HCF_BUILD_SDP_SCRIPT      == '$(HCF_BUILD_SDP_SCRIPT)'"
#	$(ECHO)	"HCF_BUILD_ARCHIVES_SCRIPT == '$(HCF_BUILD_ARCHIVES_SCRIPT)'"
#	$(ECHO)	"DIRECTORY_LIST            == '$(DIRECTORY_LIST)'"
#	$(ECHO) "------------------------------"
#	$(ECHO)	"RUNTIME_CXP_NUM           == '$(RUNTIME_CXP_NUM)'"
#	$(ECHO)	"SDK_CXP_NUM               == '$(SDK_CXP_NUM)'"
#	$(ECHO)	"D_TEMPLATE_CXP_NUM        == '$(D_TEMPLATE_CXP_NUM)'"
#	$(ECHO)	"OAMMODELS_CXP_NUM         == '$(OAMMODELS_CXP_NUM)'"
#	$(ECHO)	"SRC_CXP_NUM               == '$(SRC_CXP_NUM)'"
#	$(ECHO) "------------------------------"
#	$(ECHO)	"PROD_REV_MAJOR            == '$(PROD_REV_MAJOR)'"
#	$(ECHO)	"PROD_REV_MINOR            == '$(PROD_REV_MINOR)'"
#	$(ECHO)	"PROD_REV_CORR             == '$(PROD_REV_CORR)'"
#	$(ECHO)	"PROD_REV_INFO             == '$(PROD_REV_INFO)'"
#	$(ECHO)	"PROD_R_STATE              == '$(PROD_R_STATE)'"
#	$(ECHO) "------------------------------"
#	$(ECHO)	"PROD_REV_XYZ_FORMAT       == '$(PROD_REV_XYZ_FORMAT)'"
#	$(ECHO)	"PROD_REV_XYZ_AND_RSTATE   == '$(PROD_REV_XYZ_AND_RSTATE)'"
#	$(ECHO) "------------------------------"
#	$(ECHO)	"RUNTIME_PROD_NUM          == '$(RUNTIME_PROD_NUM)'"
#	$(ECHO)	"SDK_PROD_NUM              == '$(SDK_PROD_NUM)'"
#	$(ECHO)	"D_TEMPLATE_PROD_NUM       == '$(D_TEMPLATE_PROD_NUM)'"
#	$(ECHO)	"OAMMODELS_PROD_NUM        == '$(OAMMODELS_PROD_NUM)'"
#	$(ECHO)	"SRC_PROD_NUM              == '$(SRC_PROD_NUM)'"
#	$(ECHO) "=============================="
#	$(ECHO)
