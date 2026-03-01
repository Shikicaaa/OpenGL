#pragma once

class CGLRenderer
{
public:
	double m_eyeX;
	double m_eyeY;
	double m_eyeZ;
	double m_camDist;
	double m_camRotAlfa;
	double m_camRotBeta;

	double m_moonRot;
	bool m_bLight;

	UINT m_tsc[6];
	UINT m_m[6];
	UINT m_s[6];
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	UINT LoadTexture(char* fileName);

	void UpdateCamera();

	void DrawPatch(double R, int n);

	void DrawEarth(double R, int tes);

	void DrawSpace(double R, int tes);

	void DrawMoon(double R, int tes);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
