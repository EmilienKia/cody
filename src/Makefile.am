## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	$(WX_CXXFLAGS) \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	 -Wall \
	 -g \
	 $(WX_CPPFLAGS)

bin_PROGRAMS = cody

cody_SOURCES = \
	main-frame.cpp \
	main-frame.hpp \
	text-document.cpp \
	text-document.hpp \
	text-frame.cpp \
	text-frame.hpp \
	cody-app.cpp \
	cody-app.hpp \
	bookmark.cpp \
	bookmark.hpp \
	markbar.cpp \
	markbar.hpp

cody_LDFLAGS = 

cody_LDADD = \
	 \
	$(WX_LIBS)

