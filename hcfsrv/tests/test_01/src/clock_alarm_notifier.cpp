#include <limits.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/timerfd.h>

#include <new>

#include "hcf/core/guard.h"

#include "clock_alarm_notifier.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLOCK_ALARM_NOTIFIER__

HCF_NS_HCF_SRV_BEGIN

int __TSL_CLASS_NAME__::open (unsigned flags) {
	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_timer_fd >= 0) { // ERROR: Already opened
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_ALREADY_OPEN);
		}

		int tfd = ::timerfd_create((flags & TSL_CLOCK_POLICY_MONOTONIC ? CLOCK_MONOTONIC : CLOCK_REALTIME), TFD_NONBLOCK | TFD_CLOEXEC);

		if (tfd < 0) // ERROR: On creating the timer
			TSL_SET_ERROR_INFO_AND_RETURN(-1, errno, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot create the internal timer object");

		_timeplan_handler_map.clear();
		_atp_alarmed.resize(0);
		_timer_fd = tfd;
	TSL_CRITICAL_SECTION_END;

	return TSL_NS_HCF_CORE::ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::close () {
	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_handle_input_ongoing) {
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Alarm clock timeout handling ongoing");
		}

		if (_timer_fd >= 0) {
			if (::close(_timer_fd)) { // ERROR: Closing the internal timer object
				TSL_SET_ERROR_INFO_AND_RETURN(-1, errno, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot release the internal timer object");
			}

			_atp_alarmed.clear();

			_timeplan_handler_map_t::iterator it = _timeplan_handler_map.begin();
			while (it != _timeplan_handler_map.end()) delete (it++)->second._atp_ptr;
			_timeplan_handler_map.clear();

			_timer_fd = -1;
		}
	TSL_CRITICAL_SECTION_END;

	return TSL_NS_HCF_CORE::ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::remove_timeplan (const std::string & id) {
	::timeval tv_now;
	::gettimeofday(&tv_now, 0);

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_timer_fd < 0) { // ERROR: Not open
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Clock notifier instance not open yet");
		}

		if (_handle_input_ongoing) {
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Alarm clock timeout handling ongoing");
		}

		_timeplan_handler_map_t::iterator item_ptr = _timeplan_handler_map.find(id);

		if (item_ptr == _timeplan_handler_map.end()) { // ERROR: Item not found
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_NOT_FOUND);
		}

		// Delete the internal atp object
		delete item_ptr->second._atp_ptr;

		// Remove from the internal table
		_timeplan_handler_map.erase(item_ptr);

		if (size_t atp_alarmed_size = _atp_alarmed.size()) {
			size_t i = 0;

			while (i < atp_alarmed_size) {
				if (_atp_alarmed[i] == id) {
					if (--atp_alarmed_size > i) _atp_alarmed[i] = _atp_alarmed[atp_alarmed_size];
				} else i++;
			}

			_atp_alarmed.resize(atp_alarmed_size);

			if (!atp_alarmed_size) {
				// Disarm the timer
				::itimerspec its;
				its.it_value.tv_nsec = its.it_interval.tv_nsec = its.it_value.tv_sec = its.it_interval.tv_sec = 0;
				::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its, 0);

				// Set next alarm handlers
				activate_timer_for_next_alarms(tv_now);
			}
		}
	TSL_CRITICAL_SECTION_END;

	return TSL_NS_HCF_CORE::ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::handle_input () {
	::timeval tv_now;
	::gettimeofday(&tv_now, 0);

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_timer_fd < 0) return 0;

		_handle_input_ongoing = true;
	TSL_CRITICAL_SECTION_END;

	uint64_t expiration_count = 0;
	unsigned bytes_read = 0;
	ssize_t read_result = 0;
	unsigned char * buffer_ptr = reinterpret_cast<unsigned char *>(&expiration_count);

	while (bytes_read < sizeof(expiration_count)) {
		read_result = ::read(_timer_fd, buffer_ptr + bytes_read, sizeof(expiration_count) - bytes_read);
		if (read_result >= 0) bytes_read += read_result;
		else { // ERROR: read
			// TODO: Handle read error codes
		}
	}

	// Disarm the timer
	::itimerspec its;
	its.it_value.tv_nsec = its.it_interval.tv_nsec = its.it_value.tv_sec = its.it_interval.tv_sec = 0;
	::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its, 0);

	TSL_NS_HCF_CORE::clock_alarm_handler * cah_ptrs [256];
	unsigned cah_count = 0;

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		std::vector<std::string> atp_alarmed(_atp_alarmed);

		// Set next alarm handlers
		activate_timer_for_next_alarms(tv_now);

		// Save the handler pointers
		for (std::vector<std::string>::iterator it = atp_alarmed.begin(); it != atp_alarmed.end(); it++) {
			_timeplan_handler_map_t::iterator item_ptr = _timeplan_handler_map.find(*it);

	//		(item_ptr != _timeplan_handler_map.end()) && (item_ptr->second._cah_ptr->handle_alarm());
			(item_ptr != _timeplan_handler_map.end()) && (cah_ptrs[cah_count++] = item_ptr->second._cah_ptr);
		}
	TSL_CRITICAL_SECTION_END;

	// Now, I can call the clock alarm handlers
	for (unsigned i = 0; i < cah_count; i++) cah_ptrs[i] && (cah_ptrs[i]->handle_alarm(false));

	_handle_input_ongoing = false;

	return 0;
}

