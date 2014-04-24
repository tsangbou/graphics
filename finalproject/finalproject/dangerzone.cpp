/* BMP file loader */
#ifndef FILE_STUFF
#define FILE_STUFF
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <iostream>

using namespace std;

float cameraView=0.0;

struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};
typedef struct Image Image;
#endif

#include <gl/glut.h>


void LoadGLTextures();
void set_colour(float r, float g, float b);
void render_Day_Scene();
void render_Fog_Scene();
bool render = true;

//light stuph
GLfloat light_color0[]={0.3f,0.3f,0.3f,1.0f};
GLfloat light_position0[]={0.0f, 0.0f, 13.0f, 1.0f};



//texture variable (global)
static GLuint texture[1];

/*
* getint and getshort are help functions to load the bitmap byte by byte
*/

void set_colour(float r, float g, float b)
{
  float ambient = 0.2f;
  float diffuse = 0.7f;
  float specular = 0.4f;
  GLfloat mat[4];
      /**** set ambient lighting parameters ****/
    mat[0] = ambient*r;
    mat[1] = ambient*g;
    mat[2] = ambient*b;
    mat[3] = 1.0;
    glMaterialfv (GL_FRONT, GL_AMBIENT, mat);

      /**** set diffuse lighting parameters ******/
    mat[0] = diffuse*r;
    mat[1] = diffuse*g;
    mat[2] = diffuse*b;
    mat[3] = 1.0;
    glMaterialfv (GL_FRONT, GL_DIFFUSE, mat);

      /**** set specular lighting parameters *****/
    mat[0] = specular*r;
    mat[1] = specular*g;
    mat[2] = specular*b;
    mat[3] = 1.0;
    glMaterialfv (GL_FRONT, GL_SPECULAR, mat);
    glMaterialf (GL_FRONT, GL_SHININESS, 0.5);
}

static unsigned int getint(FILE *fp)

{
	int c, c1, c2, c3;

	/*  get 4 bytes */ 
	c = getc(fp);  
	c1 = getc(fp);  
	c2 = getc(fp);  
	c3 = getc(fp);

	return ((unsigned int) c) +   
		(((unsigned int) c1) << 8) + 
		(((unsigned int) c2) << 16) +
		(((unsigned int) c3) << 24);
}

static unsigned int getshort(FILE *fp)

{
	int c, c1;

	/* get 2 bytes*/
	c = getc(fp);  
	c1 = getc(fp);

	return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

/*  quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  */

int ImageLoad(char *filename, Image *image) {
	FILE *file;
	unsigned long size;                 /*  size of the image in bytes. */
	unsigned long i;                    /*  standard counter. */
	unsigned short int planes;          /*  number of planes in image (must be 1)  */
	unsigned short int bpp;             /*  number of bits per pixel (must be 24) */
	char temp;                          /*  used to convert bgr to rgb color. */

	/*  make sure the file is there. */
	if ((file = fopen(filename, "rb"))==NULL) {
		printf("File Not Found : %s\n",filename);
		system("pause");
		return 0;
	}

	/*  seek through the bmp header, up to the width height: */
	fseek(file, 18, SEEK_CUR);

	/*  No 100% errorchecking anymore!!! */

	/*  read the width */    image->sizeX = getint (file);

	/*  read the height */ 
	image->sizeY = getint (file);

	/*  calculate the size (assuming 24 bits or 3 bytes per pixel). */
	size = image->sizeX * image->sizeY * 3;

	/*  read the planes */    
	planes = getshort(file);
	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		system("pause");
		return 0;
	}

	/*  read the bpp */    
	bpp = getshort(file);
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		system("pause");
		return 0;
	}

	/*  seek past the rest of the bitmap header. */
	fseek(file, 24, SEEK_CUR);

	/*  read the data.  */
	image->data = (char *) malloc(size);
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		system("pause");
		return 0;	
	}

	if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		system("pause");
		return 0;
	}

	for (i=0;i<size;i+=3) { /*  reverse all of the colors. (bgr -> rgb) */
		temp = image->data[i];
		image->data[i] = image->data[i+2];
		image->data[i+2] = temp;
	}

	fclose(file); /* Close the file and release the filedes */

	/*  we're done. */
	return 1;
}

