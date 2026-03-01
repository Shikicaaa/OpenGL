#pragma once

class CGLRenderer
{
public:
	double m_eyeX;
	double m_eyeY;
	double m_eyeZ;
	double m_camDist;
	double m_camAlpha;
	double m_camBeta;
	double m_rotKabina;
	double m_rotRuka1;
	double m_rotRuka2;
	double m_rotViljuska;

	UINT m_texExcavator;
	UINT m_texEnv[6];
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

	void DrawPolygon(POINTF* points, POINTF* texCoords, int n);

	void DrawExtrudedPolygon(POINTF* points, POINTF* texCoords, int n, float zh, float r, float g, float b);

	void DrawBase();

	void DrawBody();

	void DrawArm(double zh);

	void DrawFork();

	void DrawExcavator();

	void DrawEnvCube(double a);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

};
