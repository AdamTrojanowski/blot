#include <glib.h>
#include <stdio.h>
#include <locale.h>
#include "blot.h"

#define DATA_COUNT 10000

#define DATA_X_MAX 10000
#define DATA_Y_MAX 10000

#define LINE_COUNT 2

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

#define FATAL_ERROR(error) ({ \
	if (unlikely (error)) \
		g_error("%s:%u: %s", __func__, __LINE__, (error)->message); \
})

int main(void)
{
	g_autoptr(GError) error = NULL;

	setlocale(LC_CTYPE, "");

	/* build a dummy dataset */

	static gint32 line_x[LINE_COUNT] = { DATA_X_MAX, 0 };
	static gint32 line_y[LINE_COUNT] = { (DATA_Y_MAX/2), DATA_Y_MAX/2 };
	static gint32 line2_x[LINE_COUNT] = { DATA_X_MAX/2, DATA_X_MAX/2  };
	static gint32 line2_y[LINE_COUNT] = { (0), DATA_Y_MAX };
	/* configure the figure */

	blot_figure *fig;

	fig = blot_figure_new(&error);
	FATAL_ERROR(error);

	/* add a line plot */

	blot_figure_line(fig, BLOT_DATA_INT32,
			    LINE_COUNT, line_x, line_y,
			    6, "line", &error);
	FATAL_ERROR(error);

	/* add  line two plot */

	blot_figure_line(fig, BLOT_DATA_INT32,
			    LINE_COUNT, line2_x, line2_y,
			    5, "line", &error);
	FATAL_ERROR(error);

	/* render the plots */

	blot_render_flags flags = 0;
	flags |= BLOT_RENDER_BRAILLE;
	flags |= BLOT_RENDER_LEGEND_BELOW;
	flags |= BLOT_RENDER_NO_X_AXIS;
	flags |= BLOT_RENDER_NO_Y_AXIS;

	blot_screen *scr = blot_figure_render(fig, flags, &error);
	FATAL_ERROR(error);

	/* print it to screen */

	gsize txt_size = 0;
	const wchar_t *txt = blot_screen_get_text(scr, &txt_size, &error);
	FATAL_ERROR(error);

	printf("%ls", txt);

	blot_screen_delete(scr);

	blot_figure_delete(fig);

	return 0;
}
