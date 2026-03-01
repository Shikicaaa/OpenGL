#pragma once

class CGLRenderer
{
public:
	double m_eyeX;
	double m_eyeY;
	double m_eyeZ;
	double m_camDist;
	double m_rotRuka1;
	double m_rotRuka2;
	double m_rotGlava;
	double m_camAlpha;
	double m_camBeta;

	UINT m_texEnv[7];
	UINT m_texLamp;
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	void UpdateCamera();

	void DrawAxes();

	UINT LoadTexture(char* fileName);

	void DrawEnvCube(double a);

	void DrawCylinder(double r1, double r2, double h, int nSeg, int texMode, bool bIsOpen);
	void DrawLampBase();
	void DrawLampHead();
	void DrawLampArm();
	void DrawLamp();
protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 


};
