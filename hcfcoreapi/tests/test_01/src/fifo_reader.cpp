#include <errno.h>
#include <stdio.h>

#include "fifo_reader.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_TEST_01_FIFO_READER__

int __TSL_CLASS_NAME__::handle_input () {
	/*TSL_TRACE_ON_SCREEN;*/
	unsigned char buffer [1024 * 1024] = {0};

	// Read a single char from the fifo
	if (const int call_result = ::read(_fifo_fd, buffer, TSL_ARRAY_SIZE(buffer))) {
		if (call_result < 0) { // ERROR
			::printf("[HANDLER %d]: ERROR: read: errno == %d\n", _fifo_fd, errno);
		} else {

			if ((_bytes_read += call_result) >= 1024) {
				for ( ; _bytes_read >= 1024; _bytes_read -= 1024) ++_kilo_bytes_read;
				::printf("[HANDLER %d]: Kilo bytes read == '%u'\n", _fifo_fd, _kilo_bytes_read);
			}

			//::printf("[HANDLER %d]: character read == '%02X'\n", _fifo_fd, c);
			//::sleep(1);
		}
	} else {
		::printf("[HANDLER %d]: read return 0. strange\n", _fifo_fd);
	}

	return 0;
}

int __TSL_CLASS_NAME__::handle_exception (bool error_flag, bool hungup_flag, bool read_hungup_flag) {
	/*TSL_TRACE_ON_SCREEN;*/
	::printf("[HANDLER %d]: error_flag == %d, hungup_flag == %d, read_hungup_flag == %d\n",
			_fifo_fd, error_flag, hungup_flag, read_hungup_flag);

	return -1;
}

int __TSL_CLASS_NAME__::handle_close () {
	/*TSL_TRACE_ON_SCREEN;*/
	::printf("[HANDLER %d]: handling the close: closing the fifo\n", _fifo_fd);

	if (close()) ::printf("[HANDLER %d]: ERROR: closing the fifo\n", _fifo_fd);
	else ::printf("[HANDLER %d]: fifo closed correctly\n", _fifo_fd);

	return 0;
}
