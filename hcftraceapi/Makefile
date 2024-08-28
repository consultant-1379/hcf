sinclude ../Makefile.stage-01.mk

LIB_NAME := hcftrace

LIB_ALL_TARGET = $(LIB_PREFIX)$(LIB_NAME)
LIB_SHARED_TARGET = $(LIB_PREFIX)$(LIB_NAME)_shared
LIB_STATIC_TARGET = $(LIB_PREFIX)$(LIB_NAME)_static

LIB_ALL_SYMBOLS_TARGET = $(LIB_PREFIX)$(LIB_NAME)_symbols
LIB_SHARED_SYMBOLS_TARGET = $(LIB_PREFIX)$(LIB_NAME)_shared_symbols
LIB_STATIC_SYMBOLS_TARGET = $(LIB_PREFIX)$(LIB_NAME)_static_symbols

PRECOMPILE_TARGETS_LIST := $(LIB_ALL_TARGET)_precompile

VER_MAJOR ?= 1
VER_MINOR ?= 1
VER_BUILD ?= 0

LIB_VER_MAJOR := $(VER_MAJOR)
LIB_VER_MINOR := $(VER_MINOR)
LIB_VER_BUILD :=  $(VER_BUILD)
LIB_VER := $(LIB_VER_MAJOR).$(LIB_VER_MINOR).$(LIB_VER_BUILD)

LIB_SHARED_NAME := $(LIB_PREFIX)$(LIB_NAME)$(SHARED_LIB_SUFFIX).$(LIB_VER_MAJOR)
LIB_SHARED_FULL_NAME := $(LIB_PREFIX)$(LIB_NAME)$(SHARED_LIB_SUFFIX).$(LIB_VER)

LIB_STATIC_NAME := $(LIB_PREFIX)$(LIB_NAME)-$(LIB_VER_MAJOR)$(STATIC_LIB_SUFFIX)
LIB_STATIC_FULL_NAME := $(LIB_PREFIX)$(LIB_NAME)-$(LIB_VER)$(STATIC_LIB_SUFFIX)

#========================================#
# Health Check CC library source files   #
#========================================#================================#
C_SOURCE_FILES = \
	tracepoint_probe

CXX_SOURCE_FILES = \
	dso_init_fini \
	trace_utils

#========================================#================================#
# Health Check CC library source files   #
#========================================#

ALL_CONFIG_FLAGS := -Wall -Wextra -Wcomments -Wno-variadic-macros -fPIC # -pedantic -fPIE -std=gnu++0x 
CXX_FLAGS := $(ALL_CONFIG_FLAGS) -std=gnu++0x 
C_FLAGS := $(ALL_CONFIG_FLAGS)

LIB_PROFILE_FLAGS := -pg
LIB_COVERAGE_FLAGS := --coverage -g

#LIBS += -lgcov
#-fprofile-arcs -ftest-coverage
#-g for debbugging info

ifdef PROFILE
	CXX_FLAGS += $(LIB_PROFILE_FLAGS)
	C_FLAGS += $(LIB_PROFILE_FLAGS)
endif

ifdef COVERAGE
	CXX_FLAGS += $(LIB_COVERAGE_FLAGS)
	C_FLAGS += $(LIB_COVERAGE_FLAGS)
endif

LIB_INCLUDE := ../include
ifdef COVERAGE
	LIB_INCLUDE := $(abspath $(LIB_INCLUDE))
endif

CXX_INCLUDE := $(LIB_INCLUDE)
C_INCLUDE := $(LIB_INCLUDE)

__LD_FLAGS = -fPIC -fPIE -pie -rdynamic -shared -Wl,-soname=$(LIB_SHARED_NAME)
#__LD_FLAGS += --build-id


sinclude Makefile.$(BUILD_CONF_NAME).mk

ifdef PROFILE
	__LD_FLAGS += $(LIB_PROFILE_FLAGS)
endif

ifdef COVERAGE
	__LD_FLAGS += $(LIB_COVERAGE_FLAGS)
endif

LIB_SHARED_OUTPUT_FILE := $(OUTPUT_ROOT_DIR)$(LIB_SHARED_FULL_NAME)
LIB_SHARED_OUTPUT_SYMBOLS_FILE := $(LIB_SHARED_OUTPUT_FILE)$(SYMBOLS_SUFFIX)

LIB_STATIC_OUTPUT_FILE := $(OUTPUT_ROOT_DIR)$(LIB_STATIC_FULL_NAME)
LIB_STATIC_OUTPUT_SYMBOLS_FILE := $(LIB_STATIC_OUTPUT_FILE)$(SYMBOLS_SUFFIX)

