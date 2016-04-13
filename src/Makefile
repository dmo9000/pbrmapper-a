all: libcustomwidgetsglade simple 

libcustomwidgetsglade: custom_widget.cc custom_widget_glade.c custom_widget.h custom_widget_glade.h custom_widget_pure.h
	g++ custom_widget.cc custom_widget_glade.c -g -ggdb -fPIC -shared -std=c++11  -o libcustomwidgetsglade.so  `pkg-config gtkmm-3.0 --cflags --libs`
	( [[ `./config.guess` =~ cygwin$$ ]] && \
	ln -sf libcustomwidgetsglade.so libcustomwidgetsglade.dll ) || echo

# Probably needed for glade, but not currently working on Cygwin I don't think
#	[[ `./config.guess` =~ cygwin$ ]] && \
#			ln -sf libcustomwidgetsglade.so cygcustomwidgetsglade.dll

simple: simple.cc graphnode.cc glade/default_layout.glade libcustomwidgetsglade 
	g++ graphnode.cc simple.cc custom_widget_glade.c -std=c++11 -g -ggdb -o simple `pkg-config gtkmm-3.0 --cflags --libs` -L. -lcustomwidgetsglade


clean:
	rm -rf *.o *.so *.dll simple simple.exe *~ autom4te.cache 
	find . -name "*~" -print -exec rm -f {} \;
