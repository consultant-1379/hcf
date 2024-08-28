#===================================#
# Source, object and dependency     #
# file lists                        #
#                                   #
# Note: The project can provide its #
#       values for source files.    #
#===================================#
C_SOURCE_FILES ?= $(wildcard $(C_SRC_ROOT_DIR)*$(C_SUFFIX))
CXX_SOURCE_FILES ?= $(wildcard $(CXX_SRC_ROOT_DIR)*$(CXX_SUFFIX))

C_SOURCE_PATHS = $(addsuffix $(C_SUFFIX), $(addprefix $(C_SRC_ROOT_DIR), $(C_SOURCE_FILES)))
CXX_SOURCE_PATHS = $(addsuffix $(CXX_SUFFIX), $(addprefix $(CXX_SRC_ROOT_DIR), $(CXX_SOURCE_FILES)))

C_PP_FILES = $(C_SOURCE_PATHS:$(C_SUFFIX)=$(C_PP_SUFFIX))
CXX_PP_FILES = $(CXX_SOURCE_PATHS:$(CXX_SUFFIX)=$(CXX_PP_SUFFIX))

C_OBJ_FILES = $(subst $(C_SRC_ROOT_DIR),$(C_OBJ_CONF_ROOT_DIR),$(C_SOURCE_PATHS:$(C_SUFFIX)=$(OBJ_SUFFIX)))
CXX_OBJ_FILES = $(subst $(CXX_SRC_ROOT_DIR),$(CXX_OBJ_CONF_ROOT_DIR),$(CXX_SOURCE_PATHS:$(CXX_SUFFIX)=$(OBJ_SUFFIX)))

C_DEP_FILES = $(subst $(C_SRC_ROOT_DIR),$(C_DEP_CONF_ROOT_DIR),$(C_SOURCE_PATHS:$(C_SUFFIX)=$(DEP_SUFFIX)))
CXX_DEP_FILES = $(subst $(CXX_SRC_ROOT_DIR),$(CXX_DEP_CONF_ROOT_DIR),$(CXX_SOURCE_PATHS:$(CXX_SUFFIX)=$(DEP_SUFFIX)))


#=======================================#
# Object and dependency directory lists #
#                                       #
# Note: The project can provide its     #
#       values for that variables.      #
#=======================================#
C_OBJ_DIRS = $(patsubst %/,%/,$(sort $(dir $(C_OBJ_FILES))))
CXX_OBJ_DIRS = $(patsubst %/,%/,$(sort $(dir $(CXX_OBJ_FILES))))

C_DEP_DIRS = $(patsubst %/,%/,$(sort $(dir $(C_DEP_FILES))))
CXX_DEP_DIRS = $(patsubst %/,%/,$(sort $(dir $(CXX_DEP_FILES))))

ALL_OUTPUT_DIRS = $(sort $(OUTPUT_ROOT_DIR) $(C_OBJ_DIRS) $(CXX_OBJ_DIRS) $(C_DEP_DIRS) $(CXX_DEP_DIRS))

#==========================================================#
# C/C++ Defines Flags                                      #
#                                                          #
# Note: The project shuld provide define macros            #
#       C_DEFINES_BEFORE, C_DEFINES, C_DEFINES_EXTRA,      #
#       CXX_DEFINES_BEFORE, CXX_DEFINES, CXX_DEFINES_EXTRA #
#==========================================================#
__C_DEFINES__ = $(addprefix -D,$(C_DEFINES_BEFORE) $(C_DEFINES) $(C_DEFINES_EXTRA))
__CXX_DEFINES__ = $(addprefix -D,$(CXX_DEFINES_BEFORE) $(CXX_DEFINES) $(CXX_DEFINES_EXTRA))

