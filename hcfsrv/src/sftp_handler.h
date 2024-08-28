#ifndef __INCLUDE_GUARD_CLASS_SRV_SFTP_HANDLER__
#define __INCLUDE_GUARD_CLASS_SRV_SFTP_HANDLER__ sftp_handler

/*
 *	@file sftp_handler.h
 *	@brief
 *	@author xclaele (Claudio Elefante)
 *	@date 2015-07-01
 *
 *	COPYRIGHT Ericsson AB, 2015
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+==========+============+==============+=====================================+
 *	| REV      | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+==========+============+==============+=====================================+
 *	| 2015-001 | 2015-07-01 | xclaele      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <string>
#include <queue>

#include <libssh2.h>

#include "hcf/core/thread_mutex.h"
#include "hcf/core/thread_condition.h"
#include "hcf/core/event_handler.h"
#include "internalapi_constants.h"
#include "srv_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_SFTP_HANDLER__

#define DEFAULT_SFTP_PORT 22

HCF_NS_HCF_SRV_BEGIN

/**
 * This structure stores all the information needed to transfer a file via SFTP.
 */
struct file_to_transfer {
	std::string server;
	int port;
	std::string user;
	std::string password;
	std::string remoteFile;
	std::string localFile;
};

enum FileTransferResult {
	FILE_TRANSFER_END_WAIT_ERROR	= -2,
	FILE_TRANSFER_ERROR				= -1,
	FILE_TRANSFER_OK				= 0
};

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::event_handler {
	//==========//
	// Typedefs //
	//==========//
private:
	typedef TSL_NS_HCF_CORE::thread_mutex_recursive sync_t;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::event_handler(), _sftp_folder(TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_job_exporting_path()),
			_event_fd(-1), _ftq_mutex(), _ftq(), _sftp_msg(""), _fileSent(0), _transfer_end_condition_mutex(), _transfer_end_condition()) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//======================================================//
	// event_handler class Virtual Functions Implementation //
	//======================================================//
public:
	/**
	 * This method returns the I/O handle associated to the Event Handler object.
	 *
	 * @return the I/O handle associated to the class implementer.
	 */
	inline virtual int get_handle () { return _event_fd; }

	/**
	 * This method is called when an input operation is signaled on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_input ();

	/**
	 * This method is called when an output operation is signaled on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_output ();

	/**
	 * This method is called when an error occurs on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_exception (bool error_flag, bool hungup_flag, bool read_hungup_flag);

	/**
	 * This method is called before removing the Event Handler from the reactor.
	 *
	 * @return 0 on success, -1 on failure.
	 */
	virtual int handle_close ();

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method returns the SFTP folder path.
	 *
	 * @return the SFTP folder path.
	 */
	inline std::string & get_sftp_folder () { return _sftp_folder; }

	/**
	 * This method initializes the SFTP Handler object.
	 *
	 * @return 0 on success, a non zero value otherwise.
	 */
	int initialize ();

	/**
	 * This method finalizes the SFTP Handler object.
	 *
	 * @return 0 on success, a non zero value otherwise.
	 */
	int finalize ();

	/**
	 * This method adds a file to the SFTP Handler file transfer queue and signal the event to the reactor.
	 *
	 * @param[in] file - The structure containing the info about the file to transfer.
	 * @return 0 on success, a not zero value otherwise.
	 */
	int add_file_to_ftq (file_to_transfer & file);

	/**
	 * This method signals the end of a file transfer.
	 *
	 * @param[out] fileSent - An integer value providing info about file transfer result.
	 * @param[out] sftp_msg - The message containing the info about the file transfer result.
	 */
	void wait_sftp_transfer_end (int & fileSent, std::string & sftp_msg);

private:
	/**
	 * Attempts keyboard-interactive (challenge/response) authentication.
	 *
	 * As authentication proceeds, the host issues several (1 or more) challenges
	 * and requires responses. This callback will be called at this moment.
	 * The callback is responsible to obtain responses for the challenges,
	 * fill the provided data structure and then return control.
	 * Responses will be sent to the host.
	 * String values will be freed by the library.
	 */
	static void kbd_callback (const char *name, int name_len,
			const char *instruction, int instruction_len,
			int num_prompts,
			const LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
			LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
			void **abstract);

	/**
	 * This method transfer a file via SFTP protocol.
	 *
	 * @param[in] file - The structure containing the info about the file to transfer.
	 */
	void transfer_file_via_sftp (file_to_transfer & file);

	/**
	 * This method signals the end of a file transfer.
	 */
	void signal_sftp_transfer_end ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	// Used by kbd_callback in case of authentication via keyboard-interactive
	static std::string _password;

	std::string _sftp_folder;
	int _event_fd;
	TSL_NS_HCF_CORE::thread_mutex_recursive _ftq_mutex;
	std::queue<file_to_transfer> _ftq;

	std::string _sftp_msg;
	int _fileSent;
	TSL_NS_HCF_CORE::thread_mutex_recursive _transfer_end_condition_mutex;
	TSL_NS_HCF_CORE::thread_condition _transfer_end_condition;
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_SFTP_HANDLER__ */
