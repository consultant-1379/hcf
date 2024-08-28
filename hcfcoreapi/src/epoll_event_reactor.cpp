#include <sys/eventfd.h>

#include <algorithm>

#include "hcf/core/guard.h"
#include "hcf/core/epoll_event_reactor.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_REACTOR__

TSL_NS_HCF_CORE_BEGIN

int __TSL_CLASS_NAME__::open () {
	HCF_CC_TRACE_FUNCTION;
	guard<sync_t> exec_sentinel(_access_sync);

	if (_state & TSL_EVENT_REACTOR_STATE_CLOSED) {
		// Try to create the event wake-up handle
		HCF_CC_TRACE(Open, "The reactor object is in the CLOSED state: starting opening procedure, creating the wakeup handle!", _state);
		int wakeup_handle = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
		if (wakeup_handle < 0) { // ERROR: creating the internal wakeup handle
			const int errno_save = errno;
			HCF_CC_TRACE(OpenErr, "Call 'eventfd' failed.", ERR_INTERNAL_ERROR, errno_save);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, ERR_INTERNAL_ERROR, "Creating the wakeup internal handler");
		}

		// Try to init the internal event monitor first, if not already initialized
		HCF_CC_TRACE(Open, "Initializing the internal event monitor object!", _state);
		if (init_event_monitor() == ERR_ON_INIT) {
			const int errno_save = errno;
			HCF_CC_TRACE(OpenErr, "Call 'init_event_monitor' failed: closing the wakeup handle.", ERR_ON_INIT, errno_save);
			::close(wakeup_handle);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, ERR_ON_INIT, "Initializing internal event monitor");
		}

		// Add the wakeup handle into the internal event monitor
		epoll_event_info event_info;
		event_info.events = TSL_EPOLL_EVENT_READ_GROUP_MASK | TSL_EPOLL_EVENT_EXCEPTION_GROUP_MASK;
		event_info.data.fd = wakeup_handle;

		HCF_CC_TRACE(Open, "Adding the wakeup handle to the event monitor watched file descriptors!", _state);
		if (_event_monitor.add(wakeup_handle, event_info) < 0) {
			const int errno_save = errno;
			HCF_CC_TRACE(OpenErr, "Call '_event_monitor.add' failed: closing the event monitor and wakeup handle.", ERR_INTERNAL_ERROR, errno_save);
			_event_monitor.close();
			::close(wakeup_handle);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, ERR_INTERNAL_ERROR, "Activating internal monitor on the wakeup handler");
		}

		::close(_wakeup_handler.fd);
		_wakeup_handler.fd = wakeup_handle;
		_state = (_state & ~TSL_EVENT_REACTOR_STATE_CLOSED) | TSL_EVENT_REACTOR_STATE_NOT_LOOPING;
		HCF_CC_TRACE(Open, "The reactor object is now initialized and ready to run!", _state);
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::close () {
	HCF_CC_TRACE_FUNCTION;
	size_t handlers_size = _fd_handler_map.size();
	event_handler * event_handlers[handlers_size];

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _access_sync);
		if (_state & TSL_EVENT_REACTOR_STATE_CLOSED) {
			HCF_CC_TRACE(Close, "The reactor object is already in the CLOSED state: nothing to do!", _state);
			return ERR_NO_ERROR;
		}

		if (_state & TSL_EVENT_REACTOR_STATE_LOOPING) {
			HCF_CC_TRACE(Close, "The reactor object is in the LOOPING state: stopping the event loop!", _state);
			end_event_loop();
		}

		if (_state & TSL_EVENT_REACTOR_STATE_STOPPING) {
// TODO: Wait handlers and threads
		}

		HCF_CC_TRACE(Close, "The reactor object is in the NOT LOOPING state: erasing the internal data structures!", _state);

		event_handler ** local_it = event_handlers;
		fd_handler_map_t::iterator map_item_it = _fd_handler_map.begin();

		while (map_item_it != _fd_handler_map.end()) *local_it++ = ((map_item_it++)->second)._event_handler;

		// Call users event handler handle_close functions
		for (size_t i = 0; i < handlers_size; event_handlers[i++]->handle_close()) ;

		// Release all resources
		_fd_handler_map.clear();

		::close(_wakeup_handler.fd);

		_event_monitor.close();

		// Change the state
		_state |= TSL_EVENT_REACTOR_STATE_CLOSED;
		HCF_CC_TRACE(Close, "The reactor object is now closed!", _state);
	TSL_CRITICAL_SECTION_END;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::run_event_loop () {
	HCF_CC_TRACE_FUNCTION;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _event_loop_mutex);
		HCF_CC_TRACE(RunEventLoop, "Starting the event reactor object!", _state);

		if (_state & (TSL_EVENT_REACTOR_STATE_CLOSED | TSL_EVENT_REACTOR_STATE_STOPPING)) {	// ERROR: Closed or stopping
			HCF_CC_TRACE(RunEventLoopErr, "The event reactor is in a bad state (CLOSED or STOPPING): cannot run it!", ERR_INVALID_OPERATION);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Cannot run the loop while closed or stopping");
		}

		// Try to synchronize the internal _fd_handler_map with the event monitor interface
		if (_state & TSL_EVENT_REACTOR_STATE_UPDATING_EVENT_MONITOR) {
			HCF_CC_TRACE(RunEventLoop, "The event reactor updating its data structures, updating the event monitor!", _state);
			if (update_event_monitor() == -1) {
				// TODO: React on 'update_event_monitor' failure
				HCF_CC_TRACE(RunEventLoopErr, "Call 'update_event_monitor' failed", -1);
			}
		}

		_state = (_state & ~TSL_EVENT_REACTOR_STATE_NOT_LOOPING) | TSL_EVENT_REACTOR_STATE_LOOPING;
		HCF_CC_TRACE(RunEventLoop, "The event reactor object is started:  beginning its event loop!", _state);
	TSL_CRITICAL_SECTION_END;

	switch (do_event_loop()) {
	case 0: /* 0: All OK */
		break;
	default:
		break;
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::end_event_loop () {
	HCF_CC_TRACE_FUNCTION;

	if (_state & TSL_EVENT_REACTOR_STATE_CLOSED) {
		HCF_CC_TRACE(EndEventLoop, "The event reactor object was not opened: invalid operation!", _state);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION);
	}

	bool dispatch_end_request = false;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _event_loop_mutex);
		if (_state & TSL_EVENT_REACTOR_STATE_CLOSED) {
			HCF_CC_TRACE(EndEventLoop, "The event reactor object was not opened: invalid operation!", _state);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION);
		}

		if ((dispatch_end_request = (_state & TSL_EVENT_REACTOR_STATE_LOOPING))) {
			HCF_CC_TRACE(EndEventLoop, "The event reactor is looping: signaling the stop request via wakeup handle!", _state);
			_state = (_state & ~TSL_EVENT_REACTOR_STATE_LOOPING) | TSL_EVENT_REACTOR_STATE_STOPPING;
			signal_to_wakeup();
		}
	TSL_CRITICAL_SECTION_END;

	if (dispatch_end_request) _event_loop_condition.broadcast();

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::do_event_loop () {
	HCF_CC_TRACE_FUNCTION;
	int call_result = 0;
	int return_code = 0;

	// EVENT LOOP with locking/unlocking strategy
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _event_loop_mutex);
		++_threads_looping_counter;
		HCF_CC_TRACE(DoEventLoop, "Starting the reactor event loop!", _state, _threads_looping_counter);

		while (_state & TSL_EVENT_REACTOR_STATE_LOOPING) {
			while ((_state & TSL_EVENT_REACTOR_STATE_LOOPING) && (_state & TSL_EVENT_REACTOR_STATE_LEADERSHIP_OK)) {
				HCF_CC_TRACE(DoEventLoop, "The reactor has already a leader: waiting to become a leader!", _state, _threads_looping_counter);
				if ((call_result = _event_loop_condition.wait(_event_loop_mutex))) {
					// TODO:  CHECK on call_result and return with an error code for CONDITION.WAIT error
				}
			}

			HCF_CC_TRACE(DoEventLoop, "The reactor hasn't a leader: becoming a leader!", _state, _threads_looping_counter);
			if ((call_result = ((_state & TSL_EVENT_REACTOR_STATE_LOOPING) ? take_ownership_wait_call() : 0))) {
				// TODO: CHECK on call_result and BreaK loop with a right return_code error to send to the caller for take_ownership_wait_call function
			}
		}
		HCF_CC_TRACE(DoEventLoop, "The reactor event loop is completed!", _state, _threads_looping_counter);

		if ((--_threads_looping_counter == 0) && (_state & TSL_EVENT_REACTOR_STATE_STOPPING)) {
			// Now the state can be changed to NOT_LOOPING
			_state = (_state & ~TSL_EVENT_REACTOR_STATE_STOPPING) | TSL_EVENT_REACTOR_STATE_NOT_LOOPING;
			HCF_CC_TRACE(DoEventLoop, "The current thread is the last one: setting to STOPPED the reactor state!", _state, _threads_looping_counter);
		}
	TSL_CRITICAL_SECTION_END;

	return return_code;
}

