#ifndef __INCLUDE_GUARD_FILE_IMM_TYPES_H__
#define __INCLUDE_GUARD_FILE_IMM_TYPES_H__

/**
 *  @file imm_types.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-11-10
 *
 *	COPYRIGHT Ericsson AB, 2014
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+==========+============+==============+=====================================+
 *	| REV      | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+==========+============+==============+=====================================+
 *	| 2014-001 | 2014-11-10 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <string>
#include <vector>

#include <saAis.h>
#include <saImmOi.h>
#include <saImmOm.h>

#include "imm_macros.h"

TSL_NS_HCF_IMM_BEGIN

#define NO_REGISTERED_OI 0
#define REGISTERED_OI    1

#define IMM_LIBRARY_RELEASE_CODE 'A'
#define IMM_LIBRARY_MAJOR_VERSION 0x02
#define IMM_LIBRARY_MINOR_VERSION 0x14

typedef SaUint64T IMM_API_AttrFlagsT;
typedef SaImmOiHandleT IMM_API_OiHandle;
typedef SaInvocationT IMM_API_InvocationType;
typedef SaImmAdminOperationIdT IMM_API_AdminOperationIdType;
typedef SaImmOiCcbIdT IMM_API_CcbId;
typedef SaImmCcbHandleT IMM_API_CcbHandle;
typedef SaImmAdminOwnerHandleT IMM_API_AdminOwnerHandle;


typedef enum {
	OPERATION_SUCCESS = 0,
	OPERATION_FAILURE = 1
} IMM_API_ReturnTypes;

typedef enum {
	CONFIGURATION = 1,
	RUNTIME       = 2
} IMM_API_CategoryTypes;

typedef enum {
	ATTR_INT32T  = 1, /* Int32T  int */
	ATTR_UINT32T = 2, /* Uint32T unsigned int */
	ATTR_INT64T  = 3, /* Int64T  long long */
	ATTR_UINT64T = 4, /* Uint64T unsigned long long*/
	ATTR_TIMET   = 5, /* TimeT   long long*/
	ATTR_NAMET   = 6, /* NameT   array of char*/
	ATTR_FLOATT  = 7, /* FloatT  float*/
	ATTR_DOUBLET = 8, /* DoubleT double*/
	ATTR_STRINGT = 9, /* StringT array of char*/
	ATTR_ANYT    = 10 /* AnyT */
} IMM_API_AttrValueTypes;

typedef enum {
	SCOPE_ONE 		  = 1,
	SCOPE_SUBLEVEL  = 2,
	SCOPE_SUBTREE   = 3
} IMM_API_ScopeTypes;

typedef enum {
	DISPATCH_ONE      = 1,
	DISPATCH_ALL      = 2,
	DISPATCH_BLOCKING = 3
} IMM_API_DispatchFlags;

typedef enum {
	ATTR_VALUES_ADD     = 1,
	ATTR_VALUES_DELETE  = 2,
	ATTR_VALUES_REPLACE = 3
} IMM_API_AttrModificationType;


typedef struct {
	char * attrName;
	IMM_API_AttrValueTypes attrType;
	IMM_API_AttrFlagsT attrFlag;
	void * attrDefaultVal;
} IMM_API_AttrDefinitionTypes;

typedef struct {
	char * attrName;
	IMM_API_AttrValueTypes attrType;
	unsigned int attrValuesNum;
	void ** attrValues;
} IMM_API_ValuesDefinitionTypes;

struct IMM_API_ImmParameter {
	char * attrName;
	IMM_API_AttrValueTypes attrType;
	unsigned int attrValuesNum;
	void ** attrValues;
	int flag;

	IMM_API_ImmParameter() : attrName(0), attrType(), attrValuesNum(0), attrValues(0), flag(0) {}
	void IMM_API_IMMFreeMemory (int f) {
		flag = f;
	}
	~IMM_API_ImmParameter() {
		if (flag != 0)
			delete [] attrValues;
	}
};

class Types{
public:
	Types() {}
	virtual ~Types() {}
};

class StringType : public Types {
public:
	StringType() {}
	~StringType() {}

	std::string value;
};

class IntType : public Types {
public:
	IntType() : value(0) {}
	~IntType() {}

	int value;
};

class UnsIntType : public Types {
public:
	UnsIntType() : value(0) {}
	~UnsIntType() {}

	unsigned int value;
};

class LongLongIntType : public Types {
public:
	LongLongIntType() : value(0) {}
	~LongLongIntType() {}

	long long int value;
};

