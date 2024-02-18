#include "main.hpp"
#include "bialo-plus.hpp"


void fcgi_request_c::post_c::multipart_form_data( fcgi_request_c &req ) {
}
void fcgi_request_c::post_c::form_urlencoded( fcgi_request_c &req ) {
}
void fcgi_request_c::post_c::enc_json( fcgi_request_c &req ) {
}
void fcgi_request_c::post_c::init( fcgi_request_c &req ) {
	const std::string &cl = req.env["CONTENT_LENGTH"], &ct = req.env["CONTENT_TYPE"];
	std::string_view mpfd = "multipart/form-data", fue = "application/x-www-form-urlencoded", json = "application/json";
	size_t sz = 0;
	
	if( cl.size() ) try {
		sz = std::stoull( cl );
		post_data.resize( sz );
		FCGX_GetStr( post_data.data(), sz, req.in );
	} catch( const std::exception &e ) {
		for( auto _i = req.env.begin(); _i != req.env.end(); _i++ ) {
			FCGX_FPrintF( req.err, "%s = \"%s\";", _i->first.c_str(), _i->second.c_str() );
		}
		
		FCGX_FPrintF( req.err, "\n" );
	}
	
	if( ct == "" ) {
	}else if( ct.size() >= mpfd.size() && mpfd == std::string_view( ct.data(), mpfd.size() ) ) {
		multipart_form_data( req );
	}else if( ct.size() >= fue.size() && fue == std::string_view( ct.data(), fue.size() ) ) {
		form_urlencoded( req );
	}else if( ct.size() >= json.size() && json == std::string_view( ct.data(), json.size() ) ) {
		enc_json( req );
	}else {
		for( auto _i = req.env.begin(); _i != req.env.end(); _i++ ) {
			FCGX_FPrintF( req.err, "%s = \"%s\";", _i->first.c_str(), _i->second.c_str() );
		}
		
		FCGX_FPrintF( req.err, "\n" );
	}
}
void fcgi_request_c::post_c::clear() {
	fields.clear();
	files.clear();
	post_data.clear();
}

fcgi_request_c::fcgi_request_c() {
	in = out = err = NULL; envp = NULL;
}

int fcgi_request_c::Accept() {
	post.clear();
	env.clear();
	user = "";
	password = "";
	
	if( out != NULL ) FCGX_Finish();

	int res = FCGX_Accept( &in, &out, &err, &envp );
	std::string cook;

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
		
		post.init( *this );
	}

	return res;
}

void terminate( int sig ) {
	FCGX_ShutdownPending();
	shutdown( 0, SHUT_RD );
}

int main() {
	fcgi_request_c req;
	bialo_plus_c bialo_plus;
	
	signal( SIGTERM, &terminate );

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
	}

	return 0;
}