int __TSL_CLASS_NAME__::add_timeplan_ (
		const std::string & id,
		TSL_NS_HCF_CORE::alarm_timeplan * atp,
		TSL_NS_HCF_CORE::clock_alarm_handler * handler) {
	::timeval tv_now;
	::timeval next_timer_point_relative;
	::itimerspec timer_current;
	::itimerspec its_relative;
//	TSL_NS_HCF_CORE::oneshot_alarm_timeplan * atp_new = 0;

	if (!handler) TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_NULL_ARGUMENT);

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_timer_fd < 0) { // ERROR: Not open
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Clock notifier instance not open yet");
		}

		if (_handle_input_ongoing) {
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Alarm clock timeout handling ongoing");
		}

		if (_timeplan_handler_map.count(id) > 0) { // ERROR: Item already into the map
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_FOUND);
		}

		if ((::gettimeofday(&tv_now, 0))) {
			TSL_SET_ERROR_INFO_AND_RETURN(-1, errno, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot retrieve the current system calendar time");
		}

//		if (!(atp_new = new (std::nothrow) TSL_NS_HCF_CORE::oneshot_alarm_timeplan(*atp))) { // ERROR: No memory available
//			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_NO_RESOURCES_AVAILABLE, "No memory available");
//		}

//		if (atp->get_next_timer_point(next_timer_point_relative, tv_now)) { // ERROR: next time not found
//			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_TIME_IS_OVER, "Invalid timeplan instance provided: time is over");
//		}

		if (TSL_NS_HCF_CORE::ERR_NO_ERROR == atp->get_next_timer_point(next_timer_point_relative, tv_now)) {
			if ((::timerfd_gettime(_timer_fd, &timer_current))) { // ERROR: Getting the current timer setting
				TSL_SET_ERROR_INFO_AND_RETURN(-1, errno, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot retrieve the current internal timer object settings");
			}

			if (timer_current.it_value.tv_nsec || timer_current.it_value.tv_sec) {
				// Clock timer currently armed

				::timeval timeval_current;
				::timeval timeval_750ms;
				::timeval timeval_current_left;
				::timeval timeval_current_right;

				timeval_current.tv_sec = timer_current.it_value.tv_sec;
				timeval_current.tv_usec = timer_current.it_value.tv_nsec / 1000;

				timeval_750ms.tv_sec = 0; timeval_750ms.tv_usec = 750000;

				timersub(&timeval_current, &timeval_750ms, &timeval_current_left);
				timeradd(&timeval_current, &timeval_750ms, &timeval_current_right);

				if (timercmp(&next_timer_point_relative, &timeval_current_left, <)) {
					::timeval a; a.tv_sec = its_relative.it_value.tv_sec; a.tv_usec = its_relative.it_value.tv_nsec / 1000;
					::timeval res;

					timeradd(&a, &tv_now, &res);

					// The new alarm timeplan will fire before the current one
					// Clear the atp alarmed and add only this new one
					its_relative.it_value.tv_sec = res.tv_sec;
					its_relative.it_value.tv_nsec = res.tv_usec * 1000;
					//its_relative.it_value.tv_sec = next_timer_point_relative.tv_sec + tv_now.tv_sec;
					//its_relative.it_value.tv_nsec = next_timer_point_relative.tv_usec * 1000;
					its_relative.it_interval.tv_nsec = its_relative.it_interval.tv_sec = 0;

					if (::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its_relative, 0)) { // ERROR: Setting the internal timer alarm clock
						int errno_save = errno;
						//delete atp_new;
						TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot set/arm the internal timer alarm clock");
					}

					_atp_alarmed.resize(0);
					_atp_alarmed.push_back(id);
				} else if (timercmp(&next_timer_point_relative, &timeval_current_right, <)) {
					// the new alarm timeplan will overlap the current one.
					// Adding this new alarm to the list of next firing alarms
					_atp_alarmed.push_back(id);
				} else {
					// Nothing to change: the new alarm timeplan will fire after the current one
				}
			} else {
				// Clock timer currently disarmed
				::timeval a; a.tv_sec = its_relative.it_value.tv_sec; a.tv_usec = its_relative.it_value.tv_nsec / 1000;
				::timeval res;

				timeradd(&a, &tv_now, &res);

				its_relative.it_value.tv_sec = res.tv_sec;
				its_relative.it_value.tv_nsec = res.tv_usec * 1000;
				//its_relative.it_value.tv_sec = next_timer_point_relative.tv_sec;
				//its_relative.it_value.tv_nsec = next_timer_point_relative.tv_usec * 1000;
				its_relative.it_interval.tv_nsec = its_relative.it_interval.tv_sec = 0;

				if (::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its_relative, 0)) { // ERROR: Setting the internal timer alarm clock
					int errno_save = errno;
					//delete atp_new;
					TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot set/arm the internal timer alarm clock");
				}

				_atp_alarmed.resize(0);
				_atp_alarmed.push_back(id);
			}
		}

		// Insert the (timeplan, handler) pair into the internal map
		_timeplan_handler_map[id] = atp_handler_pair(atp, handler);
	TSL_CRITICAL_SECTION_END;

	return TSL_NS_HCF_CORE::ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::activate_timer_for_next_alarms (const ::timeval & tv_from) {
	::timeval next_timer_point_relative;
	::timeval next_timer_point_relative_min;
	::timeval timeval_min_left;
	::timeval timeval_min_right;
	::timeval timeval_750ms;

	timeval_750ms.tv_sec = 0; timeval_750ms.tv_usec = 750000;

	next_timer_point_relative_min.tv_sec = LONG_MAX;
	next_timer_point_relative_min.tv_usec = 0;
	timersub(&next_timer_point_relative_min, &timeval_750ms, &timeval_min_left);
	timeradd(&next_timer_point_relative_min, &timeval_750ms, &timeval_min_right);

	std::string alarm_to_activate [128];
	unsigned alarm_to_activate_counter = 0;

	for (_timeplan_handler_map_t::iterator it = _timeplan_handler_map.begin(); it != _timeplan_handler_map.end(); it++) {
		if (it->second._atp_ptr->get_next_timer_point(next_timer_point_relative, tv_from) == 0) {
			if (timercmp(&next_timer_point_relative, &timeval_min_left, <)) {
				// A new minimal timer found
				alarm_to_activate[alarm_to_activate_counter = 0] = it->first;
				alarm_to_activate_counter++;

				next_timer_point_relative_min = next_timer_point_relative;
				timersub(&next_timer_point_relative_min, &timeval_750ms, &timeval_min_left);
				timeradd(&next_timer_point_relative_min, &timeval_750ms, &timeval_min_right);
			} else if (timercmp(&next_timer_point_relative, &timeval_min_right, <)) {
				// A simultaneously timer found. Add it to the same collection
				alarm_to_activate[alarm_to_activate_counter++] = it->first;
			}
		}
	}

	_atp_alarmed.resize(alarm_to_activate_counter);

	if (alarm_to_activate_counter) _atp_alarmed.assign(alarm_to_activate, alarm_to_activate + alarm_to_activate_counter);

	// Arm the clock timer
	::itimerspec its_relative;
	its_relative.it_value.tv_sec = timeval_min_right.tv_sec;
	its_relative.it_value.tv_nsec = timeval_min_right.tv_usec * 1000;
	its_relative.it_interval.tv_nsec = its_relative.it_interval.tv_sec = 0;






	::timeval a; a.tv_sec = its_relative.it_value.tv_sec; a.tv_usec = its_relative.it_value.tv_nsec / 1000;
	::timeval res;

	timeradd(&a, &tv_from, &res);

	its_relative.it_value.tv_sec = res.tv_sec;
	its_relative.it_value.tv_nsec = res.tv_usec * 1000;








	if (::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its_relative, 0)) { // ERROR: Setting the internal timer alarm clock
		int errno_save = errno;
		TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot set/arm the internal timer alarm clock");
	}

	return TSL_NS_HCF_CORE::ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