class UnsLongLongIntType : public Types {
public:
	UnsLongLongIntType() : value(0) {}
	~UnsLongLongIntType() {}

	unsigned long long int value;
};

class DoubleType : public Types {
public:
	DoubleType() : value(0) {}
	~DoubleType() {}

	double value;
};

class FloatType : public Types {
public:
	FloatType() : value(0) {}
	~FloatType() {}

	float value;
};

class IMM_API_ImmAttribute {
public:
	std::string attrName;
	int attrType;
	unsigned int attrValuesNum;
	void * attrValues[2048];
	int flag;
	std::vector<Types *> pointers;

	IMM_API_ImmAttribute() : attrName(), attrType(), attrValuesNum(0), attrValues(), flag(0), pointers() {}
	~IMM_API_ImmAttribute() {
		Types * c = 0;
		for (unsigned  int i = 0; i < pointers.size(); i++) {
			c = pointers[i];
			freeMemory(c);
		}
	}

private:
	void freeMemory(Types *p_pointer) {
		IntType *i = dynamic_cast<IntType *>(p_pointer);
		if(i){
			delete i;
			return;
		}

		StringType *p = dynamic_cast<StringType *>(p_pointer);
		if(p){
			delete p;
			return ;
		}

		UnsIntType *u = dynamic_cast<UnsIntType *>(p_pointer);
		if(u){
			delete u;
			return;
		}

		LongLongIntType *l = dynamic_cast<LongLongIntType *>(p_pointer);
		if(l){
			delete l;
			return;
		}

		UnsLongLongIntType *ul = dynamic_cast<UnsLongLongIntType *>(p_pointer);
		if(ul){
			delete ul;
			return;
		}

		DoubleType *d = dynamic_cast<DoubleType *>(p_pointer);
		if(d){
			delete d;
			return;
		}

		FloatType *f = dynamic_cast<FloatType *>(p_pointer);
		if(f){
			delete f;
			return;
		}
	}
};

class IMM_API_ImmObject {
public:
	std::string objName;
	std::vector<IMM_API_ImmAttribute> attributes;
	std::vector<Types *> pointers;

	IMM_API_ImmObject() {}
	~IMM_API_ImmObject(){
		Types * c = 0;

		for(unsigned  int i = 0; i < pointers.size(); i++) {
			c = pointers[i];
			freeMemory(c);
		}
	}

private:
	IMM_API_ImmObject(IMM_API_ImmObject &p_object);
	IMM_API_ImmObject& operator=(const IMM_API_ImmObject &p_obj);

	void freeMemory(Types *p_pointer) {
		IntType *i = dynamic_cast<IntType *>(p_pointer);
		if(i){
			delete i;
			return;
		}

		StringType *p = dynamic_cast<StringType *>(p_pointer);
		if(p){
			delete p;
			return ;
		}

		UnsIntType *u = dynamic_cast<UnsIntType *>(p_pointer);
		if(u){
			delete u;
			return;
		}

		LongLongIntType *l = dynamic_cast<LongLongIntType *>(p_pointer);
		if(l){
			delete l;
			return;
		}

		UnsLongLongIntType *ul = dynamic_cast<UnsLongLongIntType *>(p_pointer);
		if(ul){
			delete ul;
			return;
		}

		DoubleType *d = dynamic_cast<DoubleType *>(p_pointer);
		if(d){
			delete d;
			return;
		}

		FloatType *f = dynamic_cast<FloatType *>(p_pointer);
		if(f){
			delete f;
			return;
		}
	}
};

typedef struct {
	char * attrName;
	IMM_API_AttrValueTypes attrType;
	void * attrValues;
} IMM_API_AdminOperationParamType;

typedef struct {
	char * attrName;
	IMM_API_AttrValueTypes attrType;
	unsigned int attrValuesNum;
	void ** attrValues;
} IMM_API_AttrValues;

typedef struct {
	IMM_API_AttrModificationType modType;
	IMM_API_AttrValues modAttr;
} IMM_API_AttrModification;

class object_implementer_interface;
typedef struct {
	IMM_API_OiHandle oi_handle;
	IMM_API_CcbId ccb_id;
	IMM_API_CcbHandle ccb_handle;
	IMM_API_AdminOwnerHandle admin_owner_handler;
	object_implementer_interface * object_implementer;
	bool initialized;
} IMM_API_ObjectImplementer_CallbackInfo;

TSL_NS_HCF_IMM_END

#endif // __INCLUDE_GUARD_FILE_IMM_TYPES_H__