#====================================================#
# C/C++ Option Flags                                 #
#                                                    #
# Note: As for define macros, the project should     #
#       provide other compile flags.                 #
#       C_FLAGS_BEFORE, C_FLAGS, C_FLAGS_EXTRA,      #
#       CXX_FLAGS_BEFORE, CXX_FLAGS, CXX_FLAGS_EXTRA #
#====================================================#
__C_FLAGS__ = $(C_FLAGS_BEFORE) $(C_FLAGS) $(C_FLAGS_EXTRA)
__CXX_FLAGS__ = $(CXX_FLAGS_BEFORE) $(CXX_FLAGS) $(CXX_FLAGS_EXTRA)

ifeq ($(MAKE_USE_IODEV_BUILD),1)
	#=====================================#
	# APG/IO SDKs development integration #
	#=====================================#
	IODEV_C_INCLUDE_DIRS = \
		$(IODEV_LOTC_SDK_INC_DIR) $(IODEV_LOTC_SDK_INC_DIR)/c++/4.3 $(IODEV_LOTC_SDK_INC_DIR)/c++/4.3/x86_64-suse-linux \
		$(IODEV_COREMW_SDK_INC_DIR) \
		$(IODEV_TRACE_SDK_INC_DIR)
	IODEV_CXX_INCLUDE_DIRS = \
		$(IODEV_LOTC_SDK_INC_DIR) $(IODEV_LOTC_SDK_INC_DIR)/c++/4.3 $(IODEV_LOTC_SDK_INC_DIR)/c++/4.3/x86_64-suse-linux \
		$(IODEV_COREMW_SDK_INC_DIR) \
		$(IODEV_TRACE_SDK_INC_DIR)		
else
	HCF_SDK_C_INCLUDE_DIRS = \
		$(HCF_SDK_LDE_SDK_INC_DIR) $(HCF_SDK_LDE_SDK_INC_DIR)/c++/4.3 $(HCF_SDK_LDE_SDK_INC_DIR)/c++/4.3/x86_64-suse-linux \
		$(HCF_SDK_COREMW_SDK_INC_DIR) \
		$(HCF_SDK_TRACE_SDK_INC_DIR)
	HCF_SDK_CXX_INCLUDE_DIRS = \
		$(HCF_SDK_LDE_SDK_INC_DIR) $(HCF_SDK_LDE_SDK_INC_DIR)/c++/4.3 $(HCF_SDK_LDE_SDK_INC_DIR)/c++/4.3/x86_64-suse-linux \
		$(HCF_SDK_COREMW_SDK_INC_DIR) \
		$(HCF_SDK_TRACE_SDK_INC_DIR)
endif

#==============================================#
# C/C++ Include Directories                    #
#                                              #
# Note: As for above flags, the project should #
#       provide include directories.           #
#==============================================#
__C_INCLUDE_DIRS__ += $(C_INCLUDE_BEFORE)
ifeq ($(MAKE_USE_IODEV_BUILD),1)
	__C_INCLUDE_DIRS__ += $(IODEV_C_INCLUDE_DIRS)
else
	__C_INCLUDE_DIRS__ += $(HCF_SDK_C_INCLUDE_DIRS)
endif
__C_INCLUDE_DIRS__ += $(C_INCLUDE) $(C_INCLUDE_EXTRA)

__CXX_INCLUDE_DIRS__ += $(CXX_INCLUDE_BEFORE)
ifeq ($(MAKE_USE_IODEV_BUILD),1)
	__CXX_INCLUDE_DIRS__ += $(IODEV_CXX_INCLUDE_DIRS)
else
__CXX_INCLUDE_DIRS__ += $(HCF_SDK_CXX_INCLUDE_DIRS)
endif
__CXX_INCLUDE_DIRS__ += $(CXX_INCLUDE) $(CXX_INCLUDE_EXTRA)

__C_INCLUDE_DIRS_FLAGS__ = $(addprefix -I,$(__C_INCLUDE_DIRS__))
__CXX_INCLUDE_DIRS_FLAGS__ = $(addprefix -I,$(__CXX_INCLUDE_DIRS__))

