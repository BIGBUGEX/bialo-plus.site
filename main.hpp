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

#include <fcgiapp.h>


struct fcgi_request_c {
	FCGX_Stream *in, *out, *err;
	FCGX_ParamArray envp;
	std::map<std::string,std::string> env;
	std::string user, password;
	
	int Accept();
};

#endif // MAIN_HPP_INCLUDED

