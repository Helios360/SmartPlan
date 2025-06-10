#include <gtk/gtk.h>
#include "gui.h"

GtkBuilder *builder;

void gui(int argc, char *argv[]){
    
    GtkWidget  *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("Glade.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show_all(window);
    gtk_main();
}

void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data) {
    guint year, month, day;
    gtk_calendar_get_date(calendar, &year, &month, &day); // month is 0-based
    month++;

    GtkWidget *event_box = GTK_WIDGET(gtk_builder_get_object(builder, "event_box"));

    // Clear previous children
    GList *children = gtk_container_get_children(GTK_CONTAINER(event_box));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);

    // Display matching events
    int found = 0;
    for (int i = 0; i < event_count; i++) {
        if (events[i].year == year && events[i].month == month && events[i].day == day) {
            char buf[512];
            snprintf(buf, sizeof(buf), "-- [%02d:%02d] %s for %d minutes(Prio %d)",
                     events[i].hours, events[i].minutes,
                     events[i].desc, events[i].duration, events[i].prio);

            GtkWidget *label = gtk_label_new(buf);
            gtk_widget_set_halign(label, GTK_ALIGN_START);
            gtk_box_pack_start(GTK_BOX(event_box), label, FALSE, FALSE, 5);
            found = 1;
        }
    }
    //printf("[DEBUG] Calendar date selected: %04d-%02d-%02d\n", year, month, day);

    if (!found) {
        GtkWidget *label = gtk_label_new("No events found.");
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_box_pack_start(GTK_BOX(event_box), label, FALSE, FALSE, 5);
    }

    gtk_widget_show_all(event_box);
}
