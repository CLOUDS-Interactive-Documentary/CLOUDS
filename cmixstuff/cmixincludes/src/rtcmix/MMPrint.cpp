#include <MMPrint.h>

char MMPrint::mm_print_buf[SIZEOF_MMPRINTBUF];
char *MMPrint::mm_print_ptr;

	static char *mm_print_ptr;

char *get_mm_print_ptr()
{
	return MMPrint::mm_print_ptr;
}

void set_mm_print_ptr(int v)
{
	MMPrint::mm_print_ptr += v;
}


