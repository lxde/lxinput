/*
 *      lxinput.c
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <string.h>
#include <math.h>

#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

static char* file = NULL;
static GKeyFile* kf;

static GtkWidget *dlg;
static GtkRange *mouse_accel;
static GtkRange *mouse_threshold;
static GtkToggleButton* mouse_left_handed;
static GtkRange *kb_delay;
static GtkRange *kb_interval;
static GtkToggleButton* kb_beep;

static int accel = 20, old_accel = 20;
static int threshold = 10, old_threshold = 10;
static gboolean left_handed = FALSE, old_left_handed = FALSE;

static int delay = 500, old_delay = 500;
static int interval = 30, old_interval = 30;
static gboolean beep = TRUE, old_beep = TRUE;


static void on_mouse_accel_changed(GtkRange* range, gpointer user_data)
{
    accel = (int)gtk_range_get_value(range);
    XChangePointerControl(GDK_DISPLAY(), True, False,
                             accel, 10, 0);
}

static void on_mouse_threshold_changed(GtkRange* range, gpointer user_data)
{
    /* threshold = 110 - sensitivity. The lower the threshold, the higher the sensitivity */
    threshold = 110 - (int)gtk_range_get_value(range);
    XChangePointerControl(GDK_DISPLAY(), False, True,
                             0, 10, threshold);
}

static void on_kb_range_changed(GtkRange* range, int* val)
{
    *val = (int)gtk_range_get_value(range);
    /* apply keyboard values */
    XkbSetAutoRepeatRate(GDK_DISPLAY(), XkbUseCoreKbd, delay, interval);
}

/* This function is taken from Gnome's control-center 2.6.0.3 (gnome-settings-mouse.c) and was modified*/
#define DEFAULT_PTR_MAP_SIZE 128
static void set_left_handed_mouse()
{
    unsigned char *buttons;
    gint n_buttons, i;
    gint idx_1 = 0, idx_3 = 1;

    buttons = g_alloca (DEFAULT_PTR_MAP_SIZE);
    n_buttons = XGetPointerMapping (GDK_DISPLAY(), buttons, DEFAULT_PTR_MAP_SIZE);
    if (n_buttons > DEFAULT_PTR_MAP_SIZE)
    {
        buttons = g_alloca (n_buttons);
        n_buttons = XGetPointerMapping (GDK_DISPLAY(), buttons, n_buttons);
    }

    for (i = 0; i < n_buttons; i++)
    {
        if (buttons[i] == 1)
            idx_1 = i;
        else if (buttons[i] == ((n_buttons < 3) ? 2 : 3))
            idx_3 = i;
    }

    if ((left_handed && idx_1 < idx_3) ||
        (!left_handed && idx_1 > idx_3))
    {
        buttons[idx_1] = ((n_buttons < 3) ? 2 : 3);
        buttons[idx_3] = 1;
        XSetPointerMapping (GDK_DISPLAY(), buttons, n_buttons);
    }
}

static void on_left_handed_toggle(GtkToggleButton* btn, gpointer user_data)
{
    left_handed = gtk_toggle_button_get_active(btn);
    set_left_handed_mouse(left_handed);
}

static void on_kb_beep_toggle(GtkToggleButton* btn, gpointer user_data)
{
    XKeyboardControl values;
    beep = gtk_toggle_button_get_active(btn);
    values.bell_percent = beep ? -1 : 0;
    XChangeKeyboardControl(GDK_DISPLAY(), KBBellPercent, &values);
}

static gboolean on_change_val(GtkRange *range, GtkScrollType scroll,
                                 gdouble value, gpointer user_data)
{
    int interval = GPOINTER_TO_INT(user_data);
    return FALSE;
}

static void set_range_stops(GtkRange* range, int interval )
{
/*
    g_signal_connect(range, "change-value",
                    G_CALLBACK(on_change_val), GINT_TO_POINTER(interval));
*/
}

static void load_settings()
{
    gboolean ret;
    const char* session_name = g_getenv("DESKTOP_SESSION");
	/* load settings from current session config files */
    if(!session_name)
        session_name = "LXDE";
	file = g_build_filename( g_get_user_config_dir(), "lxsession", session_name, "desktop.conf", NULL );
	ret = g_key_file_load_from_file( kf, file, 0, NULL );

    if( ret )
    {
        int val;
        val = g_key_file_get_integer(kf, "Mouse", "AccFactor", NULL);
        if( val > 0)
            old_accel = accel = val;

        val = g_key_file_get_integer(kf, "Mouse", "AccThreshold", NULL);
        if( val > 0)
            old_threshold = threshold = val;

        old_left_handed = left_handed = g_key_file_get_boolean(kf, "Mouse", "LeftHanded", NULL);

        val = g_key_file_get_integer(kf, "Keyboard", "Delay", NULL);
        if(val > 0)
            old_delay = delay = val;
        val = g_key_file_get_integer(kf, "Keyboard", "Interval", NULL);
        if(val > 0)
            old_interval = interval = val;

        if( g_key_file_has_key(kf, "Keyboard", "Beep", NULL ) )
            old_beep = beep = g_key_file_get_boolean(kf, "Keyboard", "Beep", NULL);
    }
}

