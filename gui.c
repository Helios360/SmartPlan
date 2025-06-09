void fill_calendar_grid(GtkGrid *grid, int year, int month) {
    // Clean previous grid content (but keep header row)
    GList *children = gtk_container_get_children(GTK_CONTAINER(grid));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget *child = GTK_WIDGET(iter->data);
        int top;
        gtk_container_child_get(GTK_CONTAINER(grid), child, "top-attach", &top, NULL);
        if (top > 0) gtk_widget_destroy(child); // Keep day headers
    }
    g_list_free(children);

    // Calculate starting weekday (0 = Monday)
    struct tm date = {0};
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = 1;
    mktime(&date);
    int start_day = (date.tm_wday == 0) ? 6 : date.tm_wday - 1;

    int days_in_month = get_days_in_month(month, year);
    int day = 1;

    for (int row = 1; row <= 6 && day <= days_in_month; row++) {
        for (int col = 0; col < 7 && day <= days_in_month; col++) {
            if (row == 1 && col < start_day) continue;

            char label_text[4];
            snprintf(label_text, sizeof(label_text), "%d", day);

            GtkWidget *label = gtk_label_new(label_text);
            gtk_widget_set_hexpand(label, TRUE);
            gtk_widget_set_vexpand(label, TRUE);
            gtk_widget_set_halign(label, GTK_ALIGN_FILL);
            gtk_widget_set_valign(label, GTK_ALIGN_FILL);

            if (has_event_on_day(year, month, day)) {
                // Set red color using markup
                char markup[64];
                snprintf(markup, sizeof(markup), "<span foreground='red'><b>%d</b></span>", day);
                gtk_label_set_markup(GTK_LABEL(label), markup);
            }

            gtk_grid_attach(grid, label, col, row, 1, 1);
            day++;
        }
    }

    gtk_widget_show_all(GTK_WIDGET(grid));
}
