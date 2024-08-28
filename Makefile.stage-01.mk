.SECONDEXPANSION:

SHELL := /bin/sh

ifeq ($(VERBOSE),1)
	SILENT :=
else
	SILENT := @
endif

#===================================#
# FLAG TO CHOOSE THE SDK TO BE USED #
#===================================#
MAKE_USE_IODEV_BUILD ?= 0
MAKE_USE_SLES12_BUILD ?= 1

ifeq ($(MAKE_USE_IODEV_BUILD),1)
	#================================#
	# APG/IO development integration #
	#================================#

	#==============================#
	# APG/IO development root path #
	#==============================#
	IODEV_ROOT_DIR ?= /vobs/IO_Developments

	#========================#
	# APG/IO tools root path #
	#========================#
	IODEV_TOOLS_ROOT_DIR ?= $(IODEV_ROOT_DIR)/Tools

	#===========================#
	# APG/IO 3PP SDKs root path #
	#===========================#
	IODEV_3PP_SDKs_ROOT_DIR := $(IODEV_ROOT_DIR)/3PP_SDK

	#=======================================#
	# APG/IO cross compiler commands prefix #
	#=======================================#
	CROSS_COMPILER_PREFIX ?= $(IODEV_ROOT_DIR)/DX_SDK/compilers/bin/x86_64-dx-linux-gnu-
else
	#===================#
	# HCF SDK root path #
	#===================#
	ifeq ($(MAKE_USE_SLES12_BUILD),1)
		HCF_SDK_ROOT_DIR ?= /home/$(USER)/cba_sdk/sles12
	else
		HCF_SDK_ROOT_DIR ?= /home/$(USER)/cba_sdk/sles11
	endif
	
	#====================#
	# DX Tools root path #
	#====================#
	HCF_DX_ROOT_DIR ?= /home/$(USER)/cba_sdk
	
	#=========================#
	# HCF SDK tools root path #
	#=========================#
	HCF_SDK_TOOLS_ROOT_DIR ?= /app 

	#=============================#
	# HCF documentation root path #
	#=============================#
	DOC_ROOT_DIR ?= $(HOME)/hcf/doc

	#========================================#
	# HCF SDK cross compiler commands prefix #
	#========================================#
	CROSS_COMPILER_PREFIX ?= $(HCF_DX_ROOT_DIR)/dx_sdk/compilers/bin/x86_64-dx-linux-gnu-
endif

#===============#
# Base commands #
#===============#
SET := $(SILENT)set
ECHO := $(SILENT)echo
INSTALL := $(SILENT)install
LD := $(SILENT)$(CROSS_COMPILER_PREFIX)ld
AR := $(SILENT)$(CROSS_COMPILER_PREFIX)ar
NM := $(SILENT)$(CROSS_COMPILER_PREFIX)nm
STRIP := $(SILENT)$(CROSS_COMPILER_PREFIX)strip
OBJCOPY := $(SILENT)$(CROSS_COMPILER_PREFIX)objcopy
#OBJCOPY_KEEP_DBG = --only-keep-debug
#OBJCOPY_STRIP_DBG = --strip-debug
#OBJCOPY_DBG_LINK = --add-gnu-debuglink
OBJDUMP := $(SILENT)$(CROSS_COMPILER_PREFIX)objdump
OBJDUMP_DBG_FLAGS = --only-keep-debug
OBJDUMP_STRIP_DBG = --strip-debug
OBJDUMP_DBG_LINK = --add-gnu-debuglink
READELF := $(SILENT)$(CROSS_COMPILER_PREFIX)readelf
AWK := $(SILENT)awk
SED := $(SILENT)sed
RM := $(SILENT)rm
MV := $(SILENT)mv
CP := $(SILENT)cp
MKDIR := $(SILENT)mkdir -p
TAR := $(SILENT)tar
TOUCH := $(SILENT)touch
DIFF := $(SILENT)diff
RSYNC := $(SILENT)rsync
CHMOD := $(SILENT)chmod

NEWLINE := @echo

BUILD_DATE := $(shell date --rfc-3339=seconds)

#====================#
# Assembler commands #
#====================#
AS := $(SILENT)$(CROSS_COMPILER_PREFIX)as

#====================#
# C and C++ commands #
#====================#
CC := $(SILENT)$(CROSS_COMPILER_PREFIX)gcc
CXXC := $(SILENT)$(CROSS_COMPILER_PREFIX)g++
PP_FLAG := -E
CPP := $(CC) $(PP_FLAG)
CXXPP := $(CXXC) $(PP_FLAG)

#============================#
# Software coverage commands #
#============================#
GCOV := $(SILENT)$(CROSS_COMPILER_PREFIX)gcov

