#include "handler.hpp"

void response_c::readfile( const std::string &s ) {
	std::ifstream ifs( s, std::ifstream::binary );
	std::stringstream sbuf;
	int len;
	char buf[0x1000];

	ifs.seekg( 0, ifs.end );
	len = ifs.tellg();
	ifs.seekg( 0, ifs.beg );

	while( len > 0x1000 ) {
		ifs.read( buf, 0x1000 );
		sbuf.write( buf, 0x1000 );
		len -= 0x1000;
	}

	if( len ) {
		ifs.read( buf, len );
		sbuf.write( buf, len );
	}

	ifs.close();
	content = sbuf.str();
}

void command_c::reg( command_c *sub ) {
	unsigned i;
	
	for( i = 0; i < dir.size(); i++ )
	if( sub->path < dir[i]->path ) {
		dir.insert( dir.begin() + i, sub );
		
		break;
	}
	
	if( i == dir.size() ) dir.push_back( sub );
	
	sub->up = this;
}

void command_c::unreg( command_c *sub ) {
	for( unsigned i = 0; i < dir.size(); i++ )
	if( sub == dir[i] ) {
		dir.erase( dir.begin() + i );
		sub->up = NULL;
		
		break;
	}
}

command_c::~command_c() {
	unsigned i;
	
	for( i = 0; i < dir.size(); i++ ) {
		dir[i]->up = NULL;
		delete dir[i];
	}
	
	if( up ) up->unreg( this );
}

static
int find_last_match( const std::vector<command_c*> &v, const std::string &uri, size_t urio ) {
	int ib = 0, ie = (int)v.size(), i, ires = -1;
	std::string_view vuri( uri.data() + urio, uri.size() - urio );
	
	while( ie - ib > 0 ) {
		i = ( ib + ie ) >> 1;
		
		std::string_view ven( v[i]->path.data(), v[i]->path.size() );
		
		if( vuri < ven ) {
			ie = i;
		}else {
			ib = i + 1;
			ires = i;
		}
	}
	
	for( ; ires >= 0; ires-- )
	if( v[ires]->path.size() <= vuri.size() ) {
		std::string_view ven( v[ires]->path.data(), v[ires]->path.size() ), vseg( vuri.data(), v[ires]->path.size() );
		
		if( ven == vseg ) break;
		else if( ven[0] < vseg[0] ) {
			ires = -1;
			
			break;
		}
	}
	
	return ires;
}

search_result_s command_root_c::search( const std::string &cmd ) {
	command_c *_cmd = (command_c*)this;
	size_t soff = 0;
	int i;
	search_result_s rval;

	while( soff < cmd.size() ) {
		i = find_last_match( _cmd->dir, cmd, soff );
		
		if( i >= 0 ) {
			soff += _cmd->dir[i]->path.size();
			_cmd = _cmd->dir[i];
		}else {
			break;
		}
	}

	rval.match = std::string_view( cmd.data(), soff );
	rval.extra = std::string_view( cmd.data() + soff, cmd.size() - soff );
	rval.route = _cmd;

	return rval;
}

response_c* command_root_c::dispatch( fcgi_request_c &req ) {
	search_result_s query = search( req.env["REQUEST_URI"] );
	
	return query.route->dispatch( req, query.match, query.extra );
}

