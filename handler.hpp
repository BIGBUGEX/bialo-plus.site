#ifndef HANDLER_HPP_INCLUDED
#define HANDLER_HPP_INCLUDED

#include "main.hpp"

struct response_c {
	int status;
	unsigned max_age;
	time_t date;
	std::string mime_type, content;

	response_c() {
		date = time( NULL );
		status = 200;
		max_age = 0;
	}
	
	void readfile( const std::string &s );

	virtual void release() = 0;
	virtual ~response_c() {}
};

struct handler_c {
	virtual response_c* dispatch( fcgi_request_c &req ) = 0;
};

struct command_c {
	std::string path;
	std::vector<command_c*> dir;
	command_c *up;

	command_c() { up = NULL; }
	void reg( command_c *sub );
	void unreg( command_c *sub );
	
	template <class T>
	T* downcast() {
		return dynamic_cast<T*>( this );
	}

	virtual response_c* dispatch( fcgi_request_c &req, std::string_view match, std::string_view extra ) = 0;
	virtual ~command_c();
};

struct search_result_s {
	std::string_view match, extra;
	command_c *route;
};

struct command_root_c: handler_c, command_c {
	command_root_c() {}
	
	search_result_s search( const std::string &cmd );
	virtual response_c* dispatch( fcgi_request_c &req );
};

#endif // HANDLER_HPP_INCLUDED

