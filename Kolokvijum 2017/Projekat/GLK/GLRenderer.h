#pragma once

class CGLRenderer
{
public:
	double m_camAlpha;
	double m_camBeta;
	double m_camDist;
	double m_eyeX, m_eyeY, m_eyeZ;
	double m_rotZupcanik;

	UINT m_tex;
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	void UpdateCamera();

	UINT LoadTexture(const char* fileName);

	void SetMetalicMaterial();

	void DrawGear(double a, double alpha, int n, double h, double d);

	void NormCrossProd(double x1, double y1, double z1, double x2, double y2, double z2, double& x, double& y, double& z);

	double R(double a, double alpha, int n, double h);

	void DrawWall(double a);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
