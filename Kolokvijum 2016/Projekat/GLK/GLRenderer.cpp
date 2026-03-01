#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"

#define TORAD(X) ((X) * 3.141592 / 180.0)
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_camDist = 20;
	m_camAlpha = 1.0;
	m_camBeta = 0.0;
	m_rotKrila = 0.0;
	UpdateCamera();
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	m_texEnv = LoadTexture("env.png");
	m_texBrick = LoadTexture("brick.png");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(
		m_eyeX, m_eyeY, m_eyeZ,
		0, 10, 0,
		0, 1, 0
	);

	GLfloat light_ambient[] = { 0.2,0.2,0.2,1 };
	GLfloat light_diffuse[] = { 1,1,1,1 };
	GLfloat light_dir[] = { 1,1,1,0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	glVertex3f(1, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(1, 0, 1);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glDisable(GL_LIGHTING);
	DrawCube(200);
	glEnable(GL_LIGHTING);
	//DrawTube(1, 2, 3, 32);
	//DrawCone(3.8, 2, 32);
	/*DrawPaddle(8, 1.5);*/
	glTranslatef(0, 0, 20);
	GLfloat bijela[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bijela);
	DrawTube(2.5, 3.5, 10, 32);
	glTranslatef(0, 10, 0);
	DrawCone(3.8, 2, 32);
	glTranslatef(0, -1, -3.5);

	glRotatef(90, 0, 1, 0);
	glRotatef(m_rotKrila, 1,0,0);
	for (int i = 0; i < 4; i++) {
		glRotatef(i * (90), 1, 0, 0);
		DrawPaddle(8, 1.5);
	}
	glDisable(GL_LIGHTING);

	glFlush();
	SwapBuffers(pDC->m_hDC);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (double)w / (double) h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	glDeleteTextures(1, &m_texEnv);
	glDeleteTextures(1, &m_texBrick);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::UpdateCamera()
{
	m_eyeX = m_camDist * cos(TORAD(m_camAlpha)) * cos(TORAD(m_camBeta));
	m_eyeY = m_camDist * sin(TORAD(m_camAlpha));
	m_eyeZ = m_camDist * cos(TORAD(m_camAlpha)) * sin(TORAD(m_camBeta));
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(L"C:\\Users\\stefa\\Downloads\\II_kol_2016\\" + CString(fileName));
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_RGBA, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return texID;
}

void CGLRenderer::DrawCube(double a)
{
	double ySeg = 1.0 / 3.0;
	double xSeg = 0.25;

	//front
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texEnv);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2d(xSeg, 0);
	glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2d(xSeg, ySeg);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2d(2*xSeg, ySeg);
	glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2d(2*xSeg, 0);
	glVertex3f(a / 2, a / 2, -a / 2);

	//right
	glTexCoord2d(3 * xSeg, ySeg);
	glVertex3d(a / 2, a / 2, -a / 2);
	glTexCoord2d(2 * xSeg, ySeg);
	glVertex3d(a / 2, -a / 2, -a / 2);
	glTexCoord2d(2 * xSeg, 2 * ySeg);
	glVertex3d(a / 2, -a / 2, a / 2);
	glTexCoord2d(3 * xSeg, 2 * ySeg);
	glVertex3d(a / 2, a / 2, a / 2);

	//back
	glTexCoord2d(2 * xSeg, 1);
	glVertex3d(a / 2, a / 2, a / 2);
	glTexCoord2d(2 * xSeg, 1-ySeg);
	glVertex3d(a / 2, -a / 2, a / 2);
	glTexCoord2d(xSeg, 1-ySeg);
	glVertex3d(-a / 2, -a / 2, a / 2);
	glTexCoord2d(xSeg, 1);
	glVertex3d(-a / 2, a / 2, a / 2);

	//left
	glTexCoord2d(0, 2 * ySeg);
	glVertex3d(-a / 2, a / 2, a / 2);
	glTexCoord2d(xSeg, 2 * ySeg);
	glVertex3d(-a / 2, -a / 2, a / 2);
	glTexCoord2d(xSeg, ySeg);
	glVertex3d(-a / 2, -a / 2, -a / 2);
	glTexCoord2d(0, ySeg);
	glVertex3d(-a / 2, a / 2, -a / 2);

	//gore
	glTexCoord2d(1, ySeg);
	glVertex3d(-a / 2, a / 2, -a / 2);
	glTexCoord2d(1, 1-ySeg);
	glVertex3d(-a / 2, a / 2, a / 2);
	glTexCoord2d(1-xSeg, 1-ySeg);
	glVertex3d(a / 2, a / 2, a / 2);
	glTexCoord2d(1-xSeg, ySeg);
	glVertex3d(a / 2, a / 2, -a / 2);

	//dole
	glTexCoord2d(xSeg, ySeg);
	glVertex3d(-a / 2, -a / 2, a / 2);
	glTexCoord2d(xSeg, 2*ySeg);
	glVertex3d(-a / 2, -a / 2, -a / 2);
	glTexCoord2d(2*xSeg, 2*ySeg);
	glVertex3d(a / 2, -a / 2, -a / 2);
	glTexCoord2d(2*xSeg, ySeg);
	glVertex3d(a / 2, -a / 2, a / 2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawTube(double r1, double r2, double h, int n)
{
	const double dvapi = 3.141592 * 2;
	double texSeg = 1.0 / n;
	double segments = dvapi / n;
	glBindTexture(GL_TEXTURE_2D, m_texBrick);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n; i++) {
		double x1, x2;
		double z1, z2;

		glNormal3f(cos(i*segments), 0.0, sin(i*segments));
		double tx = i * texSeg;
		x1 = r1 * cos(segments * i);
		x2 = r2 * cos(segments * i);
		z1 = r1 * sin(segments * i);
		z2 = r2 * sin(segments * i);
		glTexCoord2d(tx, 1.0);
		glVertex3f(x1, h, z1);
		glTexCoord2d(tx, 0.0);
		glVertex3f(x2, 0, z2);
	}
	glEnd();


	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawCone(double r, double h, int n)
{
	const double dvapi = 3.141592 * 2;
	double texSeg = 1.0 / n;
	double segments = dvapi / n;
	glColor3f(1, 1, 1);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, h, 0);
	for (int i = 0; i <= n; i++) {
		double x1;
		double z1;
		glNormal3f(cos(segments * i), 0.5, sin(segments * i));
		x1 = r * cos(segments * i);
		z1 = r * sin(segments * i);
		glVertex3f(x1, 0, z1);
	}
	glEnd();
}

void CGLRenderer::DrawPaddle(double length, double width)
{
	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);

	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, length / 8);
	glVertex3f(0, -width / 8,length/8);
	glVertex3f(0, -width / 8, 0);

	glVertex3f(0, 0, length / 8);
	glVertex3f(0, 0, length * 2 / 8);
	glVertex3f(0, -width, length * 2 / 8);
	glVertex3f(0, -width / 8, length / 8);

	glVertex3f(0, 0, length * 2 / 8);
	glVertex3f(0, 0, length);
	glVertex3f(0, -width, length);
	glVertex3f(0, -width, length * 2 / 8);

	glEnd();
}