#include "hcf/core/error_info_provider.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_ERROR_INFO_PROVIDER__

TSL_NS_HCF_CORE_BEGIN

__thread int __TSL_CLASS_NAME__::_sys_error = 0;
__thread int __TSL_CLASS_NAME__::_error = 0;
__thread char __TSL_CLASS_NAME__::_error_text[ERROR_TEXT_SIZE_MAX] = {0};

TSL_NS_HCF_CORE_END
