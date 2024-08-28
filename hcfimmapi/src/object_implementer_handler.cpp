#include "object_implementer_handler_impl.h"
#include "hcf/imm/object_implementer_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_IMM_OBJECT_IMPLEMENTER_HANDLER__

TSL_NS_HCF_IMM_BEGIN

__TSL_CLASS_NAME__::TSL_CTOR () {
	_object_implementer_handler_impl = new object_implementer_handler_impl();
}

__TSL_CLASS_NAME__::TSL_DTOR() {
	delete _object_implementer_handler_impl; _object_implementer_handler_impl = 0;
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::addObjectImpl(object_implementer_interface * obj) {
	return _object_implementer_handler_impl->addObjectImpl(obj);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::removeObjectImpl(object_implementer_interface * obj) {
	return _object_implementer_handler_impl->removeObjectImpl(obj);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::addClassImpl(object_implementer_interface * obj, const char * className) {
	return _object_implementer_handler_impl->addClassImpl(obj, className);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::addMultipleClassImpl(object_implementer_interface * p_obj, std::vector<std::string> p_classNameVec) {
	return _object_implementer_handler_impl->addMultipleClassImpl(p_obj, p_classNameVec);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::removeClassImpl(object_implementer_interface * obj, const char * className) {
	return _object_implementer_handler_impl->removeClassImpl(obj, className);
}

IMM_API_ReturnTypes __TSL_CLASS_NAME__::removeMultipleClassImpl(object_implementer_interface * p_obj, std::vector<std::string> p_classNameVec) {
	return _object_implementer_handler_impl->removeMultipleClassImpl(p_obj, p_classNameVec);
}

int __TSL_CLASS_NAME__::getInternalLastError () {
	return _object_implementer_handler_impl->getInternalLastError();
}

const char * __TSL_CLASS_NAME__::getInternalLastErrorText () {
	return _object_implementer_handler_impl->getInternalLastErrorText();
}

TSL_NS_HCF_IMM_END