ifeq ($(MAKE_USE_IODEV_BUILD),1)
	#=====================================#
	# APG/IO SDKs development integration #
	#=====================================#
	IODEV_LD_LIB_DIRS = $(IODEV_LOTC_SDK_LIB_DIR) $(IODEV_COREMW_SDK_LIB_DIR) $(IODEV_TRACE_SDK_LIB_DIR)
	IODEV_LD_LIBS = $(IODEV_COREMW_LD_LIBS)
else
	HCF_SDK_LD_LIB_DIRS = $(HCF_SDK_LDE_SDK_LIB_DIR) $(HCF_SDK_COREMW_SDK_LIB_DIR) $(HCF_SDK_TRACE_SDK_LIB_DIR) 
endif

.PHONY: all
all: $(ALL_TARGETS_LIST) ;

#.PHONY: all
#all: __build_start_message__ __check_all_dirs__ $(ALL_TARGETS_LIST) | __print_include_dirs_info__
#	$(ECHO) "+==================+====================================================+"
#	$(ECHO) "| BUILD TERMINATED |"
#	$(ECHO) "+==================+"
#	$(NEWLINE)

.PHONY: __build_start_message__
__build_start_message__:
	$(NEWLINE)
	$(ECHO) "+===============+"
	$(ECHO) "| BUILD STARTED |"
	$(ECHO) "+===============+=======================================================+"
	$(ECHO) "SRC_ROOT_DIR == $(SRC_ROOT_DIR)"
	$(ECHO) "OBJ_ROOT_DIR == $(OBJ_ROOT_DIR)"
	$(ECHO) "DEP_ROOT_DIR == $(DEP_ROOT_DIR)"
	$(NEWLINE)
	$(ECHO) "C_SRC_ROOT_DIR == $(C_SRC_ROOT_DIR)"
	$(ECHO) "C_OBJ_ROOT_DIR == $(C_OBJ_ROOT_DIR)"
	$(ECHO) "C_DEP_ROOT_DIR == $(C_DEP_ROOT_DIR)"
	$(ECHO) "CXX_SRC_ROOT_DIR == $(CXX_SRC_ROOT_DIR)"
	$(ECHO) "CXX_OBJ_ROOT_DIR == $(CXX_OBJ_ROOT_DIR)"
	$(ECHO) "CXX_DEP_ROOT_DIR == $(CXX_DEP_ROOT_DIR)"
	$(NEWLINE)
	$(ECHO) "BUILD_VERSION_DATE_DAY == $(BUILD_VERSION_DATE_DAY)"
	$(ECHO) "BUILD_VERSION_DATE_WEEK == $(BUILD_VERSION_DATE_WEEK)"
	$(ECHO) "BUILD_VERSION_DATE_MONTH == $(BUILD_VERSION_DATE_MONTH)"
	$(ECHO) "BUILD_VERSION_DATE_YEAR == $(BUILD_VERSION_DATE_YEAR)"
	$(NEWLINE)
	$(ECHO) "BUILD_CONF_NAME == $(BUILD_CONF_NAME)"
	$(ECHO) "BUILD_CONF_OUTPUT_ROOT_DIR == $(BUILD_CONF_OUTPUT_ROOT_DIR)"
	$(NEWLINE)
	$(ECHO) "C_OBJ_CONF_ROOT_DIR == $(C_OBJ_CONF_ROOT_DIR)"
	$(ECHO) "C_DEP_CONF_ROOT_DIR == $(C_DEP_CONF_ROOT_DIR)"
	$(NEWLINE)
	$(ECHO) "CXX_OBJ_CONF_ROOT_DIR == $(CXX_OBJ_CONF_ROOT_DIR)"
	$(ECHO) "CXX_DEP_CONF_ROOT_DIR == $(CXX_DEP_CONF_ROOT_DIR)"
	$(NEWLINE)
	$(ECHO) "OUTPUT_ROOT_DIR == $(OUTPUT_ROOT_DIR)"
	$(NEWLINE)
	$(ECHO) "C_SOURCE_FILES == $(C_SOURCE_FILES)"
	$(ECHO) "CXX_SOURCE_FILES == $(CXX_SOURCE_FILES)"
	$(ECHO) "C_SOURCE_PATHS == $(C_SOURCE_PATHS)"
	$(ECHO) "CXX_SOURCE_PATHS == $(CXX_SOURCE_PATHS)"
	$(ECHO) "C_PP_FILES == $(C_PP_FILES)"
	$(ECHO) "CXX_PP_FILES == $(CXX_PP_FILES)"
	$(ECHO) "C_OBJ_FILES == $(C_OBJ_FILES)"
	$(ECHO) "CXX_OBJ_FILES == $(CXX_OBJ_FILES)"
	$(ECHO) "C_DEP_FILES == $(C_DEP_FILES)"
	$(ECHO) "CXX_DEP_FILES == $(CXX_DEP_FILES)"
	$(ECHO) "C_OBJ_DIRS == $(C_OBJ_DIRS)"
	$(ECHO) "CXX_OBJ_DIRS == $(CXX_OBJ_DIRS)"
	$(ECHO) "C_DEP_DIRS == $(C_DEP_DIRS)"
	$(ECHO) "CXX_DEP_DIRS == $(CXX_DEP_DIRS)"
	$(NEWLINE)
	$(ECHO) "ALL_OUTPUT_DIRS == $(ALL_OUTPUT_DIRS)"
	$(NEWLINE)