ifeq ($(MAKE_USE_IODEV_BUILD),1)
LCOV := $(SILENT)$(IODEV_TOOLS_ROOT_DIR)/lcov/bin/lcov
LCOV_GENHTML := $(IODEV_TOOLS_ROOT_DIR)/lcov/bin/genhtml
else
LCOV := $(SILENT)$(HCF_SDK_TOOLS_ROOT_DIR)/lcov/1.10/LMWP3/bin/lcov
LCOV_GENHTML := $(IODEV_TOOLS_ROOT_DIR)/lcov/1.10/LMWP3/bin/genhtml
endif
LCOV_OUTPUT := $(DOC_ROOT_DIR)/coverage

#=======================#
# Doxygen documentation #
#=======================#
ifeq ($(MAKE_USE_IODEV_BUILD),1)
DOXYGEN := $(SILENT)$(IODEV_TOOLS_ROOT_DIR)/doxygen/bin/doxygen
DOXYGENFILECONFIG := $(IODEV_TOOLS_ROOT_DIR)/linux_conf_file/doxygen_common.conf
endif
DOXYGEN := $(SILENT)doxygen
DOXYGENFLAGS := -g 

#===========================#
# Software metrics commands #
#===========================#
ifeq ($(MAKE_USE_IODEV_BUILD),1)
CPPCHECK := $(SILENT)$(IODEV_TOOLS_ROOT_DIR)/cppcheck/cppcheck
CPPCHECK_HTMLREPORT :=$(SILENT)$(IODEV_TOOLS_ROOT_DIR)/cppcheck/htmlreport/cppcheck-htmlreport 
CPPCHECK_OUTPUT := $(DOC_ROOT_DIR)/cppcheck
CPPCHECK_FLAGS := --report-progress -v --xml --enable=all --force --template "{file},{line},{severity},{id},{message}"
CPPCHECK_REPORT := cppcheck.xml

CCCC := $(SILENT)$(IODEV_TOOLS_ROOT_DIR)/cccc/cccc/cccc
CCCC_OUTPUT := $(DOC_ROOT_DIR)/cccc
CCCC_REPORT_HTML := cccc.html
CCCC_FLAGS := --outdir=$(CCCC_OUTPUT) --html_outfile=$(CCCC_REPORT_HTM)

SLOCCOUNT := $(SILENT)$(IODEV_TOOLS_ROOT_DIR)/sloccount/sloccount
SLOCCOUNT_OUTPUT := $(DOC_ROOT_DIR)/sloccount
SLOCCOUNT_FLAGS := --duplicates --wide --details --multiproject
SLOCCOUNT_REPORT := sloccount.sc

#CPPNCSS := $(SILENT)cppncss
endif

#===============#
# Perl commands #
#===============#
PERL := $(SILENT)perl

#=============================#
# Source code styler commands #
#=============================#
ifeq ($(MAKE_USE_IODEV_BUILD),1)
ASTYLE := $(SILENT)$(IODEV_TOOLS_ROOT_DIR)/astyle/build/gcc/bin/astyle
ASTYLE_FLAGS := --style=allman --recursive
endif

#======================#
# XML grammar commands #
#======================#
XMLLINT := $(SILENT)xmllint
XSLTPROC := $(SILENT)xsltproc

#========================================================#
# Default values for object, dependency and source file  #
# directories.                                           #
#                                                        #
# Note: The project can provide its values for that      #
#       variables                                        #
#========================================================#
ifdef COVERAGE
	SRC_BASE_ROOT_DIR := $(shell pwd)/
endif
SRC_ROOT_DIR ?= src/
OBJ_ROOT_DIR ?= obj/
DEP_ROOT_DIR ?= dep/

C_SRC_BASE_ROOT_DIR ?= $(SRC_BASE_ROOT_DIR)
C_SRC_ROOT_DIR ?= $(SRC_ROOT_DIR)
C_OBJ_ROOT_DIR ?= $(OBJ_ROOT_DIR)
C_DEP_ROOT_DIR ?= $(DEP_ROOT_DIR)
CXX_SRC_BASE_ROOT_DIR ?= $(SRC_BASE_ROOT_DIR)
CXX_SRC_ROOT_DIR ?= $(SRC_ROOT_DIR)
CXX_OBJ_ROOT_DIR ?= $(OBJ_ROOT_DIR)
CXX_DEP_ROOT_DIR ?= $(DEP_ROOT_DIR)

#=============================================#
# Some default values for build version part. #
#                                             #
# Note: The project can provide its values    #
#       for that variables                    #
#=============================================#
BUILD_VERSION_DATE_DAY ?= $(shell date +"%Y%m%d")
BUILD_VERSION_DATE_WEEK ?= $(shell date +"%Yw%W")
BUILD_VERSION_DATE_MONTH ?= $(shell date +"%Y%m")
BUILD_VERSION_DATE_YEAR ?= $(shell date +"%Y")

#==========================================#
# Build configuration handling             #
#                                          #
# Note: The project can provide its values #
#       for that variables                 #
#==========================================#
BUILD_CONF_NAME ?= release
BUILD_CONF_OUTPUT_ROOT_DIR ?= $(BUILD_CONF_NAME)/

