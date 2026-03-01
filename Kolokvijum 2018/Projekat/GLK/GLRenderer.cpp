#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
#include "math.h"
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_camDist = 10.0;
	m_rotAngleAlfa = 0.0;
	m_rotAngleBeta = 0.0;
	UpdateCamera();
	m_enableLight = false;
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

	m_texShip = LoadTexture("ShipT1.png");
	m_texSpace[0] = LoadTexture("front.jpg");
	m_texSpace[1] = LoadTexture("left.jpg");
	m_texSpace[2] = LoadTexture("right.jpg");
	m_texSpace[3] = LoadTexture("back.jpg");
	m_texSpace[4] = LoadTexture("top.jpg");
	m_texSpace[5] = LoadTexture("bot.jpg");
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
		m_eyex, m_eyeY, m_eyeZ,
		0, 0, 0,
		0, 1, 0
	);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(10, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);
	glEnd();

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	DrawSpaceCube(50);

	GLfloat light_position[] = { 0.0,0.0,1.0,0.0 }; //u koliko je poziciono je 1 u suprotnom 0
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	if (m_enableLight) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	glRotatef(90, 1, 0, 0);
	DrawShip();

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

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
	glDeleteTextures(1, &m_texShip);
	glDeleteTextures(6, m_texSpace);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(L"C:\\Users\\stefa\\Downloads\\II_Kol_2018\\" + CString(fileName));
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

void CGLRenderer::DrawTriangle(float d1, float d2, float rep)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texShip);
	
	double h = (d1 * d2) / (d1 * d1 + d2 * d2);
	double x = h * (d2 / d1);

	glBegin(GL_POLYGON);
	
	glColor3f(1, 1, 1);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.5 * rep, 0);
	glVertex3f(0, 0, 0);

	glTexCoord2f((0.5 + h) * rep, (1 - x) * rep);
	glVertex3f(d1, 0, 0);

	glTexCoord2f(0.5 * rep, 1 * rep);
	glVertex3f(d1, d2, 0);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawShip()
{
	double ugao = atan2(2.15,5.8) * 180/3.1415;

	//Gore levo
	glPushMatrix();
	glRotatef(-4.75, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-ugao, 0, 0, 1);
	DrawTriangle(5.8, 2.15, 3);
	glPopMatrix();

	//Gore desno
	glPushMatrix();
	glScalef(1, -1, 1);
	glRotatef(-4.75, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-ugao, 0, 0, 1);
	DrawTriangle(5.8, 2.15, 3);
	glPopMatrix();

	//Dole levo
	glPushMatrix();
	glRotatef(4.75, 0, 1, 0);
	glRotatef(-13, 1, 0, 0);
	glRotatef(-ugao, 0, 0, 1);
	DrawTriangle(5.8, 2.15, 3);
	glPopMatrix();

	//Dole desno
	glPushMatrix();
	glScalef(1, -1, 1);
	glRotatef(4.75, 0, 1, 0);
	glRotatef(-13, 1, 0, 0);
	glRotatef(-ugao, 0, 0, 1);
	DrawTriangle(5.8, 2.15, 3);
	glPopMatrix();

}

void CGLRenderer::DrawSpaceCube(double a)
{
	glEnable(GL_TEXTURE_2D);

	//front
	glBindTexture(GL_TEXTURE_2D, m_texSpace[0]);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a / 2, -a / 2);

	glEnd();

	//left
	glBindTexture(GL_TEXTURE_2D, m_texSpace[1]);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a / 2, a / 2);
	glEnd();

	//right
	glBindTexture(GL_TEXTURE_2D, m_texSpace[2]);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a / 2, -a / 2);
	glEnd();

	//back
	glBindTexture(GL_TEXTURE_2D, m_texSpace[3]);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a / 2, a / 2);
	glEnd();

	//bottom
	glBindTexture(GL_TEXTURE_2D, m_texSpace[5]);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, -a / 2, -a / 2);
	glEnd();

	//top
	glBindTexture(GL_TEXTURE_2D, m_texSpace[4]);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, a / 2, -a / 2);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}
void CGLRenderer::UpdateCamera()
{
	double to_rad = 3.1415 / 180.0;
	m_eyex = m_camDist * cos(m_rotAngleAlfa * to_rad) * cos(m_rotAngleBeta * to_rad);
	m_eyeY = m_camDist * sin(m_rotAngleBeta * to_rad);
	m_eyeZ = m_camDist * sin(m_rotAngleAlfa * to_rad) * cos(m_rotAngleBeta * to_rad);
}
