#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/eventfd.h>
#include <sys/socket.h>

#include <libssh2_sftp.h>

#include "hcf/core/logger.h"
#include "hcf/core/guard.h"
#include "hcf/core/utils.h"

#include "configurator.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_sftphandler.h"
#include "hcf/trace/tracing_macros.h"

#include "sftp_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_SFTP_HANDLER__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

std::string __TSL_CLASS_NAME__::_password = "";

int __TSL_CLASS_NAME__::handle_input () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Received a request to handle an SFTP file transfer!");

	// FIRST: consume the value stored into the event file descriptor
	uint64_t value;
	HCF_CC_TRACE(HandleInput, "Received a request to handle a SFTP file transfer operation!", "");
	if (::read(_event_fd, &value, sizeof(value)) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'read' failed, %s errno == %d", (errno_save != EINTR) ? "removing the Event Handler from the reactor," : "", errno_save);
		HCF_CC_TRACE(HandleInputErr, "Call 'read' failed.", errno_save);
		return (errno_save != EINTR) ? -1 : 0;
	}

	// SECOND: take the information about the job to be executed from the internal queue
	file_to_transfer file;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _ftq_mutex);
	file = _ftq.front();
	TSL_CRITICAL_SECTION_END;

	// THIRD: transfer the file via SFTP
	HCF_CC_TRACE(HandleInput, "Transferring over SFTP protocol the following file:", file.localFile.c_str());
	transfer_file_via_sftp(file);

	//FOURTH: signal file transfer end
	HCF_CC_TRACE(HandleInput, "Signaling the end of transfer for the following file:", file.localFile.c_str());
	signal_sftp_transfer_end();

	//FIFTH: remove file from SFTP exporting folder
	HCF_CC_TRACE(HandleInput, "Removing the file from the internal folders:", file.localFile.c_str());
	if(TSL_NS_HCF_CORE::utils::remove(file.localFile.c_str()) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_WARNING, "Failed to delete file '%s', errno == %d", file.localFile.c_str(), errno_save);
		HCF_CC_TRACE(HandleInputErr, "Call 'utils::remove' failed.", errno_save);
	}

	// SIXTH: remove from the internal queue the transferred file
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _ftq_mutex);
	_ftq.pop();
	TSL_CRITICAL_SECTION_END;

	return 0;
}

int __TSL_CLASS_NAME__::handle_output () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleOutput, "Received an output I/O event for the SFTP Handler, it "
			"shouldn't occur, removing the Event Handler from the reactor");
	HCF_CC_LOG(LEVEL_WARNING, "Received an output I/O event for the SFTP Handler, it "
			"shouldn't occur, removing the Event Handler from the reactor");
	return -1;
}

int __TSL_CLASS_NAME__::handle_exception (bool error_flag, bool hungup_flag, bool read_hungup_flag) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleException, "Received an error on the SFTP Handler I/O handle, removing the Event Handler "
			"from the reactor: error_flag == %s, hungup_flag == %s, read_hungup_flag == %s",
			(error_flag ? "TRUE" : "FALSE"), (hungup_flag ? "TRUE" : "FALSE"), (read_hungup_flag ? "TRUE" : "FALSE"));
	HCF_CC_LOG(LEVEL_WARNING, "Received an error on the SFTP Handler I/O handle, removing the Event Handler "
			"from the reactor: error_flag == %s, hungup_flag == %s, read_hungup_flag == %s",
			(error_flag ? "TRUE" : "FALSE"), (hungup_flag ? "TRUE" : "FALSE"), (read_hungup_flag ? "TRUE" : "FALSE"));
	return -1;
}

int __TSL_CLASS_NAME__::handle_close () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleClose, "Removing the SFTP Handler from the reactor!");
	HCF_CC_LOG(LEVEL_NOTICE, "Removing the SFTP Handler from the reactor!");
	return 0;
}