OUTPUT_ROOT_DIR ?= $(BUILD_CONF_OUTPUT_ROOT_DIR)

C_OBJ_CONF_ROOT_DIR ?= $(BUILD_CONF_OUTPUT_ROOT_DIR)
C_DEP_CONF_ROOT_DIR ?= $(BUILD_CONF_OUTPUT_ROOT_DIR)

CXX_OBJ_CONF_ROOT_DIR ?= $(BUILD_CONF_OUTPUT_ROOT_DIR)
CXX_DEP_CONF_ROOT_DIR ?= $(BUILD_CONF_OUTPUT_ROOT_DIR)

#===================================#
# PREFIXES                          #
#                                   #
# Note: The project can provide its #
#       values for that variables.  #
#===================================#
LIB_PREFIX ?= lib

#===================================#
# SUFFIXES                          #
#                                   #
# Note: The project can provide its #
#       values for that variables.  #
#===================================#
C_SUFFIX ?= .c
CXX_SUFFIX ?= .cpp
C_PP_SUFFIX ?= .pp
CXX_PP_SUFFIX ?= .ppx
H_SUFFIX ?= .h
HXX_SUFFIX ?= .h
TCC_SUFFIX ?= .tcc
DEP_SUFFIX ?= .d
OBJ_SUFFIX ?= .o
SHARED_LIB_SUFFIX ?= .so
STATIC_LIB_SUFFIX ?= .a
SYMBOLS_SUFFIX ?= .dbg

ifeq ($(MAKE_USE_IODEV_BUILD),1)
	#=====================================#
	# APG/IO SDKs development integration #
	#=====================================#

	# LOTC SDK
	IODEV_LOTC_SDK_ROOT_DIR = $(IODEV_ROOT_DIR)/LOTC_SDK
	IODEV_LOTC_SDK_LIB_DIR = $(IODEV_LOTC_SDK_ROOT_DIR)/usr/lib64
	IODEV_LOTC_SDK_INC_DIR = $(IODEV_LOTC_SDK_ROOT_DIR)/usr/include

	# CORE MW SDK
	IODEV_COREMW_SDK_ROOT_DIR = $(IODEV_ROOT_DIR)/MIDDLEWARE
	IODEV_COREMW_SDK_LIB_DIR = $(IODEV_COREMW_SDK_ROOT_DIR)/lib64
	IODEV_COREMW_SDK_INC_DIR = $(IODEV_COREMW_SDK_ROOT_DIR)/include
	IODEV_COREMW_LD_LIBS = SaImmOm SaImmOi SaAmf SaCkpt SaClm SaLog SaNtf

	# TRACE CC SDK
	IODEV_TRACE_SDK_ROOT_DIR = $(IODEV_ROOT_DIR)/TRACE_SDK
	IODEV_TRACE_SDK_LIB_DIR = $(IODEV_TRACE_SDK_ROOT_DIR)/lib64 
	IODEV_TRACE_SDK_INC_DIR = $(IODEV_TRACE_SDK_ROOT_DIR)/include 

	# DX Environment
	export DX_SYSROOT_X86_64=$(IODEV_LOTC_SDK_ROOT_DIR)
	export DX_CUSTOM_RPATH=${IODEV_COREMW_SDK_LIB_DIR}:$(IODEV_TRACE_SDK_LIB_DIR)
else
	# LDE SDK
	HCF_SDK_LDE_SDK_ROOT_DIR = $(HCF_SDK_ROOT_DIR)/lde_sdk
	HCF_SDK_LDE_SDK_LIB_DIR = $(HCF_SDK_LDE_SDK_ROOT_DIR)/usr/lib64
	HCF_SDK_LDE_SDK_INC_DIR = $(HCF_SDK_LDE_SDK_ROOT_DIR)/usr/include
	
	# CORE MW SDK
	HCF_SDK_COREMW_SDK_ROOT_DIR = $(HCF_SDK_ROOT_DIR)/cmw_sdk
	HCF_SDK_COREMW_SDK_LIB_DIR = $(HCF_SDK_COREMW_SDK_ROOT_DIR)/lib64
	HCF_SDK_COREMW_SDK_INC_DIR = $(HCF_SDK_COREMW_SDK_ROOT_DIR)/include
	
	# TRACE SDK
	HCF_SDK_TRACE_SDK_ROOT_DIR = $(HCF_SDK_ROOT_DIR)/trace_sdk
	HCF_SDK_TRACE_SDK_LIB_DIR = $(HCF_SDK_TRACE_SDK_ROOT_DIR)/usr/lib64
	HCF_SDK_TRACE_SDK_INC_DIR = $(HCF_SDK_TRACE_SDK_ROOT_DIR)/usr/include
	
	# DX Environment
	export DX_SYSROOT_X86_64=$(HCF_SDK_LDE_SDK_ROOT_DIR)
	export DX_CUSTOM_RPATH=${HCF_SDK_COREMW_SDK_LIB_DIR}:$(HCF_SDK_TRACE_SDK_LIB_DIR)
endif
