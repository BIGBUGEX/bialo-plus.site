#ifndef BIALO_BLUS_HPP_INCLUDED
#define BIALO_PLUS_HPP_INCLUDED

#include "handler.hpp"


struct bialo_plus_c: command_root_c {
	struct dynamic_c: response_c {
		virtual void release() { delete this; }
	};
	struct static_c: response_c {
		virtual void release() {}
	};
	
	struct cmd_static_c: command_c, static_c {
		virtual response_c* dispatch( fcgi_request_c &req, std::string_view match, std::string_view extra );
	};
	
	cmd_static_c *home, *gallery, *advertising, *sound, *contacts;
	
	bialo_plus_c();
	
	virtual response_c* dispatch( fcgi_request_c &req, std::string_view match, std::string_view extra );
};



namespace ui {

struct element_c {
	virtual void write( std::ostream &os ) = 0;
};

struct container_c {
	struct open_c: element_c {
		virtual void write( std::ostream &os ) { ( (container_c*)( (char*)this - offsetof( container_c, open ) ) ) -> cont_begin( os ); }
	} open;
	struct close_c: element_c {
		virtual void write( std::ostream &os ) { ( (container_c*)( (char*)this - offsetof( container_c, close ) ) ) -> cont_end( os ); }
	} close;
	virtual void cont_begin( std::ostream &os ) = 0;
	virtual void cont_end( std::ostream &os ) = 0;
};


__inline std::ostream& operator<<( std::ostream &os, element_c &e ) {
	e.write( os );

	return os;
}


struct menu_item_c {
	std::string address, text;

	menu_item_c() {}
	menu_item_c( const std::string &a, const std::string &t ): address(a), text(t) {}
};

struct navbar_c: element_c {
	static menu_item_c items[];
	int select;

	virtual void write( std::ostream &os );
};

struct document_c: container_c {
	bool badmin;
	std::string title;
	navbar_c nav;

	enum {
		MENU_NONE = -1,
		MENU_HOME,
		MENU_GALLERY,
		MENU_ADVERTISING,
		MENU_SOUND,
		MENU_CONTACTS,
		MENU_SIZE
	};

	document_c() {}

	virtual void cont_begin( std::ostream &os );
	virtual void cont_end( std::ostream &os );
};

}

#endif

