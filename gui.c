#include <gtk/gtk.h>
#include "gui.h"

GtkBuilder *builder;
GtkWidget *window;
GtkWidget *event_window;

void gui(int argc, char *argv[]){
    gtk_init(&argc, &argv);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "guiStyle.css", NULL);
        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
    GTK_STYLE_PROVIDER_PRIORITY_USER);

    builder = gtk_builder_new_from_file("Glade.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show_all(window);
    gtk_main();
}

void on_form_event_clicked(GtkButton *button, gpointer user_data) {
    // Get the form window from the builder
    if (!event_window) {
        event_window = GTK_WIDGET(gtk_builder_get_object(builder, "event_window"));
    }
    gtk_widget_show_all(event_window);
}

void cancel_create(GtkButton *button, gpointer user_data){
    GtkWidget *form_window = GTK_WIDGET(gtk_builder_get_object(builder, "event_window"));
    gtk_widget_hide(form_window);
}

void on_create_event_clicked(GtkButton *button, gpointer user_data){

    GtkEntry *prio_entry = GTK_ENTRY(gtk_builder_get_object(builder, "priority_id"));
    GtkEntry *year_entry = GTK_ENTRY(gtk_builder_get_object(builder, "year_id"));
    GtkEntry *month_entry = GTK_ENTRY(gtk_builder_get_object(builder, "month_id"));
    GtkEntry *day_entry = GTK_ENTRY(gtk_builder_get_object(builder, "day_id"));
    GtkEntry *hour_entry = GTK_ENTRY(gtk_builder_get_object(builder, "hour_id"));
    GtkEntry *minute_entry = GTK_ENTRY(gtk_builder_get_object(builder, "minute_id"));
    GtkEntry *second_entry = GTK_ENTRY(gtk_builder_get_object(builder, "second_id"));
    GtkEntry *duration_entry = GTK_ENTRY(gtk_builder_get_object(builder, "duration_id"));
    GtkTextView *description_entry = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "description_id"));

    int prio = atoi(gtk_entry_get_text(prio_entry));
    int year = atoi(gtk_entry_get_text(year_entry));
    int month = atoi(gtk_entry_get_text(month_entry));
    int day = atoi(gtk_entry_get_text(day_entry));
    int hour = atoi(gtk_entry_get_text(hour_entry));
    int minute = atoi(gtk_entry_get_text(minute_entry));
    int second = atoi(gtk_entry_get_text(second_entry));
    int duration = atoi(gtk_entry_get_text(duration_entry));

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(description_entry);
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar *description = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    event e;
    create_event(&e, prio, year, month, day, hour, minute, second, duration, description);

    GtkWidget *form_window = GTK_WIDGET(gtk_builder_get_object(builder, "event"));
    gtk_widget_hide(form_window);
}

void on_delete_event_clicked(GtkButton *button, gpointer user_data){
    GtkWidget *even_row = GTK_WIDGET(user_data);
    unsigned long long id = (unsigned long long)(uintptr_t)g_object_get_data(G_OBJECT(button), "event-id");
    delete_event_by_id(id);
    write_all();
    gtk_widget_destroy(even_row);
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
    unsigned long long id;
    for (int i = 0; i < event_count; i++) {
        if (events[i].year == year && events[i].month == month && events[i].day == day) {
            char buf[512];
            snprintf(buf, sizeof(buf), "- [%02d:%02d] %s for %d minutes (Prio %d)",
                    events[i].hours, events[i].minutes,
                    events[i].desc, events[i].duration, events[i].prio);
            // Create a horizontal box for the label and button
            GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

            // Create the label
            GtkWidget *label = gtk_label_new(buf);
            gtk_widget_set_halign(label, GTK_ALIGN_START);

            // Create the button
            GtkWidget *button = gtk_button_new_with_label("Delete");
            gtk_widget_set_halign(button, GTK_ALIGN_END);
            g_signal_connect(button, "clicked", G_CALLBACK(on_delete_event_clicked), hbox);
            g_object_set_data(G_OBJECT(button), "event-id", GINT_TO_POINTER(events[i].id));

            // Pack the label and button into the horizontal box
            gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
            gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 5);

            // Pack the horizontal box into the event_box
            gtk_box_pack_start(GTK_BOX(event_box), hbox, FALSE, FALSE, 5);
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