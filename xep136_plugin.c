#define PURPLE_PLUGINS

/*#include <glib.h>*/
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"

#include <gtkconv.h>
#include <gtkimhtml.h>
#include <gtkplugin.h>
#include <version.h>

#include "gtksourceundomanager.h"
#include "connection.h"

/*
 * -------------------------------------------------- 
 *            Pidgin XEP-136 plugin
 * -------------------------------------------------- 
 */
static PurplePlugin *xep136 = NULL;

static void
history_enable(GtkToggleButton *check, PidginConversation *gtkconv)
{
    PurpleConversation *conv;
    PurpleConvIm *im;
    char *text;
    char *message[] = {
	"XEP-136 message archiving enabled!",
	"XEP-136 message archiving disabled!"
    };

     if (gtk_toggle_button_get_active(check))
	 text = message[0];
     else
	 text = message[1];

    conv = gtkconv->active_conv;
    im = PURPLE_CONV_IM(conv); 

    g_return_if_fail(conv != NULL);

    purple_conv_im_write(im, NULL, text, PURPLE_MESSAGE_SYSTEM, time(NULL));
   
}

static void
detach_from_gtkconv(PidginConversation *gtkconv, gpointer null)
{
    GtkWidget *toolbar_box, *hbox;

    toolbar_box = gtkconv->toolbar;

    hbox = g_object_get_data(G_OBJECT(toolbar_box), "xep136_hbox");

    if (hbox)
	gtk_container_remove(GTK_CONTAINER(toolbar_box), hbox);
    
    gtk_widget_queue_draw(pidgin_conv_get_window(gtkconv)->window);
}

static void
attach_to_gtkconv(PidginConversation *gtkconv, gpointer null)
{
    GtkWidget *toolbar_box, *hbox, *check, *button;

    toolbar_box = gtkconv->toolbar;

    check = gtk_check_button_new();
    button = gtk_button_new_with_label("History");

    g_signal_connect(G_OBJECT(check), "toggled",
	    G_CALLBACK(history_enable), (gpointer) gtkconv);

    hbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), check, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    g_object_set_data(G_OBJECT(toolbar_box), "xep136_hbox", hbox);

    gtk_box_pack_end(GTK_BOX(toolbar_box), hbox, FALSE, FALSE, 0);

    gtk_widget_show_all(hbox);

    gtk_widget_queue_draw(pidgin_conv_get_window(gtkconv)->window);
}

static void
detach_from_pidgin_window(PidginWindow *win, gpointer null)
{
    g_list_foreach(pidgin_conv_window_get_gtkconvs(win), (GFunc) detach_from_gtkconv, NULL);
}

static void
attach_to_pidgin_window(PidginWindow *win, gpointer null)
{
    g_list_foreach(pidgin_conv_window_get_gtkconvs(win), (GFunc) attach_to_gtkconv, NULL);
}

static void
detach_from_all_windows()
{
    g_list_foreach(pidgin_conv_windows_get_list(), (GFunc) detach_from_pidgin_window, NULL);
}

static void
attach_to_all_windows()
{
    g_list_foreach(pidgin_conv_windows_get_list(), (GFunc) attach_to_pidgin_window, NULL);
}

static void
conv_created_cb(PurpleConversation *conv, gpointer null)
{
    PidginConversation *gtkconv = PIDGIN_CONVERSATION(conv);

    g_return_if_fail(gtkconv != NULL);

    attach_to_gtkconv(gtkconv, NULL);
}

static gboolean
plugin_load(PurplePlugin *plugin)
{
    xep136 = plugin;

    attach_to_all_windows();

    purple_signal_connect(purple_conversations_get_handle(), 
	    "conversation-created",
	    plugin, PURPLE_CALLBACK(conv_created_cb), NULL);

    return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
    detach_from_all_windows();

    return TRUE;
}

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    NULL,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,

    "gtk-daniel_kraic-xep136_plugin",
    "XEP-136 plugin",
    "1.2",

    "XEP-136 plugin",
    "XEP-136 plugin",
    "Daniel Kraic <danielkraic@gmail.com>",
    "https://github.com/danielkraic/Pidgin-XEP-0136-plugin",

    plugin_load,
    plugin_unload,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL, /*    plugin_actions, */
    /* this tells libpurple the address of the function to call
                       to get the list of plugin actions. */
    NULL,
    NULL,
    NULL,
    NULL
};

static void
init_plugin(PurplePlugin *plugin)
{
}

PURPLE_INIT_PLUGIN(xep136_plugin, init_plugin, info)
