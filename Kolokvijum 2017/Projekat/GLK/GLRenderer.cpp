#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"

#define TORAD(X) ((X)*3.141592/180.0)

//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_camDist = 10;
	m_camAlpha = 45.0;
	m_camBeta = 0.0;
	m_rotZupcanik = 0.0;
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
	m_tex = LoadTexture("T1.jpg");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	GLfloat light_ambient[] = { 1,1,1,1 };
	GLfloat light_diffuse[] = { 1,1,1,1 }; //bela difuzna boja
	GLfloat light_specular[] = { 1,1,1,1 }; //bela specular boja

	GLfloat light0_pos[] = { 0,0,1.0,0 }; //dolazi iz pravca Z i 0 je jer nije direkciono svetlo.
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHT0);

	gluLookAt(
		m_eyeX, m_eyeY, m_eyeZ,
		0, 0, 0,
		0, 1, 0
	);


	GLfloat light1_pos[] = { 0.0, 5.0, -10.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	glEnable(GL_LIGHT1);

	glPushMatrix();

	glTranslatef(0.0f, 5.0f, -10.0f);

		GLfloat emission_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_EMISSION, emission_color);

		glutSolidSphere(0.5, 30, 30);

		GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);

	glPopMatrix();


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
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
	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	DrawWall(100);
	SetMetalicMaterial();
	glRotatef(m_rotZupcanik, 0, 0, 1);
	DrawGear(0.5, 1, 40, 0.5, 0.5);
	glRotatef(m_rotZupcanik, 0, 0, -1);
	glRotatef(30, 0, 0, 1);
	glTranslatef(12, 0, 0);
	glRotatef(-m_rotZupcanik/10, 0, 0, 1);
	DrawGear(0.5, 2, 20, 0.5, 0.5);
	glRotatef(-m_rotZupcanik/10, 0, 0, -1);

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
	gluPerspective(45, (double)w / (double)h, 0.1, 2000);
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
	m_eyeZ = m_camDist * cos(TORAD(m_camAlpha)) * sin(TORAD(m_camBeta));
	m_eyeY = m_camDist * sin(TORAD(m_camAlpha));
}

UINT CGLRenderer::LoadTexture(const char* fileName) {
	UINT texID;
	DImage img;
	img.Load(L"C:\\Users\\stefa\\Downloads\\II_Kol_2017\\" + CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texID;
}

void CGLRenderer::SetMetalicMaterial()
{
	GLfloat ambient[] = { 0.0f,0.0f,0.2f,1.0f };
	GLfloat diffuse[] = { 0.0f,0.0f,0.3f,1.0f };
	GLfloat specular[] = { 1.0,1.0,1.0,1.0 };

	GLfloat shiny = 128.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shiny);

}

void CGLRenderer::DrawGear(double a, double alpha, int n, double h, double d)
{
	double rOut = R(a, alpha, n, h);
	double rInner = rOut - h;

	int breakPoints = n * 4;

	double* px = new double[breakPoints];
	double* py = new double[breakPoints];

	double pitchA = (2.0 * 3.141592) / n;
	double alphaRad = alpha * 3.141592 / 180.0;

	double topA = a / rOut;
	double slopeA = (h * tan(alphaRad)) / rInner;

	int idx = 0;
	for (int i = 0; i < n; i++) {
		double startAngle = i * pitchA;
		
		double a1 = startAngle - topA / 2.0 - slopeA;
		px[idx] = rInner * cos(a1);
		py[idx] = rInner * sin(a1);
		idx++;
		double a2 = startAngle - topA / 2.0;
		px[idx] = rOut * cos(a2);
		py[idx] = rOut * sin(a2);
		idx++;
		double a3 = startAngle + topA / 2.0;
		px[idx] = rOut * cos(a3);
		py[idx] = rOut * sin(a3);
		idx++;
		double a4 = startAngle + topA / 2.0 + slopeA;
		px[idx] = rInner * cos(a4);
		py[idx] = rInner * sin(a4);
		idx++;
	}

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0, 1);
	glVertex3f(0.0, 0, d / 2.0);
	for (int i = 0; i <= breakPoints; i++) {
		int k = i % breakPoints;
		glVertex3f(px[k], py[k], d / 2.0);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0, -1);
	glVertex3f(0.0, 0, -d / 2.0);
	for (int i = 0; i <= breakPoints; i++) {
		int k = i % breakPoints;
		glVertex3f(px[k], py[k], -d / 2.0);
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int i = 0; i < breakPoints; i++) {
		int next = (i + 1) % breakPoints;

		double nx, ny, nz;
		NormCrossProd(
			px[next] - px[i], py[next] - py[i], 0.0,
			0.0, 0.0, -d,
			nx, ny, nz
		);

		glNormal3f(nx, ny, nz);

		glVertex3f(px[i], py[i], d / 2.0);
		glVertex3f(px[next], py[next], d / 2.0);
		glVertex3f(px[next], py[next], -d / 2.0);
		glVertex3f(px[i], py[i], -d / 2.0);
	}
	glEnd();

	delete[] px;
	delete[] py;
}

void CGLRenderer::NormCrossProd(double x1, double y1, double z1,
	double x2, double y2, double z2,
	double& x, double& y, double& z)
{
	x = y1 * z2 - z1 * y2;
	y = z1 * x2 - x1 * z2;
	z = x1 * y2 - y1 * x2;

	double length = sqrt(x * x + y * y + z * z);
	if (length > 1e-5)
	{
		x /= length;
		y /= length;
		z /= length;
	}
}

double CGLRenderer::R(double a, double alpha, int n, double h)
{
	double obim = n * (a * 2.5);
	double rOuter = obim / (2.0 * 3.14159265);
	return rOuter + h;
}

void CGLRenderer::DrawWall(double a)
{
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a / 2, -a / 2);

	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a / 2, a / 2);

	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a / 2, a / 2);

	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a / 2, -a / 2);

	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, -a / 2, -a / 2);

	glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a / 2, -a / 2);
	glEnd();


	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}