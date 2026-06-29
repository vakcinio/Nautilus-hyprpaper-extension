/* nautilus-hyprpaper.c
 *
 * Copyright (C) 2026 vakcinio
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: vakcinio
 */

#include <glib.h>
#include <nautilus-extension.h>
#include <sys/stat.h>
#include <glib/gi18n-lib.h>
#define I18N_INIT() \
	bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR)

G_DECLARE_FINAL_TYPE (
    NautilusHyprpaper,
    nautilus_hyprpaper,
    NAUTILUS,
    HYPRPAPER,
    GObject
)

struct _NautilusHyprpaper
{
	GObject parent_instance;
};

static GType nautilus_hyprpaper_type;
static GType module_types[1];

static void
save_wallpaper_path(const char *path)
{
    char *file_path = g_build_filename(
        g_get_user_state_dir(),
        "hyprland",
        "wallpaper",
        NULL
    );

    g_mkdir_with_parents(g_path_get_dirname(file_path), S_IRWXU);

    g_file_set_contents(file_path, path, -1, NULL);

    g_free(file_path);
}

static void
nautilus_hyprpaper_set_image (
    NautilusMenuItem * const menu_item,
    gpointer v_unused
) {
    GList * const file_selection = g_object_get_data(
        G_OBJECT(menu_item),
        "nautilus_hyprpaper_files"
    );

    if (file_selection) {
        NautilusFileInfo *file = NAUTILUS_FILE_INFO(file_selection->data);

        char *uri = nautilus_file_info_get_uri(file);
        char *path = g_filename_from_uri(uri, NULL, NULL);
        

        char *arg = g_strdup_printf(",%s", path);

        GSubprocess *proc = g_subprocess_new(
            G_SUBPROCESS_FLAGS_NONE,
            NULL,
            "hyprctl",
            "hyprpaper",
            "wallpaper",
            arg,
            NULL
        );

        save_wallpaper_path(path);
        g_object_unref(proc);
        g_free(arg);
        g_free(path);
        g_free(uri);
    }
}

static GList * 
nautilus_hyprpaper_get_file_items (
    NautilusMenuProvider * const provider,
    GList * const file_selection
) {

    for (GList * iter = file_selection; iter; iter = iter->next) {

        NautilusFileInfo *file = NAUTILUS_FILE_INFO(iter->data);

        if (nautilus_file_info_is_directory(file)) {
            return NULL;
        }

        const char *mime_type = nautilus_file_info_get_mime_type(file);

        if (!g_str_has_prefix(mime_type, "image/")) {
            return NULL;
        }

        if (!g_str_equal(mime_type, "image/png") &&
            !g_str_equal(mime_type, "image/jpeg") &&
            !g_str_equal(mime_type, "image/jpg") &&
            !g_str_equal(mime_type, "image/webp")) {
            return NULL;
        }

    }

    NautilusMenuItem * const menu_item = nautilus_menu_item_new(
        "NautilusHyprpaper::set",
        _("Set as Hyprland wallpaper"),
        _("Sets the Hyprland wallpaper using hyprpaper"),
        NULL
    );

    g_signal_connect(
        menu_item,
        "activate",
        G_CALLBACK(nautilus_hyprpaper_set_image),
        NULL
    );

    g_object_set_data_full(
        G_OBJECT(menu_item),
        "nautilus_hyprpaper_files",
        nautilus_file_info_list_copy(file_selection),
        (GDestroyNotify) nautilus_file_info_list_free
    );

    return g_list_append(NULL, menu_item);

}

static void
nautilus_hyprpaper_menu_provider_iface_init (
    NautilusMenuProviderInterface * const iface,
    gpointer const iface_data
) {
    iface->get_file_items = nautilus_hyprpaper_get_file_items;
}

static void 
nautilus_hyprpaper_register_type (
    GTypeModule * const module
) {
    static const GTypeInfo info = {
        sizeof(NautilusHyprpaperClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) NULL,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof(NautilusHyprpaper),
        0,
        (GInstanceInitFunc) NULL,
        (GTypeValueTable *) NULL
    };

    static const GInterfaceInfo menu_provider_iface_info = {
        (GInterfaceInitFunc) nautilus_hyprpaper_menu_provider_iface_init,
        (GInterfaceFinalizeFunc) NULL,
        NULL
    };

    nautilus_hyprpaper_type = g_type_module_register_type(
        module,
        G_TYPE_OBJECT,
        "NautilusHyprpaper",
        &info,
        0
    );

    g_type_module_add_interface(
        module,
        nautilus_hyprpaper_type,
        NAUTILUS_TYPE_MENU_PROVIDER,
        &menu_provider_iface_info
    );
}

GType
nautilus_hyprpaper_get_type (void) {
    return nautilus_hyprpaper_type;
}

void
nautilus_module_shutdown (void) {
    /*  Any module-specific shutdown  */
}

void
nautilus_module_list_types (
    const GType ** const types,
    int * const num_types
) {
    *types = module_types;
    *num_types = G_N_ELEMENTS(module_types);
}

void
nautilus_module_initialize (
    GTypeModule * const module
) {
    I18N_INIT();
    nautilus_hyprpaper_register_type(module);
    *module_types = nautilus_hyprpaper_get_type();
}