GLvoid LoadTexture(char *filename, int texturenumber)
{	
	Image *TextureImage;

	TextureImage = (Image *) malloc(sizeof(Image));
	if (TextureImage == NULL) {
		printf("Error allocating space for image");
		exit(1);
	}

	if (!ImageLoad(filename, TextureImage)) {
		exit(1);
	}  

	/*  2d texture, level of detail 0 (normal), 3 components (red, green, blue),            */
	/*  x size from image, y size from image,                                              */    
	/*  border 0 (normal), rgb color data, unsigned byte data, and finally the data itself. */ 
	glBindTexture(GL_TEXTURE_2D,texturenumber);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 3,
		TextureImage->sizeX, TextureImage->sizeY,
		0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);

	free( TextureImage->data);
	free( TextureImage );

}


void init(void) 
{
	   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 50.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };


	glClearColor (0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);

	
	glGenTextures(2,texture);
	LoadTexture("dessert.bmp",texture[0]);
	LoadTexture("mountain.bmp",texture[1]);
	glEnable(GL_TEXTURE_2D);

	   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void  render_Day_Scene(){
	glPushMatrix();
	glRotatef(cameraView,0,1,0);//roate the screen

	glEnable(GL_TEXTURE_2D);

	//floor
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0,0.0);	glVertex3f(-7, -1.5, 5);
	glTexCoord2f(0.0,1.0);	glVertex3f(7, -1.5, 5);
	glTexCoord2f(1.0,1.0);	glVertex3f(7, -1.5, -6);
	glTexCoord2f(1.0,0.0);	glVertex3f(-7, -1.5, -6);
	glEnd();

	//mountains
	/*
	glBindTexture(GL_TEXTURE_2D,texture[1]);
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord3f(-7.0,-1.51, -6.0);	glVertex3f(-7.0,-1.51, -6.0);
			glTexCoord3f(-6.0, 0.15, -6.0);	glVertex3f(-6.0, 0.10, -6.0);
			glTexCoord3f(-4.3, 0.75, -6.0);	glVertex3f(-4.3, 0.75, -6.0);
			glTexCoord3f(-4.1, 0.80, -6.0);	glVertex3f(-4.1, 0.80, -6.0);
			glTexCoord3f(-3.0, 0.60, -6.0);	glVertex3f(-3.0, 0.60, -6.0);
			glTexCoord3f(-2.0, -1.51, -6.0);glVertex3f(-2.0, -1.51, -6.0);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord3f(0.0,-1.51, -6.0);	glVertex3f(-0.5,-1.51, -6.0);
			glTexCoord3f(0.5, 0.10, -6.0);	glVertex3f(0.5, 0.10, -6.0);
			glTexCoord3f(1.0, 0.25, -6.0);	glVertex3f(1.0, 0.25, -6.0);
			glTexCoord3f(1.5, 0.30, -6.0);	glVertex3f(1.5, 0.30, -6.0);
			glTexCoord3f(2.5, 0.20, -6.0);	glVertex3f(2.5, 0.20, -6.0);
			glTexCoord3f(-3.5, -1.51, -6.0);glVertex3f(3.5, -1.51, -6.0);
	glEnd();
	*/
	//Disable Texture
	glDisable(GL_TEXTURE_2D);
	
	//buildings

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	set_colour(0.958824,0.676471,0.539216);
	glScalef(1.0,1.0,1.0);
	glTranslatef(-3.5,-1.0,-1.7);
	glutSolidCube(0.7);
	glPopMatrix();

	glPushMatrix();
	set_colour(0.958824,0.676471,0.539216);
	glTranslatef(-2.0,-1.0,-1.7);
	glutSolidCube(0.7);
	glPopMatrix();

	glPushMatrix();
	set_colour(1,1,0);
	glTranslatef(-6.0,-1.4,-1.0);
	glScalef(0.05,0.05,0.05);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();
	
	glPushMatrix();
	set_colour(1,1,0);
	glTranslatef(3.0,-1.4,-1.0);
	glScalef(0.05,0.05,0.05);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	glPushMatrix();
	set_colour(1,1,0);
	glTranslatef(3.0,-1.4,1.0);
	glScalef(0.05,0.05,0.05);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	glPushMatrix();
	set_colour(1,1,0);
	glTranslatef(-6.0,-1.4,1.0);
	glScalef(0.05,0.05,0.05);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);


	//plane
	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	set_colour(0.1,0.1,0.1);
	//glTranslatef(-4.7f, -1.0f,-0.10f);
	glScalef(0.5,0.1,0.2);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	//front wing
	glPushMatrix();
	glTranslatef(0.15,0.0,0.0);
	set_colour(0.1,0.1,0.1);
	glRotatef(90,0,1,0);
	glScalef(0.70,0.03,0.12);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	//horizontal stabilzer
	glPushMatrix();
	glTranslatef(-0.2,0.0,0.0);
	set_colour(0.1,0.1,0.1);
	glRotatef(90,0,1,0);
	glScalef(0.45,0.03,0.2);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	//vertical stabilzer
	glPushMatrix();
	glTranslatef(-0.3,0.04,0.0);
	set_colour(0.1,0.1,0.1);
	glRotatef(50,0,0,1);
	glScalef(0.05,0.23,0.02);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();


	//cockpit
	glPushMatrix();
	glTranslatef(0.15,0.045,0.0);
	set_colour(0.1,0.1,1);
	glScalef(0.1,0.1,0.1);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	/*
	//blade
	//will rotate on animation
	glPushMatrix();
	glTranslatef(0.4,0.0,0.0);
	set_colour(0,0,0);
	glRotatef(45,1,0,0);
	glScalef(0.01,0.15,0.02);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();
	*/

	//engines
	glPushMatrix();       
    glTranslatef(-0.37,0.0,0);
	set_colour(0.1,0.1,0.1);
	glRotatef(90,0,1,0);
	glScalef(0.07,0.07,0.07);
	glutSolidTorus(0.25,0.6,10,10);
    glPopMatrix();

	glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);


	//Landing Strip
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(0.3f, 0.3f, 0.3f);
		glVertex3f(-6.0,-1.4,-1.0);
		glVertex3f(3.0,-1.4,-1.0);
		glVertex3f(3.0,-1.4,1.0);
		glVertex3f(-6.0,-1.4,1.0);
	glEnd();
	glPopMatrix();
	
	//Landing Strip lines

	int xLandingStrip = -5.7;
		glPushMatrix();
		glBegin(GL_QUADS);
		for(int i=0;i<8;i++){
			glColor3f(1.0,1.0,1.0);
			glVertex3f(xLandingStrip,-1.35,-0.10);
			glVertex3f(xLandingStrip+0.5,-1.35,-0.10);
			glVertex3f(xLandingStrip+0.5,-1.35,0.10);
			glVertex3f(xLandingStrip,-1.35,0.10);
			xLandingStrip+=1.0;
		}
		glEnd();


	//background
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 1.0f);
		glVertex3f(-7.0,-1.51f, -6.0);
		glVertex3f(7.0, -1.51f, -6.0);
		glVertex3f(7.0, 5.0, -6.0);
		glVertex3f(-7.0, 5.0, -6.0);
	glEnd();
	glPopMatrix();
	//left wall
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 1.0f);
		glVertex3f(-7.0,-1.51f, -6.0);
		glVertex3f(-7.0,-1.51f, 5.0);
		glVertex3f(-7.0,5.0f, 5.0);
		glVertex3f(-7.0,5.0f, -6.0);
	glEnd();
	glPopMatrix();
	//right wall
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 1.0f);
		glVertex3f(7.0,-1.51f, -6.0);
		glVertex3f(7.0,-1.51f, 5.0);
		glVertex3f(7.0,5.0f, 5.0);
		glVertex3f(7.0,5.0f, -6.0);
	glEnd();
	glPopMatrix();
	//ceiling
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 1.0f);
		glVertex3f(-3, 1.5, 5);
		glVertex3f(3, 1.5, 5);
		glVertex3f(7, 1.5, -6);
		glVertex3f(-7, 1.5, -6);
	glEnd();
	glPopMatrix();

	//behind camera wall
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(0.7f, 0.7f, 1.0f);
		glVertex3f(-7.0,-1.51, 5.0);
		glVertex3f(7.0,-1.51, 5.0);
		glVertex3f(7.0,5.0, 5.0);
		glVertex3f(-7.0,5.0, 5.0);
	glEnd();
	glPopMatrix();

	glPopMatrix();//from rotate camera
}

