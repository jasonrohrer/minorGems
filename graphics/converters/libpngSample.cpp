



#include <png.h>
#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

int main() {
    
    int w = 100;
    int h = 100;
    
    unsigned int *data = new unsigned int[ w * h ];
        
    // red fades toward bottom
    // green fades toward right
    
    unsigned int **rows = new unsigned int *[ h ];

    for( int y=0; y<h; y++ ) {
        for( int x=0; x<w; x++ ) {
            unsigned char red = 255 - ( 255 * y / h );
            unsigned char green = 255 - ( 255 * x / h );
            
            data[ y * w + x ] = red | green << 8 | 255 << 24;
            }

        rows[y] = &( data[ y * w ] );
        }

    
    
    

	/* create file */
	FILE *fp = fopen( "test.png", "wb");
	if (!fp)
		abort_("[write_png_file] File %s could not be opened for writing",
               "test.png" );
    
    png_structp png_ptr;
    png_infop info_ptr;

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		abort_("[write_png_file] png_create_write_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during init_io");

	png_init_io(png_ptr, fp);


	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, info_ptr, w, h,
		     8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))

		abort_("[write_png_file] Error during writing bytes");

	png_write_image(png_ptr, (png_byte**)rows);


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during end of write");

	png_write_end(png_ptr, NULL);

    
    /* cleanup heap allocation */
	/*for (y=0; y<height; y++)
		free(row_pointers[y]);
	free(row_pointers);
    */
    fclose(fp);
    }
