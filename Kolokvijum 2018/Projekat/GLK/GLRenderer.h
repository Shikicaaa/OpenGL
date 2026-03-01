#pragma once

class CGLRenderer
{

public:
	double m_eyex;
	double m_eyeY;
	double m_eyeZ;
	double m_rotAngleAlfa;
	double m_rotAngleBeta;
	double m_camDist;
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	UINT LoadTexture(char* fileName);

	void DrawTriangle(float d1, float d2, float rep);

	void DrawShip();

	void DrawSpaceCube(double a);

	void UpdateCamera();


public:
	UINT m_texShip;
	UINT m_texSpace[6];
	bool m_enableLight;

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
