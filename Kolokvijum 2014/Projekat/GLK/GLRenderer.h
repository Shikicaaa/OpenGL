#pragma once

class CGLRenderer
{
public:
	double m_camAlpha;
	double m_camBeta;
	double m_camDist;
	double m_eyeX, m_eyeY, m_eyeZ;
	double m_rotGroup1;
	double m_rotGroup2;
	double m_rotGroup3;
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

	void DrawCube(float a, int i, int j, int k, int count);

	void DrawCube(float a);

	void DrawRubikCube(double a, int count);

	void LoadTexture();

	void SetWhiteLight();

	void SetMaterial(float r, float g, float b);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
