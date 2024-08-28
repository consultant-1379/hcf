#include "hcf/imm/runtime_owner.h"
#include "runtime_owner_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_RUNTIME_OWNER__

TSL_NS_HCF_IMM_BEGIN

__TSL_CLASS_NAME__::TSL_CTOR() {
	_runtime_owner_impl = new (std::nothrow) runtime_owner_impl();
}

__TSL_CLASS_NAME__::TSL_DTOR() {
	delete _runtime_owner_impl;
	_runtime_owner_impl = 0;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::init(std::string impName) {
	return _runtime_owner_impl->init(impName, this);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::finalize() {
	return _runtime_owner_impl->finalize();
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::createRuntimeObj(const char * p_className, const char * p_parentName, std::vector<IMM_API_ValuesDefinitionTypes> m_attrValuesList) {
	return _runtime_owner_impl->createRuntimeObj(p_className, p_parentName, m_attrValuesList);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteRuntimeObj(const char * p_objName) {
	return _runtime_owner_impl->deleteRuntimeObj( p_objName );
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyRuntimeObj(const char * p_objName, IMM_API_ImmParameter * p_attributeValue) {
	return _runtime_owner_impl->modifyRuntimeObj(p_objName, p_attributeValue);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::dispatch(IMM_API_DispatchFlags p_flag){
	return _runtime_owner_impl->dispatch(p_flag);
}

int __TSL_CLASS_NAME__::getSelObj() const {
	return _runtime_owner_impl->getSelObj();
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::adminOperationResult(IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result) {
	return _runtime_owner_impl->adminOperationResult(oiHandle, invocation, result);
}

int __TSL_CLASS_NAME__::getInternalLastError() {
	return _runtime_owner_impl->getInternalLastError();
}

const char * __TSL_CLASS_NAME__::getInternalLastErrorText() {
	return _runtime_owner_impl->getInternalLastErrorText();
}

IMM_API_ReturnTypes  __TSL_CLASS_NAME__::adminOperationResult(IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result, std::vector<IMM_API_AdminOperationParamType> outParamVector) {
	return _runtime_owner_impl->adminOperationResult(oiHandle, invocation, result, outParamVector);
}

TSL_NS_HCF_IMM_END