int __TSL_CLASS_NAME__::take_ownership_wait_call () {
	HCF_CC_TRACE_FUNCTION;
	// Set the state to be LEADERSHIP OK
	_state |= TSL_EVENT_REACTOR_STATE_LEADERSHIP_OK;

	TSL_CRITICAL_SECTION_UNLOCKER_BEGIN(sync_t, _event_loop_mutex);
		if (_state & TSL_EVENT_REACTOR_STATE_LOOPING) {
			HCF_CC_TRACE(TakeOwnershipWait, "This thread was chosen as new leader: waiting on the file descriptors set.", _state);
			epoll_event_info event_info;

			// Wait on the event monitor
			const int wait_result = _event_monitor.wait(event_info);

			if (is_wakeup_handler_signalled()) {
				HCF_CC_TRACE(TakeOwnershipWait, "The wakeup handle has been signaled: propagating the stop event to all the threads!", _state);
				_event_loop_condition.broadcast();
				return 0;
			}

			TSL_CRITICAL_SECTION_BEGIN(sync_t, _event_loop_mutex);
				// Release the leadership
				_state &= ~TSL_EVENT_REACTOR_STATE_LEADERSHIP_OK;
			TSL_CRITICAL_SECTION_END;

			if (_state & TSL_EVENT_REACTOR_STATE_LOOPING) {
				HCF_CC_TRACE(TakeOwnershipWait, "An event must be handled and the reactor is still running: signal to one thread to become a leader!", _state);
				_event_loop_condition.signal();
			}
			else {
				HCF_CC_TRACE(TakeOwnershipWait, "The reactor is not running: skipping the event and propagate the stop to all threads!", _state);
				_event_loop_condition.broadcast();
				return 0;
			}

			if (wait_result) {
				if (wait_result > 0) { // An I/O event signaled. Handle this I/O event
					switch (handle_event(event_info)) {
					case ERR_INT_MAP_ITEM_NOT_FOUND:
						HCF_CC_TRACE(TakeOwnershipWait, "Call 'handle_event' failed: the item associated to the handle was not found!", _state);
						break;
					case ERR_INT_EVENT_HANDLER_NULL_POINTER:
						HCF_CC_TRACE(TakeOwnershipWait, "Call 'handle_event' failed: no event handler found for the provided item!", _state);
						break;
					case ERR_CHANGING_HANDLER:
						HCF_CC_TRACE(TakeOwnershipWait, "Call 'handle_event' failed: failed to update the internal data structures!", _state);
						break;
					default:
						break;
					}
				} else { // An error occurred on waiting into the monitor
					// TODO: CHECK: ERROR on epoll wait: CHOOSE the right error code or nothing for EINTR
				}
			} else { // wait_result == 0
				HCF_CC_TRACE(TakeOwnershipWait, "The event was correctly handled!", _state);
			}
		} else {
			HCF_CC_TRACE(TakeOwnershipWait, "The reactor is not running: signaling the stop to each thread!", _state);
			_event_loop_condition.broadcast(); // Not running. Wake-up all threads and exit immediately
		}
	TSL_CRITICAL_SECTION_END;

	return 0;
}

