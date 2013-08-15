#include <glib.h>
#include "glib-binreloc.h"

int
main ()
{
	GError *error = NULL;
	gchar *exe, *data, *locale;

	if (!gbr_init (&error)) {
		g_print ("BinReloc failed to initialize:\n");
		g_print ("Domain: %d (%s)\n",
			 (int) error->domain,
			 g_quark_to_string (error->domain));
		g_print ("Code: %d\n", error->code);
		g_print ("Message: %s\n", error->message);
		g_error_free (error);
		g_print ("----------------\n");
	}

	exe    = gbr_find_exe ();
	data   = gbr_find_data_dir   ("default data dir");
	locale = gbr_find_locale_dir ("default locale dir");

	g_print ("Executable filename: %s\n", exe);
	g_print ("Data dir  : %s\n", data);
	g_print ("Locale dir: %s\n", locale);
	
	g_free (exe);
	g_free (data);
	g_free (locale);
	return 0;
}