.PHONY: __check_all_dirs__
__check_all_dirs__: | $(ALL_OUTPUT_DIRS) ;

#__check_all_dirs__: __check_c_obj_dirs__ __check_cxx_obj_dirs__ __check_c_dep_dirs__ __check_cxx_dep_dirs__ ;
#
#.PHONY: __check_c_obj_dirs__
#__check_c_obj_dirs__: | $(C_OBJ_DIRS) ;
#
#.PHONY: __check_cxx_obj_dirs__
#__check_cxx_obj_dirs__: | $(CXX_OBJ_DIRS) ;
#
#.PHONY: __check_c_dep_dirs__
#__check_c_dep_dirs__: | $(C_DEP_DIRS) ;
#
#.PHONY: __check_cxx_dep_dirs__
#__check_cxx_dep_dirs__: | $(CXX_DEP_DIRS) ;

.PHONY: __print_include_dirs_info__
__print_include_dirs_info__:
	$(ECHO) "Used the following include directories"
	$(ECHO) "  $(__C_INCLUDE_DIRS__) $(__CXX_INCLUDE_DIRS__)"
	$(NEWLINE)

.PHONY: precompile
#precompile: __precompile_start_message__ $(PRECOMPILE_TARGETS_LIST)
precompile: $(PRECOMPILE_TARGETS_LIST) ;

#	$(NEWLINE)
#	$(ECHO) "+============================+"
#	$(ECHO) "| PRE-COMPILATION TERMINATED |"
#	$(ECHO) "+============================+"
#	$(NEWLINE)

.PHONY: __precompile_start_message__
__precompile_start_message__:
	$(ECHO) "+=========================+"
	$(ECHO) "| PRE-COMPILATION STARTED |"
	$(ECHO) "+=========================+"
	$(NEWLINE)

#========================#
# Make directories rules #
#========================#
#$(OUTPUT_ROOT_DIR) $(C_OBJ_DIRS) $(CXX_OBJ_DIRS) $(C_DEP_DIRS) $(CXX_DEP_DIRS):
$(ALL_OUTPUT_DIRS):
	$(ECHO) "Directory '$@' doesn't exist. Making it..."
	$(call run_command_if_exists,$(MKDIR),-p '$@')
	$(NEWLINE)

