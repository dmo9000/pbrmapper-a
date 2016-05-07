#include <gtkmm.h>
#include <iostream>
#include <assert.h>
#include "custom_widget.h"
#include "custom_widget_glade.h"
#include "geglio.h"
#include "xmlio.h"
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#define MY_ENCODING "ISO-8859-1"
Gtk::Window* pMainWindow = nullptr;
CustomWidget *pCustomWidget = NULL;
Gtk::Statusbar* pStatusBar = nullptr;
Gtk::Viewport *pViewPort = nullptr;
Gtk::ScrolledWindow *myScrolledWindow = nullptr;

static void on_new_clicked();
static void on_open_clicked();
static void on_save_clicked();
static void on_saveas_clicked();
void Set_MainWindow_Title(std::string);

static
void on_button_clicked()
{
    if(pMainWindow)
        pMainWindow->hide(); //hide() will cause main::run() to end.
}

static
void on_new_clicked()
{
		fprintf(stderr, "on_new_clicked()\n");
		fflush(NULL);
		pCustomWidget->ClearCanvas();
		Set_MainWindow_Title("untitled");
}

static
void on_open_clicked()
{
		fprintf(stderr, "on_open_clicked()\n");
		fflush(NULL);
		pCustomWidget->ClearCanvas();
		pCustomWidget->run_file_chooser();
	  if (XML_Load(pCustomWidget->GetFilename())) {
			pCustomWidget->SetBackingStoreEnabled(true);
			Set_MainWindow_Title(pCustomWidget->GetFilename());
			pCustomWidget->SetDirty(false);
			}

		fflush(NULL);
}

static
void on_save_clicked()
{

	if (!pCustomWidget->GetBackingStoreEnabled()) {
			on_saveas_clicked();	
			return;
	}

  if (XML_Save(pCustomWidget->GetFilename())) {
			pCustomWidget->SetBackingStoreEnabled(true);
			pCustomWidget->SetDirty(false);
			Set_MainWindow_Title(pCustomWidget->GetFilename());
			}

}

static
void on_saveas_clicked()
{
		pCustomWidget->SetBackingStoreEnabled(false);
		if (pCustomWidget->run_file_chooser()) {
		    if (XML_Save(pCustomWidget->GetFilename())) {
					pCustomWidget->SetBackingStoreEnabled(true);
					pCustomWidget->SetDirty(false);
				Set_MainWindow_Title(pCustomWidget->GetFilename());
					}
				}
}

int main (int argc, char **argv)
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
    custom_widgets_register();
    //Load the GtkBuilder file and instantiate its widgets:
    auto refBuilder = Gtk::Builder::create();
    try
    {
        refBuilder->add_from_file("glade/default_layout.glade");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    }
    catch(const Glib::MarkupError& ex)
    {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return 1;
    }
    catch(const Gtk::BuilderError& ex)
    {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }
    //Get the GtkBuilder-instantiated Dialog:
    //
    refBuilder->get_widget("applicationwindow1", pMainWindow);
    pMainWindow->set_title("untitled1");
    refBuilder->get_widget("statusbar1", pStatusBar);
    refBuilder->get_widget("viewport1", pViewPort);
    refBuilder->get_widget("customwidget1", pCustomWidget);
		refBuilder->get_widget("scrolledwindow1", myScrolledWindow); 
		
    pStatusBar->push("Welcome to Substance Instainer!");

		pCustomWidget->SetParent(myScrolledWindow, pViewPort);
		GraphVector *sws = pCustomWidget->GetScrolledWindowSize();
    pCustomWidget->show_now();
    pCustomWidget->enable_timeout();

    /* initialise GEGL */
    geglio_init();
    if(pMainWindow) {
        /* Connect File menu callbacks */
        Gtk::ImageMenuItem* pButton = nullptr;
        refBuilder->get_widget("imagemenuitem1", pButton);
        if(pButton) pButton->signal_activate().connect( sigc::ptr_fun(on_new_clicked));
        refBuilder->get_widget("imagemenuitem2", pButton);
        if(pButton) pButton->signal_activate().connect( sigc::ptr_fun(on_open_clicked));
        refBuilder->get_widget("imagemenuitem3", pButton);
        if(pButton) pButton->signal_activate().connect( sigc::ptr_fun(on_save_clicked));
        refBuilder->get_widget("imagemenuitem4", pButton);
        if(pButton) pButton->signal_activate().connect( sigc::ptr_fun(on_saveas_clicked));
        refBuilder->get_widget("imagemenuitem5", pButton);
        if(pButton) pButton->signal_activate().connect( sigc::ptr_fun(on_button_clicked));
        app->run(*pMainWindow);
    }

    delete pMainWindow;
    geglio_exit();

    return 0;
}


void Set_MainWindow_Title(std::string s) 
{

  pMainWindow->set_title(s);
	return;
}