int __TSL_CLASS_NAME__::initialize () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Initializing the SFTP Handler object!");

	// Initialize the libssh2 functions.
	HCF_CC_TRACE(Initialize, "Received a request to initialize the SFTP Handler object, initializing the libSSH2 library!", "");
	if (libssh2_init(0) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'libssh2_init(0)' failed, errno == %d", errno_save);
		HCF_CC_TRACE(InitializeErr, "Call 'libssh2_init' failed.", errno_save);
		return -1;
	}

	HCF_CC_TRACE(Initialize, "Received a request to initialize the SFTP Handler object, creating the internal file descriptor!", "");
	// Create the file descriptor to be used to signal the events to the SFTP Handler
	_event_fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK | EFD_SEMAPHORE);
	if (_event_fd < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'eventfd' failed, errno == %d", errno_save);
		HCF_CC_TRACE(InitializeErr, "Call 'eventfd' failed.", errno_save);
		return -1;
	}

	// Before creating the folder, remove its eventual pre-existing content
	HCF_CC_TRACE(Initialize, "Removing the (eventually existing) content of the SFTP Handler private folder!", _sftp_folder.c_str());
	TSL_NS_HCF_CORE::utils::remove_folder_recursive(_sftp_folder.c_str());

	// Create the folder to store files to transfer
	HCF_CC_TRACE(Initialize, "Creating the SFTP Handler private folder!", _sftp_folder.c_str());
	if (TSL_NS_HCF_CORE::utils::create_dir_recursive(_sftp_folder.c_str(), 0770) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Failed to create the SFTP folder to store files to transfer (%s), errno == %d!",
				_sftp_folder.c_str(), errno_save);
		HCF_CC_TRACE(InitializeErr, "Call 'create_dir_recursive' failed.", errno_save);
		return -1;
	}

	return 0;
}

int __TSL_CLASS_NAME__::finalize () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Finalizing the SFTP Handler object!");

	// Close file descriptor
	HCF_CC_TRACE(Finalize, "Received a request to finalize the SFTP Handler object: closing internal file descriptor!");
	::close(_event_fd);

	// Exit the libssh2 functions and frees all memory used internal.
	HCF_CC_TRACE(Finalize, "Received a request to finalize the SFTP Handler object: finalizing libSSH2!");
	libssh2_exit();

	// Remove the folder to store files to transfer
	HCF_CC_TRACE(Finalize, "Received a request to finalize the SFTP Handler object: removing internal folders!");
	if (TSL_NS_HCF_CORE::utils::remove_folder_recursive(_sftp_folder.c_str()) < 0) {
		HCF_CC_LOG(LEVEL_ERROR, "Failed to remove the SFTP folder '%s'", _sftp_folder.c_str());
		HCF_CC_TRACE(Finalize, "Call 'remove_folder_recursive' failed.");
	}

	return 0;
}

int __TSL_CLASS_NAME__::add_file_to_ftq (file_to_transfer& file) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(AddFileToFTQ, "Received a request to transfer a new file over SFTP protocol!", file.localFile.c_str());

	// FIRST: Push file to transfer into the internal queue
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _ftq_mutex);
	_ftq.push(file);
	TSL_CRITICAL_SECTION_END;

	// SECOND: signal to the SFTP Handler that there is a new file to transfer
	const uint64_t value = 1;
	HCF_CC_TRACE(AddFileToFTQ, "Signaling to SFTP Handler object that a new file must be transferred!", file.localFile.c_str());
	if (::write(_event_fd, &value, sizeof(value)) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'write' failed, errno == %d", errno_save);
		HCF_CC_TRACE(AddFileToFTQErr, "Call 'write' failed.", errno_save);
		return -1;
	}

	return 0;
}

/* kbd_callback for keyboard-interactive authentication*/
void __TSL_CLASS_NAME__:: kbd_callback(const char * TSL_UNUSED_PAR(name), int TSL_UNUSED_PAR(name_len),
		const char * TSL_UNUSED_PAR(instruction), int TSL_UNUSED_PAR(instruction_len),
		int num_prompts,
		const LIBSSH2_USERAUTH_KBDINT_PROMPT * TSL_UNUSED_PAR(prompts),
		LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
		void ** TSL_UNUSED_PAR(abstract))
{
	HCF_CC_TRACE_FUNCTION;
	if (num_prompts == 1) {
		responses[0].text = strdup(_password.c_str());
		responses[0].length = strlen(_password.c_str());
	}
}

