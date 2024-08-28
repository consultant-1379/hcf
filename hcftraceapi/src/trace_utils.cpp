#include "hcf/trace/trace_utils.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_TRACE_TRACE_UTILS__

TSL_NS_HCF_TRACE_BEGIN

const std::string __TSL_CLASS_NAME__::dump_buffer (char * buffer, size_t size, size_t dumping_size, size_t dumping_line_length) {
	uint8_t * b = reinterpret_cast<uint8_t *>(buffer);
	size_t output_buffer_size = 2 + 8 + 2 + 3*dumping_line_length + 16;
	char output_buffer[output_buffer_size];

	for (size_t i = 0; (i < size) && (i < dumping_size); ) {
		int chars = ::snprintf(output_buffer, output_buffer_size, "  %08zX:", i);
		for (size_t col = 0; (col < dumping_line_length) && (i < size) && (i < dumping_size); ++i, ++col)
			chars += ::snprintf(output_buffer + chars, output_buffer_size - chars, " %02X", b[i]);
	}
	return std::string(output_buffer);
}

TSL_NS_HCF_TRACE_END
