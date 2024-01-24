#include "bialo-plus.hpp"

#define STATIC_CONTENT_MAXAGE 3600

ui::menu_item_c ui::navbar_c::items[] = {
	{ "/", u8"бяло+" },
	{ "/gallery", u8"галерия" },
	{ "/advertising", u8"реклама" },
	{ "/sound", u8"озвучаване" },
	{ "/contacts", u8"контакти" }
};

void ui::navbar_c::write( std::ostream &os ) {
	os << "<nav class=\"navbar navbar-expand-sm bg-secondary navbar-dark\">\n";
	os << " <a class=\"navbar-brand\" href=\"" << items[0].address << "\">" << items[0].text << "</a>\n";
	os << " <ul class=\"navbar-nav\">\n";

	for( int i = 1; i < ui::document_c::MENU_SIZE; i++ )
	os << "   <li class=\"nav-item" << ( i == select ? " active\">": "\">" )
	<< "<a class=\"nav-link\" href=\"" << items[i].address << "\">" << items[i].text << "</a></li>\n";

	os << " </ul>\n</nav>\n";
}

void ui::document_c::cont_begin( std::ostream &os ) {
	os << u8R"(<!DOCTYPE HTML>
<html lang="bg">
<head>
 <meta name="viewport" content="width=device-width, initial-scale=1">
 <meta name="keywords" content="варна, реклама, озвучаване, галерия"/>
 <!-- Global site tag (gtag.js) - Google Analytics -->
 <script async src="https://www.googletagmanager.com/gtag/js?id=UA-45306608-2"></script>
 <script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());

  gtag('config', 'UA-45306608-2');
 </script>

 <title>)" << title << u8R"(</title>

<link rel="icon" type="image/x-icon" href="/assets/favicon.ico">
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/css/bootstrap.min.css">
 <script src="https://cdn.jsdelivr.net/npm/jquery@3.7.1/dist/jquery.slim.min.js"></script>
 <script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.1/dist/umd/popper.min.js"></script>
 <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/js/bootstrap.bundle.min.js"></script>

</head>
<body>
<img class="img-fluid" src="/assets/ui/site.jpg" style="width:100%" alt="header">
)" << nav << "<div class=\"container\" style=\"margin-top:30px; min-height:400px\">\n";
}
void ui::document_c::cont_end( std::ostream &os ) {
	os << u8R"(</div>
<div class="container-fluid bg-secondary text-white p-5" style="min-height:200px">
 <div class="container">
  <p> Powered by <a class="text-light" href="https://github.com/BIGBUGEX/bialo-plus.site" target="_blank">C++</a> </p>
 </div>
</div>
</body>
</html>
)";
}


response_c* bialo_plus_c::cmd_static_c::dispatch( fcgi_request_c &req, std::string_view match, std::string_view extra ) {
	response_c *res;
	
	if( !extra.size() ) {
		const std::string &rm = req.env["REQUEST_METHOD"];
		
		res = (response_c*)this;
		
		if( rm == "POST" || rm == "PUT" || rm == "DELETE" ) {
			std::stringstream ss;
			ui::document_c doc;
			
			doc.title = "Unauthorized";
			doc.nav.select = ui::document_c::MENU_NONE;
			ss << doc.open << u8"<p>Нямате права за промени в конфигурацията.</p>\n" << doc.close;
			res = new dynamic_c();
			res->mime_type = "text/html; charset=utf-8";
			res->status = 401;
			res->content = ss.str();
		}
	}else {
		command_c *root = up;
		
		while( root->up ) root = root->up;
		
		res = root->dispatch( req, match, extra );
	}
	
	return res;
}