void render_Fog_Scene(){
	glPushMatrix();
	glRotatef(cameraView,0,1,0);//roate the screen

	glEnable(GL_TEXTURE_2D);

	//floor
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0,0.0);	glVertex3f(-7, -1.5, 5);
	glTexCoord2f(0.0,1.0);	glVertex3f(7, -1.5, 5);
	glTexCoord2f(1.0,1.0);	glVertex3f(7, -1.5, -6);
	glTexCoord2f(1.0,0.0);	glVertex3f(-7, -1.5, -6);
	glEnd();

	//mountains
	/*
	glBindTexture(GL_TEXTURE_2D,texture[1]);
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord3f(-7.0,-1.51, -6.0);	glVertex3f(-7.0,-1.51, -6.0);
			glTexCoord3f(-6.0, 0.15, -6.0);	glVertex3f(-6.0, 0.10, -6.0);
			glTexCoord3f(-4.3, 0.75, -6.0);	glVertex3f(-4.3, 0.75, -6.0);
			glTexCoord3f(-4.1, 0.80, -6.0);	glVertex3f(-4.1, 0.80, -6.0);
			glTexCoord3f(-3.0, 0.60, -6.0);	glVertex3f(-3.0, 0.60, -6.0);
			glTexCoord3f(-2.0, -1.51, -6.0);glVertex3f(-2.0, -1.51, -6.0);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord3f(0.0,-1.51, -6.0);	glVertex3f(-0.5,-1.51, -6.0);
			glTexCoord3f(0.5, 0.10, -6.0);	glVertex3f(0.5, 0.10, -6.0);
			glTexCoord3f(1.0, 0.25, -6.0);	glVertex3f(1.0, 0.25, -6.0);
			glTexCoord3f(1.5, 0.30, -6.0);	glVertex3f(1.5, 0.30, -6.0);
			glTexCoord3f(2.5, 0.20, -6.0);	glVertex3f(2.5, 0.20, -6.0);
			glTexCoord3f(-3.5, -1.51, -6.0);glVertex3f(3.5, -1.51, -6.0);
	glEnd();
	*/
	//Disable Texture
	glDisable(GL_TEXTURE_2D);
	
	//buildings

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	set_colour(0.958824,0.676471,0.539216);
	glScalef(1.0,1.0,1.0);
	glTranslatef(-3.5,-1.0,-1.7);
	glutSolidCube(0.7);
	glPopMatrix();

	glPushMatrix();
	set_colour(0.958824,0.676471,0.539216);
	glTranslatef(-2.0,-1.0,-1.7);
	glutSolidCube(0.7);
	glPopMatrix();

	glPushMatrix();
	set_colour(1,1,0);
	glTranslatef(-6.0,-1.4,-1.0);
	glScalef(0.05,0.05,0.05);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();
	
	glPushMatrix();
	set_colour(1,1,0);
	glTranslatef(3.0,-1.4,-1.0);
	glScalef(0.05,0.05,0.05);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	glPushMatrix();
	set_colour(1,1,0);
	glTranslatef(3.0,-1.4,1.0);
	glScalef(0.05,0.05,0.05);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	glPushMatrix();
	set_colour(1,1,0);
	glTranslatef(-6.0,-1.4,1.0);
	glScalef(0.05,0.05,0.05);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);


	//plane
	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	set_colour(0.1,0.1,0.1);
	//glTranslatef(-4.7f, -1.0f,-0.10f);
	glScalef(0.5,0.1,0.2);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	//front wing
	glPushMatrix();
	glTranslatef(0.15,0.0,0.0);
	set_colour(0.1,0.1,0.1);
	glRotatef(90,0,1,0);
	glScalef(0.70,0.03,0.12);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	//horizontal stabilzer
	glPushMatrix();
	glTranslatef(-0.2,0.0,0.0);
	set_colour(0.1,0.1,0.1);
	glRotatef(90,0,1,0);
	glScalef(0.45,0.03,0.2);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	//vertical stabilzer
	glPushMatrix();
	glTranslatef(-0.3,0.04,0.0);
	set_colour(0.1,0.1,0.1);
	glRotatef(50,0,0,1);
	glScalef(0.05,0.23,0.02);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();


	//cockpit
	glPushMatrix();
	glTranslatef(0.15,0.045,0.0);
	set_colour(0.1,0.1,1);
	glScalef(0.1,0.1,0.1);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();

	/*
	//blade
	//will rotate on animation
	glPushMatrix();
	glTranslatef(0.4,0.0,0.0);
	set_colour(0,0,0);
	glRotatef(45,1,0,0);
	glScalef(0.01,0.15,0.02);
	glutSolidSphere(0.8, 15, 15);
	glPopMatrix();
	*/

	//engines
	glPushMatrix();       
    glTranslatef(-0.37,0.0,0);
	set_colour(0.1,0.1,0.1);
	glRotatef(90,0,1,0);
	glScalef(0.07,0.07,0.07);
	glutSolidTorus(0.25,0.6,10,10);
    glPopMatrix();

	glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);


	//Landing Strip
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(0.3f, 0.3f, 0.3f);
		glVertex3f(-6.0,-1.4,-1.0);
		glVertex3f(3.0,-1.4,-1.0);
		glVertex3f(3.0,-1.4,1.0);
		glVertex3f(-6.0,-1.4,1.0);
	glEnd();
	glPopMatrix();
	
	//Landing Strip lines

	int xLandingStrip = -5.7;
		glPushMatrix();
		glBegin(GL_QUADS);
		for(int i=0;i<8;i++){
			glColor3f(1.0,1.0,1.0);
			glVertex3f(xLandingStrip,-1.35,-0.10);
			glVertex3f(xLandingStrip+0.5,-1.35,-0.10);
			glVertex3f(xLandingStrip+0.5,-1.35,0.10);
			glVertex3f(xLandingStrip,-1.35,0.10);
			xLandingStrip+=1.0;
		}
		glEnd();


	//background
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(.9f, 0.9f, 0.9f);
		glVertex3f(-7.0,-1.51f, -6.0);
		glVertex3f(7.0, -1.51f, -6.0);
		glVertex3f(7.0, 5.0, -6.0);
		glVertex3f(-7.0, 5.0, -6.0);
	glEnd();
	glPopMatrix();
	//left wall
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(.9f, 0.9f, 0.9f);
		glVertex3f(-7.0,-1.51f, -6.0);
		glVertex3f(-7.0,-1.51f, 5.0);
		glVertex3f(-7.0,5.0f, 5.0);
		glVertex3f(-7.0,5.0f, -6.0);
	glEnd();
	glPopMatrix();
	//right wall
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(.9f, 0.9f, 0.9f);
		glVertex3f(7.0,-1.51f, -6.0);
		glVertex3f(7.0,-1.51f, 5.0);
		glVertex3f(7.0,5.0f, 5.0);
		glVertex3f(7.0,5.0f, -6.0);
	glEnd();
	glPopMatrix();
	//ceiling
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(.9f, 0.9f, 0.9f);
		glVertex3f(-3, 1.5, 5);
		glVertex3f(3, 1.5, 5);
		glVertex3f(7, 1.5, -6);
		glVertex3f(-7, 1.5, -6);
	glEnd();
	glPopMatrix();

	//behind camera wall
	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3f(.9f, 0.9f, 0.9f);
		glVertex3f(-7.0,-1.51, 5.0);
		glVertex3f(7.0,-1.51, 5.0);
		glVertex3f(7.0,5.0, 5.0);
		glVertex3f(-7.0,5.0, 5.0);
	glEnd();
	glPopMatrix();

	glPopMatrix();//from rotate camera
}