void __TSL_CLASS_NAME__::transfer_file_via_sftp (file_to_transfer & file) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Starting processing for exporting local file '%s' via SFTP to remote host '%s' into remote file '%s'",
			file.localFile.c_str(), file.server.c_str(), file.remoteFile.c_str());

	_fileSent = FILE_TRANSFER_ERROR;

	/* First of all check local file */
	HCF_CC_TRACE(TransferFileViaSFTP, "Received a request to transfer a file over SFTP: checking if the file exists!", file.localFile.c_str());
	struct stat stat_buff;
	if(::stat(file.localFile.c_str(), &stat_buff) == -1) {
		const int errno_save = errno;
		_sftp_msg = "I/O error";
		HCF_CC_LOG(LEVEL_ERROR, "Unable to open local file '%s', errno == %d", file.localFile.c_str(), errno_save);
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'stat' failed.", errno_save, _sftp_msg.c_str());
		return;
	}

	/***************** BEGIN *****************/
	/*         Connect to remote host        */
	/*****************************************/
	/*
	 * The application code is responsible for creating the socket
	 * and establishing the connection
	 */
	struct sockaddr_in server;
	memset(&server,'\0',sizeof(struct sockaddr_in));

	/* Retrieve the IP address associated to the retrieved host name */
	HCF_CC_TRACE(TransferFileViaSFTP, "Retrieving the IP address of the following host:", file.server.c_str());
	if (configurator::get_addr_by_hostname(file.server.c_str(), (struct sockaddr *)(&server))) {
		_sftp_msg = "connection failure";
		HCF_CC_LOG(LEVEL_ERROR, "get_addr_by_hostname' failed for host name '%s'", file.server.c_str());
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'configurator::get_addr_by_hostname' failed.", -1, _sftp_msg.c_str());
		return;
	}

	server.sin_family = AF_INET;
	int port = ((file.port == 0) ? DEFAULT_SFTP_PORT : file.port);
	server.sin_port = htons(port);

	HCF_CC_TRACE(TransferFileViaSFTP, "Creating a socket to connect to the remote server. The port used will be:", std::to_string(port).c_str());
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		const int errno_save = errno;
		_sftp_msg = "generic error";
		HCF_CC_LOG(LEVEL_ERROR, "Error creating socket for connecting to '%s:%d'", file.server.c_str(), port);
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'socket' failed.", errno_save, _sftp_msg.c_str());
		return;
	}
	HCF_CC_TRACE(TransferFileViaSFTP, "Socket correctly created: setting it as non-blocking!", "");

	int result;

	/* Getting the socket properties before setting the socket non blocking */
	int arg1 = fcntl( sock, F_GETFL, NULL );

	/* Setting the socket non blocking */
	result = fcntl( sock, F_SETFL, O_NONBLOCK);

	errno = 0;
	/* Connecting to the remote ip address */
	HCF_CC_LOG(LEVEL_NOTICE, "Connecting to '%s:%d'", file.server.c_str(), port);
	HCF_CC_TRACE(TransferFileViaSFTP, "Connecting to the following remote server:", file.server.c_str());
	result = connect(sock, (struct sockaddr*)(&server),sizeof(struct sockaddr_in));
	if(result == -1) {
		const int errno_save = errno;
		if (errno_save == EINPROGRESS) {
			fd_set writefdSet;
			FD_ZERO( &writefdSet);
			FD_SET( sock, &writefdSet );

			struct timeval tval;
			tval.tv_usec = 0;
			tval.tv_sec = 2;

			/* Waiting 2 seconds for reply */
			HCF_CC_TRACE(TransferFileViaSFTP, "The connection operation is still in progress, let's wait its conclusion for some seconds!", "");
			int result1 = select(sock+1, (fd_set*)0, &writefdSet, (fd_set*)0, &tval);
			if( result1 <=  0 )	{ // TIMEOUT or select error
				close(sock);
				_sftp_msg = "connection failure";
				HCF_CC_LOG(LEVEL_ERROR, "Timeout or select error, destination '%s:%d' unreachable, result == %d, errno == %d", file.server.c_str(), port, result1, errno);
				HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'connect' failed.", -1, _sftp_msg.c_str());
				return;
			}
			else {
				if(FD_ISSET(sock, &writefdSet)) {
					HCF_CC_TRACE(TransferFileViaSFTP, "The connection operation is concluded: checking its result!", "");
					int so_error = 0;
					socklen_t len = sizeof so_error;
					getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
					if (so_error == 0) {
						/*
						 * Connection established successful
						 * Setting the sock of blocking type
						 */
						HCF_CC_TRACE(TransferFileViaSFTP, "Connect operation was successful: setting again the socket as blocking!", "");
						fcntl( sock, F_SETFL, arg1);
					}
					else {
						close(sock);
						if (so_error == ECONNREFUSED) {
							_sftp_msg = "connection refused";
						}
						else {
							_sftp_msg = "connection failure";
						}
						HCF_CC_LOG(LEVEL_ERROR, "Connection failure[%d] while connecting to '%s:%d'", so_error, file.server.c_str(), port);
						HCF_CC_TRACE(TransferFileViaSFTPErr, "Connect operation was unsuccessful!", so_error, _sftp_msg.c_str());
						return;
					}
				}
				else {
					close(sock);
					_sftp_msg = "generic error";
					HCF_CC_LOG(LEVEL_ERROR, "Error while connecting to '%s:%d', socket file descriptor is not set!", file.server.c_str(), port);
					HCF_CC_TRACE(TransferFileViaSFTPErr, "Generic error occurred while connecting to remote server: file descriptor not set!", -1, _sftp_msg.c_str());
					return;
				}
			}
		}
		else {
			close(sock);
			_sftp_msg = "generic error";
			HCF_CC_LOG(LEVEL_ERROR, "Connection error while connecting to '%s:%d', errno == %d", file.server.c_str(), port, errno_save);
			HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'connect' failed.", errno_save, _sftp_msg.c_str());
			return;
		}
	}
	/*****************************************/
	/*         Connect to remote host        */
	/*****************  END  *****************/

	/***************** BEGIN *****************/
	/*              SSH session              */
	/*****************************************/
	/* Create a session instance */
	HCF_CC_TRACE(TransferFileViaSFTP, "Creating a new SSH2 protocol session!", "");
	LIBSSH2_SESSION *session = libssh2_session_init();
	if(!session) {
		close(sock);
		_sftp_msg = "generic error";
		HCF_CC_LOG(LEVEL_ERROR, "libssh2_session_init() failed to create session");
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'libssh2_session_init' failed.", -1, _sftp_msg.c_str());
		return;
	}
	HCF_CC_LOG(LEVEL_NOTICE, "libssh2_session_init() was successful");

	/* Set blocking mode on session */
	HCF_CC_TRACE(TransferFileViaSFTP, "SSH2 session correctly created: setting blocking and timeout properties!", "");
	libssh2_session_set_blocking(session, 1);

	/* Set timeout in milliseconds for blocking functions */
	libssh2_session_set_timeout(session, 5000);
	HCF_CC_LOG(LEVEL_NOTICE, "The current session timeout is %lu ms", libssh2_session_get_timeout(session));

	/*
	 * ... start it up. This will trade welcome banners, exchange keys,
	 * and setup crypto, compression, and MAC layers
	 */
	HCF_CC_TRACE(TransferFileViaSFTP, "Starting the SSH2 communication: executing the handshaking phase!", "");
	int rc = libssh2_session_handshake(session, sock);
	if(rc) {
		HCF_CC_LOG(LEVEL_ERROR, "libssh2_session_handshake() was unsuccessful: SSH session errno == %d", libssh2_session_last_errno(session));
		char * err_msg;
		libssh2_session_last_error(session, &err_msg, NULL, 0);
		HCF_CC_LOG(LEVEL_ERROR, "SSH session error message == %s", err_msg);

		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		close(sock);

		_sftp_msg = "generic error";
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'libssh2_session_handshake' failed.", libssh2_session_last_errno(session), err_msg);
		return;
	}
	/* libssh2_session_handshake was successful */
	HCF_CC_LOG(LEVEL_NOTICE, "libssh2_session_handshake() was successful");

	/*
	 * At this point we havn't yet authenticated.  The first thing to do
	 * is check the hostkey's fingerprint against our known hosts Your app
	 * may have it hard coded, may go to a file, may present it to the
	 * user, that's your call
	 */
	libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

	/* Check what authentication methods are available */
	HCF_CC_TRACE(TransferFileViaSFTP, "Retrieving the available authentication methods for the following user:", file.user.c_str());
	char * userauthlist;
	if(!(userauthlist = libssh2_userauth_list(session, file.user.c_str(), strlen(file.user.c_str())))) {
		HCF_CC_LOG(LEVEL_ERROR, "libssh2_userauth_list() was unsuccessful: SSH session errno == %d", libssh2_session_last_errno(session));
		char *err_msg;
		libssh2_session_last_error(session, &err_msg, NULL, 0);
		HCF_CC_LOG(LEVEL_ERROR, "SSH session error message == %s", err_msg);

		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		close(sock);

		_sftp_msg = "generic error";
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'libssh2_userauth_list' failed.", libssh2_session_last_errno(session), err_msg);
		return;
	}
	HCF_CC_LOG(LEVEL_NOTICE, "Authentication methods available: %s", userauthlist);
	HCF_CC_TRACE(TransferFileViaSFTP, "The remote server supports the following authentication types:", userauthlist);

	/* Authenticate...*/
	if (strstr(userauthlist, "password")) {
		/* We could authenticate via password */
		HCF_CC_TRACE(TransferFileViaSFTP, "The authentication via password is supported: using the user-provided password!", "");
		if (libssh2_userauth_password(session, file.user.c_str(), file.password.c_str())) {
			HCF_CC_LOG(LEVEL_ERROR, "Authentication by password failed: SSH session errno == %d", libssh2_session_last_errno(session));
			char *err_msg;
			libssh2_session_last_error(session, &err_msg, NULL, 0);
			HCF_CC_LOG(LEVEL_ERROR, "SSH session error message == %s", err_msg);

			libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
			libssh2_session_free(session);
			close(sock);

			_sftp_msg = "authentication failure";
			HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'libssh2_userauth_password' failed.", libssh2_session_last_errno(session), err_msg);
			return;
		}
		/* Authentication by password was successful */
		HCF_CC_LOG(LEVEL_NOTICE, "Authentication by password was successful");
		HCF_CC_TRACE(TransferFileViaSFTP, "The following user is correctly authenticated via password:", file.user.c_str());
	}
	else if (strstr(userauthlist, "keyboard-interactive")) {
		/* We could authenticate via keyboard-interactive */
		HCF_CC_TRACE(TransferFileViaSFTP, "The keyboard-interactive authentication is supported: setting the password for the callback method!", "");

		/* First of all, set password for kbd_callback*/
		_password = file.password;

		if (libssh2_userauth_keyboard_interactive(session, file.user.c_str(), &kbd_callback)) {
			HCF_CC_LOG(LEVEL_ERROR, "Authentication by keyboard-interactive failed: SSH session errno == %d", libssh2_session_last_errno(session));
			char *err_msg;
			libssh2_session_last_error(session, &err_msg, NULL, 0);
			HCF_CC_LOG(LEVEL_ERROR, "SSH session error message == %s", err_msg);

			_password.clear(); // Clean password

			libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
			libssh2_session_free(session);
			close(sock);

			_sftp_msg = "authentication failure";
			HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'libssh2_userauth_keyboard_interactive' failed.", libssh2_session_last_errno(session), err_msg);
			return;
		}
		/* Authentication by keyboard-interactive was successful */
		HCF_CC_LOG(LEVEL_NOTICE, "Authentication by keyboard-interactive was successful");
		HCF_CC_TRACE(TransferFileViaSFTP, "The following user is correctly authenticated via keyboard-interactive authentication:", file.user.c_str());
		_password.clear(); // Clean password
	}
	else {
		/* No supported authentication methods found */
		HCF_CC_LOG(LEVEL_ERROR, "No supported authentication methods found!");

		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		close(sock);

		_sftp_msg = "authentication failure";
		HCF_CC_TRACE(TransferFileViaSFTPErr, "No supported authentication methods were found!", -1, _sftp_msg.c_str());
		return;
	}
	/* ...we are authenticated!*/
	/*****************************************/
	/*              SSH session              */
	/*****************  END  *****************/

	/***************** BEGIN *****************/
	/*              SFTP session             */
	/*****************************************/
	/* Open SFTP channel for the given SSH session */
	HCF_CC_TRACE(TransferFileViaSFTP, "Creating a new SFTP protocol session!", "");
	LIBSSH2_SFTP * sftp_session = libssh2_sftp_init(session);

	if (!sftp_session) {
		HCF_CC_LOG(LEVEL_ERROR, "Unable to open SFTP channel:");
		int sessionErrno = libssh2_session_last_errno(session);
		if( sessionErrno == LIBSSH2_ERROR_TIMEOUT) {
			HCF_CC_LOG(LEVEL_ERROR, "SSH session errno == LIBSSH2_ERROR_TIMEOUT");
		}
		else {
			HCF_CC_LOG(LEVEL_ERROR, "SSH session errno == %d", sessionErrno);
		}
		char *err_msg;
		libssh2_session_last_error(session, &err_msg, NULL, 0);
		HCF_CC_LOG(LEVEL_ERROR, "SSH session error message == %s", err_msg);

		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		close(sock);

		_sftp_msg = "generic error";
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'libssh2_sftp_init' failed.", libssh2_session_last_errno(session), err_msg);
		return;
	}
	/* libssh2_sftp_init() was successful */
	HCF_CC_LOG(LEVEL_NOTICE, "libssh2_sftp_init() was successful");

	/* Open remote file */
	HCF_CC_TRACE(TransferFileViaSFTP, "Opening the following destination file on the remote server:", file.remoteFile.c_str());
	LIBSSH2_SFTP_HANDLE * sftp_handle = libssh2_sftp_open(sftp_session, file.remoteFile.c_str(),
			LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
			LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);

	if (!sftp_handle) {
		HCF_CC_LOG(LEVEL_ERROR, "Unable to open file '%s' on remote system:", file.remoteFile.c_str());
		int sessionErrno = libssh2_session_last_errno(session);
		if( sessionErrno == LIBSSH2_ERROR_TIMEOUT) {
			HCF_CC_LOG(LEVEL_ERROR, "SSH session errno == LIBSSH2_ERROR_TIMEOUT");
		}
		else {
			HCF_CC_LOG(LEVEL_ERROR, "SSH session errno == %d", sessionErrno);
		}
		char *err_msg;
		libssh2_session_last_error(session, &err_msg, NULL, 0);
		HCF_CC_LOG(LEVEL_ERROR, "SSH session error message == %s", err_msg);
		HCF_CC_LOG(LEVEL_ERROR, "SFTP error == %lu", libssh2_sftp_last_error(sftp_session));

		libssh2_sftp_shutdown(sftp_session);
		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		close(sock);

		_sftp_msg = "unable to open remote file " + file.remoteFile;
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'libssh2_sftp_open' failed.", libssh2_session_last_errno(session), err_msg);
		return;
	}
	/* libssh2_sftp_open() was successful */
	HCF_CC_LOG(LEVEL_NOTICE, "libssh2_sftp_open() was successful, file '%s' is open on remote system", file.remoteFile.c_str());
	HCF_CC_TRACE(TransferFileViaSFTP, "The following destination file on the remote server was correctly opened!", file.remoteFile.c_str());
	/*****************************************/
	/*              SFTP session             */
	/*****************  END  *****************/

	/***************** BEGIN *****************/
	/*        Send file to remote host       */
	/*****************************************/
	/* Open local file for reading */
	HCF_CC_TRACE(TransferFileViaSFTP, "Opening the file to be transferred!", file.localFile.c_str());
	FILE * fs = fopen(file.localFile.c_str(), "r");
	if(!fs) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Unable to open local file '%s', errno == %d", file.localFile.c_str(), errno_save);

		libssh2_sftp_close_handle(sftp_handle);
		libssh2_sftp_shutdown(sftp_session);
		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		close(sock);

		_sftp_msg = "I/O error";
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'fopen' failed.", errno_save, _sftp_msg.c_str());
		return;
	}

	/* Send local file to the remote system */
	char mem[1024 * 100]= { 0 };
	size_t nread = 0;
	int rc1 = 0;
	int rc2 = 0;
	char *ptr = 0;

	HCF_CC_LOG(LEVEL_NOTICE, "libssh2_sftp_write() loop START");
	HCF_CC_TRACE(TransferFileViaSFTP, "Starting to transfer data from the following local file to the remote server!", file.localFile.c_str());
	do {
		nread = fread(mem, 1, sizeof(mem), fs);
		if (nread <= 0) {
			/* end of file */
			HCF_CC_LOG(LEVEL_NOTICE, "End of file '%s'", file.localFile.c_str());
			HCF_CC_TRACE(TransferFileViaSFTP, "No more data to be read from the following file:", file.localFile.c_str());
			break;
		}
		HCF_CC_TRACE(TransferFileViaSFTP, "The following number of bytes were read from the input file:", std::to_string(nread).c_str());
		ptr = mem;

		do {
			/* write data in a loop until we block */
			HCF_CC_TRACE(TransferFileViaSFTP, "Writing the read data to the following output file:", file.remoteFile.c_str());
			rc1 = libssh2_sftp_write(sftp_handle, ptr, nread);

			if(rc1 < 0) {
				HCF_CC_LOG(LEVEL_ERROR, "libssh2_sftp_write() error:");
				switch(rc1) {
				case LIBSSH2_ERROR_ALLOC :
					HCF_CC_LOG(LEVEL_ERROR, "return code == LIBSSH2_ERROR_ALLOC, SSH session errno == %d", libssh2_session_last_errno(session));
					break;
				case LIBSSH2_ERROR_SOCKET_SEND :
					HCF_CC_LOG(LEVEL_ERROR, "return code == LIBSSH2_ERROR_SOCKET_SEND, SSH session errno == %d", libssh2_session_last_errno(session));
					break;
				case LIBSSH2_ERROR_SOCKET_TIMEOUT :
					HCF_CC_LOG(LEVEL_ERROR, "return code == LIBSSH2_ERROR_SOCKET_TIMEOUT, SSH session errno == %d", libssh2_session_last_errno(session));
					break;
				case LIBSSH2_ERROR_SFTP_PROTOCOL :
					HCF_CC_LOG(LEVEL_ERROR, "return code == LIBSSH2_ERROR_SFTP_PROTOCOL, SSH session errno == %d", libssh2_session_last_errno(session));
					break;
				case LIBSSH2_ERROR_EAGAIN :
					HCF_CC_LOG(LEVEL_ERROR, "return code == LIBSSH2_ERROR_EAGAIN, SSH session errno == %d", libssh2_session_last_errno(session));
					break;
				case LIBSSH2_ERROR_TIMEOUT :
					HCF_CC_LOG(LEVEL_ERROR, "return code == LIBSSH2_ERROR_TIMEOUT, SSH session errno == %d", libssh2_session_last_errno(session));
					break;
				default :
					HCF_CC_LOG(LEVEL_ERROR, "return code == %d, SSH session errno == %d", rc1, libssh2_session_last_errno(session));
					break;
				}
				char *err_msg;
				libssh2_session_last_error(session, &err_msg, NULL, 0);
				HCF_CC_LOG(LEVEL_ERROR, "SSH session error message == %s", err_msg);
				HCF_CC_LOG(LEVEL_ERROR, "SFTP error == %lu", libssh2_sftp_last_error(sftp_session));
				HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'libssh2_sftp_write' failed.", libssh2_sftp_last_error(sftp_session), err_msg);
			break;
			}
			ptr += rc1;
			nread -= rc1;
		} while (nread);
	} while (rc1 > 0);
	HCF_CC_LOG(LEVEL_NOTICE, "libssh2_sftp_write() loop END, last rc == %d", rc1);
	HCF_CC_TRACE(TransferFileViaSFTP, "Data has been transferred to the following file on remote server!", file.remoteFile.c_str());
	/*****************************************/
	/*        Send file to remote host       */
	/*****************  END  *****************/

	/***************** BEGIN *****************/
	/*      Shutdown and free resources      */
	/*****************************************/
	fclose(fs);

	HCF_CC_LOG(LEVEL_NOTICE, "Closing remote file...");
	HCF_CC_TRACE(TransferFileViaSFTP, "Closing the following file on the remote server:", file.remoteFile.c_str());
	rc2 = libssh2_sftp_close_handle(sftp_handle);
	if( rc2 < 0 ) {
		HCF_CC_LOG(LEVEL_ERROR, "libssh2_sftp_close_handle() failed for remote file '%s':", file.remoteFile.c_str());
		if( rc2  == LIBSSH2_ERROR_TIMEOUT) {
			HCF_CC_LOG(LEVEL_ERROR, "return code == LIBSSH2_ERROR_TIMEOUT, SSH session errno == %d", libssh2_session_last_errno(session));
		}
		else {
			HCF_CC_LOG(LEVEL_ERROR, "return code == %d, SSH session errno == %d", rc2, libssh2_session_last_errno(session));
		}
		char *err_msg;
		libssh2_session_last_error(session, &err_msg, NULL, 0);
		HCF_CC_LOG(LEVEL_ERROR, "SSH session error message == %s", err_msg);
		HCF_CC_LOG(LEVEL_ERROR, "SFTP error == %lu", libssh2_sftp_last_error(sftp_session));
		HCF_CC_TRACE(TransferFileViaSFTPErr, "Call 'libssh2_sftp_write' failed.", libssh2_sftp_last_error(sftp_session), err_msg);
	}
	HCF_CC_LOG(LEVEL_NOTICE, "libssh2_sftp_close_handle() END");

	HCF_CC_TRACE(TransferFileViaSFTP, "Shutting down the SFTP protocol session!", "");
	libssh2_sftp_shutdown(sftp_session);

	HCF_CC_TRACE(TransferFileViaSFTP, "Shutting down the SSH2 protocol session!", "");
	libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
	libssh2_session_free(session);

	close(sock);
	/*****************************************/
	/*      Shutdown and free resources      */
	/*****************  END  *****************/

	_fileSent = FILE_TRANSFER_OK;
	_sftp_msg = "file transfer completed";
	HCF_CC_LOG(LEVEL_NOTICE, "File transfer completed!");
	HCF_CC_TRACE(TransferFileViaSFTP, "File transfer over SFTP correctly completed!", "");
	return;
}

