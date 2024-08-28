#include <limits.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/timerfd.h>

#include <new>

#include "hcf/core/guard.h"
#include "hcf/core/logger.h"
#include "hcf/core/utils.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_clockalarmnotifier.h"
#include "hcf/trace/tracing_macros.h"

#include "clock_alarm_notifier.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLOCK_ALARM_NOTIFIER__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::open (unsigned flags) {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_timer_fd >= 0) { // ERROR: Already opened
			HCF_CC_TRACE(OpenErr, "The clock alarm notifier object has been already opened!", TSL_NS_HCF_CORE::ERR_ALREADY_OPEN);
			HCF_CC_LOG(LEVEL_ERROR, "The clock alarm notifier object has been already opened!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_ALREADY_OPEN);
		}
		HCF_CC_TRACE(Open, "Initializing the clock alarm notifier object internal structures.", flags);

		int tfd = ::timerfd_create((flags & TSL_CLOCK_POLICY_MONOTONIC ? CLOCK_MONOTONIC : CLOCK_REALTIME), TFD_NONBLOCK | TFD_CLOEXEC);
		if (tfd < 0) { // ERROR: On creating the timer
			const int errno_save = errno;
			HCF_CC_TRACE(OpenErr, "Call 'timerfd_create' failed.", errno_save);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to create the internal timer object, errno == %d, flags == %u", errno_save, flags);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, errno, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot create the internal timer object");
		}
		HCF_CC_TRACE(Open, "Clock alarm notifier object correctly created, storing info into internal structures.", flags);

		_timeplan_handler_map.clear();
		_atp_alarmed.resize(0);
		_timer_fd = tfd;
	TSL_CRITICAL_SECTION_END;

	return TSL_NS_HCF_CORE::ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::close () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_handle_input_ongoing) {
			HCF_CC_TRACE(CloseErr, "Cannot close the clock alarm notifier object since there is an handle input ongoing!", TSL_NS_HCF_CORE::ERR_INVALID_OPERATION);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to close the clock alarm notifier object, an handle input operation is ongoing!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Alarm clock timeout handling ongoing");
		}

		if (_timer_fd >= 0) {
			HCF_CC_TRACE(Close, "Closing the internal file descriptor object!");

			if (::close(_timer_fd)) { // ERROR: Closing the internal timer object
				const int errno_save = errno;
				HCF_CC_TRACE(CloseErr, "Call 'close' failed.", errno_save);
				HCF_CC_LOG(LEVEL_ERROR, "Failed to release the internal timer object, errno == %d", errno_save);
				TSL_SET_ERROR_INFO_AND_RETURN(-1, errno, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot release the internal timer object");
			}
			HCF_CC_TRACE(Close, "Erasing the data stored into the internal data structures!");

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
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(RemoveTimeplan, "Received a request to remove a timeplan.", id.c_str());
	::timeval tv_now;
	::gettimeofday(&tv_now, 0);

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_timer_fd < 0) { // ERROR: Not open
			HCF_CC_TRACE(RemoveTimeplanErr, "Cannot remove timeplan: the clock alarm notifier object has not been opened!", TSL_NS_HCF_CORE::ERR_INVALID_OPERATION);
			HCF_CC_LOG(LEVEL_ERROR, "The clock alarm notifier object has not been opened!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Clock notifier instance not open yet");
		}

		if (_handle_input_ongoing) {
			HCF_CC_TRACE(RemoveTimeplanErr, "Cannot remove timeplan: there is a handle input operation ongoing!", TSL_NS_HCF_CORE::ERR_INVALID_OPERATION);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to remove the timeplan having id == '%s', an handle input operation is ongoing!", id.c_str());
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Alarm clock timeout handling ongoing");
		}

		_timeplan_handler_map_t::iterator item_ptr = _timeplan_handler_map.find(id);

		if (item_ptr == _timeplan_handler_map.end()) { // ERROR: Item not found
			HCF_CC_TRACE(RemoveTimeplanErr, "The requested timeplan was not found!", TSL_NS_HCF_CORE::ERR_NOT_FOUND);
			HCF_CC_LOG(LEVEL_ERROR, "The timeplan having id == '%s' was not found!", id.c_str());
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_NOT_FOUND);
		}
		HCF_CC_TRACE(RemoveTimeplan, "Deleting the internal data structures associated to the timeplan.", id.c_str());

		// Delete the internal atp object
		delete item_ptr->second._atp_ptr;

		// Remove from the internal table
		_timeplan_handler_map.erase(item_ptr);

		if (size_t atp_alarmed_size = _atp_alarmed.size()) {
			size_t i = 0;
			HCF_CC_TRACE(RemoveTimeplan, "There are some alarmed items: check if the deleted timeplan is also in that list and remove it.", id.c_str());

			while (i < atp_alarmed_size) {
				if (_atp_alarmed[i] == id) {
					if (--atp_alarmed_size > i) _atp_alarmed[i] = _atp_alarmed[atp_alarmed_size];
				} else i++;
			}

			_atp_alarmed.resize(atp_alarmed_size);

			if (!atp_alarmed_size) {
				HCF_CC_TRACE(RemoveTimeplan, "No more events will be triggered: disarm the timer and found new events.", id.c_str());
				// Disarm the timer
				::itimerspec its;
				its.it_value.tv_nsec = its.it_interval.tv_nsec = its.it_value.tv_sec = its.it_interval.tv_sec = 0;
				::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its, 0);

				timerclear(&_armed_timeval);

				// Set next alarm handlers
				activate_timer_for_next_alarms(tv_now);
			}
		}
	TSL_CRITICAL_SECTION_END;

	return TSL_NS_HCF_CORE::ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::handle_input () {
	HCF_CC_TRACE_FUNCTION;
	::timeval tv_now;
	::gettimeofday(&tv_now, 0);

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_timer_fd < 0) {
			HCF_CC_TRACE(HandleInputErr, "Cannot handle input event: very strange, this shouldn't occur!", 0);
			return 0;
		}

		HCF_CC_TRACE(HandleInput, "Received an input event!", TSL_NS_HCF_CORE::utils::calculate_time_string(&_armed_timeval).c_str());
		_handle_input_ongoing = true;
	TSL_CRITICAL_SECTION_END;

	::timeval timeval_pi;
	timeval_pi.tv_sec = 0; timeval_pi.tv_usec = 314159;

	::timeval timeval_armed_right;
	timeradd(&_armed_timeval, &timeval_pi, &timeval_armed_right);

	bool event_in_the_past = false;
	if (timercmp(&tv_now, &timeval_armed_right, >)) {
		::tm armed_tm;
		::localtime_r(&(_armed_timeval.tv_sec), &armed_tm);

		HCF_CC_TRACE(HandleInput, "The event has been triggered, but the time is over!", TSL_NS_HCF_CORE::utils::calculate_time_string(&_armed_timeval).c_str());
		HCF_CC_LOG(LEVEL_WARNING, "An event has been triggered, but is was scheduled for '%04u/%02u/%02u %02u:%02u:%02u': "
				"it is in the past (probably a date change operation has been performed)!", armed_tm.tm_year + 1900,
				armed_tm.tm_mon + 1, armed_tm.tm_mday, armed_tm.tm_hour, armed_tm.tm_min, armed_tm.tm_sec);
		event_in_the_past = true;
	}
	HCF_CC_TRACE(HandleInput, "An event has been triggered, consuming data on internal structures!", TSL_NS_HCF_CORE::utils::calculate_time_string(&_armed_timeval).c_str());

	uint64_t expiration_count = 0;
	unsigned bytes_read = 0;
	ssize_t read_result = 0;
	unsigned char * buffer_ptr = reinterpret_cast<unsigned char *>(&expiration_count);

	while (bytes_read < sizeof(expiration_count)) {
		read_result = ::read(_timer_fd, buffer_ptr + bytes_read, sizeof(expiration_count) - bytes_read);
		if (read_result >= 0) bytes_read += read_result;
		else { // ERROR: read
			// TODO: Handle read error codes (return!?)
			const int errno_save = errno;
			HCF_CC_TRACE(HandleInputErr, "Call 'read' failed.", errno_save);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to read from the internal timer object, errno == %d", errno_save);
		}
	}
	HCF_CC_TRACE(HandleInput, "Before signaling the event to all registered objects, disarm the timer and choose a new time point!", TSL_NS_HCF_CORE::utils::calculate_time_string(&_armed_timeval).c_str());

	// Disarm the timer
	::itimerspec its;
	its.it_value.tv_nsec = its.it_interval.tv_nsec = its.it_value.tv_sec = its.it_interval.tv_sec = 0;
	::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its, 0);

	timerclear(&_armed_timeval);

	TSL_NS_HCF_CORE::clock_alarm_handler * cah_ptrs [256];
	unsigned cah_count = 0;

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		std::vector<std::string> atp_alarmed(_atp_alarmed);

		// Set next alarm handlers
		activate_timer_for_next_alarms(tv_now);

		// Save the handler pointers
		for (std::vector<std::string>::iterator it = atp_alarmed.begin(); it != atp_alarmed.end(); it++) {
			_timeplan_handler_map_t::iterator item_ptr = _timeplan_handler_map.find(*it);

	//		(item_ptr != _timeplan_handler_map.end()) && (item_ptr->second._cah_ptr->k());
			(item_ptr != _timeplan_handler_map.end()) && (cah_ptrs[cah_count++] = item_ptr->second._cah_ptr);
		}
	TSL_CRITICAL_SECTION_END;

	// Now, I can call the clock alarm handlers
	for (unsigned i = 0; i < cah_count; i++) cah_ptrs[i] && (cah_ptrs[i]->handle_alarm(event_in_the_past));

	_handle_input_ongoing = false;

	return 0;
}

