#include "hcf/imm/om_handler.h"
#include "om_handler_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_OM_HANDLER__

TSL_NS_HCF_IMM_BEGIN

__TSL_CLASS_NAME__::TSL_CTOR() {
	_om_handler_implementation = new (std::nothrow) om_handler_impl();
}

__TSL_CLASS_NAME__::TSL_DTOR() {
	delete _om_handler_implementation;
	_om_handler_implementation = 0;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::Init(int p_registeredImpl) {
	return _om_handler_implementation->Init(p_registeredImpl);;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::Finalize() {
	return _om_handler_implementation->Finalize();
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::defineClass(char * m_className, IMM_API_CategoryTypes m_classCategory, std::vector<IMM_API_AttrDefinitionTypes> m_attrList) {
	return _om_handler_implementation->defineClass(m_className, m_classCategory, m_attrList);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::removeClass(char * m_className) {
	return _om_handler_implementation->removeClass(m_className);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::createObject(const char * m_className, const char * m_parentName, std::vector<IMM_API_ValuesDefinitionTypes> m_attrValuesList) {
	return _om_handler_implementation->createObject(m_className, m_parentName, m_attrValuesList);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::createObject(const char * p_className, const char * p_parentName, std::vector<IMM_API_ValuesDefinitionTypes> p_attrValuesList, std::string p_transactionName) {
	return _om_handler_implementation->createObject(p_className, p_parentName, p_attrValuesList, p_transactionName);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObject(const char * m_objectName) {
	return _om_handler_implementation->deleteObject(m_objectName);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObject(const char * m_objectName, IMM_API_ScopeTypes p_scope) {
	return _om_handler_implementation->deleteObject(m_objectName, p_scope);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObject(const char * p_objectName, std::string p_transactionName) {
	return _om_handler_implementation->deleteObject(p_objectName, p_transactionName);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::deleteObject(const char * p_objectName, std::string p_transactionName, IMM_API_ScopeTypes p_scope) {
	return _om_handler_implementation->deleteObject(p_objectName, p_transactionName, p_scope);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::getAttribute(const char * m_objectName, IMM_API_ImmParameter * m_attributeValue) {
	return _om_handler_implementation->getAttribute(m_objectName, m_attributeValue);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::getAttribute(const char * p_objectName, std::vector<IMM_API_ImmAttribute *> p_attributeList) {
	return _om_handler_implementation->getAttribute(p_objectName, p_attributeList);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::getObject(IMM_API_ImmObject * object) {
	return _om_handler_implementation->getObject(object);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::getChildren(const char * p_rootName, IMM_API_ScopeTypes p_scope, std::vector<std::string> * p_rdnList) {
	return _om_handler_implementation->getChildren(p_rootName, p_scope, p_rdnList);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::getClassInstances(const char * p_className, std::vector<std::string> & p_dnList) {
	return _om_handler_implementation->getClassInstances(p_className, p_dnList);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyAttribute(const char * m_objectName, IMM_API_ImmParameter * m_attributeValue) {
	return _om_handler_implementation->modifyAttribute(m_objectName, m_attributeValue);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::modifyAttribute(const char * p_objectName, IMM_API_ImmParameter * p_attributeValue, std::string p_transactionName) {
	return _om_handler_implementation->modifyAttribute(p_objectName, p_attributeValue, p_transactionName);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::applyRequest(std::string p_transactionName) {
	return _om_handler_implementation->applyRequest(p_transactionName);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::resetRequest(std::string p_transactionName) {
	return _om_handler_implementation->resetRequest(p_transactionName);
}

int __TSL_CLASS_NAME__::getInternalLastError() {
	return _om_handler_implementation->getInternalLastError();
}

const char * __TSL_CLASS_NAME__::getInternalLastErrorText() {
	return _om_handler_implementation->getInternalLastErrorText();
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::getExitCode(int & p_errorId, std::string & p_errorText) {
	return _om_handler_implementation->getExitCode(p_errorId, p_errorText);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::getExitCode(std::string p_transactionName, int & p_errorId, std::string & p_errorText) {
	return _om_handler_implementation->getExitCode(p_transactionName, p_errorId, p_errorText);
}

unsigned long long int __TSL_CLASS_NAME__::getCcbHandleValue() {
	return _om_handler_implementation->getCcbHandleValue();
}

TSL_NS_HCF_IMM_END
