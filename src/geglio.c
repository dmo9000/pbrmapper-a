/* GEGL I/O adapter */
#include <stdio.h>
#include <gegl.h>

int geglio_init()
{

	fprintf(stderr, "geglio_init()\n");
	gegl_init(0, NULL);


}
