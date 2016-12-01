#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct {
    GtkWidget *img;
    GtkWidget *label;
} Data;

void clicked(GtkButton *button, Data *data)
{
    if (g_random_boolean()) {
        gtk_label_set_text(GTK_LABEL(data->label), "heads");
        gtk_image_set_from_file(GTK_IMAGE(data->img), "heads.jpg");
    } else {
        gtk_label_set_text(GTK_LABEL(data->label), "tails");
        gtk_image_set_from_file(GTK_IMAGE(data->img), "tails.jpg");
    }
    gtk_widget_set_size_request(data->img, 100, 100);
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *but = gtk_button_new_with_label("flip");
    GtkWidget *img =
        gtk_image_new_from_icon_name("dialog-question", GTK_ICON_SIZE_DIALOG);
    GtkWidget *lab = gtk_label_new("");

    gtk_box_pack_start(GTK_BOX(box), but, true, true, 6);
    gtk_box_pack_start(GTK_BOX(box), img, true, true, 6);
    gtk_box_pack_start(GTK_BOX(box), lab, true, true, 6);
    gtk_container_add(GTK_CONTAINER(win), box);

    g_signal_connect(win, "delete-event", gtk_main_quit, NULL);
    Data d = {.img = img, .label = lab};
    g_signal_connect(but, "clicked", G_CALLBACK(clicked), &d);

    gtk_widget_show_all(win);
    gtk_main();
}