#==========================#
# C/C++ dependencies rules #
#==========================#
$(C_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX): __TARGET_FOR_DEP__ = $(C_OBJ_CONF_ROOT_DIR)$*$(OBJ_SUFFIX) $@
$(C_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX): $(C_SRC_ROOT_DIR)%$(C_SUFFIX) | $(dir $(C_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX))
	$(call C_make_depend,$(C_SRC_ROOT_DIR)$*$(C_SUFFIX),$@,$(__TARGET_FOR_DEP__))
	$(NEWLINE)

$(CXX_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX): __TARGET_FOR_DEP__ = $(CXX_OBJ_CONF_ROOT_DIR)$*$(OBJ_SUFFIX) $@
$(CXX_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX): $(CXX_SRC_ROOT_DIR)%$(CXX_SUFFIX) | $(dir $(CXX_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX))
	$(call CXX_make_depend,$(CXX_SRC_ROOT_DIR)$*$(CXX_SUFFIX),$@,$(__TARGET_FOR_DEP__))
	$(NEWLINE)

#===========================#
# C/C++ preprocessing rules #
#===========================#
$(C_SRC_ROOT_DIR)%$(C_PP_SUFFIX): $(C_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX) $(C_SRC_ROOT_DIR)%$(C_SUFFIX)
	$(ECHO) "Pre-Compiling file '$(C_SRC_ROOT_DIR)$*$(C_SUFFIX)' into pre-compilation source '$@'..."
	$(call run_command_if_exists,$(CPP),-o $@ -fmessage-length=0 $(__C_DEFINES__) $(__C_FLAGS__) $(__C_INCLUDE_DIRS_FLAGS__) $(C_SRC_ROOT_DIR)$*$(C_SUFFIX))
	$(NEWLINE)

$(CXX_SRC_ROOT_DIR)%$(CXX_PP_SUFFIX): $(CXX_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX) $(CXX_SRC_ROOT_DIR)%$(CXX_SUFFIX)
	$(ECHO) "Pre-Compiling file '$(CXX_SRC_ROOT_DIR)$*$(CXX_SUFFIX)' into pre-compilation source '$@'..."
	$(call run_command_if_exists,$(CXXPP),-o $@ -fmessage-length=0 $(__CXX_DEFINES__) $(__CXX_FLAGS__) $(__CXX_INCLUDE_DIRS_FLAGS__) $(CXX_SRC_ROOT_DIR)$*$(CXX_SUFFIX))
	$(NEWLINE)

#=======================#
# C/C++ compiling rules #
#=======================#
$(C_OBJ_CONF_ROOT_DIR)%$(OBJ_SUFFIX): $(C_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX) $(C_SRC_ROOT_DIR)%$(C_SUFFIX) | $(dir $(C_OBJ_CONF_ROOT_DIR)%$(OBJ_SUFFIX))
	$(ECHO) "Compiling file '$(C_SRC_BASE_ROOT_DIR)$(C_SRC_ROOT_DIR)$*$(C_SUFFIX)' into object module '$@'..."
	$(call run_command_if_exists,$(CC),-c -o $@ -fmessage-length=0 $(__C_DEFINES__) $(__C_FLAGS__) $(__C_INCLUDE_DIRS_FLAGS__) $(C_SRC_BASE_ROOT_DIR)$(C_SRC_ROOT_DIR)$*$(C_SUFFIX))
	$(NEWLINE)

$(CXX_OBJ_CONF_ROOT_DIR)%$(OBJ_SUFFIX): $(CXX_DEP_CONF_ROOT_DIR)%$(DEP_SUFFIX) $(CXX_SRC_ROOT_DIR)%$(CXX_SUFFIX) | $(dir $(CXX_OBJ_CONF_ROOT_DIR)%$(OBJ_SUFFIX))
	$(ECHO) "Compiling file '$(CXX_SRC_BASE_ROOT_DIR)$(CXX_SRC_ROOT_DIR)$*$(CXX_SUFFIX)' into object module '$@'..."
	$(call run_command_if_exists,$(CXXC),-c -o $@ -fmessage-length=0 $(__CXX_DEFINES__) $(__CXX_FLAGS__) $(__CXX_INCLUDE_DIRS_FLAGS__) $(CXX_SRC_BASE_ROOT_DIR)$(CXX_SRC_ROOT_DIR)$*$(CXX_SUFFIX))
	$(NEWLINE)

