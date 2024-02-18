#ifndef MAIN_HPP_INCLUDED
#define MAIN_HPP_INCLUDED

#include <ctime>
#include <cstdlib>

#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>
#include <cstddef>
#include <cstdio>
#include <csignal>
#include <sys/socket.h>

#include <fcgiapp.h>


struct fcgi_request_c {
	FCGX_Stream *in, *out, *err;
	FCGX_ParamArray envp;
	
	std::map<std::string,std::string> env;
	std::string user, password;
	
	struct post_c {
		struct file_c {
			std::string_view name, mime_type, data;
		};
		
		std::map<std::string_view,std::string_view> fields;
		std::map<std::string_view,file_c> files;
		std::string post_data;
		
		void multipart_form_data( fcgi_request_c &req );
		void form_urlencoded( fcgi_request_c &req );
		void enc_json( fcgi_request_c &req );
		
		void init( fcgi_request_c &req );
		
		void clear();
	} post;
	
	fcgi_request_c();
	
	int Accept();
};

#endif // MAIN_HPP_INCLUDED

