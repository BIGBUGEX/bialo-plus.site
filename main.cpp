#include "main.hpp"
#include "bialo-plus.hpp"


int fcgi_request_c::Accept() {
	int res = FCGX_Accept( &in, &out, &err, &envp );
	std::string cook;

	env.clear();
	user = "";
	password = "";

	if( res >= 0 ) {
		for( char **p = envp; *p; p++ )
		for( int i = 0; p[0][i]; i++ )
		if( p[0][i] == '=' ) {
			env[std::string(*p,(size_t)i)] = std::string( *p + i + 1 );

			break;
		}

		cook = env["HTTP_COOKIE"];

		size_t i = cook.find( "UID" ), j;

		if( i != std::string::npos ) {
			for( i += 3; i < cook.size(); )
			if( cook[i++] == '=' ) break;

			while( i < cook.size() && cook[i] == ' ' ) i++;

			for( j = i; j < cook.size(); j++ )
			if( cook[j] == '@' ) {
				user = std::string( cook, i, j++ );
				break;
			}

			for( i = j; i < cook.size(); i++ )
			if( cook[i] == ';' ) break;

			password = std::string( cook, j, i );
		}
	}

	return res;
}

int main() {
	fcgi_request_c req;
	bialo_plus_c bialo_plus;

	while (req.Accept() >= 0) {
		response_c *_res = ( (handler_c*)&bialo_plus )->dispatch( req );
		tm *_t = gmtime( &_res->date );
		time_t msgtime;
		char modified[80], date[80];

		strftime( modified, 80, "%a, %d %b %G %T GMT", _t );
		msgtime = time( 0 );
		_t = gmtime( &msgtime );
		strftime( date, 80, "%a, %d %b %G %T GMT", _t );
		FCGX_FPrintF(req.out, "Status: %i\r\nContent-Type: %s\r\nContent-Length: %u\r\nDate: %s\r\nLast-Modified: %s\r\n", _res ->status, _res ->mime_type.c_str(), _res ->content.size(), date, modified );

		if( _res ->max_age )
		FCGX_FPrintF( req.out, "Cache-Control: max-age=%u\r\n", _res ->max_age );

		FCGX_FPrintF( req.out, "\r\n" );

		if( req.env["REQUEST_METHOD"] != "HEAD" )
		FCGX_PutStr( _res ->content.c_str(), _res ->content.size(), req.out );

		_res ->release();

		//std::cout << buf << std::endl;

		/*for( std::map<std::string,std::string>::const_iterator _i = req.env.begin(); _i != req.env.end(); _i++ ) {
			FCGX_FPrintF( req.out, "%s = \"%s\"\n", _i ->first.c_str(), _i ->second.c_str() );
		}*/
	}

	return 0;
}
