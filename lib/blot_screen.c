/* blot: screen represents the final render object */
/* vim: set noet sw=8 ts=8 tw=80: */
#include <wchar.h>
#include "blot_screen.h"
#include "blot_error.h"
#include "blot_canvas.h"
#include "blot_color.h"
#include "blot_braille.h"

/* create/delete */

blot_screen * blot_screen_new(unsigned cols, unsigned rows,
			      blot_render_flags flags, GError **error)
{
	gsize char_len = cols * rows;

	// add extra space for screen clear escape code
	if (flags & BLOT_RENDER_CLEAR)
		char_len ++;

	// assume every character is escaped (overkill)
	char_len *= 16;

	gsize data_bytes = char_len * sizeof(gunichar);
	gsize total_size = sizeof(blot_screen) + data_bytes;
	blot_screen *scr = g_malloc(total_size);
	RETURN_ERROR(!scr, NULL, error, "new blot_screen [%zu]", char_len);

	scr->flags     = flags;
	scr->cols      = cols;
	scr->rows      = rows;
	scr->data_size = char_len;
	scr->data_used = 0;

	return scr;
}

void blot_screen_delete(blot_screen *scr)
{
	g_free(scr);
}

/* merge/render */

static bool blot_screen_merge_cans(blot_screen *scr, unsigned count,
				   struct blot_canvas *const*cans,
				   GError **error)
{
	gunichar *p = scr->data + scr->data_used,
		 *end = scr->data + scr->data_size;

	for (unsigned iy=0; iy<scr->rows; iy++) {

		/* y-axis is inverted on the screen */
		unsigned y = (scr->flags & BLOT_RENDER_DONT_INVERT_Y_AXIS)
			? iy : scr->rows - 1 - iy;

		wchar_t wch;
		for (unsigned x=0; x<scr->cols; x++) {

			gunichar top_cell = 0;
			blot_color top_col = 0;

			for (int ci=0; ci<count; ci++) {
				const struct blot_canvas *can = cans[ci];

				gunichar cell = blot_canvas_get_cell(can, x, y);
				if (!cell)
					continue;

				top_cell = cell;
				top_col = can->color;
			}

			wch = L' ';
			if (top_cell) {
				// TODO: apply color
				(void)top_col;
				wch = top_cell;
			}
			*(p++) = wch;
			g_assert_cmpuint((uintptr_t)p, <, (uintptr_t)end);
		}

		*(p++) = L'\n';

		g_assert_cmpuint((uintptr_t)p, <, (uintptr_t)end);
	}

	*p = 0;
	scr->data_used = p - scr->data;
	return true;
}

bool blot_screen_merge(blot_screen *scr, unsigned count,
		       struct blot_canvas *const*cans,
		       GError **error)
{
	RETURN_EFAULT_IF(scr==NULL, NULL, error);

	scr->data_used = 0;
	if (scr->flags & BLOT_RENDER_CLEAR) {
		gsize len = wcslen(L(CLR_SCR));
		memcpy(scr->data, L(CLR_SCR), len*sizeof(gunichar));
		scr->data_used = len;
	}

	return blot_screen_merge_cans(scr, count, cans, error);
}

const gunichar * blot_screen_get_text(const blot_screen *scr,
				  gsize *txt_size, GError **error)
{
	RETURN_EFAULT_IF(scr==NULL, NULL, error);

	*txt_size = scr->data_used;
	return scr->data;
}
