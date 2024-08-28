#include "hcf/imm/object_implementer_interface.h"
#include "object_implementer_interface_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_INTERFACE__

TSL_NS_HCF_IMM_BEGIN

__TSL_CLASS_NAME__::TSL_CTOR ()
		TSL_CTOR_INIT_LIST(_obj_name(), _impl_name(), _scope()) {
	_object_implementer_interface_impl = new (std::nothrow) object_implementer_interface_impl();
}

__TSL_CLASS_NAME__::TSL_CTOR (std::string p_impName)
		TSL_CTOR_INIT_LIST(_obj_name(), _impl_name(p_impName), _scope()) {
	_object_implementer_interface_impl = new (std::nothrow) object_implementer_interface_impl(p_impName);
}

__TSL_CLASS_NAME__::TSL_CTOR (std::string p_objName, std::string p_impName, IMM_API_ScopeTypes p_scope)
		TSL_CTOR_INIT_LIST(_obj_name(p_objName), _impl_name(p_impName), _scope(p_scope)) {
	_object_implementer_interface_impl = new (std::nothrow) object_implementer_interface_impl(p_objName, p_impName, p_scope);
}

__TSL_CLASS_NAME__::TSL_DTOR () {
	delete _object_implementer_interface_impl;
	_object_implementer_interface_impl = 0;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::adminOperationResult(IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result) {
	return _object_implementer_interface_impl->adminOperationResult(oiHandle, invocation, result);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::adminOperationResult(IMM_API_OiHandle oiHandle, IMM_API_InvocationType invocation, int result, std::vector<IMM_API_AdminOperationParamType> outParamVector) {
	return _object_implementer_interface_impl->adminOperationResult(oiHandle, invocation, result, outParamVector);
}

std::string __TSL_CLASS_NAME__::getObjName() {
	return _object_implementer_interface_impl->getObjName();
}

void __TSL_CLASS_NAME__::setObjName(std::string p_objName) {
	_object_implementer_interface_impl->setObjName(p_objName);
}

std::string __TSL_CLASS_NAME__::getImpName() {
	return _object_implementer_interface_impl->getImpName();
}

void __TSL_CLASS_NAME__::setImpName(std::string p_impName) {
	_object_implementer_interface_impl->setImpName(p_impName);
}

IMM_API_ScopeTypes __TSL_CLASS_NAME__::getScope() {
	return _object_implementer_interface_impl->getScope();
}

void __TSL_CLASS_NAME__::setScope(IMM_API_ScopeTypes p_scope) {
	_object_implementer_interface_impl->setScope(p_scope);
}

int __TSL_CLASS_NAME__::getSelObj() const {
	return _object_implementer_interface_impl->getSelObj();
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::dispatch(IMM_API_DispatchFlags p_flag) {
	return _object_implementer_interface_impl->dispatch(p_flag);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyRuntimeObj(const char * p_objName, IMM_API_ImmParameter * p_attributeValue) {
	return _object_implementer_interface_impl->modifyRuntimeObj(p_objName, p_attributeValue);
}

void __TSL_CLASS_NAME__::setExitCode(int p_exitCode, std::string p_exitCodeMessage) {
	_object_implementer_interface_impl->setExitCode(p_exitCode, p_exitCodeMessage);
}

std::string __TSL_CLASS_NAME__::getExitCodeString() {
	return _object_implementer_interface_impl->getExitCodeString();
}

void __TSL_CLASS_NAME__::setExitCode(int p_exitCode) {
	return _object_implementer_interface_impl->setExitCode(p_exitCode);
}

SaAisErrorT __TSL_CLASS_NAME__::getExitCode() {
	return _object_implementer_interface_impl->getExitCode();
}

int __TSL_CLASS_NAME__::getInternalLastError() {
	return _object_implementer_interface_impl->getInternalLastError();
}

const char * __TSL_CLASS_NAME__::getInternalLastErrorText() {
	return _object_implementer_interface_impl->getInternalLastErrorText();
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::getCcbAugmentationInitialize(IMM_API_OiHandle oiHandle, IMM_API_CcbId ccbId, IMM_API_CcbHandle * ccbHandle, IMM_API_AdminOwnerHandle * adminOwnerHandler) {
	return _object_implementer_interface_impl->getCcbAugmentationInitialize(oiHandle, ccbId, ccbHandle, adminOwnerHandler);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::createObjectAugmentCcb(IMM_API_CcbHandle ccbHandle, IMM_API_AdminOwnerHandle adminOwnerHandler, const char * p_className, const char * p_parentName, std::vector<IMM_API_ValuesDefinitionTypes> p_attrValuesList) {
	return _object_implementer_interface_impl->createObjectAugmentCcb(ccbHandle, adminOwnerHandler, p_className, p_parentName, p_attrValuesList);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyObjectAugmentCcb(IMM_API_CcbHandle ccbHandle, IMM_API_AdminOwnerHandle adminOwnerHandler, const char * p_objectName, IMM_API_ImmParameter * p_attributeValue) {
	return _object_implementer_interface_impl->modifyObjectAugmentCcb(ccbHandle, adminOwnerHandler, p_objectName, p_attributeValue);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObjectAugmentCcb(IMM_API_CcbHandle ccbHandle, IMM_API_AdminOwnerHandle adminOwnerHandler, const char * m_objectName) {
	return _object_implementer_interface_impl->deleteObjectAugmentCcb(ccbHandle, adminOwnerHandler, m_objectName);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::applyAugmentCcb(IMM_API_CcbHandle ccbHandle) {
	return _object_implementer_interface_impl->applyAugmentCcb(ccbHandle);
}

SaUint64T __TSL_CLASS_NAME__::getImmOiHandle() {
	return _object_implementer_interface_impl->getImmOiHandle();
}

void __TSL_CLASS_NAME__::setImmOiHandle(SaUint64T immOiHandle) {
	_object_implementer_interface_impl->setImmOiHandle(immOiHandle);
}

void __TSL_CLASS_NAME__::setSelObj(SaSelectionObjectT p_selObj) {
	_object_implementer_interface_impl->setSelObj(p_selObj);
}

TSL_NS_HCF_IMM_END
