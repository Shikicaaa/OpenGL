#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"

//#pragma comment(lib, "GL\\glut32.lib")

#define ToRad(x)  ((x)*3.141592/180.0)

CGLRenderer::CGLRenderer(void)
{
	m_rotAlpha = 0.0;
	m_rotBeta = 0.0;
	m_camDist = 10;
	m_flowerAngle = 0;
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

	m_env = LoadTexture("Env.jpg");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	GLfloat global_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	GLfloat light0_pos[] = { 0.0f,0.0f,1.0f, 0.0f };
	GLfloat light0_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light0_diff_spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diff_spec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_diff_spec);
	glEnable(GL_LIGHT0);

	gluLookAt(
		m_eyeX, m_eyeY, m_eyeZ,
		0, 0, 0,
		0, 1, 0
	);

	GLfloat light1_pos[] = { 0.0,1.0,1.0,0.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light0_diff_spec);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light0_diff_spec);
	glEnable(GL_LIGHT1);


	glPushMatrix();
	glTranslatef(m_eyeX, m_eyeY, m_eyeZ);
	glDisable(GL_DEPTH_TEST);
	DrawSphere(1, 20, 20);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	DrawAxes();
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	DrawFlower();
	glTranslatef(9, 0, 9);
	DrawFlower();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(9, 0,-9);
	DrawFlower();
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
	gluPerspective(50, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &m_env);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{
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

void CGLRenderer::UpdateCamera()
{
	const double pi2 = 3.1415/2.0;

	m_eyeX = m_camDist * cos(ToRad(m_rotBeta)) * cos(ToRad(m_rotAlpha));
	m_eyeY = m_camDist * sin(ToRad(m_rotAlpha));
	m_eyeZ = m_camDist * cos(ToRad(m_rotAlpha)) * sin(ToRad(m_rotBeta));
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;

	img.Load(L"C:\\Users\\stefa\\Downloads\\II kolokvijum 2024\\" + CString(fileName));

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

void CGLRenderer::PolarToCartesian(double R, double phi, double theta, double& x, double& y, double& z)
{
	x = R * cos(theta) * sin(phi);
	y = R * sin(theta);
	z = R * cos(theta) * cos(phi);
}

void CGLRenderer::DrawSphere(float R, int n, int m)
{
	double betaSeg = 360.0 / n;
	double alphaSeg = 180.0 / m;

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, m_env);

	for (double i = -90.0; i < 90.0; i += alphaSeg) {
		glBegin(GL_QUAD_STRIP);
		for (double j = 0.0; j <= 360.0; j += betaSeg) {
			double x, y, z;

			PolarToCartesian(R, ToRad(j), ToRad(i), x, y, z);

			double tu = j / 360.0;
			double tv = 1.0-((i + 90.0) / 180.0);
			glTexCoord2d(tu, tv);
			glVertex3f(x, y, z);

			PolarToCartesian(R, ToRad(j), ToRad(i + alphaSeg), x, y, z);

			double tu_next = j / 360.0;
			double tv_next = 1.0-((i + alphaSeg + 90.0) / 180.0);
			glTexCoord2d(tu_next, tv_next);
			glVertex3f(x, y, z);
		}
		glEnd();
	}

	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawSphFlower(float R, int n, int m, float factor, unsigned
	char R1, unsigned char G1, unsigned char B1,
	unsigned char R2, unsigned char G2, unsigned char B2
)
{
	double alphaSeg = 180.0 / m;
	double betaSeg = 360.0 / n;

	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	float ambient[] = { (R1 * 0.2f) / 255.0f, (G1 * 0.2f) / 255.0f, (B1 * 0.2f) / 255.0f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

	float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	for (double j = 0.0; j < 360.0; j += betaSeg) {

		glPushMatrix();

		for (double i = -90; i <= 90.01 - alphaSeg; i += alphaSeg) {

			double axis_lat = (i == -90.0) ? (-90.0 + alphaSeg) : i;
			double x1, y1, z1, x2, y2, z2, ax, ay, az;
			PolarToCartesian(R, ToRad(j), ToRad(axis_lat), x1, y1, z1);
			PolarToCartesian(R, ToRad(j + betaSeg), ToRad(axis_lat), x2, y2, z2);
			CalcRotAxis(x1, y1, z1, x2, y2, z2, ax, ay, az);

			double px, py, pz;
			PolarToCartesian(R, ToRad(j), ToRad(i), px, py, pz);

			glTranslatef(px, py, pz);

			double rot_angle = m_flowerAngle * factor;
			if (rot_angle > alphaSeg) rot_angle = alphaSeg;

			glRotatef(rot_angle, ax, ay, az);
			glTranslatef(-px, -py, -pz);

			glBegin(GL_QUADS);

			double t1 = (i + 90.0) / 180.0f;
			float r_diff1 = (R1 * (1.0 - t1) + R2 * t1) / 255.0f;
			float g_diff1 = (G1 * (1.0 - t1) + G2 * t1) / 255.0f;
			float b_diff1 = (B1 * (1.0 - t1) + B2 * t1) / 255.0f;
			glColor3f(r_diff1, g_diff1, b_diff1);

			double x, y, z;

			PolarToCartesian(R, ToRad(j), ToRad(i), x, y, z);
			glNormal3f(x / R, y / R, z / R);
			glVertex3f(x, y, z);

			PolarToCartesian(R, ToRad(j + betaSeg), ToRad(i), x, y, z);
			glNormal3f(x / R, y / R, z / R);
			glVertex3f(x, y, z);

			double t2 = (i + alphaSeg + 90.0) / 180.0f;
			float r_diff2 = (R1 * (1.0 - t2) + R2 * t2) / 255.0f;
			float g_diff2 = (G1 * (1.0 - t2) + G2 * t2) / 255.0f;
			float b_diff2 = (B1 * (1.0 - t2) + B2 * t2) / 255.0f;
			glColor3f(r_diff2, g_diff2, b_diff2);

			PolarToCartesian(R, ToRad(j + betaSeg), ToRad(i + alphaSeg), x, y, z);
			glNormal3f(x / R, y / R, z / R);
			glVertex3f(x, y, z);

			PolarToCartesian(R, ToRad(j), ToRad(i + alphaSeg), x, y, z);
			glNormal3f(x / R, y / R, z / R);
			glVertex3f(x, y, z);

			glEnd();
		}

		glPopMatrix();
	}

	glDisable(GL_COLOR_MATERIAL);
}

void CGLRenderer::CalcRotAxis(double x1, double y1, double z1, double x2,
	double y2, double z2, double& x3, double& y3, double& z3)
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	double dz = z2 - z1;
	double d = sqrt(dx*dx+dy*dy+dz*dz);
	if (d == 0) {
		x3 = x2;
		y3 = y2;
		z3 = z2;
	}
	else {
		x3 = dx / d;
		y3 = dy / d;
		z3 = dz / d;
	}
}

void CGLRenderer::DrawFlower()
{
	double r_spolja = 2;
	double r_srednja = r_spolja*0.75;
	double r_unutra = r_spolja / 2;

	//Unutrasnji
	DrawSphFlower(r_unutra, 36, 18, 0.25, 192, 192, 0, 192, 192, 192);

	//Srednji
	DrawSphFlower(r_srednja, 36, 18, 0.5, 0, 0, 255, 255, 0, 0);

	//Spoljna
	DrawSphFlower(r_spolja, 36, 18, 1, 0, 192, 0, 0, 255, 0);
}