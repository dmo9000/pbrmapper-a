all: simple 

simple: simple.cc clock.cc glade/default_layout.glade 
	g++ custom_widget.cc custom_widget_glade.c -g -ggdb -fPIC -shared -std=c++11  -o libcustomwidgetsglade.so  `pkg-config gtkmm-3.0 --cflags --libs` 
	g++ simple.cc custom_widget_glade.c -std=c++11 -g -ggdb -o simple `pkg-config gtkmm-3.0 --cflags --libs` -L. -lcustomwidgetsglade

#custom_widget_example:
#	g++ -g -ggdb -std=c++11 custom_widget_example.cc custom_widget.cc custom_widget_glade.c -o custom_widget_example -fPIC `pkg-config --cflags --libs gtkmm-3.0` -L. -lcustomwidgetsglade	



clean:
	rm -rf *.o *.so simple simple.exe *~ autom4te.cache custom_widget_example
	find . -name "*~" -print -exec rm -f {} \;
