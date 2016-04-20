all: libcustomwidgetsglade simple 

libcustomwidgetsglade: geglio.c custom_widget.cc graphnode.cc custom_widget_glade.c custom_widget.h custom_widget_glade.h custom_widget_pure.h
	gcc -c geglio.c -fPIC `pkg-config glib-2.0 --cflags` `pkg-config gegl-0.2 --cflags` 
	g++ geglio.o graphnode.cc custom_widget.cc custom_widget_glade.c -g -ggdb -fPIC -shared -std=c++11  -o libcustomwidgetsglade.so  `pkg-config gtkmm-3.0 --cflags --libs` `pkg-config gegl-0.2 --libs` 
	( [[ `./config.guess` =~ cygwin$$ ]] && \
	ln -sf libcustomwidgetsglade.so libcustomwidgetsglade.dll ) || echo

simple: simple.cc glade/default_layout.glade libcustomwidgetsglade 
	g++ simple.cc custom_widget_glade.c -std=c++11 -g -ggdb -o simple `pkg-config gtkmm-3.0 --cflags --libs` `pkg-config gegl-0.2 --libs` `pkg-config glib-2.0 --cflags` -L. -lcustomwidgetsglade 


clean:
	rm -rf *.o *.so *.dll simple simple.exe *~ autom4te.cache 
	find . -name "*~" -print -exec rm -f {} \;