void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if(render==true){
		render_Day_Scene();
	}
	else{
		render_Fog_Scene();
	}
	

	glFlush();
	//animate
	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);   
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);  
	glLoadIdentity();
	gluLookAt (0.0, 0.0, 5.0,  0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//check for rotation
}


void keyboard (unsigned char key, int x, int y)
{   
	switch (key) 
	{    
	case 27: //escape / quit program       
		exit(0);        
		break;
	case 'q':
		//rotate the the screen to the left
		cout<<"q"<<endl;
		cameraView-=40;
		break;
	case 'w':
		//rotate the screen to the right
		cout<<"w"<<endl;
		cameraView+=40;
		break;
	case 'a':
		//start animation
		cout<<"a"<<endl;
		//blade spin
		break;
	case 'f':
		cout<<"f"<<endl;
		render=false;
		break;
	case 'd':
		cout<<"d"<<endl;
		render=true;
		break;
	default:         
		break;   
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow ("DangerZone"); 
	cout<<"***Plane Scene***"<<endl;
	cout<<"q:to rotate the screen to the left"<<endl;
	cout<<"w:to rotate the screen to the right"<<endl;
	cout<<"a:to start animation"<<endl;
	cout<<"f:to turn on fog"<<endl;
	cout<<"d:to turn to day"<<endl;
	init ();   
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);   
	glutMainLoop();
	
	return 0;
}