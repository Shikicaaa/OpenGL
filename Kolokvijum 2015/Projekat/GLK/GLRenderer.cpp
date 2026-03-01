#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_camRotAlfa = 0.5;
	m_camRotBeta = 0.5;
	m_camDist = 10.0;
	m_moonRot = 0.0;
	m_bLight = false;
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
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	m_tsc[0] = LoadTexture("TSC0.jpg");
	m_tsc[1] = LoadTexture("TSC1.jpg");
	m_tsc[2] = LoadTexture("TSC2.jpg");
	m_tsc[3] = LoadTexture("TSC3.jpg");
	m_tsc[4] = LoadTexture("TSC4.jpg");
	m_tsc[5] = LoadTexture("TSC5.jpg");
	m_m[0] = LoadTexture("M0.jpg");
	m_m[1] = LoadTexture("M1.jpg");
	m_m[2] = LoadTexture("M2.jpg");
	m_m[3] = LoadTexture("M3.jpg");
	m_m[4] = LoadTexture("M4.jpg");
	m_m[5] = LoadTexture("M5.jpg");
	m_s[0] = LoadTexture("S0.jpg");
	m_s[1] = LoadTexture("S1.jpg");
	m_s[2] = LoadTexture("S2.jpg");
	m_s[3] = LoadTexture("S3.jpg");
	m_s[4] = LoadTexture("S4.jpg");
	m_s[5] = LoadTexture("S5.jpg");
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

	//glPushMatrix();
	//glDisable(GL_DEPTH_TEST);
	//glTranslatef(m_eyeX, m_eyeY, m_eyeZ);
	;
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	DrawSpace(60, 20);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);


	//glEnable(GL_DEPTH_TEST);
	//glPopMatrix();

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(10, 0, 0);
	glVertex3f(0, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);
	glEnd();

	if (m_bLight) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	float light_ambient[] = { 0,0,0,1.0 };
	float light_diffuse[] = { 1.0,1.0,1.0,1.0 };
	float light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	GLfloat light_position[] = { 0,0,1,0 };

	//glEnable(GL_DEPTH_TEST);

	DrawEarth(3, 20);

	glPushMatrix();
	glRotatef(m_moonRot, 0, 1, 0);
	glTranslatef(50.0f, 0, 0);
	DrawMoon(1, 20);
	glPopMatrix();

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
	glDeleteTextures(6, m_tsc);
	glDeleteTextures(6, m_m);
	glDeleteTextures(6, m_s);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	// Ucitavanje u memoriju
	UINT texID;
	DImage img;
	img.Load(L"C:\\Users\\stefa\\Downloads\\II_kol_2015\\" + CString(fileName));
	//Generisanje ID-ja
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Podesavanje pravila
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return texID;


}
void CGLRenderer::UpdateCamera()
{
	const double pi2 = 3.1415/2;
	if (m_camRotAlfa > pi2) m_camRotAlfa = pi2;
	if (m_camRotAlfa < -pi2) m_camRotAlfa = -pi2;
	m_eyeX = m_camDist * cos(m_camRotAlfa) * cos(m_camRotBeta);
	m_eyeY = m_camDist * sin(m_camRotAlfa);
	m_eyeZ = m_camDist * cos(m_camRotAlfa) * sin(m_camRotBeta);
}

void CGLRenderer::DrawPatch(double R, int n)
{
	/*
	fi = arctgx
	teta=arctg(y*cosfi)

	iz polarnih u dekartove:
	x = Rcos(theta)sin(fi)
	z = Rcos(Theta)cos(fi)
	y = Rsin(theta)
	
	*/
	double delta = 2.0 / (double)n;
	double y = 1.0;
	for (int i = 0; i < n; i++) {
		glBegin(GL_TRIANGLE_STRIP);
		double x = -1.0;
		for (int j = 0; j <= n; j++) {
			double phi, theta;
			phi = atan(x);
			theta = atan(y * cos(phi));

			double xd, yd, zd;

			xd = R * cos(theta) * sin(phi);
			yd = R * sin(theta);
			zd = R * cos(theta) * cos(phi);

			glNormal3f(xd / R, yd / R, zd / R);
			glTexCoord2f((x + 1.0) / 2, (-y + 1.0) / 2);
			glVertex3f(xd, yd, zd);

			phi = atan(x);
			theta = atan((y - delta) * cos(phi));

			xd = R * cos(theta) * sin(phi);
			yd = R * sin(theta);
			zd = R * cos(theta) * cos(phi);

			glNormal3f(xd / R, yd / R, zd / R);
			glTexCoord2f((x + 1.0) / 2, (-y + delta + 1.0) / 2);
			glVertex3f(xd, yd, zd);

			x += delta;
		}
		glEnd();
		y -= delta;
	}
}

void CGLRenderer::DrawEarth(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, m_tsc[i]);
		DrawPatch(R, tes);
		glRotatef(90.0, 0.0, 1.0, 0.0);
	}
	glPopMatrix();
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, m_tsc[5]);
	glRotatef(90, 1, 0.0, 0.0);
	DrawPatch(R, tes);
	glBindTexture(GL_TEXTURE_2D, m_tsc[4]);
	glRotatef(-180, 1, 0.0, 0.0);
	DrawPatch(R, tes);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawSpace(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, m_s[i]);
		DrawPatch(R, tes);
		glRotatef(90.0, 0.0, 1.0, 0.0);
	}
	glPopMatrix();
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, m_s[5]);
	glRotatef(90, 1, 0.0, 0.0);
	DrawPatch(R, tes);
	glBindTexture(GL_TEXTURE_2D, m_s[4]);
	glRotatef(-180, 1, 0.0, 0.0);
	DrawPatch(R, tes);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
void CGLRenderer::DrawMoon(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glPushMatrix();
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, m_m[i]);
		DrawPatch(R, tes);
		glRotatef(90.0, 0.0, 1.0, 0.0);
	}
	glPopMatrix();
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, m_m[5]);
	glRotatef(90, 1, 0.0, 0.0);
	DrawPatch(R, tes);
	glBindTexture(GL_TEXTURE_2D, m_m[4]);
	glRotatef(-180, 1, 0.0, 0.0);
	DrawPatch(R, tes);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}