int __TSL_CLASS_NAME__::add_timeplan_ (
		const std::string & id,
		TSL_NS_HCF_CORE::alarm_timeplan * atp,
		TSL_NS_HCF_CORE::clock_alarm_handler * handler) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(AddTimeplan, "Received a request to add a timeplan.", id.c_str());
	::timeval tv_now;
	::timeval next_timer_point_relative;
	::itimerspec timer_current;
	::itimerspec its_relative;
//	TSL_NS_HCF_CORE::oneshot_alarm_timeplan * atp_new = 0;

	if (!handler) {
		HCF_CC_TRACE(AddTimeplanErr, "NULL pointer provided for the timeplan handler.", TSL_NS_HCF_CORE::ERR_NULL_ARGUMENT);
		HCF_CC_LOG(LEVEL_ERROR, "NULL instance provided for the handler of the timeplan having id == '%s'!", id.c_str());
		TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_NULL_ARGUMENT);
	}

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_timer_fd < 0) { // ERROR: Not open
			HCF_CC_TRACE(AddTimeplanErr, "Cannot add timeplan: the clock alarm notifier object has not been opened!", TSL_NS_HCF_CORE::ERR_INVALID_OPERATION);
			HCF_CC_LOG(LEVEL_ERROR, "The clock alarm notifier object has not been opened!");
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Clock notifier instance not open yet");
		}

		if (_handle_input_ongoing) {
			HCF_CC_TRACE(AddTimeplanErr, "Cannot add timeplan: there is a handle input operation ongoing!", TSL_NS_HCF_CORE::ERR_INVALID_OPERATION);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to add the timeplan having id == '%s', an handle input operation is ongoing!", id.c_str());
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_INVALID_OPERATION, "Alarm clock timeout handling ongoing");
		}

		if (_timeplan_handler_map.count(id) > 0) { // ERROR: Item already into the map
			HCF_CC_TRACE(AddTimeplanErr, "Cannot add timeplan: the item is already present!", TSL_NS_HCF_CORE::ERR_FOUND);
			HCF_CC_LOG(LEVEL_ERROR, "The timeplan having id == '%s' is already present!", id.c_str());
			TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_FOUND);
		}

		if ((::gettimeofday(&tv_now, 0))) {
			const int errno_save = errno;
			HCF_CC_TRACE(AddTimeplanErr, "Cannot add timeplan: failed to get system time!", errno_save);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to retrieve the current system calendar time, errno == %d", errno_save);
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
				const int errno_save = errno;
				HCF_CC_TRACE(AddTimeplanErr, "Cannot add timeplan: failed to get internal timer settings!", errno_save);
				HCF_CC_LOG(LEVEL_ERROR, "Failed to retrieve the current internal timer object settings, errno == %d", errno_save);
				TSL_SET_ERROR_INFO_AND_RETURN(-1, errno, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot retrieve the current internal timer object settings");
			}

			if (timer_current.it_value.tv_nsec || timer_current.it_value.tv_sec) {
				// Clock timer currently armed
				HCF_CC_TRACE(AddTimeplan, "The timer is currently armed: checking if a new timer point must be chosen!", id.c_str());
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
					::timeval a; a.tv_sec = next_timer_point_relative.tv_sec; a.tv_usec = next_timer_point_relative.tv_usec;
					::timeval res;

					timeradd(&a, &tv_now, &res);

					// The new alarm timeplan will fire before the current one
					// Clear the atp alarmed and add only this new one
					its_relative.it_value.tv_sec = res.tv_sec;
					its_relative.it_value.tv_nsec = res.tv_usec * 1000;
					//its_relative.it_value.tv_sec = next_timer_point_relative.tv_sec + tv_now.tv_sec;
					//its_relative.it_value.tv_nsec = next_timer_point_relative.tv_usec * 1000;
					its_relative.it_interval.tv_nsec = its_relative.it_interval.tv_sec = 0;

					HCF_CC_TRACE(AddTimeplan, "The new time point is nearer, setting it!", id.c_str());
					if (::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its_relative, 0)) { // ERROR: Setting the internal timer alarm clock
						int errno_save = errno;
						//delete atp_new;
						HCF_CC_TRACE(AddTimeplanErr, "Call 'timerfd_settime' failed.", errno_save);
						HCF_CC_LOG(LEVEL_ERROR, "Failed to set the internal timer alarm clock, errno == %d", errno_save);
						TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot set/arm the internal timer alarm clock");
					}

					_armed_timeval.tv_sec = res.tv_sec;
					_armed_timeval.tv_usec = res.tv_usec;
					_atp_alarmed.resize(0);
					_atp_alarmed.push_back(id);
				} else if (timercmp(&next_timer_point_relative, &timeval_current_right, <)) {
					// the new alarm timeplan will overlap the current one.
					// Adding this new alarm to the list of next firing alarms
					HCF_CC_TRACE(AddTimeplan, "The new time point will overlap the already armed event: adding to the alarmed items list!", id.c_str());
					_atp_alarmed.push_back(id);
				} else {
					// Nothing to change: the new alarm timeplan will fire after the current one
					HCF_CC_TRACE(AddTimeplan, "The new time point is bigger than the alarmed one: nothing to do!", id.c_str());
				}
			} else {
				// Clock timer currently disarmed
				::timeval a; a.tv_sec = next_timer_point_relative.tv_sec; a.tv_usec = next_timer_point_relative.tv_usec;
				::timeval res;

				timeradd(&a, &tv_now, &res);

				its_relative.it_value.tv_sec = res.tv_sec;
				its_relative.it_value.tv_nsec = res.tv_usec * 1000;
				//its_relative.it_value.tv_sec = next_timer_point_relative.tv_sec;
				//its_relative.it_value.tv_nsec = next_timer_point_relative.tv_usec * 1000;
				its_relative.it_interval.tv_nsec = its_relative.it_interval.tv_sec = 0;

				HCF_CC_TRACE(AddTimeplan, "The timer is not armed: setting the time!", id.c_str());
				if (::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its_relative, 0)) { // ERROR: Setting the internal timer alarm clock
					int errno_save = errno;
					//delete atp_new;
					HCF_CC_TRACE(AddTimeplanErr, "Call 'timerfd_settime' failed.", errno_save);
					HCF_CC_LOG(LEVEL_ERROR, "Failed to set the internal timer alarm clock, errno == %d", errno_save);
					TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot set/arm the internal timer alarm clock");
				}

				_armed_timeval.tv_sec = res.tv_sec;
				_armed_timeval.tv_usec = res.tv_usec;
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
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ActivateTimerForNextAlarms, "Received a request to select and set a new time point, starting from:", TSL_NS_HCF_CORE::utils::calculate_time_string(&tv_from).c_str());
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
		HCF_CC_TRACE(ActivateTimerForNextAlarms, "Requesting the next time point for the item:", it->first.c_str());
		if (it->second._atp_ptr->get_next_timer_point(next_timer_point_relative, tv_from) == 0) {
			if (timercmp(&next_timer_point_relative, &timeval_min_left, <)) {
				HCF_CC_TRACE(ActivateTimerForNextAlarms, "A new minimum has been found, in the item:", it->first.c_str());

				// A new minimal timer found
				alarm_to_activate[alarm_to_activate_counter = 0] = it->first;
				alarm_to_activate_counter++;

				next_timer_point_relative_min = next_timer_point_relative;
				timersub(&next_timer_point_relative_min, &timeval_750ms, &timeval_min_left);
				timeradd(&next_timer_point_relative_min, &timeval_750ms, &timeval_min_right);
			} else if (timercmp(&next_timer_point_relative, &timeval_min_right, <)) {
				HCF_CC_TRACE(ActivateTimerForNextAlarms, "A simultaneous event was found: add to the collection:", it->first.c_str());
				// A simultaneously timer found. Add it to the same collection
				alarm_to_activate[alarm_to_activate_counter++] = it->first;
			}
		}
	}
	_atp_alarmed.resize(alarm_to_activate_counter);

	if (!alarm_to_activate_counter)	{
		HCF_CC_TRACE(ActivateTimerForNextAlarms, "No events were found to be activated!", "");
		return TSL_NS_HCF_CORE::ERR_NO_ERROR;
	}

	_atp_alarmed.assign(alarm_to_activate, alarm_to_activate + alarm_to_activate_counter);

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

	HCF_CC_TRACE(ActivateTimerForNextAlarms, "The event was found, arming the timer!", TSL_NS_HCF_CORE::utils::calculate_time_string(&res).c_str());
	if (::timerfd_settime(_timer_fd, TFD_TIMER_ABSTIME, &its_relative, 0)) { // ERROR: Setting the internal timer alarm clock
		int errno_save = errno;
		HCF_CC_TRACE(ActivateTimerForNextAlarmsErr, "Call 'timerfd_settime' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Failed to set the internal timer alarm clock, errno == %d", errno_save);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR, "Cannot set/arm the internal timer alarm clock");
	}

	_armed_timeval.tv_sec = res.tv_sec;
	_armed_timeval.tv_usec = res.tv_usec;
	return TSL_NS_HCF_CORE::ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
