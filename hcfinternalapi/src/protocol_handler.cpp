#include <arpa/inet.h>
#include <string.h>

#include <new>

#include "hcf/trace/com_ericsson_common_hcf_internal_protocolhandler.h"
#include "hcf/trace/tracing_macros.h"

#include "internalapi_constants.h"
#include "protocol_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_INTERNALAPI_PROTOCOL_HANDLER__

TSL_NS_HCF_INTERNALAPI_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::make_primitive (uint32_t primitive_id, uint32_t msg_to_send_len, const char * msg_to_send) {
	HCF_CC_TRACE_FUNCTION;

	// Delete the buffer previously allocated (if any)
	if (_buffer_length > 0) {
		HCF_CC_TRACE(MakePrimitive, "Received a request to build a new primitive, but an old buffer already "
				"exists: erasing it before building primitive!", primitive_id, msg_to_send_len, ((msg_to_send) ? msg_to_send : ""));
		delete _buffer;
		_buffer = 0; _buffer_length = 0;
	}
	HCF_CC_TRACE(MakePrimitive, "Received a request to build a new primitive: allocating the needed memory!", primitive_id, msg_to_send_len, ((msg_to_send) ? msg_to_send : ""));

	// Allocate the buffer object to store the primitive
	size_t str_size = (msg_to_send) ? (msg_to_send_len + 1) : 0;
	_buffer_length = 2 * sizeof(uint32_t) + str_size;
	if (!(_buffer = new (std::nothrow) char [_buffer_length])) {
		_buffer_length = 0;
		HCF_CC_TRACE(MakePrimitiveErr, "Failed to allocate memory for the internal data structures.", ERR_MEMORY_ALLOCATION_FAILED);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_MEMORY_ALLOCATION_FAILED, "Failed to allocate memory for the internal buffer.");
	}
	HCF_CC_TRACE(MakePrimitive, "Received a request to build a new primitive: storing the "
			"info into the internal data structures!", primitive_id, msg_to_send_len, ((msg_to_send) ? msg_to_send : ""));

	// Create the primitive buffer with the provided arguments
	char * buf_ptr = _buffer;

	// FIRST: set the primitive ID
	*(reinterpret_cast<uint32_t *>(buf_ptr)) = htonl(static_cast<uint32_t>(primitive_id));
	buf_ptr += sizeof(uint32_t);

	// SECOND: set the size of the next string field
	*(reinterpret_cast<uint32_t *>(buf_ptr)) = htonl(static_cast<uint32_t>(str_size));
	buf_ptr += sizeof(uint32_t);

	// THIRD: set the string field (if any)
	if (msg_to_send)	::strcpy(buf_ptr, msg_to_send);

	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::unpack_primitive (uint32_t & primitive_id, char * primitive_msg, uint32_t primitive_msg_size) {
	HCF_CC_TRACE_FUNCTION;

	// Check if there is some primitive to be unpacked
	if (_buffer_length <= 0) {
		HCF_CC_TRACE(UnpackPrimitiveErr, "Cannot unpack primitive: there is no primitive to be unpacked!", ERR_UNPACK_PRIMITIVE_FAILED);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_UNPACK_PRIMITIVE_FAILED, "There is no primitive to be unpacked!");
	}
	HCF_CC_TRACE(UnpackPrimitive, "Received a request to unpack the received primitive: "
			"extracting the information from the internal data structures!", -1, primitive_msg_size, "");

	// If a valid primitive is present, extract the requested fields
	char * buf_ptr = _buffer;

	// FIRST: extract from the internal buffer the primitive ID
	primitive_id = ntohl(*(reinterpret_cast<uint32_t *> (buf_ptr)));
	buf_ptr += sizeof(uint32_t);

	// SECOND: extract from the internal buffer the primitive MSG
	uint32_t msg_size = ntohl(*(reinterpret_cast<uint32_t *> (buf_ptr)));
	buf_ptr += sizeof(uint32_t);

	HCF_CC_TRACE(UnpackPrimitive, "Received a request to unpack the received primitive: "
			"ID and primitive length extracted!", primitive_id, msg_size, "");

	// THIRD: check if the provided buffer is large enough to store the primitive MSG
	if (msg_size > primitive_msg_size) {	// msg_size considers also the string terminator character
		primitive_id = -1;
		HCF_CC_TRACE(UnpackPrimitiveErr, "The provided buffer has an insufficient length to store the primitive message!", ERR_NOT_ENOUGH_SPACE);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NOT_ENOUGH_SPACE, "The provided buffer has an insufficient "
				"length (%u) to store the primitive message (length == %u)", primitive_msg_size, msg_size);
	}

	// FOURTH: if the buffer is large enough, store the received string in it
	::strncpy(primitive_msg, buf_ptr, msg_size);
	HCF_CC_TRACE(UnpackPrimitive, "Received a request to unpack the received primitive: primitive "
			"message extracted!", primitive_id, msg_size, ((primitive_msg) ? primitive_msg : ""));

	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::send_primitive () {
	HCF_CC_TRACE_FUNCTION;

	// Check if there is some primitive to be send
	if (_buffer_length <= 0) {
		HCF_CC_TRACE(SendPrimitiveErr, "Cannot send primitive: there is no primitive ready to be sent!", ERR_NO_PRIMITIVE_TO_SEND);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NO_PRIMITIVE_TO_SEND, "There is no primitive to be sent!");
	}
	HCF_CC_TRACE(SendPrimitive, "Received a request to send a primitive: sending over socket the "
			"data stored into the internal structures!", _socket, _buffer_length, _buffer);

	// If there is some primitive to be sent, send it on the socket
	ssize_t bytes_sent = -1;
	if ((bytes_sent = ::send(_socket, _buffer, _buffer_length, MSG_NOSIGNAL)) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(SendPrimitiveErr, "Call 'send' failed.", errno_save);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_PRIMITIVE_SEND_FAILED,
				"Call 'send' failed, return_code == %zd, errno == %d", bytes_sent, errno_save);
	}
	else if (static_cast<size_t>(bytes_sent) != _buffer_length) {	// Sent a smaller number of bytes than the expected one
		HCF_CC_TRACE(SendPrimitiveErr, "Call 'send' failed: a smaller number of bytes (than the expected one) was sent!", bytes_sent);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_PRIMITIVE_SEND_FAILED,
				"Call 'send' failed, sent '%zd' bytes instead of '%zu'", bytes_sent, _buffer_length);
	}

	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::recv_primitive () {
	HCF_CC_TRACE_FUNCTION;

	// Delete the buffer previously allocated (if any)
	if (_buffer_length > 0) {
		HCF_CC_TRACE(ReceivePrimitive, "Received a request to receive a primitive, but an old buffer already "
				"exists: erasing it before receiving the primitive!", _socket, -1, "");
		delete _buffer;
		_buffer = 0; _buffer_length = 0;
	}

	const size_t header_size = 2 * sizeof(uint32_t);
	char header_buffer[header_size] = {0};
	ssize_t bytes_read = -1;

	// FIRST: read the primitive header (two 32 bits values) storing the primitive ID and the string size
	HCF_CC_TRACE(ReceivePrimitive, "Received a request to receive a primitive: reading from the socket the header part!", _socket, header_size, "");
	if ((bytes_read = ::recv(_socket, header_buffer, header_size, MSG_PEEK)) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(ReceivePrimitiveErr, "Call 'recv' failed for message header.", errno_save);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_PRIMITIVE_RECV_FAILED,
				"Call 'recv' failed, return_code == %zd, errno == %d", bytes_read, errno_save);
	}
	else if (static_cast<size_t>(bytes_read) != header_size) {	// Received a smaller number of bytes than the expected one
		HCF_CC_TRACE(ReceivePrimitiveErr, "Call 'recv' failed for message header: less than 32 bytes were received!", bytes_read);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_PRIMITIVE_RECV_FAILED,
				"Call 'recv' failed, received '%zd' bytes instead of '%zu'", bytes_read, header_size);
	}
	HCF_CC_TRACE(ReceivePrimitive, "Header part correctly read: extracting needed info and allocating "
			"memory for the primitive message part!", _socket, header_size, header_buffer);

	// SECOND: extract the primitive message size from the header
	char * buf_ptr = header_buffer + sizeof(uint32_t);
	uint32_t primitive_msg_size = ntohl(*(reinterpret_cast<uint32_t *> (buf_ptr)));

	// THIRD: allocate the internal buffer and read the entire primitive from the socket
	_buffer_length = header_size + primitive_msg_size;
	if (!(_buffer = new (std::nothrow) char [_buffer_length])) {
		_buffer_length = 0;
		HCF_CC_TRACE(ReceivePrimitiveErr, "Failed to allocate memory for the internal data structures.", ERR_MEMORY_ALLOCATION_FAILED);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_MEMORY_ALLOCATION_FAILED, "Failed to allocate memory for the internal buffer.");
	}

	bytes_read = -1;
	HCF_CC_TRACE(ReceivePrimitive, "Reading from the socket the entire primitive data!", _socket, _buffer_length, "");
	if ((bytes_read = ::recv(_socket, _buffer, _buffer_length, 0)) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(ReceivePrimitiveErr, "Call 'recv' failed.", errno_save);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_PRIMITIVE_RECV_FAILED,
				"Call 'recv' failed, return_code == %zd, errno == %d", bytes_read, errno_save);
	}
	else if (static_cast<size_t>(bytes_read) != _buffer_length) {	// Received a smaller number of bytes than the expected one
		HCF_CC_TRACE(ReceivePrimitiveErr, "Call 'recv' failed: a smaller number of bytes (than the expected one) was received!", bytes_read);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_PRIMITIVE_RECV_FAILED,
				"Call 'recv' failed, received '%zd' bytes instead of '%zu'", bytes_read, _buffer_length);
	}
	HCF_CC_TRACE(ReceivePrimitive, "Primitive correctly received!", _socket, _buffer_length, _buffer);

	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

TSL_NS_HCF_INTERNALAPI_END
