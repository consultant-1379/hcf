#ifndef __INCLUDE_GUARD_CLASS_SRV_CMD_HANDLER__
#define __INCLUDE_GUARD_CLASS_SRV_CMD_HANDLER__ cmd_handler

/*
 *	@file cmd_handler.h
 *	@brief
 *	@author xlucpet (Luca Petrunti)
 *	@date 2015-01-15
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
 *	| 2015-001 | 2015-01-15 | xlucpet      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *	| 2015-002 | 2015-03-06 | xcasale      | Event handling adaptation.          |
 *	+==========+============+==============+=====================================+
 */

#include "srv_macros.h"

#include "hcf/core/event_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CMD_HANDLER__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::event_handler {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR()
		TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::event_handler(), _server_socket_fd(-1), _installed_rule()) {}

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
	inline virtual int get_handle () { return _server_socket_fd; }

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
	 * This method initializes the Command Handler object.
	 *
	 * @return 0 on success, a non zero value otherwise.
	 */
	int initialize ();

	/**
	 * This method finalizes the Command Handler object.
	 *
	 * @return 0 on success, a non zero value otherwise.
	 */
	int finalize ();

	/**
	 * This method performs all the needed operations about the RSF, as:
	 * 		1. Install previously installed RSFs
	 * 		2. Install (or update) all the scheduled RSFs
	 * 		3. Delete all the scheduled RSFs.
	 *
	 * @return 0 on success, a not zero value otherwise.
	 */
	int handle_startup_operations ();

private:
	/**
	 * This method performs all the needed operations to install a RSF.
	 *
	 * @param[out] reply_primitive - The primitive to send back to the command.
	 * @param[out] reply_message - The primitive message to send back to the command.
	 * @param[in]  reply_message_size - The size of the reply_message string.
	 * @param[in]  not_startup_phase - Flag indicating if the installation has been invoked during the startup phase or not.
	 *                                 This is useful to understand if the RSF source file must be deleted when the
	 *                                 on installation failure and if the duplicated rule object event must be considered
	 *                                 as an error or not.
	 */
	int handle_install_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size, bool not_startup_phase = true);

	/**
	 * This method performs all the needed operations to delete a RSF.
	 *
	 * @param[out] reply_primitive - The primitive to send back to the command.
	 * @param[out] reply_message - The primitive message to send back to the command.
	 * @param[in]  reply_message_size - The size of the reply_message string.
	 */
	int handle_delete_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size);

	/**
	 * This method performs all the needed operations to validate a RSF.
	 *
	 * @param[out] reply_primitive - The primitive to send back to the command.
	 * @param[out] reply_message - The primitive message to send back to the command.
	 * @param[in]  reply_message_size - The size of the reply_message string.
	 */
	int handle_validation_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size);

	/**
	 * This method performs all the needed operations to list all the available RSFs.
	 *
	 * @param[out] reply_primitive - The primitive to send back to the command.
	 * @param[out] reply_message - The primitive message to send back to the command.
	 * @param[in]  reply_message_size - The size of the reply_message string.
	 */
	int handle_listing_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size);

	/**
	 * This method performs all the needed operations to update a RSF.
	 *
	 * @param[out] reply_primitive - The primitive to send back to the command.
	 * @param[out] reply_message - The primitive message to send back to the command.
	 * @param[in]  reply_message_size - The size of the reply_message string.
	 */
	int handle_update_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	int _server_socket_fd;
	std::list <std::pair<std::string,std::string> > _installed_rule;
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_CLASS_HANDLER__ */
