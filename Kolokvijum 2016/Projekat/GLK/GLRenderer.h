#pragma once

class CGLRenderer
{
public:
	double m_camDist;
	double m_eyeX, m_eyeY, m_eyeZ;
	double m_camAlpha, m_camBeta;
	double m_rotKrila;

	UINT m_texEnv;
	UINT m_texBrick;
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,
	void UpdateCamera();
	UINT LoadTexture(char* fileName);
	void DrawCube(double a);
	void DrawTube(double r1, double r2, double h, int n);
	void DrawCone(double r, double h, int n);
	void DrawPaddle(double length, double width);
protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