int __TSL_CLASS_NAME__::handle_event (epoll_event_info & event_info) {
	HCF_CC_TRACE_FUNCTION;
	event_handler * event_handler_ptr = 0;

	// Retrieve the internal info structure from the map using the signalled file descriptor
	fd_handler_map_t::iterator map_item_it = _fd_handler_map.end();

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _event_loop_mutex);
		if (!(_state & TSL_EVENT_REACTOR_STATE_LOOPING)) {
			HCF_CC_TRACE(HandleEvent, "The reactor is not running: signaling the stop event!", event_info.data.fd, event_info.events, 0);
			_event_loop_condition.broadcast();
			return 0;
		}

		HCF_CC_TRACE(HandleEvent, "Looking for the item associated to the signaled file descriptor!", event_info.data.fd, event_info.events, 0);
		if ((map_item_it = _fd_handler_map.find(event_info.data.fd)) == _fd_handler_map.end()) {
			// Item not found. Maybe someone has removed the event handler in the meantime.
			// To be safely, try to remove the I/O event from the epoll interface internal repository
			_event_monitor.remove(event_info.data.fd);
			HCF_CC_TRACE(HandleEventErr, "No item associated to the file descriptor was found!", ERR_INT_MAP_ITEM_NOT_FOUND);
			return ERR_INT_MAP_ITEM_NOT_FOUND;
		}

		// Get the user event handler object
		HCF_CC_TRACE(HandleEvent, "Getting the event handler object pointer!", event_info.data.fd, event_info.events, 0);
		if (!(event_handler_ptr = ((map_item_it->second)._event_handler))) {
			// A NULL user event handler pointer was found. This is a very strange situation.
			// Now it's preferable to remove the event handler without calling the handle_close
			// function on it because that's not possible.
			HCF_CC_TRACE(HandleEventErr, "NULL event handler found for the signaled file descriptor: erasing it from internal structures.", ERR_INT_EVENT_HANDLER_NULL_POINTER);

			if (const int call_result = remove_handler_(map_item_it, TSL_EVENT_DONT_CALL_MASK)) {
				HCF_CC_TRACE(HandleEventErr, "Call 'remove_handler_' failed.", call_result);
			}

			return ERR_INT_EVENT_HANDLER_NULL_POINTER;
		}

		int handle_result = 0;
		uint32_t events_mask = (map_item_it->second)._events_mask;

		// Check the I/O event triggered to call the right user handle function
		HCF_CC_TRACE(HandleEvent, "Dispatching the I/O event according to the received events and to the registered event mask!", event_info.data.fd, event_info.events, events_mask);
		if ((events_mask & TSL_EVENT_READ_MASK) && (event_info.events & TSL_EPOLL_EVENT_READ_GROUP_MASK)) {
			// Call the event_handler_handle_input function
			handle_result = event_handler_handle_input(map_item_it->second);
		} else if ((events_mask & TSL_EVENT_WRITE_MASK) && (event_info.events & TSL_EPOLL_EVENT_WRITE_GROUP_MASK)) {
			// Call the event_handler_handle_output function
			handle_result = event_handler_handle_output(map_item_it->second);
		} else if (event_info.events & TSL_EPOLL_EVENT_EXCEPTION_GROUP_MASK) {
			// Call the event_handler_handle_exception function in case of any error onto the file descriptor
			// Here the client/caller is suggested to implement the exception handling in any case
			// To avoid fast looping that can force the CPU to run at 100%
			handle_result = event_handler_handle_exception(map_item_it->second, event_info.events & EPOLLERR,
					event_info.events & EPOLLHUP, event_info.events & EPOLLRDHUP);
		} else {
			HCF_CC_TRACE(HandleEvent, "Cannot dispatch the I/O event: no match with the registered mask found!", event_info.data.fd, event_info.events, events_mask);
		}

		// Check the handle_result value
		if (handle_result == -1) {
			HCF_CC_TRACE(HandleEvent, "The call to the handle_* function was failed and the -1 value "
					"was returned: removing the event handler from the reactor!", event_info.data.fd, event_info.events, events_mask);

			// The event handler should be removed checking if the handle_close must be called also
			if (const int call_result = remove_handler_(event_info.data.fd, 0)) {
				HCF_CC_TRACE(HandleEventErr, "Call 'remove_handler_' failed.", call_result);
			}

			// Here the event user handler pointer is considered no more referenceable.
		} else if ((map_item_it = _fd_handler_map.find(event_info.data.fd)) != _fd_handler_map.end()) {
			HCF_CC_TRACE(HandleEvent, "I/O event dispatching completed: arming the reactor with the file descriptor!", event_info.data.fd, event_info.events, events_mask);

			(map_item_it->second)._action_state &= ~TSL_HANDLER_ACTION_HANDLE_ONGOING_GROUP;

			(map_item_it->second)._event_info.events |= EPOLLONESHOT;

			if (_event_monitor.change(event_info.data.fd, (map_item_it->second)._event_info) < 0) { // ERROR
				const int errno_save = errno;
				HCF_CC_TRACE(HandleEventErr, "Call 'remove_handler_' failed.", errno_save);
				return ERR_CHANGING_HANDLER;
			}
		}
	TSL_CRITICAL_SECTION_END;

	return 0;
}

