#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"

#define TORAD(X) ((X)*3.141592/180)
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_camDist = 10.0;
	m_camAlpha = 0.0;
	m_camBeta = 0.0;
	m_rotKabina = 0.0;
	m_rotRuka1 = 45.0;
	m_rotRuka2 = 45.0;
	m_rotViljuska = 0.0;
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

	m_texExcavator = LoadTexture("excavator.png");
	m_texEnv[0] = LoadTexture("front.jpg");
	m_texEnv[1] = LoadTexture("left.jpg");
	m_texEnv[2] = LoadTexture("back.jpg");
	m_texEnv[3] = LoadTexture("right.jpg");
	m_texEnv[4] = LoadTexture("bot.jpg");
	m_texEnv[5] = LoadTexture("top.jpg");
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
		0, 2, 0,
		0, 1, 0
	);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(10, 0, 0);
	glVertex3f(0, 0, 0);
	glColor3f(1, 0, 0);
	glVertex3f(0, 10, 0);
	glVertex3f(0, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 10);
	glVertex3f(0, 0, 0);
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	//DrawPolygon();
	//DrawBase();
	//DrawBody();
	/*glPushMatrix();
	glTranslatef(2, 2, 0);
	glRotatef(m_rotRuka1, 0, 0, 1);
	glTranslatef(-2, -2, 0);
	DrawArm(1);
	glPopMatrix();*/
	//DrawFork();

	DrawEnvCube(100);
	DrawExcavator();


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
	gluPerspective(55, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	glDeleteTextures(1, &m_texExcavator);
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
	m_eyeX = m_camDist * cos(TORAD(m_camAlpha)) * cos(TORAD(m_camBeta));
	m_eyeY = m_camDist * sin(TORAD(m_camAlpha));
	m_eyeZ = m_camDist * cos(TORAD(m_camAlpha)) * sin(TORAD(m_camBeta));
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(L"C:\\Users\\stefa\\Downloads\\II_Kol_2019\\" + CString(fileName));

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

void CGLRenderer::DrawPolygon(POINTF* points, POINTF* texCoords, int n)
{
	glBindTexture(GL_TEXTURE_2D, m_texExcavator);
	//glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1, 1, 1);
	for (int i = 0; i < n; i++) {
		glTexCoord2f(texCoords[i].x, texCoords[i].y);
		glVertex3f(points[i].x, points[i].y, 0);
	}
	glEnd();
	glEnable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawExtrudedPolygon(POINTF* points,
	POINTF* texCoords, int n, float zh, float r, float g, float b)
{
	glPushMatrix();
	DrawPolygon(points, texCoords, n);
	glTranslatef(0, 0, zh);
	DrawPolygon(points, texCoords, n);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_QUAD_STRIP);
	glColor3f(1, 1, 1);
	glColor3f(r, g, b);
	for (int i = 0; i <= n; i++)
	{
		int idx = i % n;
		glVertex3f(points[idx].x, points[idx].y, 0);
		glVertex3f(points[idx].x, points[idx].y, zh);
	}

	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawBase()
{
	double const texSeg = 1.0 / 16.0;
	POINTF koordinate[] = {
		{1,0},
		{15,0},
		{16,-1},
		{16,-3},
		{15,-4},
		{1,-4},
		{0,-3},
		{0,-1}
	};
	POINTF texKoordinate[] = {
		{1 * texSeg,12 * texSeg},
		{15 * texSeg,12 * texSeg},
		{1, 13 * texSeg},
		{1, 15 * texSeg },
		{15 * texSeg, 1},
		{1*texSeg, 1},
		{0, 15*texSeg},
		{0,13*texSeg}
	};

	DrawExtrudedPolygon(koordinate, texKoordinate, 8, 5, 0, 0, 0);
}

void CGLRenderer::DrawBody()
{
	double const texSeg = 1.0 / 16.0;
	POINTF koordinate[] = {
		{-4, -4},
		{4,  -4},
		{4,   0},
		{0,   4},
		{-4,  4}
	};
	POINTF texKoordinate[] = {
		{8 * texSeg, 8*texSeg},
		{1, 8*texSeg},
		{1, 4 * texSeg},
		{12 * texSeg, 0},
		{8 * texSeg, 0}
	};
	DrawExtrudedPolygon(koordinate, texKoordinate, 5, 4, 0.96, 0.5, 0.12);

}

void CGLRenderer::DrawArm(double zh)
{
	double const texSeg = 1.0 / 16.0;
	POINTF koordinate[] = {
		{1,0},
		{16,1},
		{16,3},
		{1,4},
		{0,3},
		{0,1}
	};
	POINTF texKoordinate[] = {
		{texSeg, 12 * texSeg},
		{1, 11 * texSeg},
		{1, 9 * texSeg},
		{texSeg, 8 * texSeg},
		{0, 9 * texSeg},
		{0,11 * texSeg}
	};

	DrawExtrudedPolygon(koordinate, texKoordinate, 6, zh, 0.96, 0.5, 0.12);
}
void CGLRenderer::DrawFork()
{
	const double texSeg = 1.0 / 16;
	POINTF koord[] = {
		{0,0},
		{8,0},
		{8,5},
		{7,6},
		{1,6},
		{0,5}
	};

	POINTF texKoordinate[] = {
		{0, 6 * texSeg},
		{8 * texSeg, 6 * texSeg},
		{8 * texSeg, 1 * texSeg},
		{7 * texSeg, 0},
		{texSeg, 0},
		{0,1*texSeg}
	};

	DrawExtrudedPolygon(koord, texKoordinate, 6, 1, 0.7, 0.7, 0.7);

}
void CGLRenderer::DrawExcavator()
{
	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);
	glTranslatef(-8, 4, 0);
	DrawBase();
	
	glTranslatef(8, 4, 0.5);
	
	glTranslatef(0, 0, 2);
	glRotatef(m_rotKabina, 0, 1, 0);
	glTranslatef(0, 0, -2);
	
	DrawBody();

	glTranslatef(0, -2, 1.5);
	glRotatef(m_rotRuka1, 0, 0, 1);
	glTranslatef(-1, -2, 0);
	DrawArm(1);
	glTranslatef(13, 0, -0.25);

	glTranslatef(2, 2, 0);
	glRotatef(m_rotRuka2, 0, 0, 1);
	glTranslatef(-2, -2, 0);

	DrawArm(1.5);

	glTranslatef(13, 0, 0);
	glTranslatef(2, 2, 0);
	glRotatef(m_rotViljuska, 0, 0, 1);
	glTranslatef(-2, -2, 0);
	DrawFork();

	glPopMatrix();
}

void CGLRenderer::DrawEnvCube(double a)
{
	// first
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, m_texEnv[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, a / 2);
	glEnd();

	// right
	glBindTexture(GL_TEXTURE_2D, m_texEnv[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, 0, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, -a / 2);
	glEnd();

	// left
	glBindTexture(GL_TEXTURE_2D, m_texEnv[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a, a / 2);
	glEnd();

	// back

	glBindTexture(GL_TEXTURE_2D, m_texEnv[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, a, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, 0, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, 0, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a, -a / 2);
	glEnd();

	// bot

	glBindTexture(GL_TEXTURE_2D, m_texEnv[4]);
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

	// top

	glBindTexture(GL_TEXTURE_2D, m_texEnv[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, a, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, a, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, a / 2);

	glEnd();
	
	glEnable(GL_LIGHTING);
	//glDisable(GL_TEXTURE_2D);

}