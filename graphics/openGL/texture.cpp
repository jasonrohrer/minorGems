/*

    OpenGL Tutor : Texture



    Copyright 1999 by Nate 'm|d' Miller

    For non-commercial use only!



    File        -- texture.c

    Date        -- 6/22/99

    Author      -- Nate 'm|d' Miller

    Contact     -- vandals1@home.com

    Web         -- http://members.home.com/vandals1



    This is about as simple of a texture mapping tutorial that you will get.  

    The program loads "texture.tga" and displays it onto a quad.  



    If you want to compile this make sure that you link too the following 

    libraries: opegl32.lib glu32.lib glut32.lib

*/

#define APP_NAME                "OpenGL Tutor : Texture"

#include "tga.h"

#include <stdio.h>

#include <stdlib.h>

#include "gl/glut.h"



int winW = 640; /* window width */

int winH = 480; /* window height */



/*

=============

drawFace

=============



Draws a textured face.  Take note that the texture vertex must come BEFORE the 

vertex that it will be assigned too. 



Texture Coordinate Orrientation



(0,1)-------------(1,1)

  |                 |

  |     Image       |

  |                 |

  |                 |

(0,0)-------------(1,0)



*/

void drawFace (void)

{

//    glEnable (GL_TEXTURE_2D); /* enable texture mapping */

//    glBindTexture (GL_TEXTURE_2D, 13); /* bind to our texture, has id of 13 */



    glBegin (GL_QUADS);

        //glTexCoord2f (0.0f,0.0f); /* lower left corner of image */

        glColor3f(1.0, 1.0, 1.0);

        glVertex3f (-10.0f, -10.0f, 0.0f);

        //glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */

        glColor3f(0, 1.0, 1.0);

        glVertex3f (10.0f, -10.0f, 0.0f);

        //glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */

        glColor3f(1.0, 1.0, 0);

        glVertex3f (10.0f, 10.0f, 0.0f);

        //glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */

        glColor3f(1.0, 0, 0);

        glVertex3f (-10.0f, 10.0f, 0.0f);

    glEnd ();



 //   glDisable (GL_TEXTURE_2D); /* disable texture mapping */

}







void drawSphere (void)

{

    /* 

        This sphere is going to be rotated around the *current axis*.  This is 

        because the model was first rotated and then translated.

    */

    glPushMatrix ();



    glRotatef (rotate, axis[0], axis[1], axis[2]);



    glTranslatef (-15.0, 10.0, 5.0);



    glColor3f (1.0, 0.0, 0.0);



    glutWireSphere (5.0, 6.0, 6.0);



    glPopMatrix ();



    /* 

        This sphere is going to be rotated around the its own center.  This is 

        because the model was first translated and then rotated.  

    */

    glPushMatrix ();



    glTranslatef (5.0, 10.0, 0.0);



    glRotatef (rotate, axis[0], axis[1], axis[2]);



    glColor3f (0.0, 1.0, 0.0);



    glutWireSphere (2.0, 6.0, 6.0);



    glPopMatrix ();

}















/*

=============

glutDisplay

=============



Our display function

*/

void glutDisplay (void)

{

    if (!winH)

        return;



    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	glMatrixMode (GL_MODELVIEW);

	glLoadIdentity ();

	glTranslatef (0, 0, -50); /* eye position */



    drawFace ();

    drawSphere ();



	glutSwapBuffers();

}

/*

=============

glutResize

=============     



Resize function.  Called when our window is created and resized.  

*/

void glutResize (int w, int h)

{	

	winW = w;

	winH = h;

    

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport (0, 0, winW, winH);



	glMatrixMode (GL_PROJECTION);

    glLoadIdentity ();



	gluPerspective (90, winW / winH, 1, 9999);



	glutPostRedisplay ();

}

/*

=============

glutKeyboard

=============     



Keyboard handler.

*/

void glutKeyboard (unsigned char key, int x, int y)

{

    switch (key)

    {

        /* exit the program */

        case 27:

        case 'q':

        case 'Q':

            exit (1);

        break;

    }

}

/*

=============

glInit

=============



Sets up some OpenGL states and loads image.   



*/

void glInit (void)

{

  glEnable (GL_DEPTH_TEST);

  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);



  loadTGA ("texture.tga", 13);



}

void main (void)

{

	glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_FULLSCREEN);

	glutInitWindowSize (winW,winH);

	glutCreateWindow (APP_NAME);

	glutKeyboardFunc (glutKeyboard);

	glutDisplayFunc (glutDisplay);

	glutReshapeFunc (glutResize);



	glInit ();



	glutMainLoop(); // we never return...

}