int main(int argc, char** argv)
{
    GtkBuilder* builder;
    GError* err = NULL;
    char* str = NULL;

#ifdef ENABLE_NLS
    bindtextdomain ( GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR );
    bind_textdomain_codeset ( GETTEXT_PACKAGE, "UTF-8" );
    textdomain ( GETTEXT_PACKAGE );
#endif

    gtk_init(&argc, &argv);

    gtk_icon_theme_prepend_search_path(gtk_icon_theme_get_default(), PACKAGE_DATA_DIR);

    /* build the UI */
    builder = gtk_builder_new();

    gtk_builder_add_from_file( builder, PACKAGE_DATA_DIR "/lxinput.ui", NULL );
    dlg = (GtkWidget*)gtk_builder_get_object( builder, "dlg" );
    gtk_dialog_set_alternative_button_order( (GtkDialog*)dlg, GTK_RESPONSE_OK, GTK_RESPONSE_CANCEL, -1 );

    mouse_accel = (GtkRange*)gtk_builder_get_object(builder,"mouse_accel");
    mouse_threshold = (GtkRange*)gtk_builder_get_object(builder,"mouse_threshold");
    mouse_left_handed = (GtkToggleButton*)gtk_builder_get_object(builder,"left_handed");

    kb_delay = (GtkRange*)gtk_builder_get_object(builder,"kb_delay");
    kb_interval = (GtkRange*)gtk_builder_get_object(builder,"kb_interval");
    kb_beep = (GtkToggleButton*)gtk_builder_get_object(builder,"beep");

    g_object_unref( builder );


    /* read the config flie */
    kf = g_key_file_new();
    load_settings();

    /* init the UI */
    gtk_range_set_value(mouse_accel, accel);
    gtk_range_set_value(mouse_threshold, 110-threshold);
    gtk_toggle_button_set_active(mouse_left_handed, left_handed);

    gtk_range_set_value(kb_delay, delay);
    gtk_range_set_value(kb_interval, interval);
    gtk_toggle_button_set_active(kb_beep, beep);

    set_range_stops(mouse_accel, 10);
    g_signal_connect(mouse_accel, "value-changed", G_CALLBACK(on_mouse_accel_changed), NULL);
    set_range_stops(mouse_threshold, 10);
    g_signal_connect(mouse_threshold, "value-changed", G_CALLBACK(on_mouse_threshold_changed), NULL);
    g_signal_connect(mouse_left_handed, "toggled", G_CALLBACK(on_left_handed_toggle), NULL);

    set_range_stops(kb_delay, 10);
    g_signal_connect(kb_delay, "value-changed", G_CALLBACK(on_kb_range_changed), &kb_delay);
    set_range_stops(kb_interval, 10);
    g_signal_connect(kb_interval, "value-changed", G_CALLBACK(on_kb_range_changed), &kb_interval);
    g_signal_connect(kb_beep, "toggled", G_CALLBACK(on_kb_beep_toggle), NULL);

    if( gtk_dialog_run( (GtkDialog*)dlg ) == GTK_RESPONSE_OK )
    {
        gsize len;

        g_key_file_set_integer(kf, "Mouse", "AccFactor", accel);
        g_key_file_set_integer(kf, "Mouse", "AccThreshold", threshold);
        g_key_file_set_integer(kf, "Mouse", "LeftHanded", !!left_handed);

        g_key_file_set_integer(kf, "Keyboard", "Delay", delay);
        g_key_file_set_integer(kf, "Keyboard", "Interval", interval);
        g_key_file_set_integer(kf, "Keyboard", "Beep", !!beep);

        if( str = g_key_file_to_data( kf, &len, NULL ) )
        {
            if( g_file_set_contents( file, str, len, &err ) )
            {
                /* ask the settigns daemon to reload */
                /* FIXME: is this needed? */
                /* g_spawn_command_line_sync("lxde-settings-daemon reload", NULL, NULL, NULL, NULL); */
            }
            else
            {
                g_error_free( err );
            }
            g_free(str);
        }
    }
    else
    {
        /* restore to original settings */

        /* keyboard */
        delay = old_delay;
        interval = old_interval;
        beep = old_beep;
        XkbSetAutoRepeatRate(GDK_DISPLAY(), XkbUseCoreKbd, delay, interval);
        /* FIXME: beep? */

        /* mouse */
        accel = old_accel;
        threshold = old_threshold;
        left_handed = old_left_handed;
        XChangePointerControl(GDK_DISPLAY(), True, True,
                                 accel, 10, threshold);
        set_left_handed_mouse();
    }

    gtk_widget_destroy( dlg );

	g_free( file );
    g_key_file_free( kf );

    return 0;
}
