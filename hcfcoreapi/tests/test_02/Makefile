sinclude ../../../Makefile.stage-01.mk

APP_NAME := test_02

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
APP_SOURCE_FILES := \
	main 

CXX_SOURCE_FILES := $(APP_SOURCE_FILES)
#=======================================#================================#
# Health Check CC server source files   #
#=======================================#

ALL_CONFIG_FLAGS := -std=gnu++0x -Wall -Wextra -Wcomments -Wno-variadic-macros -fPIC #-fPIE -pedantic

CXX_DEFINES := HCF_CC_HAS_LOGGING HCF_CC_HAS_DUMPING HCF_CC_LOGGING_HAS_SOURCE_INFO HCF_CC_LOGGING_USE_PRETTY_FUNCTION
CXX_FLAGS := $(ALL_CONFIG_FLAGS)

APP_PROFILE_FLAGS := -pg
APP_COVERAGE_FLAGS := --coverage

#LIBS += -lgcov
#-fprofile-arcs -ftest-coverage -g


ifdef PROFILE
	CXX_FLAGS += $(APP_PROFILE_FLAGS)
endif

ifdef COVERAGE
	CXX_FLAGS += $(APP_COVERAGE_FLAGS)
endif

APP_INCLUDE := ../../../include 

CXX_INCLUDE := $(APP_INCLUDE)

__LD_FLAGS := -fPIC -fPIE -pie -rdynamic --build-id=uuid

sinclude Makefile.$(BUILD_CONF_NAME).mk

ifdef PROFILE
	__LD_FLAGS += $(APP_PROFILE_FLAGS)
endif

ifdef COVERAGE
	__LD_FLAGS += $(APP_COVERAGE_FLAGS)
endif

APP_OUTPUT_FILE := $(OUTPUT_ROOT_DIR)$(APP_TARGET)
APP_OUTPUT_SYMBOLS_FILE := $(APP_OUTPUT_FILE)$(SYMBOLS_SUFFIX)

sinclude ../../../Makefile.stage-02.mk

# Put here other application library directories
#LD_LIB_DIRS_BEFORE :=
LD_LIB_DIRS := ../../../hcfcoreapi/release/

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

LD_LIBS_BEFORE := rt pthread
LD_LIBS := hcfcore SaLog

__LD_LIBS += $(LD_LIBS_BEFORE) $(LD_LIBS) $(LD_LIBS_EXTRA)

ifneq "$(strip $(__LD_LIBS))" ""
  __LD_LIBS_FLAGS := $(addprefix -l,$(__LD_LIBS))
endif

.PHONY: $(APP_TARGET)
$(APP_TARGET): $(APP_OUTPUT_FILE) ;

.PHONY: $(APP_OUTPUT_FILE)
#$(APP_OUTPUT_FILE): $(CXX_OBJ_FILES) | $(OUTPUT_ROOT_DIR) __print_library_dirs_info__ __print_library_link_info__
$(APP_OUTPUT_FILE): $(CXX_OBJ_FILES) | $(OUTPUT_ROOT_DIR)
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
$(APP_TARGET)_precompile: $(CXX_PP_FILES)
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
