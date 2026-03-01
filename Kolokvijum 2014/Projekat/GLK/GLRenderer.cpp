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
	m_camAlpha = 45.0;
	m_camBeta = 0.0;
	m_camDist = 40.0;
	m_rotGroup1 = 0.0;
	m_rotGroup2 = 0.0;
	m_rotGroup3 = 0.0;
	//LoadTexture();
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

	wglMakeCurrent(pDC->m_hDC, m_hrc);
	LoadTexture();
	wglMakeCurrent(NULL, NULL);

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	UpdateCamera();
	//glEnable(GL_CULL_FACE);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	GLfloat light1_pos[] = { 0,0,0,1 };
	GLfloat light1_dir[] = { 0,0,-1 };
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_dir);

	gluLookAt(
		m_eyeX, m_eyeY, m_eyeZ,
		0, 0, 0,
		0, 1, 0
	);

	GLfloat light0_pos[] = { 5,20,0,1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

	//DrawCube(5);
	SetWhiteLight();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	DrawRubikCube(5, 3);
	glDisable(GL_TEXTURE_2D);

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
	gluPerspective(40, (double)w / (double)h, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	glDeleteTextures(1, &m_tex);
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

void CGLRenderer::DrawCube(float a, int i, int j, int k, int count)
{
	//glBegin(GL_QUADS);
	double ui1, ui2;
	double vj1, vj2;
	double wk1, wk2;
	ui1 = i * 1.0 / count;
	ui2 = (i + 1) * 1.0 / count;
	vj1 = (count-j)*1.0/count;
	vj2 = (count-j-1) * 1.0 / count;
	wk1 = k * 1.0 / count;
	wk2 = (k + 1) * 1.0 / count;

	//front
	SetMaterial(1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glTexCoord2f(ui1, vj2); glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(ui1, vj1); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(ui2, vj1); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(ui2, vj2); glVertex3f(a / 2, a / 2, a / 2);
	glEnd();

	//right
	SetMaterial(0, 0, 1);
	glBegin(GL_QUADS);
	glNormal3f(1, 0, 0);
	glTexCoord2f(wk2, vj2); glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(wk2, vj1); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(wk1, vj1); glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(wk1, vj2); glVertex3f(a / 2, a / 2, -a / 2);
	glEnd();

	//back
	SetMaterial(1, 0.5, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(ui1, vj2); glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(ui1, vj1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(ui2, vj1); glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(ui2, vj2); glVertex3f(a / 2, a / 2, -a / 2);
	glEnd();

	//left
	SetMaterial(0, 1, 0);
	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glTexCoord2f(wk1, vj2); glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(wk1, vj1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(wk2, vj1); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(wk2, vj2); glVertex3f(-a / 2, a / 2, a / 2);
	glEnd();

	//up
	SetMaterial(1, 1, 1);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(ui1, wk2); glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(ui1, wk1); glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(ui2, wk1); glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(ui2, wk2); glVertex3f(a / 2, a / 2, a / 2);
	glEnd();

	//down
	SetMaterial(1, 1, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(ui1, wk2); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(ui1, wk1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(ui2, wk1); glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(ui2, wk2); glVertex3f(a / 2, -a / 2, a / 2);
	glEnd();

}

void CGLRenderer::DrawRubikCube(double a, int count)
{
	double gap = 0.05 * a;
	double offset = (count - 1) * (a + gap) / 2.0;
	for (int i = 0; i < count; i++) {
		glPushMatrix();
		if (i == 0) glRotatef(m_rotGroup1, 1.0, 0.0, 0.0);
		else if (i == 1) glRotatef(m_rotGroup2, 1.0, 0.0, 0.0);
		else if (i == 2) glRotatef(m_rotGroup3, 1.0, 0.0, 0.0);
		for (int j = 0; j < count; j++) {
			for (int k = 0; k < count; k++) {
				glPushMatrix();

				double tx = i * (a + gap) - offset;
				double ty = j * (a + gap) - offset;
				double tz = k * (a + gap) - offset;
				glTranslatef(tx, ty,tz);
				DrawCube(a, i, j, k, count);

				glPopMatrix();
			}
		}
		glPopMatrix();
	}
}

void CGLRenderer::LoadTexture()
{
	DImage img;
	img.Load(L"C:\\Users\\stefa\\Downloads\\GL-Kol-2014\\OpenGL.bmp");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_2D, m_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

}

void CGLRenderer::SetWhiteLight()
{
	glEnable(GL_LIGHTING);

	GLfloat white_light[] = { 1,1,1,1 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 13.0f);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0f);
	glEnable(GL_LIGHT1);

	GLfloat global_ambient[] = { 0.5,0.5,0.5,1 };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void CGLRenderer::SetMaterial(float r, float g, float b)
{
	GLfloat diffuse[] = { r,g,b,1 };
	GLfloat specular[] = { r,g,b,1 };
	GLfloat emission[] = { 0,0,0,1 };
	GLfloat ambient[] = { r * 0.2,g * 0.2,b * 0.2,1 };
	GLfloat shininess = 64.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}