bialo_plus_c::bialo_plus_c() {
	/* home */{
		std::stringstream ss;
		ui::document_c doc;
		
		doc.title = u8"бяло+";
		doc.nav.select = ui::document_c::MENU_HOME;
		ss << doc.open <<
u8R"(<div class="row mb-3">
 <div class="col"><img src="/assets/ui/art1.jpg" width="100%" class="rounded"></div>
 <div class="col"><img src="/assets/ui/art2.jpg" width="100%" class="rounded"></div>
 <div class="col"><img src="/assets/ui/gal1.jpg" width="100%" class="rounded"></div>
</div>
<p>  Ателие галерия Бяло плюс е малко, но активно пространство за създаване и представяне на авторски произведения на изящното и приложно изкуство. Намира се в района на Лятно кино Тракия, в близост до новоизградения храм „Свети Прокопий Варненски”. Като довършителен елемент във визуализацията на творбите предлагаме и рамкиране.</p>
<p> Тук намират реализация нестандартни и иновативни дизайнерски решения в сферата на печатната, интериорна и екстериорна реклама.</p>
<p> Звукозапис и обработка на звука.</p>
)" << doc.close;
		home = new cmd_static_c();
		home->path = "/";
		home->mime_type = "text/html; charset=utf-8";
		home->content = ss.str();
		reg( home );
	}
	/* gallery */{
		std::stringstream ss;
		ui::document_c doc;
		
		doc.title = u8"бяло+(галерия)";
		doc.nav.select = ui::document_c::MENU_GALLERY;
		ss << doc.open <<
u8R"(<video class="rounded float-left mr-4" width="60%" controls>
 <source src="/assets/Otkrivane_izlojba_26_XII_21.mp4">
</video>
<p> Основна дейност на галерията е представяне на различни автори от Варна и региона, утвърдени и начинаещи, работещи в различни сфери на визуалните изкуства. Идеята е да се осъществи пряк контакт между интересите на жителите в района и творческата диаспора, създаваща артистичния продукт.</p>
<p> За нас е особено важно да дадем възможност творческият продукт, /картини, принтове, графика, пластики, дизайнерски проекти, и други артистични и музикални идеи/, да намерят своя адресат. Като допълнение към визуалното представяне, влючваме довършителни работи, свързани с рамкиране и оформление на работите.</p>
<p> Ателие – галерия „Бяло плюс” предлага и рекламно обезпечаване на призведения и събития като изработва печатната и пространствена реклама.</p>
)" << doc.close;
		gallery = new cmd_static_c();
		gallery->path = "/gallery";
		gallery->mime_type = "text/html; charset=utf-8";
		gallery->content = ss.str();
		reg( gallery );
	}
	/* advertising */{
		std::stringstream ss;
		ui::document_c doc;
		
		doc.title = u8"бяло+(реклама)";
		doc.nav.select = ui::document_c::MENU_ADVERTISING;
		ss << doc.open <<
u8R"(<p> Дизайн, предпечат и реклама /печатна реклама – визитки, флаери, каталози, папки/.</p>
<p> Лого, фирмен дизайн и брандиране.</p>
<p> Интериорен и екстериорен дизайн, фасадни решения, изрязване на надписи от PVC фолио, пълноцветен печат върху самозалепващо фолио, перфофолио и винил.</p>
)" << doc.close;
		advertising = new cmd_static_c();
		advertising->path = "/advertising";
		advertising->mime_type = "text/html; charset=utf-8";
		advertising->content = ss.str();
		reg( advertising );
	}
	/* sound */{
		std::stringstream ss;
		ui::document_c doc;
		
		doc.title = u8"бяло+(озвучаване)";
		doc.nav.select = ui::document_c::MENU_SOUND;
		ss << doc.open <<
u8R"(<p>Важна част от дейността на ателие галерия „Бяло плюс” е обработката на звук, звукозапис и озвучаване на събития.</p>
<ul>
<li><strong>Звукозапис</strong> – Звукозапис на живо до 24 канала и обработка на звука в цифрова среда, или с аналогова периферия.</li>
<li><strong>Озвучаване</strong> – Озвучаване в малки зали – до 600 места, класически концерти, клубно озвучаване, откриване на изложби, чествания и други събития.</li>
<li>Обезпечаване на проявите от професионален тонрежисьор с качествена техника и специализирани микрофони.</li>
</ul>
)" << doc.close;
		sound = new cmd_static_c();
		sound->path = "/sound";
		sound->mime_type = "text/html; charset=utf-8";
		sound->content = ss.str();
		reg( sound );
	}
	/* contacts */{
		std::stringstream ss;
		ui::document_c doc;
		
		doc.title = u8"бяло+(контакти)";
		doc.nav.select = ui::document_c::MENU_CONTACTS;
		ss << doc.open <<
u8R"(<p>гр. Варна ул. „Гаврил Кръстевич“ № 5 /вход през вътрешен двор/.</p>
<p> Контакт за реклама/галерия - тел: 0886-310-080 </p>
<p> Контакт за озвучаване - тел: 0889-626-523 </p>
)" << doc.close;
		contacts = new cmd_static_c();
		contacts->path = "/contacts";
		contacts->mime_type = "text/html; charset=utf-8";
		contacts->content = ss.str();
		reg( contacts );
	}
}

response_c* bialo_plus_c::dispatch( fcgi_request_c &req, std::string_view match, std::string_view extra ) {
	dynamic_c *res = new dynamic_c();
	std::stringstream ss;
	ui::document_c doc;
	
	doc.title = u8"бяло+(404)";
	doc.nav.select = ui::document_c::MENU_NONE;
	ss << doc.open << u8"<p> Няма такъв ресурс \"<a href=\"" << match << "\">" << match << "</a>" << extra << "\".</p>\n" << doc.close;
	res->status = 404;
	res->mime_type = "text/html; charset=utf-8";
	res->content = ss.str();
	
	return res;
}