#================#
# Cleaning rules #
#================#
.PHONY: clean
clean:
ifneq ($(strip $(C_OBJ_FILES)),)
	$(call run_command_if_exists,$(RM),-f $(C_OBJ_FILES))
endif
ifneq ($(strip $(CXX_OBJ_FILES)),)
	$(call run_command_if_exists,$(RM),-f $(CXX_OBJ_FILES))
endif
ifneq ($(strip $(C_DEP_FILES)),)
	$(call run_command_if_exists,$(RM),-f $(C_DEP_FILES))
endif
ifneq ($(strip $(CXX_DEP_FILES)),)
	$(call run_command_if_exists,$(RM),-f $(CXX_DEP_FILES))
endif

.PHONY: veryclean
veryclean:
ifneq ($(strip $(C_OBJ_DIRS)),)
	$(call run_command_if_exists,$(RM),-fR $(C_OBJ_CONF_ROOT_DIR)*)
endif
ifneq ($(strip $(CXX_OBJ_DIRS)),)
	$(call run_command_if_exists,$(RM),-fR $(CXX_OBJ_CONF_ROOT_DIR)*)
endif
ifneq ($(strip $(C_DEP_DIRS)),)
	$(call run_command_if_exists,$(RM),-fR $(C_DEP_CONF_ROOT_DIR)*)
endif
ifneq ($(strip $(CXX_DEP_DIRS)),)
	$(call run_command_if_exists,$(RM),-fR $(CXX_DEP_CONF_ROOT_DIR)*)
endif
ifneq ($(strip $(OUTPUT_ROOT_DIR)),)
	$(call run_command_if_exists,$(RM),-fR $(OUTPUT_ROOT_DIR)*)
endif
ifneq ($(strip $(C_PP_FILES)),)
	$(call run_command_if_exists,$(RM),-f $(C_PP_FILES))
endif
ifneq ($(strip $(CXX_PP_FILES)),)
	$(call run_command_if_exists,$(RM),-f $(CXX_PP_FILES))
endif

sinclude $(C_DEP_FILES) $(CXX_DEP_FILES)

#===================#
# UTILITY FUNCTIONS #
#===================#
# Usage: $(call run_command,cmd,args)
define run_command
	$1 $2
endef

# Usage: $(call run_command_if_exists,cmd,args)
define run_command_if_exists
	$(if $(shell which $(patsubst @%,%,$(firstword $1)) >/dev/null 2>&1 && echo FOUND),$(call run_command,$1,$2),$(call stop_by_error,Command '$(patsubst @%,%,$(firstword $1))' not found on the system. Stopping make...))
endef

# Usage: $(call stop_by_error,text)
define stop_by_error
	$(error $1)
endef

# Usage: $(call C_make_depend,src_file,dep_file,target)
define C_make_depend
	$(ECHO) "Generating dependencies for '$1' into the file '$2'..."
	$(call run_command_if_exists,$(CC),-MM -MF '$2' -MP -MT '$3' $(__C_FLAGS__) $(__C_INCLUDE_DIRS_FLAGS__) '$1')
endef

# Usage: $(call CXX_make_depend,src_file,dep_file,target)
define CXX_make_depend
	$(ECHO) "Generating dependencies for '$1' into the file '$2'..."
	$(call run_command_if_exists,$(CXXC),-MM -MF '$2' -MP -MT '$3' $(__CXX_FLAGS__) $(__CXX_INCLUDE_DIRS_FLAGS__) '$1')
endef

# Usage: $(call strip_symbols, infile, outfile)
define strip_symbols
	$(OBJCOPY) --only-keep-debug $1 $2
	$(OBJCOPY) --strip-debug $1
	$(OBJCOPY) --add-gnu-debuglink $2 $1
endef