int __TSL_CLASS_NAME__::event_handler_handle_input (epoll_fd_handler_info_t & fd_handler_info_ref) {
	HCF_CC_TRACE_FUNCTION;
	fd_handler_info_ref._action_state |= TSL_HANDLER_ACTION_HANDLE_INPUT_ONGOING;

	event_handler * event_handler_ptr = fd_handler_info_ref._event_handler;

	guard<sync_t, TSL_GUARD_STRATEGY_UNLOCK_LOCK> exec_sentinel(_event_loop_mutex);

	return event_handler_ptr->handle_input();
}

int __TSL_CLASS_NAME__::event_handler_handle_output (epoll_fd_handler_info_t & fd_handler_info_ref) {
	HCF_CC_TRACE_FUNCTION;
	fd_handler_info_ref._action_state |= TSL_HANDLER_ACTION_HANDLE_OUTPUT_ONGOING;

	event_handler * event_handler_ptr = fd_handler_info_ref._event_handler;

	guard<sync_t, TSL_GUARD_STRATEGY_UNLOCK_LOCK> exec_sentinel(_event_loop_mutex);

	return event_handler_ptr->handle_output();
}

int __TSL_CLASS_NAME__::event_handler_handle_exception (
		epoll_fd_handler_info_t & fd_handler_info_ref,
		bool error_flag,
		bool hungup_flag,
		bool read_hungup_flag) {
	HCF_CC_TRACE_FUNCTION;
	fd_handler_info_ref._action_state |= TSL_HANDLER_ACTION_HANDLE_EXCEPTION_ONGOING;

	event_handler * event_handler_ptr = fd_handler_info_ref._event_handler;

	guard<sync_t, TSL_GUARD_STRATEGY_UNLOCK_LOCK> exec_sentinel(_event_loop_mutex);

	return event_handler_ptr->handle_exception(error_flag, hungup_flag, read_hungup_flag);
}

