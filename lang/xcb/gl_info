/**
1. man page offically:
	http://www.khronos.org/opengles/sdk/docs/man/


  */
#include <stdio.h>
#include <GL/glut.h>

GLfloat color0[]= { 0.0, 0.0, 0.0 , 0.0  };
GLfloat colorV[]= { 1.0, 1.0, 1.0 , 1.0 };
GLfloat colorR[]= { 1.0, 0.0, 0.0 , 0.5 };
GLfloat colorG[]= { 0.0, 1.0, 0.0 , 0.5 };
GLfloat colorB[]= { 0.0, 0.0, 1.0 , 0.5 };

GLfloat V1[]= { 0.0, 0.0, 0.0 };
GLfloat V0[]= { -0.0, -0.5, -0.3 };
GLfloat V2[]= { 0.9, 1.0, 0.9 };

void init(void)
{
//   glEnable(GL_DEPTH_TEST);

  /* Setup the view of the cube. */
 // glMatrixMode(GL_PROJECTION);
//  gluPerspective( /* field of view in degree */ 40.0,
   // /* aspect ratio */ 1.0,
   // /* Z near */ 1.0, /* Z far */ 10.0);
 // glMatrixMode(GL_MODELVIEW);
 // gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
  //  0.0, 0.0, 0.0,      /* center is at (0,0,0) */
 //   0.0, 1.0, 0.);      /* up is in positive Y direction */

  /* Adjust cube position to be asthetic angle. */
//  glTranslatef(0.0, 0.0, -1.0);
 // glRotatef(60, 1.0, 0.0, 0.0);
 // glRotatef(-20, 0.0, 0.0, 1.0);
	glClearColor( 0.0, 0.0, 0.0 , 0.0   );
	glShadeModel(GL_FLAT);
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT );
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_POLYGON);
  	glEdgeFlag(GL_TRUE);
	glVertex3fv(V0);
	glEdgeFlag(GL_FALSE);
	glVertex3fv(V1);
	glEdgeFlag(GL_TRUE);
	glVertex3fv(V2);
  glEnd();
  glutSwapBuffers();
  */

  glColor4fv(colorV);
//  glColor3f( 1.0, 1.0, 1.0 );
  glLoadIdentity();
  gluPerspective( 90.0,    2.0,    1.0,  1.0);  // degree, ratio, near, far
  gluLookAt(  5.0,5.0,5.0,  // p, center, up
			  0.0,0.0,0.0,
			  0.0,1.0,0.0	);
  glScalef( 3.0, 1.0, 3.0 );
  glutWireCube(1.0);
  glFlush();
}


void reshape(int w, int h)
{
	printf(" reshape [%d,%d]\n", w,h);
	glViewport( 0, 0,  (GLsizei)w, (GLsizei)h);
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();
	glFrustum( -1.0, 1.0,  -1.0, 1.0, 1.5, 20.0);
	glMatrixMode( GL_MODELVIEW );

		test_FBO();
}

void test_FBO()
{
	GLuint fb, depth_rb, tex;
	// create objects
	glGenFramebuffersEXT(1, &fb);
	glGenRenderbuffersEXT(1, &depth_rb);
	glGenTextures(1, &tex);
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fb);

	int width=300, height=200;
	//initialize texture 
	glBindTexture( GL_TEXTURE_2D, tex);
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	//set texture parameter here

	//attach texture to framebuffer color buffer
	glFramebufferTexture2DEXT ( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex, 0);

	//initilize depth renderbuffer
	glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, depth_rb );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);

	//attach renderbuffer to framebuffer depth buffer
	glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb );

	//check fb status

	//render to the FBO
	glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT, fb );
	//draw something here,  rendering to texture

	//render to the window , using the texture
	glBindFramebufferEXT ( GL_FRAMEBUFFER_EXT, 0 );
	glBindTexture ( GL_TEXTURE_2D, tex );

}

void dump_info()
{
	char *ext=NULL;
	//1. configs
	//2. exts
	//direct render support
	// version string( vender, renderer, shader)
	printf("GL_VERSION=%s\n", glGetString(GL_VERSION));
	printf("shader ver=%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	ext = glGetString( GL_EXTENSIONS );
	printf("GL_ARB_framebuffer_object is %s supported\n", 
			strstr( ext, "GL_ARB_framebuffer_object")==NULL?"not":"");
	printf("GL_EXT_framebuffer_object is %s supported\n", 
			strstr( ext, "GL_EXT_framebuffer_object")==NULL?"not":"");

	GLenum status;
	status =  glCheckFramebufferStatusEXT ( GL_FRAMEBUFFER_EXT );
	if( status == GL_FRAMEBUFFER_COMPLETE_EXT){
		printf(" status is COMPLETE_EXT\n");
	}else if(status == GL_FRAMEBUFFER_UNSUPPORTED_EXT){
		printf(" status is unsupported ext\n");
	}


}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
  //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(350,350);
  glutCreateWindow("red 3D lighted cube");
  dump_info();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  init();
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
