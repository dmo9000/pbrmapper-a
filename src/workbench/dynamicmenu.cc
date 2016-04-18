#include <gtkmm.h>

class MainWindow : public Gtk::Window
{
public:
    MainWindow();
    virtual ~MainWindow();
    void newTab();

protected:
    void buttonAddClicked();
    void buttonDeleteClicked();

    Gtk::Box box_;
    Gtk::MenuItem* item;
    Gtk::Menu* submenu1;
    Gtk::MenuItem* subitem1;
    Gtk::MenuItem* item2;
    Gtk::Menu* submenu2;
    Gtk::MenuItem* subitem2;
    Gtk::ButtonBox buttonbox_;
    Gtk::Button button_delete_;
    Gtk::Button button_add_;
};

MainWindow::MainWindow()
{
    this->set_title("Menu Test");
    this->set_default_size(300, 400);
    box_.set_orientation(Gtk::ORIENTATION_VERTICAL);
    this->add(box_);

    item = Gtk::manage(new Gtk::MenuItem("test"));
    submenu1 = Gtk::manage(new Gtk::Menu);
    subitem1 = Gtk::manage(new Gtk::MenuItem("sub Item"));
    submenu1->append(*subitem1);
    item->set_submenu(*submenu1);
    item2 = Gtk::manage(new Gtk::MenuItem("test2"));
    submenu2 = Gtk::manage(new Gtk::Menu);
    subitem2 = Gtk::manage(new Gtk::MenuItem("sub item 2"));
    submenu2->append(*subitem2);
    item2->set_submenu(*submenu2);
    Gtk::MenuBar* menubar = Gtk::manage(new Gtk::MenuBar);
    //nmenu->append(*item);
    menubar->append(*item);
    menubar->append(*item2);
    box_.add(*menubar);

    button_delete_.set_label("Delete");
    button_add_.set_label("Add");
    box_.add(buttonbox_);
    buttonbox_.add(button_delete_);
    buttonbox_.add(button_add_);

    button_delete_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::buttonDeleteClicked));
    button_add_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::buttonAddClicked));

    this->show_all_children();
}

MainWindow::~MainWindow()
{

}

void MainWindow::buttonAddClicked()
{
    Gtk::MenuItem* newitem = Gtk::manage(new Gtk::MenuItem("new Item"));
    submenu1->append(*newitem);
    newitem->show_all();
}

void MainWindow::buttonDeleteClicked()
{
    subitem2->hide();
}

int main(int argc, char** argv)
{   
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "menu.example");

    MainWindow mw;

    app->run(mw);

    return 0;
}