int __TSL_CLASS_NAME__::register_handler_ (int io_handle, event_handler * handler, uint32_t events_mask) {
	HCF_CC_TRACE_FUNCTION;
	guard<sync_t> exec_sentinel(_access_sync);

	epoll_fd_handler_info_t & fd_handler_info_ref = _fd_handler_map[io_handle];

	if (fd_handler_info_ref._event_handler) {
		// A fd_handler_info instance found because already present into the map
		// Perform a change on its data
		HCF_CC_TRACE(RegisterHandler, "Received a request to register an event handler: "
				"the handle was already registered, updating it!", io_handle, events_mask);

		epoll_fd_handler_info_t fd_handler_info_save = fd_handler_info_ref;

		// Changing event data and signal mask
		fd_handler_info_ref._event_handler = handler;
		fd_handler_info_ref._event_info.events = map_to_epoll_events_mask(fd_handler_info_ref._events_mask = events_mask);

		// Change also the epoll interface layer only if the monitor is ready
		if (!(_state & TSL_EVENT_REACTOR_STATE_CLOSED)) {
			HCF_CC_TRACE(RegisterHandler, "The reactor is currently running: updating the event monitor "
					"to change the information associated to the I/O handle!", io_handle, events_mask);

			if (_event_monitor.change(io_handle, fd_handler_info_ref._event_info) < 0) { // ERROR
				const int errno_save = errno;
				fd_handler_info_ref = fd_handler_info_save;
				HCF_CC_TRACE(RegisterHandlerErr, "Call '_event_monitor.change' failed.", errno_save);
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_CHANGING_HANDLER);
			}

			// OK. Event monitor updated
			fd_handler_info_ref._action_state = TSL_HANDLER_ACTION_NONE;
		}

		return ERR_NO_ERROR;
	}

	HCF_CC_TRACE(RegisterHandler, "Received a request to register an event handler: creating "
			"a new item in the internal structures!", io_handle, events_mask);

	// Fill the handler info internal structure
	fd_handler_info_ref._event_handler = handler;
	fd_handler_info_ref._event_info.events = map_to_epoll_events_mask(fd_handler_info_ref._events_mask = events_mask);
	fd_handler_info_ref._event_info.data.fd = io_handle;

	// Add also into the epoll interface layer if the processor is running
	if (!(_state & TSL_EVENT_REACTOR_STATE_CLOSED)) {
		HCF_CC_TRACE(RegisterHandler, "The reactor is currently running: updating the "
				"event monitor to add the new I/O handle!", io_handle, events_mask);

		if (_event_monitor.add(io_handle, fd_handler_info_ref._event_info) < 0) { // ERROR
			const int errno_save = errno;
			_fd_handler_map.erase(io_handle);
			HCF_CC_TRACE(RegisterHandlerErr, "Call '_event_monitor.add' failed.", errno_save);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, errno_save, ERR_REGISTERING_HANDLER);
		}
	} else {
		// The internal event monitor must be updated adding this event
		_state |= TSL_EVENT_REACTOR_STATE_UPDATING_EVENT_MONITOR;
		fd_handler_info_ref._action_state = TSL_HANDLER_ACTION_TABLE_ADD;
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::remove_handler (int io_handle, uint32_t events_mask) {
	HCF_CC_TRACE_FUNCTION;
	if (io_handle < 0) {
		HCF_CC_TRACE(RemoveHandlerErr, "A negative value for the I/O handle has been provided!", ERR_INVALID_HANDLE);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_HANDLE);
	}

	guard<sync_t> exec_sentinel(_access_sync);

	// Search for the event handler client object
	fd_handler_map_t::iterator map_item_it = _fd_handler_map.find(io_handle);

	if (map_item_it == _fd_handler_map.end()) {
		// To be safely, try to remove the I/O event from the epoll interface internal repository
		_event_monitor.remove(io_handle);

		HCF_CC_TRACE(RemoveHandlerErr, "The I/O handle was not found in the internal data structures!", ERR_NOT_FOUND);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NOT_FOUND);
	}

	// Is there any operation ongoing on that event handler client object?
	if ((map_item_it->second)._action_state & TSL_HANDLER_ACTION_HANDLE_ONGOING_GROUP) {	// YES
		HCF_CC_TRACE(RemoveHandlerErr, "There is an ongoing callback for the requested handle: cannot remove it!", ERR_INVALID_OPERATION);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION);
	}

	return remove_handler_(map_item_it, events_mask);
}

