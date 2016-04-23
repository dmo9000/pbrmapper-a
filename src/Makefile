LIB_SRC_C=geglio.c custom_widget.h custom_widget_glade.h custom_widget_pure.h
LIB_OBJ_C=geglio.o  
LIB_SRC_CPP=graphnode.cc custom_widget.cc custom_widget_glade.c
LIB_OBJ_CPP=graphnode.o custom_widget.o custom_widget_glade.o

MAIN_SRC_C=custom_widget_glade.c
MAIN_OBJ_C=custom_widget_glade.o

MAIN_SRC_CPP=simple.cc xmlio.cc
MAIN_OBJ_CPP=simple.o xmlio.o

CC=gcc
CXX=g++

LIB_CFLAGS:= $(shell pkg-config glib-2.0 gegl-0.2 --cflags)
LIB_CPPFLAGS:= $(shell pkg-config glib-2.0 gegl-0.2 gtkmm-3.0 --cflags)
LIB_LDFLAGS:= $(shell pkg-config glib-2.0 gegl-0.2 gtkmm-3.0 --libs)

all: libcustomwidgetsglade.so custom_widget_glade.o simple 

geglio.o: geglio.c
	$(CC) -c $^ $(LIB_CFLAGS) -fPIC -g -ggdb

custom_widget_glade.o: custom_widget_glade.c
	$(CXX) -c $^ -o $@ `pkg-config gtkmm-3.0 gegl-0.2 --cflags` -std=c++11 -fPIC -g -ggdb
 
%.o:	%.cc
	$(CXX) -c $^ -o $@ $(LIB_CPPFLAGS) -std=c++11 -fPIC -g -ggdb 

all: libcustomwidgetsglade.so simple 


libcustomwidgetsglade.so: $(LIB_OBJ_C) $(LIB_OBJ_CPP)  
	g++  $(LIB_OBJ_C) $(LIB_OBJ_CPP) -g -ggdb -fPIC -shared -std=c++11  -o libcustomwidgetsglade.so  `pkg-config gtkmm-3.0 --cflags --libs` `pkg-config gegl-0.2 --libs` 
	( [[ `./config.guess` =~ cygwin$$ ]] && \
	ln -sf libcustomwidgetsglade.so libcustomwidgetsglade.dll ) || echo

simple: $(MAIN_SRC_CPP) glade/default_layout.glade libcustomwidgetsglade.so 
	g++ $(MAIN_SRC_CPP) custom_widget_glade.c -std=c++11 -g -ggdb -o simple `pkg-config gtkmm-3.0 gegl-0.2 --cflags --libs`  `pkg-config glib-2.0 --cflags` -L. -lcustomwidgetsglade `pkg-config --cflags --libs libxml-2.0` 


clean:
	rm -rf *.o *.so *.dll simple simple.exe *~ autom4te.cache testfile.xml 
	find . -name "*~" -print -exec rm -f {} \;
