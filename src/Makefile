

simple: main.cc clock.cc glade/default_layout.glade 
	g++ main.cc -std=c++11  -o simple `pkg-config gtkmm-3.0 --cflags --libs`
	g++ custom_widget.cc custom_widget_glade.c -fPIC -fpic -shared -std=c++11  -o libcustomwidget.so  `pkg-config gtkmm-3.0 --cflags --libs`


clean:
	rm -rf *.o *.so simple simple.exe *~ autom4te.cache
