#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
#define ToRad(x) ((x) * 3.141592 / 180.0)
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_camDist = 40.0;
	m_rotGlava = 0.0;
	m_rotRuka1 = 0.0;
	m_rotRuka2 = 0.0;
	m_camAlpha = 1.0;
	m_camBeta = 1.0;
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
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);

	m_texLamp = LoadTexture("lamp.jpg");
	m_texEnv[0] = LoadTexture("front.jpg");
	m_texEnv[1] = LoadTexture("left.jpg");
	m_texEnv[2] = LoadTexture("right.jpg");
	m_texEnv[3] = LoadTexture("back.jpg");
	m_texEnv[4] = LoadTexture("top.jpg");
	m_texEnv[5] = LoadTexture("bot.jpg");
	m_texEnv[6] = LoadTexture("side.jpg");
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
		0, 0, 0,
		0, 1, 0
	);

	/*glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(m_eyeX, m_eyeY, m_eyeZ);
	DrawEnvCube(1);
	glTranslatef(-m_eyeX, m_eyeY, m_eyeZ);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);*/
	DrawAxes();

	//DrawLampBase();
	//DrawLampHead();
	//DrawLampArm();
	DrawLamp();
	DrawEnvCube(200);
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
	glDeleteTextures(1, &m_texLamp);
	glDeleteTextures(6, m_texEnv);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::UpdateCamera()
{
	m_eyeX = m_camDist * cos(ToRad(m_camAlpha)) * cos(ToRad(m_camBeta));
	m_eyeZ = m_camDist * cos(ToRad(m_camAlpha)) * sin(ToRad(m_camBeta));
	m_eyeY = m_camDist * sin(ToRad(m_camAlpha));
}

void CGLRenderer::DrawAxes()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);

	glBegin(GL_LINES);
	glColor3f(0,0,1);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);

	glColor3f(1,0,0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
}
UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(L"C:\\Users\\stefa\\Downloads\\II kolokvijum 2023\\" + CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return texID;
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	
	glBindTexture(GL_TEXTURE_2D, m_texEnv[6]);
	glBegin(GL_QUADS);

	//Front
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, a / 2);
	//Right
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, 0, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, -a / 2);
	//Back
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, 0, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, 0, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a, -a / 2);
	//Left
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a, a / 2);
	glEnd();
	//Floor
	glBindTexture(GL_TEXTURE_2D, m_texEnv[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, 0, a / 2);
	glEnd();


	glDisable(GL_TEXTURE_2D);

}

void CGLRenderer::DrawCylinder(double r1, double r2, double h, int nSeg, int texMode, bool bIsOpen)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texLamp);
	glColor3f(1.0f, 1.0f, 1.0f);

	double PI = 3.141592;
	double degSeg = 2.0 * PI / nSeg;

	double vStart = (texMode == 0) ? 0.0 : 0.5;
	double vEnd = vStart + 0.5;
	//Omotac
	glBegin(GL_QUAD_STRIP);
	for (int j = 0; j <= nSeg; j++) {
		double angle = j * degSeg;
		if (j == nSeg) angle = 0.0;

		double x1 = r1 * cos(angle);
		double z1 = r1 * sin(angle);
		double x2 = r2 * cos(angle);
		double z2 = r2 * sin(angle);

		double u = (double)j / nSeg;

		glTexCoord2f(u, vStart);
		glVertex3f(x1, 0, z1);

		glTexCoord2f(u, vEnd);
		glVertex3f(x2, h, z2);
	}
	glEnd();
	//Baza
	if (!bIsOpen) {

		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5, 0.25);
		glVertex3f(0.0, 0.0, 0.0);
		for (int j = nSeg; j >= 0; j--) {
			double angle = j * degSeg;
			if (j == nSeg) angle = 0.0;

			double x = r1 * cos(angle);
			double z = r1 * sin(angle);

			double u = 0.5 + 0.5 * cos(angle);
			double v = 0.25 + 0.25 * sin(angle);

			glTexCoord2f(u, v);
			glVertex3f(x, 0.0, z);
		}
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5, 0.25);
		glVertex3f(0.0, h, 0.0);
		for (int j = 0; j <= nSeg; j++) {
			double angle = j * degSeg;
			if (j == nSeg) angle = 0.0;

			double x = r2 * cos(angle);
			double z = r2 * sin(angle);

			double u = 0.5 + 0.5 * cos(angle);
			double v = 0.25 + 0.25 * sin(angle);

			glTexCoord2f(u, v);
			glVertex3f(x, h, z);
		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawLampBase()
{
	DrawCylinder(8, 7, 2, 32, 1, 0);
}

void CGLRenderer::DrawLampHead()
{
	glPushMatrix();
	glRotatef(180.0, 1, 0, 0);
	glTranslatef(0, -5, 0);
	DrawCylinder(3, 6, 5, 32, 0, 1);
	glTranslatef(0, 5, 0);
	glRotatef(-180.0, 1, 0, 0);
	glTranslatef(0, 5, 0);

	glPushMatrix();
	DrawCylinder(3, 3, 5, 32, 1, 0);
	glTranslatef(0, 5, 0);
	DrawCylinder(3, 2, 1, 32, 1, 0);
	glPopMatrix();
	glRotatef(90, 1, 0, 0);
	glTranslatef(3, -1, -2);
	DrawCylinder(3, 3, 2, 32, 1, 0);
	glPopMatrix();
}

void CGLRenderer::DrawLampArm()
{
	glPushMatrix();
	glTranslatef(-1, 3, 0);
	glRotatef(-90, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	DrawCylinder(3, 3, 2, 32, 1, 0);
	glRotatef(-90, 0, 1, 0);
	glRotatef(90, 0, 0, 1);
	glTranslatef(1, 0, 0);
	DrawCylinder(1, 1, 15, 32, 1, 0);
	glPopMatrix();
}

void CGLRenderer::DrawLamp()
{
	glPushMatrix();
	DrawLampBase();
	glRotatef(m_rotRuka1, 1, 0, 0);
	DrawLampArm();
	glTranslatef(0, 17, 0);
	glRotatef(m_rotRuka2, 1, 0,0);
	DrawLampArm();
	glTranslatef(0, 10, 0);
	glRotatef(m_rotGlava, 1, 0, 0);
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 0, 5);
	DrawLampHead();
	glRotatef(-90, 0, 1, 0);
	glRotatef(-m_rotGlava, 0, 0, 0);
	glRotatef(-m_rotRuka2, 1, 0, 0);
	glRotatef(-m_rotRuka1, 1, 0, 0);
	glPopMatrix();

}