int __TSL_CLASS_NAME__::remove_handler_ (fd_handler_map_t::iterator & map_item_it, uint32_t events_mask) {
	HCF_CC_TRACE_FUNCTION;
	epoll_fd_handler_info_t fd_handler_info_copy = map_item_it->second;
	HCF_CC_TRACE(RemoveHandler, "Received a request to unregister an event handler from the reactor!", fd_handler_info_copy._event_info.data.fd, events_mask);

	// Remove the event info from the event monitor
	_event_monitor.remove(fd_handler_info_copy._event_info.data.fd);

	// Delete the entry from the internal map.
	_fd_handler_map.erase(map_item_it);

	// Calling the handle_close function if needed or requested
	if ((events_mask & TSL_EVENT_DONT_CALL_MASK) || (fd_handler_info_copy._events_mask & TSL_EVENT_DONT_CALL_MASK)) {	// DO NOT CALL.
		HCF_CC_TRACE(RemoveHandler, "The caller requested to NOT CALL the handle_close function!", fd_handler_info_copy._event_info.data.fd, events_mask);
	} else {
		// Call handle_close function
		HCF_CC_TRACE(RemoveHandler, "Before completing the removing procedure, call the handle_close finalization function.", fd_handler_info_copy._event_info.data.fd, events_mask);
		(fd_handler_info_copy._event_handler)->handle_close();
	}

	// Here the event user handler pointer is considered no more referenceable.
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::update_event_monitor () {
	HCF_CC_TRACE_FUNCTION;
	register uint32_t action_state = TSL_HANDLER_ACTION_NONE;
	epoll_fd_handler_info_t * ptr = 0;
	unsigned failed_calls = 0;

	for (fd_handler_map_t::iterator it = _fd_handler_map.begin(); (it != _fd_handler_map.end()) && (ptr = &it->second); ++it) {
		if ((action_state = ptr->_action_state) == TSL_HANDLER_ACTION_NONE) continue;

		// Using a template should be more elegant, but the time is over

		(_event_monitor.*((action_state == TSL_HANDLER_ACTION_TABLE_ADD)
			 ? static_cast<int (epoll_event_monitor::*)(int, epoll_event_info &)>(&epoll_event_monitor::add)
			 : static_cast<int (epoll_event_monitor::*)(int, epoll_event_info &)>(&epoll_event_monitor::change))
		)(ptr->_event_info.data.fd, ptr->_event_info)

			? (++failed_calls)	// ERROR
			: (ptr->_action_state &= ~TSL_HANDLER_ACTION_TABLE_ADD_CHANGE_GROUP);	// OK
	}

	if (failed_calls) return -1;
	else _state &= ~TSL_EVENT_REACTOR_STATE_UPDATING_EVENT_MONITOR;

	return 0;
}

TSL_NS_HCF_CORE_END