sinclude ../Makefile.stage-02.mk

# Put here other application library directories
#LD_LIB_DIRS_BEFORE +=
LD_LIB_DIRS += 

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
LD_LIBS := lttng-ust lttng-ust-fork

__LD_LIBS += $(LD_LIBS_BEFORE) $(LD_LIBS) $(LD_LIBS_EXTRA)

ifneq "$(strip $(__LD_LIBS))" ""
	__LD_LIBS_FLAGS := $(addprefix -l,$(__LD_LIBS))
endif

.PHONY: $(LIB_ALL_TARGET)
$(LIB_ALL_TARGET): $(LIB_SHARED_TARGET) $(LIB_STATIC_TARGET) ;

.PHONY: $(LIB_SHARED_TARGET)
$(LIB_SHARED_TARGET): $(LIB_SHARED_OUTPUT_FILE) ;

.PHONY: $(LIB_SHARED_OUTPUT_FILE)
#$(LIB_SHARED_OUTPUT_FILE): $(C_OBJ_FILES) $(CXX_OBJ_FILES) | $(OUTPUT_ROOT_DIR) __print_library_dirs_info__ __print_library_link_info__
$(LIB_SHARED_OUTPUT_FILE): $(C_OBJ_FILES) $(CXX_OBJ_FILES) | $(OUTPUT_ROOT_DIR)
	$(ECHO) "Creating shared library '$@', version == '$(LIB_VER)', build config == '$(BUILD_CONF_NAME)'"
	$(call run_command_if_exists,$(CXXC),-o $@ $^ $(__LD_FLAGS) $(__LD_LIB_DIRS_FLAGS) $(__LD_LIBS_FLAGS))
	$(NEWLINE)

.PHONY: $(LIB_STATIC_TARGET)
$(LIB_STATIC_TARGET) : $(LIB_STATIC_OUTPUT_FILE) ;

.PHONY: $(LIB_STATIC_OUTPUT_FILE)
#$(LIB_STATIC_OUTPUT_FILE): $(C_OBJ_FILES) $(CXX_OBJ_FILES) | $(OUTPUT_ROOT_DIR) __print_library_dirs_info__ __print_library_link_info__
$(LIB_STATIC_OUTPUT_FILE): $(C_OBJ_FILES) $(CXX_OBJ_FILES) | $(OUTPUT_ROOT_DIR)
	$(ECHO) "Creating static library '$@', version == '$(LIB_VER)', build config == '$(BUILD_CONF_NAME)'"
	$(call run_command_if_exists,$(AR),crs $@ $^)
	$(NEWLINE)

.PHONY: $(LIB_ALL_SYMBOLS_TARGET)
$(LIB_ALL_SYMBOLS_TARGET): $(LIB_SHARED_SYMBOLS_TARGET) $(LIB_STATIC_SYMBOLS_TARGET) ;

.PHONY: $(LIB_SHARED_SYMBOLS_TARGET)
$(LIB_SHARED_SYMBOLS_TARGET): $(LIB_SHARED_OUTPUT_SYMBOLS_FILE) ;

.PHONY: $(LIB_SHARED_OUTPUT_SYMBOLS_FILE)
$(LIB_SHARED_OUTPUT_SYMBOLS_FILE): $(LIB_SHARED_OUTPUT_FILE) | $(OUTPUT_ROOT_DIR)
	$(ECHO) "Building symbols file '$@', build config == '$(BUILD_CONF_NAME)'"
	$(call strip_symbols, $(LIB_SHARED_OUTPUT_FILE), $@)
	$(NEWLINE)

.PHONY: $(LIB_STATIC_SYMBOLS_TARGET)
$(LIB_STATIC_SYMBOLS_TARGET): $(LIB_STATIC_OUTPUT_SYMBOLS_FILE) ;

.PHONY: $(LIB_STATIC_OUTPUT_SYMBOLS_FILE)
$(LIB_STATIC_OUTPUT_SYMBOLS_FILE): $(LIB_STATIC_OUTPUT_FILE) | $(OUTPUT_ROOT_DIR)
	$(NEWLINE)

#	$(ECHO) "WARNING: No symbols file needed for static libraries, build config == '$(BUILD_CONF_NAME)'"
#	$(NEWLINE)
#
#	$(call strip_symbols, $(LIB_SHARED_OUTPUT_FILE), $@)
#	$(NEWLINE)

.PHONY: $(LIB_ALL_TARGET)_precompile
$(LIB_ALL_TARGET)_precompile: $(C_PP_FILES) $(CXX_PP_FILES)
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