void __TSL_CLASS_NAME__::wait_sftp_transfer_end (int & fileSent, std::string & sftp_msg) {
	HCF_CC_TRACE_FUNCTION;
	struct timespec abstime;
	abstime.tv_nsec = 0;
	abstime.tv_sec = ::time(0) + (5 * 60); // Now + 5 minutes

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _transfer_end_condition_mutex);
		HCF_CC_TRACE(WaitSFTPTransferEnd, "Waiting for some seconds the termination of the SFTP transfer procedure!");
		if (_transfer_end_condition.wait(_transfer_end_condition_mutex, &abstime) < 0) {
			fileSent = FILE_TRANSFER_END_WAIT_ERROR;
			const int errno_save = errno;
			if(errno_save == ETIMEDOUT) {
				sftp_msg = "timeout expired waiting for file transfer end";
				HCF_CC_LOG(LEVEL_WARNING, "Call '_transfer_end_condition.wait' returned -1, errno == ETIMEDOUT");
				HCF_CC_TRACE(WaitSFTPTransferEndErr, "A timeout occurred while trying to wait the end of the SFTP transfer operation!", errno_save, sftp_msg.c_str());
			}
			else {
				sftp_msg = "generic error";
				HCF_CC_LOG(LEVEL_ERROR, "Call '_transfer_end_condition.wait' returned -1, errno == %d", errno_save);
				HCF_CC_TRACE(WaitSFTPTransferEndErr, "Generic error occurred while trying to wait the end of the SFTP transfer operation!", errno_save, sftp_msg.c_str());
			}
		}
		else {
			fileSent = _fileSent;
			sftp_msg = _sftp_msg;
			HCF_CC_LOG(LEVEL_NOTICE, "_transfer_end_condition signaled, fileSent == %d, sftp_msg == %s", fileSent, sftp_msg.c_str());
			HCF_CC_TRACE(WaitSFTPTransferEnd, "The SFTP transfer operation was correctly completed!");
		}
		_sftp_msg.clear();
	TSL_CRITICAL_SECTION_END;
}

void __TSL_CLASS_NAME__::signal_sftp_transfer_end () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_INFO, "Signaling _transfer_end_condition");
	HCF_CC_TRACE(SignalSFTPTransferEnd, "Signaling the stop condition for the SFTP transfer!");
	_transfer_end_condition.signal();
}

HCF_NS_HCF_SRV_END
