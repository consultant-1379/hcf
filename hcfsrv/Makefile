sinclude ../Makefile.stage-01.mk

APP_NAME := hcfd

APP_TARGET = $(APP_NAME)
APP_SYMBOLS_TARGET = $(APP_NAME)_symbols

#ALL_TARGETS_LIST = $(APP_TARGET)
PRECOMPILE_TARGETS_LIST = $(APP_TARGET)_precompile

VER_MAJOR ?= 0
VER_MINOR ?= 1

APP_VER_MAJOR := $(VER_MAJOR)
APP_VER_MINOR := $(VER_MINOR)
APP_VER_BUILD := $(BUILD_VERSION_DATE_MONTH)
APP_VER := $(APP_VER_MAJOR).$(APP_VER_MINOR).$(APP_VER_BUILD)

#=======================================#
# Health Check CC server source files   #
#=======================================#================================#
C_SOURCE_FILES := \
	tracepoint_adapter

CXX_SOURCE_FILES := \
	clock_alarm_notifier \
	cmd_option_agent \
	cmd_handler \
	configuration_handler \
	configurator \
	constants \
	date_time \
	date_time_parser \
	hcf_application_manager \
	hcfd \
	imm_object_hccalendar_event \
	imm_object_hcjob_scheduler \
	imm_object_hcjob \
	imm_object_hcperiodic_event \
	imm_object_hcrule_inputparameter \
	imm_object_hcrule_parameter \
	imm_object_hcrule \
	imm_object_hcsingle_event \
	imm_object_healthcheckm \
	job_executor \
	job_scheduling_policy \
	output_file_writer \
	root_worker_thread \
	rule_callbacks_handler \
	rule_executor \
	ruleset_file_parser \
	sftp_handler \
	srv_utils \
	worker_thread_set

#=======================================#================================#
# Health Check CC server source files   #
#=======================================#

CXX_DEFINES := HCF_CC_HAS_LOGGING HCF_CC_HAS_DUMPING HCF_CC_LOGGING_HAS_SOURCE_INFO 
CXX_DEFINES += HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS 
CXX_DEFINES += HCF_CC_PRODUCT_REVISION=$(PRODUCT_REVISION)

ALL_CONFIG_FLAGS := -Wall -Wextra -Wcomments -Wno-variadic-macros -fPIC #-fPIE  -pedantic
CXX_FLAGS := $(ALL_CONFIG_FLAGS) -std=gnu++0x
C_FLAGS := $(ALL_CONFIG_FLAGS)

APP_PROFILE_FLAGS := -pg
APP_COVERAGE_FLAGS := --coverage -g

#LIBS += -lgcov
#-fprofile-arcs -ftest-coverage -g


ifdef PROFILE
	CXX_FLAGS += $(APP_PROFILE_FLAGS)
endif

ifdef COVERAGE
	CXX_FLAGS += $(APP_COVERAGE_FLAGS)
endif

APP_INCLUDE := ../include ../hcfinternalapi/include ../3pp/libssh2/include

ifdef COVERAGE
	APP_INCLUDE := $(abspath $(APP_INCLUDE))
endif

C_INCLUDE := $(APP_INCLUDE)
CXX_INCLUDE := $(APP_INCLUDE)

__LD_FLAGS := -fPIC -fPIE -pie -rdynamic -Wl,-rpath=/opt/hcf/lib/

sinclude Makefile.$(BUILD_CONF_NAME).mk

ifdef PROFILE
	__LD_FLAGS += $(APP_PROFILE_FLAGS)
endif

ifdef COVERAGE
	__LD_FLAGS += $(APP_COVERAGE_FLAGS)
endif

APP_OUTPUT_FILE := $(OUTPUT_ROOT_DIR)$(APP_TARGET)
APP_OUTPUT_SYMBOLS_FILE := $(APP_OUTPUT_FILE)$(SYMBOLS_SUFFIX)

sinclude ../Makefile.stage-02.mk

# Put here other application library directories
#LD_LIB_DIRS_BEFORE :=
LD_LIB_DIRS := ../hcfcoreapi/$(BUILD_CONF_NAME) ../hcfimmapi/$(BUILD_CONF_NAME) ../hcfinternalapi/$(BUILD_CONF_NAME) ../3pp/libssh2/lib64

__LD_LIB_DIRS += $(LD_LIB_DIRS_BEFORE)
ifeq ($(MAKE_USE_IODEV_BUILD),1)
  __LD_LIB_DIRS += $(IODEV_LD_LIB_DIRS)
else
  __LD_LIB_DIRS += $(HCF_SDK_LD_LIB_DIRS)
endif
__LD_LIB_DIRS += $(LD_LIB_DIRS) $(LD_LIB_DIRS_EXTRA)

ifneq "$(strip $(__LD_LIB_DIRS))" ""
  __LD_LIB_DIRS_FLAGS := $(addprefix -L,$(__LD_LIB_DIRS))
endif

LD_LIBS_BEFORE := rt pthread dl gcc_s xml2 ssh2
LD_LIBS := SaLog SaAmf SaImmOm SaImmOi hcfcore hcfimmapi hcfinternal

__LD_LIBS += $(LD_LIBS_BEFORE) $(LD_LIBS) $(LD_LIBS_EXTRA)

ifneq "$(strip $(__LD_LIBS))" ""
  __LD_LIBS_FLAGS := $(addprefix -l,$(__LD_LIBS))
endif

.PHONY: $(APP_TARGET)
$(APP_TARGET): $(APP_OUTPUT_FILE) ;

.PHONY: $(APP_OUTPUT_FILE)
#$(APP_OUTPUT_FILE): $(C_OBJ_FILES) $(CXX_OBJ_FILES) | $(OUTPUT_ROOT_DIR) __print_library_dirs_info__ __print_library_link_info__
$(APP_OUTPUT_FILE): $(C_OBJ_FILES) $(CXX_OBJ_FILES) | $(OUTPUT_ROOT_DIR)
	$(ECHO) "Building application '$@', version == '$(APP_VER)', build config == '$(BUILD_CONF_NAME)'"
	$(call run_command_if_exists,$(CXXC),-o $@ $^ $(__LD_FLAGS) $(__LD_LIB_DIRS_FLAGS) $(__LD_LIBS_FLAGS))
	$(NEWLINE)

.PHONY: $(APP_SYMBOLS_TARGET)
$(APP_SYMBOLS_TARGET): $(APP_OUTPUT_SYMBOLS_FILE) ;

.PHONY: $(APP_OUTPUT_SYMBOLS_FILE)
$(APP_OUTPUT_SYMBOLS_FILE): $(APP_OUTPUT_FILE) | $(OUTPUT_ROOT_DIR)
	$(ECHO) "Building symbols file '$@', build config == '$(BUILD_CONF_NAME)'"
	$(call strip_symbols, $(APP_OUTPUT_FILE), $@)
	$(NEWLINE)

.PHONY: $(APP_TARGET)_precompile
$(APP_TARGET)_precompile: $(C_PP_FILES) $(CXX_PP_FILES)
	$(NEWLINE)

.PHONY: __print_library_dirs_info__
__print_library_dirs_info__:
	$(ECHO) "Using the following extra library directories"
	$(ECHO) "  $(__LD_LIB_DIRS)"
	$(NEWLINE)

.PHONY: __print_library_link_info__
__print_library_link_info__:
	$(ECHO) "Using the following library link"
	$(ECHO) "  $(__LD_LIBS_FLAGS)"
	$(NEWLINE)
