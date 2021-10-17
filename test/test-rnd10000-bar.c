#define _GNU_SOURCE
#include <glib.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include "blot.h"

#define DATA_COUNT 100
#define DATA_MAX   100

#define DATA_Y_MAX 100000

#define FATAL_ERROR(error) ({ \
	if (unlikely (error)) \
		g_error("%s:%u: %s", __func__, __LINE__, (error)->message); \
})

int main(void)
{
	g_autoptr(GError) error = NULL;

	setlocale(LC_CTYPE, "");
	srand(time(NULL));

	/* build a dummy dataset */

	static uint32_t data1[DATA_COUNT];
	static uint32_t data2[DATA_COUNT];
	static uint32_t data3[DATA_COUNT];
	static char *xlabels[DATA_COUNT];

	for (int i=0; i<DATA_COUNT; i++) {
		data1[i] = rand() % DATA_Y_MAX;

		/* let's make up some years */
		asprintf(&xlabels[i], "%u", 2000-DATA_COUNT+i);
	}
	
	for (int i=0; i<DATA_COUNT; i++) {
		data2[i] = rand() % DATA_Y_MAX;

		/* let's make up some years */
		asprintf(&xlabels[i], "%u", 2000-DATA_COUNT+i);
	}

	for (int i=0; i<DATA_COUNT; i++) {
		data3[i] = rand() % DATA_Y_MAX;

		/* let's make up some years */
		asprintf(&xlabels[i], "%u", 2000-DATA_COUNT+i);
	}

	/* configure the figure */

	blot_figure *fig;

	fig = blot_figure_new(&error);
	FATAL_ERROR(error);

	blot_figure_set_axis_color(fig, 8, &error);
	FATAL_ERROR(error);

	blot_figure_set_x_axis_labels(fig, DATA_COUNT, xlabels, &error);
	FATAL_ERROR(error);

	/* add a bar plot */

	blot_figure_bar(fig, BLOT_DATA_INT32,
			DATA_COUNT, NULL, data1,
			9, "histogram", &error);
	FATAL_ERROR(error);

	blot_figure_bar(fig, BLOT_DATA_INT32,
			DATA_COUNT, NULL, data2,
			8, "histogram", &error);
	FATAL_ERROR(error);

	blot_figure_bar(fig, BLOT_DATA_INT32,
			DATA_COUNT, NULL, data3,
			7, "histogram", &error);
	FATAL_ERROR(error);

	/* render the plots */

	blot_render_flags flags = 0;
	flags |= BLOT_RENDER_BRAILLE;
	flags |= BLOT_RENDER_LEGEND_BELOW;

	blot_screen *scr = blot_figure_render(fig, flags, &error);
	FATAL_ERROR(error);

	/* print it to screen */

	gsize txt_size = 0;
	const wchar_t *txt = blot_screen_get_text(scr, &txt_size, &error);
	FATAL_ERROR(error);

	printf("%ls", txt);

	blot_screen_delete(scr);

	blot_figure_delete(fig);

	for (int i=0; i<DATA_COUNT; i++)
		free(xlabels[i]);

	return 0;
}
