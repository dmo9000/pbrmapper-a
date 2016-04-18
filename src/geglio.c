/* GEGL I/O adapter */
#include <stdio.h>
#include <gegl.h>

int geglio_init()
{

	fprintf(stderr, "geglio_init()\n");
	gegl_init(0, NULL);

	return 0;
}


int geglio_exit()
{

	fprintf(stderr, "geglio_exit()\n");
	gegl_exit();
	return 0;

}

