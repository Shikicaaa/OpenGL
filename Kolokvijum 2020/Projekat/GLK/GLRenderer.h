#pragma once

class CGLRenderer
{
public:
	UINT m_texSpider;
	UINT m_texEnv[6];
	double m_cameraR;
	double m_cameraAngleXY;
	double m_cameraAngleXZ;
	double m_eyeX;
	double m_eyeY;
	double m_eyeZ;
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,
	void DrawAxes();
	UINT LoadTexture(char* fileName);
	void DrawEnvCube(double a);
	void UpdateCameraPosition();
	void RotateView(double dx, double dy);
	void ZoomView(double dx);
	void DrawSphere(double r, int nSeg, double texU, double texV, double texR);
	void DrawCone(double r, double h, int nSeg, double texU, double texV, double texR);
	void DrawLegSegment(double r, double h, int nSeg);
	void DrawLeg();
	void DrawSpiderBody();
	void DrawSpider();
 protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
