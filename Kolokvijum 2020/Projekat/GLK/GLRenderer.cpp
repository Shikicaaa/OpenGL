#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
#include <math.h>

#define PI 3.14159265358979323846
#define ToRad(x) ((x) * PI / 180.0)
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_cameraR = 30.0;
	m_cameraAngleXY = 90.0;
	m_cameraAngleXZ = 0.0;

	UpdateCameraPosition();
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

	m_texSpider = LoadTexture("spider.png");
	m_texEnv[0] = LoadTexture("front.jpg");
	m_texEnv[1] = LoadTexture("back.jpg");
	m_texEnv[2] = LoadTexture("left.jpg");
	m_texEnv[3] = LoadTexture("right.jpg");
	m_texEnv[4] = LoadTexture("top.jpg");
	m_texEnv[5] = LoadTexture("bot.jpg");

	glEnable(GL_TEXTURE_2D);
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

	DrawAxes();
	DrawEnvCube(100);

	DrawSpider();

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
	gluPerspective(50, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &m_texSpider);
	glDeleteTextures(6, m_texEnv);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{
	glLineWidth(2.0);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);
	glEnd();
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(L"C:\\Users\\stefa\\Downloads\\II kolokvijum 2020\\" + CString(fileName));
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return texID;
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_TEXTURE_2D);

	//front
	glBindTexture(GL_TEXTURE_2D, m_texEnv[0]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(1.0, 0.0);
	glVertex3d(-a / 2, a / 2, -a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(a / 2, -a / 2, -a / 2);
	glTexCoord2f(0.0, 0.0);
	glVertex3d(a / 2, a / 2, -a / 2);

	glEnd();

	//back
	glBindTexture(GL_TEXTURE_2D, m_texEnv[1]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(-a / 2, a / 2, a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(a / 2, a / 2, a / 2);

	glEnd();

	//left
	glBindTexture(GL_TEXTURE_2D, m_texEnv[2]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(a / 2, a / 2, a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(a / 2, a / 2, -a / 2);

	glEnd();

	//right
	glBindTexture(GL_TEXTURE_2D, m_texEnv[3]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(-a / 2, a / 2, a / 2);

	glEnd();

	//top
	glBindTexture(GL_TEXTURE_2D, m_texEnv[4]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(a / 2, a / 2, -a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(a / 2, a / 2, a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(-a / 2, a / 2, a / 2);

	glEnd();

	//bot
	glBindTexture(GL_TEXTURE_2D, m_texEnv[5]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(-a / 2, -a / 2, a / 2);

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::UpdateCameraPosition()
{
	double radXY = ToRad(m_cameraAngleXY);
	double radXZ = ToRad(m_cameraAngleXZ);

	m_eyeX = m_cameraR * cos(radXZ) * cos(radXY);
	m_eyeY = m_cameraR * sin(radXZ);
	m_eyeZ = m_cameraR * cos(radXZ) * sin(radXY);
}

void CGLRenderer::RotateView(double dx, double dy)
{
	m_cameraAngleXY -= dx;
	m_cameraAngleXZ += dy;

	if (m_cameraAngleXZ > 180) m_cameraAngleXZ = 180;
	if (m_cameraAngleXZ < 0) m_cameraAngleXZ = 0;

	UpdateCameraPosition();
}

void CGLRenderer::ZoomView(double dx)
{
	m_cameraR += dx;
	if (m_cameraR > 50) m_cameraR = 50;
	if (m_cameraR < 8) m_cameraR = 8;

	UpdateCameraPosition();
}

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR)
{
	double longSeg = 180.0 / nSeg;
	double latSeg = 360.0 / nSeg;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texSpider);
	for (double i = -90.0; i <= 90.0; i += longSeg) {
		
		glBegin(GL_QUAD_STRIP);
		for (double j = 0.0; j <= 360.0; j += latSeg) {
			double x1 = r * cos(ToRad(i)) * cos(ToRad(j));
			double y1 = r * sin(ToRad(i));
			double z1 = r * cos(ToRad(i)) * sin(ToRad(j));

			double x2 = r * cos(ToRad(i + longSeg)) * cos(ToRad(j));
			double y2 = r * sin(ToRad(i + longSeg));
			double z2 = r * cos(ToRad(i + longSeg)) * sin(ToRad(j));

			double texX1 = x1 / r * texR + texU;
			double texX2 = x2 / r * texR + texU;
			double texZ1 = z1 / r * texR + texV;
			double texZ2 = z2 / r * texR + texV;

			glTexCoord2f(texX1, texZ1);
			glVertex3f(x1, y1, z1);
			glTexCoord2f(texX2, texZ2);
			glVertex3f(x2, y2, z2);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR)
{
	double seg = 360.0 / nSeg;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texSpider);


	//Base
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(texU, texV);
	glVertex3f(0, 0, 0);
	for (double i = 0.0; i <= 360.0; i += seg) {
		double x = r * cos(ToRad(i));
		double z = r * sin(ToRad(i));

		double tx = x / r * texR + texU;
		double tz = z / r * texR + texV;
		
		glTexCoord2f(tx, tz);
		glVertex3f(x, 0.0, z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(texU, texV);
	glVertex3f(0, h, 0);

	for (double i = 0.0; i <= 360; i += seg) {
		double x = r * cos(ToRad(i));
		double z = r * sin(ToRad(i));

		double tx = x / r * texR + texU;
		double tz = z / r * texR + texV;

		glTexCoord2f(tx, tz);
		glVertex3f(x, 0.0, z);
	}
	glEnd();

}

void CGLRenderer::DrawLegSegment(double r, double h, int nSeg)
{
	glPushMatrix();
	glTranslatef(0, r, 0);
	DrawSphere(r, 2 * nSeg, 0.25, 0.25, 0.24);
	DrawCone(r, h, nSeg, 0.75, 0.75, 0.24);
	glPopMatrix();
}

void CGLRenderer::DrawLeg()
{
	double radius = 1;
	double nSeg = 5;
	double l1 = 10;
	double l2 = 15;
	glPushMatrix();
	DrawLegSegment(radius, l1, nSeg);
	glTranslatef(0, l1 + 1, 0);
	glRotatef(85, 1.0, 0.0, 0.0);
	DrawLegSegment(radius, l2, nSeg);
	glPopMatrix();
}

void CGLRenderer::DrawSpiderBody()
{
	//Glava
	glPushMatrix();
	glScalef(1.0, 0.5, 1.0);
	DrawSphere(2, 10, 0.75, 0.25, 0.24);
	glScalef(1.0, 2, 1.0);
	
	//Grud
	glTranslatef(4, 0, 0);
	glScalef(1.0, 0.5, 1.0);
	DrawSphere(3, 10, 0.25, 0.25, 0.24);
	glScalef(1.0, 2, 1.0);
	//stomak
	glTranslatef(6, 0, 0);
	glScalef(1, 0.8, 1);
	DrawSphere(5, 10, 0.25, 0.25, 0.24);
	glScalef(1, 1/0.8, 1);
	glPopMatrix();

}

void CGLRenderer::DrawSpider() {
	glPushMatrix();
	DrawSpiderBody();
	glTranslatef(4, 0, 0);
	for (int i = -60; i < 60; i += 30) {
		glPushMatrix();
		glRotatef(i, 0, 1.0, 0);
		glRotatef(45, 1.0, 0, 0);
		DrawLeg();
		glPopMatrix();

		glPushMatrix();
		glScalef(1.0, 1.0, -1.0);
		glRotatef(i, 0, 1.0, 0);
		glRotatef(45, 1.0, 0, 0);
		DrawLeg();
		glPopMatrix();
	}

	glPopMatrix();
}

