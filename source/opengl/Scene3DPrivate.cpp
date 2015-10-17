
#include "../AppSetup.h"

#include "Scene3DPrivate.h"

//---------------------------------------------------------------------

namespace
{	
	GLfloat openglColorWhite[4]		= {1.0f,1.0f,1.0f,1.0f};

	GLfloat openglColorDiffuse[4]	= {1.f,1.f,1.f,1.0f};
	GLfloat openglColorSpecular[4]	= {1.0f,1.0f,1.0f,1.0f};
	GLfloat openglColorAmbient[4]	= {0.5f,0.5f,0.5f,1.0f};

	void calculateAndSetNormals(float* tmp_u, float* tmp_v)
	{
		glNormal3f(
			tmp_u[1] * tmp_v[2] - tmp_u[2] * tmp_v[1],
			tmp_u[2] * tmp_v[0] - tmp_u[0] * tmp_v[2],
			tmp_u[0] * tmp_v[1] - tmp_u[1] * tmp_v[0]
			);
	}
}

//---------------------------------------------------------------------

Scene3DPrivate::Scene3DPrivate():angleDebugFPSVision(0)
{
	Assert(GL_MAX_LIGHTS >= MAX_LIGHTS);
	//------------
	//misc

	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);//DONT_CARE);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//GL_DONT_CARE);
	//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);//GL_DONT_CARE);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);//GL_DONT_CARE);
	
    quadric = (void*)gluNewQuadric();
    gluQuadricTexture((GLUquadricObj*)quadric, GL_TRUE);
    gluQuadricDrawStyle((GLUquadricObj*)quadric, GLU_FILL);
    gluQuadricOrientation((GLUquadricObj*)quadric, GLU_INSIDE);

    glMaterialfv(GL_FRONT, GL_DIFFUSE, openglColorDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, openglColorSpecular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, openglColorAmbient);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);

	// Setup And Enable glFogCoordEXT
	//glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC) wglGetProcAddress("glFogCoordfEXT");
	//glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);

	glShadeModel(GL_SMOOTH);

	//------------
	//light

	/*float maxLights;
	glGetFloatv(GL_MAX_LIGHTS, &maxLights);
	PrintValue(maxLights);
	Assert(maxLights > 1);*/
	
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, openglColorAmbient);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	
	//glEnable(GL_COLOR_MATERIAL);

	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		m_lightPosition[i] = Float4(0.f,0.f,0.f,1.f);
		m_lightIsEnable[i] = false;
		m_lightIntensity[i] = 1.f;
	}
	
	for (int i=0; i<GL_MAX_LIGHTS; ++i)
        glDisable(GL_LIGHT0 + i);
	
	for (int i = 0; i < 16; ++i)
		eyeMatrix[i] = 0;

	//this->configureLight(0, true, Float3(100,100,100), 1000);

	//glutInitContextProfile(GLUT_CORE_PROFILE);
	
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  //Problem: glewInit failed, something is seriously wrong.
		Utils::dieErrorMessageToUser((const char*)glewGetErrorString(err));
	}

	//AssertMessage(glewIsSupported("GL_VERSION_1_3"), "Not supported: GL_VERSION_1_3");
	AssertMessage(glewIsSupported("GL_EXT_multi_draw_arrays"), "Not supported: GL_EXT_multi_draw_arrays");

	const GLubyte* vendor   = glGetString (GL_VENDOR);
	const GLubyte* renderer = glGetString (GL_RENDERER);
	const GLubyte* version  = glGetString (GL_VERSION);
	const GLubyte* glsl_ver = glGetString (GL_SHADING_LANGUAGE_VERSION);
	const GLubyte* gl_extensions = glGetString (GL_EXTENSIONS);

	outputln("Vendor/renderer: " << vendor << " - " << renderer);
	outputln("OpenGL version: " << version);
	if (glsl_ver == NULL)
	{
		outputln("No support for GLSL");
	}
	else
	{
		outputln("GLSL: " << glsl_ver);
	}
	outputln("Supported extensions: " << gl_extensions);
}

//---------------------------------------------------------------------

bool Scene3DPrivate::areShadersSupported() const { return glGetString (GL_SHADING_LANGUAGE_VERSION) != NULL; }

//---------------------------------------------------------------------

void Scene3DPrivate::setRenderOppositeFace(bool yesNo)
{
	glCullFace(yesNo ? GL_FRONT : GL_BACK);
	//if (yesNo){glPolygonMode(GL_BACK, GL_FILL);glPolygonMode(GL_FRONT, GL_LINE);}
	//else{glPolygonMode(GL_BACK, GL_LINE);glPolygonMode(GL_FRONT, GL_FILL);}
}

//---------------------------------------------------------------------
// this function is useful because opengl fog is calculated using Z distance to camera, 
// instead of absolute distance

void Scene3DPrivate::adjustFog(float distanceToCameraInWorldCoord, float distanceToCameraInCameraCoord)
{		
	//glFogf(GL_FOG_START, (3.f*fogMaxDistance/4.f) * distanceToCameraInCameraCoord/distanceToCameraInWorldCoord);
	//glFogf(GL_FOG_END, fogMaxDistance * distanceToCameraInCameraCoord/distanceToCameraInWorldCoord);
	;//TODO
}

bool Scene3DPrivate::isFogEnabled() const
{
	return true;
	/*GLboolean val;
	glGetBooleanv(GL_FOG, &val);
	return (bool)(val != 0);*/
}

//---------------------------------------------------------------------

void Scene3DPrivate::setFogDisable()
{
	;//TODO
}

//---------------------------------------------------------------------

void Scene3DPrivate::drawCube5FacesWithIntPos(unsigned int textureSidesId, unsigned int textureTopId, unsigned int textureBottomId, const BoundingBoxes::AdvancedBox& box, int depth, bool calculateNormals)
{
	glEnable(GL_TEXTURE_2D);
    //glDisable(GL_AUTO_NORMAL);

    float tmp_u[3];
    float tmp_v[3];

    //---------------
    // Top Facef

    glBindTexture(GL_TEXTURE_2D, (GLuint)textureTopId);
    glBegin(GL_QUADS);

    if (calculateNormals)
    {
        //glNormal3f( 0.0f, 1.0f, 0.0f);
        tmp_u[0] = float(box.getRightTopPoint().x() - box.getLeftTopPoint().x());
        tmp_u[1] = float(box.getRightTopPoint().y() - box.getLeftTopPoint().y());
        tmp_u[2] = float(2*depth);
        tmp_v[0] = float(0);
        tmp_v[1] = float(0);
        tmp_v[2] = float(2*depth);
        calculateAndSetNormals(tmp_u, tmp_v);
    }

    glTexCoord2f(0.0f, 1.0f); glVertex3i(box.getLeftTopPoint().x(), box.getLeftTopPoint().y(), -depth);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3i(box.getLeftTopPoint().x(), box.getLeftTopPoint().y(),  depth);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3i(box.getRightTopPoint().x(), box.getRightTopPoint().y(),  depth);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3i(box.getRightTopPoint().x(), box.getRightTopPoint().y(), -depth);	// Top Right Of The Texture and Quad
    glEnd();

    //---------------
    // Bottom Face

    glBindTexture(GL_TEXTURE_2D, (GLuint)textureBottomId);
    glBegin(GL_QUADS);

    if (calculateNormals)
    {
        //glNormal3f( 0.0f,-1.0f, 0.0f);
        tmp_u[0] = float(box.getRightBottomPoint().x() - box.getLeftBottomPoint().x());
        tmp_u[1] = float(box.getRightBottomPoint().y() - box.getLeftBottomPoint().y());
        tmp_u[2] = float(2*depth);
        tmp_v[0] = float(box.getRightBottomPoint().x() - box.getLeftBottomPoint().x());
        tmp_v[1] = float(box.getRightBottomPoint().y() - box.getLeftBottomPoint().y());
        tmp_v[2] = float(0);
        calculateAndSetNormals(tmp_u, tmp_v);
    }
    glTexCoord2f(1.0f, 1.0f); glVertex3i(box.getLeftBottomPoint().x(), box.getLeftBottomPoint().y(), -depth);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3i(box.getRightBottomPoint().x(), box.getRightBottomPoint().y(), -depth);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3i(box.getRightBottomPoint().x(), box.getRightBottomPoint().y(), depth);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3i(box.getLeftBottomPoint().x(), box.getLeftBottomPoint().y(), depth);	// Bottom Left Of The Texture and Quad
    glEnd();

    glBindTexture(GL_TEXTURE_2D, (GLuint)textureSidesId);
    glBegin(GL_QUADS);

    //---------------
    // Front Face
    if (calculateNormals)
        glNormal3f( 0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3i(box.getLeftBottomPoint().x(), box.getLeftBottomPoint().y(),  depth);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3i(box.getRightBottomPoint().x(), box.getRightBottomPoint().y(),  depth);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3i(box.getRightTopPoint().x(), box.getRightTopPoint().y(),  depth);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3i(box.getLeftTopPoint().x(), box.getLeftTopPoint().y(),  depth);	// Top Left Of The Texture and Quad

    //---------------
    // Right face
    if (calculateNormals)
    {
        //glNormal3f( 1.0f, 0.0f, 0.0f);
        tmp_u[0] = float(box.getRightTopPoint().x() - box.getRightBottomPoint().x());
        tmp_u[1] = float(box.getRightTopPoint().y() - box.getRightBottomPoint().y());
        tmp_u[2] = float(2 * depth);
        tmp_v[0] = float(box.getRightTopPoint().x() - box.getRightBottomPoint().x());
        tmp_v[1] = float(box.getRightTopPoint().y() - box.getRightBottomPoint().y());
        tmp_v[2] = float(0);
        calculateAndSetNormals(tmp_u, tmp_v);
    }
    glTexCoord2f(1.0f, 1.0f); glVertex3i(box.getRightBottomPoint().x(), box.getRightBottomPoint().y(), -depth);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3i(box.getRightTopPoint().x(), box.getRightTopPoint().y(), -depth);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3i(box.getRightTopPoint().x(), box.getRightTopPoint().y(), depth);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3i(box.getRightBottomPoint().x(), box.getRightBottomPoint().y(), depth);	// Bottom Left Of The Texture and Quad

    //---------------
    // Left Face
    if (calculateNormals)
    {
        //glNormal3f(-1.0f, 0.0f, 0.0f);
        tmp_u[0] = float(box.getLeftTopPoint().x() - box.getLeftBottomPoint().x());
        tmp_u[1] = float(box.getLeftTopPoint().y() - box.getLeftBottomPoint().y());
        tmp_u[2] = float(2 * depth);
        tmp_v[0] = float(0);
        tmp_v[1] = float(0);
        tmp_v[2] = float(2 * depth);
        calculateAndSetNormals(tmp_u, tmp_v);
    }
    glTexCoord2f(0.0f, 1.0f); glVertex3i(box.getLeftBottomPoint().x(), box.getLeftBottomPoint().y(), -depth);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3i(box.getLeftBottomPoint().x(), box.getLeftBottomPoint().y(), depth);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3i(box.getLeftTopPoint().x(), box.getLeftTopPoint().y(), depth);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3i(box.getLeftTopPoint().x(), box.getLeftTopPoint().y(), -depth);	// Top Left Of The Texture and Quad
    glEnd();
}

//---------------------------------------------------------------------

void Scene3DPrivate::drawCylinder(unsigned int textureId, float width, float height, int subDiv1, int subDiv2)
{
	glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, (GLuint)textureId);
    gluCylinder((GLUquadricObj*)quadric, width*2, width*2, height, subDiv1, subDiv2);
	glPopMatrix();
}

//---------------------------------------------------------------------

void Scene3DPrivate::updateCameraSyncronisedWith2D(const Int2& virtualWindowSize, Camera& camera)
{
	Assert(false);
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(camera.getFieldOfView(),(GLfloat)virtualWindowSize.width()/(GLfloat)virtualWindowSize.height(),0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	float widthWindow = (float)virtualWindowSize.width();
    float heightWindow = (float)virtualWindowSize.height();

    //gluPerspective(90.f, (widthWindow/heightWindow));
	glTranslatef(-camera.getPosition().x(),-camera.getPosition().y(),-camera.getPosition().z());
    
    glScalef(0.1f * 768.f * (widthWindow/heightWindow) / widthWindow,-0.1f * 768.f / heightWindow, 0.036f);
    glTranslatef(-widthWindow/2,-heightWindow/2,-1000.f);

    m_lightPosition[0].data[0] = 0.70710678f;//widthWindow;
    m_lightPosition[0].data[1] = -0.70710678f;
    m_lightPosition[0].data[2] = 110.f;
    m_lightPosition[0].data[3] = 0;

    float norm = 0.f;
    for (int i = 0; i < 3; ++i)
        norm += m_lightPosition[0].data[i];
    for (int i = 0; i < 3; ++i)
        m_lightPosition[0].data[i] /= norm;

    /*lightDirection[0] = -0.70710678f;//-sqrt(2)/2
    lightDirection[1] = 0.70710678f;//sqrt(2)/2
    lightDirection[2] = 0;*/
	
	nbTrianglesDrawn = 0;
}

//---------------------------------------------------------------------
//if parIntensity == 0.f: light has no attenuation

void Scene3DPrivate::configureLight(int idLight, bool enable, const Float3& pos, float parIntensity)
{
	m_lightPosition[idLight].data[0] = pos.x();
	m_lightPosition[idLight].data[1] = pos.y();
	m_lightPosition[idLight].data[2] = pos.z();

	m_lightIntensity[idLight] = parIntensity;

	if (enable) glEnable(GL_LIGHT0 + idLight);
	else glDisable(GL_LIGHT0 + idLight);
	m_lightIsEnable[idLight] = enable;
}

//---------------------------------------------------------------------

void Scene3DPrivate::drawFaceWithIntPos(unsigned int textureId, const BoundingBoxes::AdvancedBox& box, bool mirrorX, bool mirrorY)
{
	float X0 = (float)(int)!mirrorX;
    float Y0 = (float)(int)!mirrorY;
    float X1 = (float)(int)mirrorX;
    float Y1 = (float)(int)mirrorY;
    
    glBindTexture(GL_TEXTURE_2D, (GLuint)textureId);
    glBegin(GL_QUADS);
    // Front Face
    glNormal3f( 0.0f, 0.0f, 1.0f);
    glTexCoord2f(X1, Y0); glVertex3i(box.getLeftBottomPoint().x(), box.getLeftBottomPoint().y(),  0);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(X0, Y0); glVertex3i(box.getRightBottomPoint().x(), box.getRightBottomPoint().y(),  0);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(X0, Y1); glVertex3i(box.getRightTopPoint().x(), box.getRightTopPoint().y(),  0);	// Top Right Of The Texture and Quad
    glTexCoord2f(X1, Y1); glVertex3i(box.getLeftTopPoint().x(), box.getLeftTopPoint().y(),  0);	// Top Left Of The Texture and Quad
    glEnd();
}

//---------------------------------------------------------------------

void Scene3DPrivate::drawCylinderWithIntPos(unsigned int textureId, const Int2& position, float rotationAngle, float width, float height, int subDiv1, int subDiv2)
{
    glPushMatrix();

    glTranslatef((float)position.x(),(float)position.y(),0.f);
    glRotatef(-90.f, 1.0f, 0.f, 0.f);
    glRotatef(-rotationAngle, 0.0f, 1.f, 0.f);
    glTranslatef(0.f, 0.f, -height/2);

    drawCylinder(textureId, width, height, subDiv1, subDiv2);

	glPopMatrix();
}

//---------------------------------------------------------------------
/*
void Scene3DPrivate::injectKeyForDebug(unsigned char key)
{
    const u8 keylower = tolower(key);
    if (keylower == 'h')
		m_camera.setPosition(m_camera.getPosition()+Float3(10.f, 0.f, 0.f));
    else if (keylower == 'k')
        m_camera.setPosition(m_camera.getPosition()+Float3(-10.f, 0.f, 0.f));
    if (keylower == 'u')
        m_camera.setPosition(m_camera.getPosition()+Float3(0.f, 10.f, 0.f));
    else if (keylower == 'j')
        m_camera.setPosition(m_camera.getPosition()+Float3(0.f, -10.f, 0.f));
    if (keylower == 'o')
        m_camera.setPosition(m_camera.getPosition()+Float3(10.f, 0.f, -10.f));
    else if (keylower == 'l')
        m_camera.setPosition(m_camera.getPosition()+Float3(10.f, 0.f, 10.f));
}
*/
//---------------------------------------------------------------------
/*
void Scene3DPrivate::injectKeyForDebugCollision(unsigned char key, ObjModel3D* model)
{
	Float3 oldPos = m_camera.getPosition();
	injectKeyForDebug(key);
	Float3 tmpIntersectionPt;
	if (model->isThere3DIntersection(oldPos, m_camera.getPosition(), tmpIntersectionPt))
	{
		m_camera.setPosition(oldPos);
	}
}
*/
//---------------------------------------------------------------------
/*
void Scene3DPrivate::setSlowlyCameraAngle(Float3 parAngleCamera)
{
	Float3 cameraAngle = m_camera.getRotation() - parAngleCamera;
	if (cameraAngle.x() > 180.f) cameraAngle.data[0] -= 360.f;
	if (cameraAngle.y() > 180.f) cameraAngle.data[1] -= 360.f;
	if (cameraAngle.z() > 180.f) cameraAngle.data[2] -= 360.f;
	if (cameraAngle.x() < -180.f) cameraAngle.data[0] += 360.f;
	if (cameraAngle.y() < -180.f) cameraAngle.data[1] += 360.f;
	if (cameraAngle.z() < -180.f) cameraAngle.data[2] += 360.f;
	float cameraAngleMove = (float)Engine::instance().getFrameDuration() / 100000.f;
	if (cameraAngleMove > 1.f) cameraAngleMove = 1.f;
	cameraAngle.data[0] += -cameraAngleMove * cameraAngle.x();
	cameraAngle.data[1] += -cameraAngleMove * cameraAngle.y();
	cameraAngle.data[2] += -cameraAngleMove * cameraAngle.z();
	m_camera.setRotation(cameraAngle + parAngleCamera);
}
*/
//---------------------------------------------------------------------
/*
void Scene3DPrivate::injectMouseMotionForCameraAngle(const Int2& parMousePosition, bool parIsClicking)
{
	if (parIsClicking)
	{
		Int2 diff = parMousePosition - previousMousePosition;
		m_camera.rotateX( (float)-diff.y());
		m_camera.rotateY( (float)-diff.x());
	}
	previousMousePosition = parMousePosition;
}
*/
//---------------------------------------------------------------------
/*
float Scene3DPrivate::updateCameraDebugFPSVisionAndManageFall(Object3D* model)
{	
	set3DMode();
	//fall
	float fallValue = 0;
	//Float3 newPositionFromLook = positionFromLook;
	//do 
	//{
	//	positionFromLook.data[1] = newPositionFromLook.data[1];
	//	newPositionFromLook.data[1] -= 3.f;
	//	fallValue -= 3.f;
	//}
	//while (!model->isThere3DIntersection(positionFromLook, newPositionFromLook));
	Float3 newPositionFromLook = m_camera.getPosition();
	newPositionFromLook.data[1] -= 3.f;
	Float3 tmpIntersectionPt;
	if (!model->isThere3DIntersection(m_camera.getPosition(), newPositionFromLook, tmpIntersectionPt))
	{
		m_camera.setPosition(newPositionFromLook);
		fallValue = 1;
	}
	
	//set camera position and angle
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(this->getFOV(),windowRatio,0.1f,100000.0f);
	glRotatef(-angleDebugFPSVision, 0.f, 1.f, 0.f);
	glTranslatef(-m_camera.getPosition().x(),-m_camera.getPosition().y(),-m_camera.getPosition().z());
	glTranslatef(-0,-10,0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	updateLight();

	nbTrianglesDrawn = 0;

	return fallValue;
}
*/
//---------------------------------------------------------------------

void Scene3DPrivate::updateLight()
{   
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (m_lightIsEnable[i])
		{
			float intensity = m_lightIntensity[i];
			if (intensity <= 0.f)
			{
				glLightf(GL_LIGHT0+i, GL_CONSTANT_ATTENUATION, 1.f);
				glLightf(GL_LIGHT0+i, GL_LINEAR_ATTENUATION, 0.f);
			}
			else
			{
				glLightf(GL_LIGHT0+i, GL_CONSTANT_ATTENUATION, 1.f);
				glLightf(GL_LIGHT0+i, GL_LINEAR_ATTENUATION, 1.f / intensity);
			}
			glLightf(GL_LIGHT0+i, GL_QUADRATIC_ATTENUATION, intensity<=0.f?0.f:0.00005f);

			glLightfv(GL_LIGHT0+i, GL_DIFFUSE, openglColorDiffuse);
			glLightfv(GL_LIGHT0+i, GL_SPECULAR, openglColorSpecular);
			glLightfv(GL_LIGHT0+i, GL_AMBIENT, openglColorAmbient);
			glLightfv(GL_LIGHT0+i, GL_EMISSION, openglColorAmbient);
			glLightf(GL_LIGHT0+i, GL_SHININESS, 20.0f);
			//glLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, lightDirection);
			//glLightf(GL_LIGHT0+i, GL_SPOT_CUTOFF, 180.0);
			//glLightf(GL_LIGHT0+i, GL_SPOT_EXPONENT, 128);
		}
	}

	for (int i = 0; i < MAX_LIGHTS; ++i)
		if (m_lightIsEnable[i])
			glLightfv(GL_LIGHT0 + i, GL_POSITION, m_lightPosition[i].data);
}

//---------------------------------------------------------------------

void Scene3DPrivate::set3DMode(Camera& camera)
{
	glViewport(0,0,Engine::instance().getScene2DMgr().getWindowRealSize().width(), Engine::instance().getScene2DMgr().getWindowRealSize().height());
	
	// if (enableLighting)
    //{
    //    glEnable(GL_LIGHTING);
    //    gluQuadricNormals((GLUquadricObj*)quadric, GLU_FLAT);
    //}
    //else
    //{
        glDisable(GL_LIGHTING);
        gluQuadricNormals((GLUquadricObj*)quadric, GLU_NONE);
    //}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	this->setWireframeMode(false);

	AppSetup::instance().set3DMode(camera.getFieldOfView(), camera.getMinViewDistance(),camera.getMaxViewDistance());

	float matrix[16];
	camera.getMatrix(matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(matrix);
	glTranslatef(-camera.getPosition().x(), -camera.getPosition().y(), -camera.getPosition().z());
	glGetFloatv(GL_MODELVIEW_MATRIX, eyeMatrix);
	
	updateLight();

	nbTrianglesDrawn = 0;
}

//---------------------------------------------------------------------

void Scene3DPrivate::set3DModeLookAtMode(const Float3& parCenter, float distance, Camera& camera)
{
	camera.setPosition(parCenter);
	float angleXRad = MathUtils::PI /2 + camera.getAngleX() * MathUtils::PI_OVER_180;
	float angleYRad = MathUtils::PI /2 - camera.getAngleY() * MathUtils::PI_OVER_180;

	camera.move(Float3(
		distance * sin(angleXRad) * cos(angleYRad), 
		distance * cos(angleXRad), 
		distance * sin(angleXRad) * sin(angleYRad)));
	this->set3DMode(camera);
}

//---------------------------------------------------------------------
/*
void Scene3DPrivate::injectKeyForDebugFPSCollision(unsigned char key, Object3D* model)
{
	const u8 keylower = tolower(key);
    
	const float movingCapacity = 10.f;
	const float climbAbility = 5.f;
	const float rotationCapacity = 10.f;
	
	if (keylower == 'h')
        angleDebugFPSVision += rotationCapacity;
    else if (keylower == 'k')
        angleDebugFPSVision -= rotationCapacity;
	
	if (keylower == 'e')
	{
		m_camera.setPosition(m_camera.getPosition().x(), m_camera.getPosition().y() +500, m_camera.getPosition().z());
		return;
	}
	else if (keylower == 'z')
	{
		m_camera.setPosition(m_camera.getPosition().x(), m_camera.getPosition().y() +50, m_camera.getPosition().z());
		return;
	}
	else if (keylower == 'i')
	{
		m_camera.setPosition(Float3(0.f,0.f,0.f));
		return;
	}
	
	//move
	float angleRadians = (angleDebugFPSVision + 90.f) * MathUtils::PI / 180.f;
    Float3 newPositionFromLook = m_camera.getPosition();
    if (keylower == 'u')
	{
        newPositionFromLook.data[0] += cos(angleRadians) * movingCapacity;
		newPositionFromLook.data[2] -= sin(angleRadians) * movingCapacity;
	}
    else if (keylower == 'j')
	{
        newPositionFromLook.data[0] -= cos(angleRadians) * movingCapacity;
		newPositionFromLook.data[2] += sin(angleRadians) * movingCapacity;
	}

	Float3 tmpIntersectionPt;
	if (model->isThere3DIntersection(m_camera.getPosition(), newPositionFromLook, tmpIntersectionPt))
	{
		newPositionFromLook.data[1] += climbAbility;
		m_camera.setPosition(m_camera.getPosition().x(), m_camera.getPosition().y()+climbAbility, m_camera.getPosition().z());
		if (model->isThere3DIntersection(m_camera.getPosition(), newPositionFromLook, tmpIntersectionPt))
			m_camera.setPosition(m_camera.getPosition().x(), m_camera.getPosition().y()-climbAbility, m_camera.getPosition().z());
		else
			m_camera.setPosition( newPositionFromLook );
	}
	else
	{
		m_camera.setPosition( newPositionFromLook );
	}
}
*/
//---------------------------------------------------------------------

void Scene3DPrivate::draw3DSegment(const Float3& parSegmentPt1, const Float3& parSegmentPt2)
{
	unsigned char isenabletexture;
	glGetBooleanv(GL_TEXTURE_2D, &isenabletexture);
	glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);
    glVertex3f(parSegmentPt1.x(), parSegmentPt1.y(), parSegmentPt1.z());
    glVertex3f(parSegmentPt2.x(), parSegmentPt2.y(), parSegmentPt2.z());
    glEnd();
	if (isenabletexture) glEnable(GL_TEXTURE_2D);
}

//---------------------------------------------------------------------

void Scene3DPrivate::setWireframeMode(bool trueForYes)
{
	if (trueForYes)
	{
		glDisable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);//FRONT);
		glEnable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}
}

//---------------------------------------------------------------------

void Scene3DPrivate::startDrawingTriangles(const int textureId)
{
	if (textureId < 0)
		glDisable(GL_TEXTURE_2D);
	else
		glBindTexture(GL_TEXTURE_2D, (unsigned int)textureId);
	
	glBegin(GL_TRIANGLES);
}

void Scene3DPrivate::stopDrawingTriangles()
{
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void Scene3DPrivate::drawTriangles(const std::vector<Float3>& parPositionPoints, const int textureId, float scaleFactor)
{
	startDrawingTriangles(textureId);
	drawTriangles_requireStartDrawing(parPositionPoints, scaleFactor);
	stopDrawingTriangles();
}

void Scene3DPrivate::drawTriangles_requireStartDrawing(const std::vector<Float3>& parPositionPoints, float scaleFactor)
{
	if (parPositionPoints.size() == 0) return;
	for (std::vector<Float3>::const_iterator it = parPositionPoints.begin(); it != parPositionPoints.end();)
	{
		glTexCoord2f(0.f, 0.f);
		if (it == parPositionPoints.end()) break;
		glVertex3f((*it).x() * scaleFactor, (*it).y() * scaleFactor, (*it).z() * scaleFactor);
		it++;
		glTexCoord2f(1.f, 0.f);
		if (it == parPositionPoints.end()) break;
		glVertex3f((*it).x() * scaleFactor, (*it).y() * scaleFactor, (*it).z() * scaleFactor);
		it++;
		glTexCoord2f(1.f, 1.f);
		if (it == parPositionPoints.end()) break;
		glVertex3f((*it).x() * scaleFactor, (*it).y() * scaleFactor, (*it).z() * scaleFactor);
		it++;
	}
	nbTrianglesDrawn += parPositionPoints.size() / 3;
}

//---------------------------------------------------------------------

const float MARGIN_VIEW_RANGE = 5.f;

static bool renderPlaneCheckOutOfRange(
	const Float3 pt[4], const Float2 ptTextureCoordinates[4], const Float3& cubePos, bool doOptimizations, float horizontalFOV, const Camera& camera, float fogMaxDistance)
{
	bool pb1 = false;
	bool pb2 = false;
	bool pb3 = false;
	bool pb4 = false;
	for (int i = 0; ; ++i) 
	{
		Float3 posCamCoord = camera.convertPositionInCameraCoordinates(cubePos + pt[i]);
		float distanceZToCameraInCameraCoord = -posCamCoord.z();

		Float3 sphericalCoord = CoreUtils::getSphericalCoordinatesFrom3DCoordinates(posCamCoord);
		float midHorizontalFOV = (horizontalFOV/2.f) * MathUtils::PI_OVER_180;
		float midVerticalFOV   = (camera.getFieldOfView()/2.f) * MathUtils::PI_OVER_180;
		float angleSpherical = MathUtils::getAngleInPiRange(sphericalCoord.data[2] + MathUtils::PI/2);
		float angleSpherical2 = MathUtils::getAngleInPiRange(sphericalCoord.data[1] - MathUtils::PI/2);
	
		bool outOfRangeFog = false;
		if (false //Engine::instance().getScene3DMgr().isFogEnabled()
			&& distanceZToCameraInCameraCoord  > fogMaxDistance+MARGIN_VIEW_RANGE) 
			outOfRangeFog = true;
		
		if (distanceZToCameraInCameraCoord < -MARGIN_VIEW_RANGE) pb1 = true;
		else if (!(angleSpherical  > -midHorizontalFOV && angleSpherical  < midHorizontalFOV)) pb2 = true;
		//else if (!(angleSpherical2 > -midVerticalFOV && angleSpherical2 < midVerticalFOV)) pb3 = true;
		//else if (outOfRangeFog) pb4 = true;
		else
		{
			// if point is in range of the camera, display the whole plane
			break;
		}
		// if problems are different, display the whole plane
		if (((int)pb1 + (int)pb2 + (int)pb3 + (int)pb4) >= 2) break;

		// if all points are out of range for the same reason, do not display
		if (i == 3) return false;
	}

	glTexCoord2f(ptTextureCoordinates[0].x(), ptTextureCoordinates[0].y());
	glVertex3f(pt[0].x(), pt[0].y(), pt[0].z());	// Top Left Of The Texture and Quad
	glTexCoord2f(ptTextureCoordinates[1].x(), ptTextureCoordinates[1].y());
	glVertex3f(pt[1].x(), pt[1].y(), pt[1].z());	// Bottom Left Of The Texture and Quad
	glTexCoord2f(ptTextureCoordinates[2].x(), ptTextureCoordinates[2].y());
	glVertex3f(pt[2].x(), pt[2].y(), pt[2].z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(ptTextureCoordinates[3].x(), ptTextureCoordinates[3].y());
	glVertex3f(pt[3].x(), pt[3].y(), pt[3].z());	// Top Right Of The Texture and Quad

	return true;
}

//---------------------------------------------------------------------

//static -> avoid allocations at each call
static Float3 ptCheck[4];
static Float2 ptTextureCoordinates[4];

//return false if not rendered because out of camera range
bool Scene3DPrivate::drawCubeFace(
   const Float3& parSize, unsigned int textureId, const Float3& parTextureRepeat, 
   int faceToDraw, const Float3& cubePosition, bool doOptimizations, float horizontalFOV, const Camera& camera, float fogMaxDistance)
{
	Float3 size = parSize / 2;

	Float3 pos = cubePosition;
	/*Float3 posFromCam = pos - getCamera().getPosition();
	pos.data[1] = pos.data[1] - 1000.f*(1.f-sin(atan2(1000.f,posFromCam.x()))) - 1000.f*(1.f-sin(atan2(1000.f,posFromCam.z())));
	Float3 allPoints[8];
	allPoints[0] = posFromCam + Float3( size.x(), size.y(),  size.z());
	allPoints[1] = posFromCam + Float3( -size.x(), size.y(),  size.z());
	allPoints[2] = posFromCam + Float3( size.x(), -size.y(),  size.z());
	allPoints[3] = posFromCam + Float3( size.x(), size.y(), -size.z());
	allPoints[4] = posFromCam + Float3( -size.x(), -size.y(), size.z());
	allPoints[5] = posFromCam + Float3( -size.x(), size.y(), -size.z());
	allPoints[6] = posFromCam + Float3( size.x(), -size.y(), -size.z());
	allPoints[7] = posFromCam + Float3( -size.x(), -size.y(), -size.z());
	for (int i = 0; i < 8; ++i)
	{
		allPoints[i].data[1] = allPoints[i].data[1] - 1000.f*(1.f-sin(atan2(1000.f,allPoints[i].data[0]))) - 1000.f*(1.f-sin(atan2(1000.f,allPoints[i].data[2])));
		allPoints[i] -= posFromCam;
	}*/

	movePosition(pos);
	
	if (textureId)
		glBindTexture(GL_TEXTURE_2D, textureId);
	else
		setWireframeMode(true);
	
	glBegin(GL_QUADS);
	
	switch (faceToDraw)
	{
	case 0:
			// Top Facef
			glNormal3f(0.f, 1.f, 0.f);
			ptCheck[0] = Float3(-size.x(), size.y(), -size.z());//allPoints[5];//
			ptCheck[1] = Float3(-size.x(), size.y(),  size.z());//allPoints[1];//
			ptCheck[2] = Float3( size.x(), size.y(),  size.z());//allPoints[0];//
			ptCheck[3] = Float3( size.x(), size.y(), -size.z());//allPoints[3];//

			ptTextureCoordinates[0] = Float2(parTextureRepeat.x(), parTextureRepeat.z());
			ptTextureCoordinates[1] = Float2(parTextureRepeat.x(), 0.f);
			ptTextureCoordinates[2] = Float2(0.f, 0.f);
			ptTextureCoordinates[3] = Float2(0.f, parTextureRepeat.z());

			break;
	case 1:
			// Bottom Face
			glNormal3f(0.f, -1.f, 0.f);
			ptCheck[0] = Float3(-size.x(), -size.y(), -size.z());//allPoints[7];//
			ptCheck[1] = Float3( size.x(), -size.y(), -size.z());//allPoints[6];//
			ptCheck[2] = Float3( size.x(), -size.y(),  size.z());//allPoints[2];//
			ptCheck[3] = Float3(-size.x(), -size.y(),  size.z());//allPoints[4];//

			ptTextureCoordinates[0] = Float2(parTextureRepeat.x(), parTextureRepeat.z());
			ptTextureCoordinates[1] = Float2(0.f, parTextureRepeat.z());
			ptTextureCoordinates[2] = Float2(0.f, 0.f);
			ptTextureCoordinates[3] = Float2(parTextureRepeat.x(), 0.f);

			break;
	case 2:
			// Front Face
			glNormal3f( 0.0f, 0.0f, 1.0f);
			ptCheck[0] = Float3(-size.x(), -size.y(),  size.z());//allPoints[4];//
			ptCheck[1] = Float3( size.x(), -size.y(),  size.z());//allPoints[2];//
			ptCheck[2] = Float3( size.x(),  size.y(),  size.z());//allPoints[0];//
			ptCheck[3] = Float3(-size.x(),  size.y(),  size.z());//allPoints[1];//

			ptTextureCoordinates[0] = Float2(0.f, parTextureRepeat.y());
			ptTextureCoordinates[1] = Float2(parTextureRepeat.x(), parTextureRepeat.y());
			ptTextureCoordinates[2] = Float2(parTextureRepeat.x(), 0.f);
			ptTextureCoordinates[3] = Float2(0.f, 0.f);

			break;
	case 3:
			// Back Face
			glNormal3f( 0.0f, 0.0f, -1.0f);
			ptCheck[0] = Float3(-size.x(),  size.y(),  -size.z());//allPoints[5];//
			ptCheck[1] = Float3( size.x(),  size.y(),  -size.z());//allPoints[3];//
			ptCheck[2] = Float3( size.x(), -size.y(),  -size.z());//allPoints[6];//
			ptCheck[3] = Float3(-size.x(), -size.y(),  -size.z());//allPoints[7];//

			ptTextureCoordinates[0] = Float2(parTextureRepeat.x(), 0.f);
			ptTextureCoordinates[1] = Float2(0.f, 0.f);
			ptTextureCoordinates[2] = Float2(0.f, parTextureRepeat.y());
			ptTextureCoordinates[3] = Float2(parTextureRepeat.x(), parTextureRepeat.y());

			break;
	case 4:
			// Left Face
			glNormal3f( -1.0f, 0.f, 0.f);
			ptCheck[0] = Float3(-size.x(), -size.y(), -size.z());//allPoints[7];//
			ptCheck[1] = Float3(-size.x(), -size.y(),  size.z());//allPoints[4];//
			ptCheck[2] = Float3(-size.x(),  size.y(),  size.z());//allPoints[1];//
			ptCheck[3] = Float3(-size.x(),  size.y(), -size.z());//allPoints[5];//

			ptTextureCoordinates[0] = Float2(0.f, parTextureRepeat.y());
			ptTextureCoordinates[1] = Float2(parTextureRepeat.z(), parTextureRepeat.y());
			ptTextureCoordinates[2] = Float2(parTextureRepeat.z(), 0.f);
			ptTextureCoordinates[3] = Float2(0.f, 0.f);

			break;
	case 5:
			// Right face
			glNormal3f( 1.0f, 0.0f, 0.0f);
			ptCheck[0] = Float3( size.x(),-size.y(), -size.z());//allPoints[6];//
			ptCheck[1] = Float3( size.x(), size.y(), -size.z());//allPoints[3];//
			ptCheck[2] = Float3( size.x(), size.y(),  size.z());//allPoints[0];//
			ptCheck[3] = Float3(size.x(), -size.y(),  size.z());//allPoints[2];//

			ptTextureCoordinates[0] = Float2(parTextureRepeat.z(), parTextureRepeat.y());
			ptTextureCoordinates[1] = Float2(parTextureRepeat.z(), 0.f);
			ptTextureCoordinates[2] = Float2(0.f, 0.f);
			ptTextureCoordinates[3] = Float2(0.f, parTextureRepeat.y());

			break;
	}

	bool res = renderPlaneCheckOutOfRange(ptCheck, ptTextureCoordinates, pos, doOptimizations, horizontalFOV, camera, fogMaxDistance);
	if (res) nbTrianglesDrawn += 2;

    glEnd();

	movePosition(pos * -1.f);

	if (!textureId) setWireframeMode(false);
	return res;
}

//---------------------------------------------------------------------

void Scene3DPrivate::drawCube(const Float3& parSize, const unsigned int parTextureId[6], const Float3& parTextureRepeat, bool reverseToSkyBox)
{
	Float3 size = parSize / 2;
	
	GLuint currentTextureId = 0;
	if (!parTextureId) {setWireframeMode(true);glBegin(GL_QUADS);}
	
	float reverse = reverseToSkyBox?-1.f:1.f;

	//---------------
    // Top Facef

	if (parTextureId)
	{
		currentTextureId = parTextureId[0];    
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}
	
	glNormal3f(0.f, 1.f, 0.f);
    
	glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.z());
	glVertex3f(-size.x(), size.y() * reverse, -size.z());	// Top Left Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), 0.f);
	glVertex3f(-size.x(), size.y() * reverse,  size.z());	// Bottom Left Of The Texture and Quad
	glTexCoord2f(0.f, 0.f);
	glVertex3f(size.x(), size.y() * reverse,  size.z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(0.f, parTextureRepeat.z());
	glVertex3f(size.x(), size.y() * reverse, -size.z());	// Top Right Of The Texture and Quad
	
    //---------------
    // Bottom Face
	
	if (parTextureId && currentTextureId != parTextureId[1])
	{
		currentTextureId = parTextureId[1];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f(0.f, -1.f, 0.f);

	glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.z());
	glVertex3f(-size.x(), -size.y() * reverse, -size.z());	// Top Left Of The Texture and Quad
	glTexCoord2f(0.f, parTextureRepeat.z());
	glVertex3f(size.x(), -size.y() * reverse, -size.z());	// Top Right Of The Texture and Quad
	glTexCoord2f(0.f, 0.f);
	glVertex3f(size.x(), -size.y() * reverse, size.z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), 0.f);
	glVertex3f(-size.x(), -size.y() * reverse, size.z());	// Bottom Left Of The Texture and Quad
	
    //---------------
    // Front Face

	if (parTextureId && currentTextureId != parTextureId[2])
	{
		currentTextureId = parTextureId[2];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f( 0.0f, 0.0f, 1.0f);

	glTexCoord2f(0.f, parTextureRepeat.y());
	glVertex3f(-size.x(), -size.y(),  size.z() * reverse);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.y());
	glVertex3f(size.x(), -size.y(),  size.z() * reverse);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), 0.f);
	glVertex3f(size.x(), size.y(),  size.z() * reverse);	// Top Right Of The Texture and Quad
	glTexCoord2f(0.f, 0.f);
	glVertex3f(-size.x(), size.y(),  size.z() * reverse);	// Top Left Of The Texture and Quad
	
    //---------------
    // Back Face

	if (parTextureId && currentTextureId != parTextureId[3])
	{
		currentTextureId = parTextureId[3];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f( 0.0f, 0.0f, -1.0f);

	glTexCoord2f(parTextureRepeat.x(), 0.f);
	glVertex3f(-size.x(), size.y(),  -size.z() * reverse);
	glTexCoord2f(0.f, 0.f);
	glVertex3f(size.x(), size.y(),  -size.z() * reverse);
	glTexCoord2f(0.f, parTextureRepeat.y());
	glVertex3f(size.x(), -size.y(),  -size.z() * reverse);
	glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.y());
	glVertex3f(-size.x(), -size.y(),  -size.z() * reverse);
	
    //---------------
    // Left Face

	if (parTextureId && currentTextureId != parTextureId[4])
	{
		currentTextureId = parTextureId[4];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f( -1.0f, 0.f, 0.f);

	glTexCoord2f(0.f, parTextureRepeat.y());
	glVertex3f(-size.x() * reverse, -size.y(), -size.z());	// Bottom Left Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.z(), parTextureRepeat.y());
	glVertex3f(-size.x() * reverse, -size.y(), size.z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.z(), 0.f);
	glVertex3f(-size.x() * reverse, size.y(), size.z());	// Top Right Of The Texture and Quad
	glTexCoord2f(0.f, 0.f);
	glVertex3f(-size.x() * reverse, size.y(), -size.z());	// Top Left Of The Texture and Quad
	
	//---------------
    // Right face
    
	if (parTextureId && currentTextureId != parTextureId[5])
	{
		currentTextureId = parTextureId[5];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f( 1.0f, 0.0f, 0.0f);

	glTexCoord2f(parTextureRepeat.z(), parTextureRepeat.y());
	glVertex3f(size.x() * reverse, -size.y(), -size.z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.z(), 0.f);
	glVertex3f(size.x() * reverse, size.y(), -size.z());	// Top Right Of The Texture and Quad
	glTexCoord2f(0.f, 0.f);
	glVertex3f(size.x() * reverse, size.y(), size.z());	// Top Left Of The Texture and Quad
	glTexCoord2f(0.f, parTextureRepeat.y());
	glVertex3f(size.x() * reverse, -size.y(), size.z());	// Bottom Left Of The Texture and Quad
	
	//---------------
	// finished

    glEnd();

	if (!parTextureId) setWireframeMode(false);
}

//---------------------------------------------------------------------

void Scene3DPrivate::getVerticesAndTexCoordCube(
	Float3* vertices, Float2* texCoord, const Float3& parTexCoordBegin, const Float3& parTexCoordEnd, 
	const Float3& parTexCoordBeginOtherSide, const Float3& parTexCoordEndOtherSide, const Float3& size, bool randomTexCoord) // must be size 24
{
	float reverse = 1.f;

	//top
	vertices[0] = Float3(-size.x(), size.y() * reverse,-size.z());
	vertices[1] = Float3(-size.x(), size.y() * reverse, size.z());
	vertices[2] = Float3( size.x(), size.y() * reverse, size.z());
	vertices[3] = Float3( size.x(), size.y() * reverse,-size.z());

	texCoord[3] = Float2(parTexCoordBeginOtherSide.x(), parTexCoordEndOtherSide.z());
	texCoord[2] = Float2(parTexCoordBeginOtherSide.x(), parTexCoordBeginOtherSide.z());
	texCoord[1] = Float2(parTexCoordEndOtherSide.x(),   parTexCoordBeginOtherSide.z());
	texCoord[0] = Float2(parTexCoordEndOtherSide.x(),   parTexCoordEndOtherSide.z());

	//bottom
	vertices[4] = Float3(-size.x(),-size.y() * reverse,-size.z());
	vertices[5] = Float3(size.x(), -size.y() * reverse, -size.z());
	vertices[6] = Float3(size.x(), -size.y() * reverse, size.z());
	vertices[7] = Float3(-size.x(), -size.y() * reverse, size.z());
	
	texCoord[4] = Float2(parTexCoordBegin.x(), parTexCoordBegin.z());
	texCoord[5] = Float2(parTexCoordEnd.x(),   parTexCoordBegin.z());
	texCoord[6] = Float2(parTexCoordEnd.x(),   parTexCoordEnd.z());
	texCoord[7] = Float2(parTexCoordBegin.x(), parTexCoordEnd.z());

	//front
	vertices[8]  = Float3(-size.x(), -size.y(),  size.z() * reverse);
	vertices[9]  = Float3(size.x(), -size.y(),  size.z() * reverse);
	vertices[10] = Float3(size.x(), size.y(),  size.z() * reverse);
	vertices[11] = Float3(-size.x(), size.y(),  size.z() * reverse);
	
	texCoord[8]  = Float2(parTexCoordBegin.x(), parTexCoordBegin.y());
	texCoord[9]  = Float2(parTexCoordEnd.x(),   parTexCoordBegin.y());
	texCoord[10] = Float2(parTexCoordEnd.x(),   parTexCoordEnd.y());
	texCoord[11] = Float2(parTexCoordBegin.x(), parTexCoordEnd.y());
	
	//back
	vertices[12] = Float3(-size.x(), size.y(),  -size.z() * reverse);
	vertices[13] = Float3(size.x(), size.y(),  -size.z() * reverse);
	vertices[14] = Float3(size.x(), -size.y(),  -size.z() * reverse);
	vertices[15] = Float3(-size.x(), -size.y(),  -size.z() * reverse);
	
	texCoord[12] = Float2(1.f-parTexCoordEndOtherSide.x(),   parTexCoordEndOtherSide.y());
	texCoord[13] = Float2(1.f-parTexCoordBeginOtherSide.x(), parTexCoordEndOtherSide.y());
	texCoord[14] = Float2(1.f-parTexCoordBeginOtherSide.x(), parTexCoordBeginOtherSide.y());
	texCoord[15] = Float2(1.f-parTexCoordEndOtherSide.x(),   parTexCoordBeginOtherSide.y());
	
	//left
	vertices[16] = Float3(-size.x() * reverse, -size.y(), -size.z());
	vertices[17] = Float3(-size.x() * reverse, -size.y(), size.z());
	vertices[18] = Float3(-size.x() * reverse, size.y(), size.z());
	vertices[19] = Float3(-size.x() * reverse, size.y(), -size.z());
	
	texCoord[16] = Float2(parTexCoordBegin.z(), parTexCoordBegin.y());
	texCoord[17] = Float2(parTexCoordEnd.z(),   parTexCoordBegin.y());
	texCoord[18] = Float2(parTexCoordEnd.z(),   parTexCoordEnd.y());
	texCoord[19] = Float2(parTexCoordBegin.z(), parTexCoordEnd.y());
	
	//right
	vertices[20] = Float3(size.x() * reverse, -size.y(), -size.z());
	vertices[21] = Float3(size.x() * reverse, size.y(), -size.z());
	vertices[22] = Float3(size.x() * reverse, size.y(), size.z());
	vertices[23] = Float3(size.x() * reverse, -size.y(), size.z());

	texCoord[20] = Float2(parTexCoordEndOtherSide.z(),   parTexCoordBeginOtherSide.y());
	texCoord[21] = Float2(parTexCoordEndOtherSide.z(),   parTexCoordEndOtherSide.y());
	texCoord[22] = Float2(parTexCoordBeginOtherSide.z(), parTexCoordEndOtherSide.y());
	texCoord[23] = Float2(parTexCoordBeginOtherSide.z(), parTexCoordBeginOtherSide.y());

	if (randomTexCoord)
	{
		Float2 rand(Utils::random(parTexCoordEnd.x()), Utils::random(parTexCoordEnd.z()));
		texCoord[0] += Float2(rand);
		texCoord[1] += Float2(rand);
		texCoord[2] += Float2(rand);
		texCoord[3] += Float2(rand);
		texCoord[4] += Float2(rand);
		texCoord[5] += Float2(rand);
		texCoord[6] += Float2(rand);
		texCoord[7] += Float2(rand);

		rand = Float2(Utils::random(parTexCoordEnd.x()), Utils::random(parTexCoordEnd.y()));

		texCoord[8] +=  Float2(rand);
		texCoord[9] +=  Float2(rand);
		texCoord[10] += Float2(rand);
		texCoord[11] += Float2(rand);
		texCoord[12] += Float2(rand);
		texCoord[13] += Float2(rand);
		texCoord[14] += Float2(rand);
		texCoord[15] += Float2(rand);

		rand = Float2(Utils::random(parTexCoordEnd.z()), Utils::random(parTexCoordEnd.y()));

		texCoord[16] += Float2(rand);
		texCoord[17] += Float2(rand);
		texCoord[18] += Float2(rand);
		texCoord[19] += Float2(rand);
		texCoord[20] += Float2(rand);
		texCoord[21] += Float2(rand);
		texCoord[22] += Float2(rand);
		texCoord[23] += Float2(rand);
	}
}

//---------------------------------------------------------------------

void Scene3DPrivate::drawCube2(const Float3& parSize, const unsigned int parTextureId[6], const Float3& parTextureRepeat, bool reverseToSkyBox, bool alphaTestEffect)
{
	Float3 size = parSize / 2;
	
	GLuint currentTextureId = 0;
	if (!parTextureId) {setWireframeMode(true);glBegin(GL_QUADS);}
	
	float reverse = reverseToSkyBox?-1.f:1.f;
	
	if (alphaTestEffect)
		glAlphaFunc(GL_GREATER, 0.9f);
	
	//---------------
    // Bottom Facef

	if (parTextureId)
	{
		currentTextureId = parTextureId[0];    
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}
	
	glNormal3f(0.f, 1.f, 0.f);
    
	glTexCoord2f(0.f, parTextureRepeat.z());
	glVertex3f(-size.x(), size.y() * reverse, -size.z());	// Top Left Of The Texture and Quad
	glTexCoord2f(0.f, 0.f);
	glVertex3f(-size.x(), size.y() * reverse,  size.z());	// Bottom Left Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), 0.f);
	glVertex3f(size.x(), size.y() * reverse,  size.z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.z());
	glVertex3f(size.x(), size.y() * reverse, -size.z());	// Top Right Of The Texture and Quad
	
    //---------------
    // Top Face
	
	if (parTextureId && currentTextureId != parTextureId[1])
	{
		currentTextureId = parTextureId[1];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f(0.f, -1.f, 0.f);

	glTexCoord2f(0.f, 0.f);
	glVertex3f(-size.x(), -size.y() * reverse, -size.z());	// Top Left Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), 0.f);
	glVertex3f(size.x(), -size.y() * reverse, -size.z());	// Top Right Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.z());
	glVertex3f(size.x(), -size.y() * reverse, size.z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(0.f, parTextureRepeat.z());
	glVertex3f(-size.x(), -size.y() * reverse, size.z());	// Bottom Left Of The Texture and Quad
	
    //---------------
    // Front Face

	if (parTextureId && currentTextureId != parTextureId[2])
	{
		currentTextureId = parTextureId[2];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f( 0.0f, 0.0f, 1.0f);

	glTexCoord2f(0.f, 0.f);
	glVertex3f(-size.x(), -size.y(),  size.z() * reverse);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), 0.f);
	glVertex3f(size.x(), -size.y(),  size.z() * reverse);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.y());
	glVertex3f(size.x(), size.y(),  size.z() * reverse);	// Top Right Of The Texture and Quad
	glTexCoord2f(0.f, parTextureRepeat.y());
	glVertex3f(-size.x(), size.y(),  size.z() * reverse);	// Top Left Of The Texture and Quad
	
    //---------------
    // Back Face

	if (parTextureId && currentTextureId != parTextureId[3])
	{
		currentTextureId = parTextureId[3];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f( 0.0f, 0.0f, -1.0f);

	glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.y());
	glVertex3f(-size.x(), size.y(),  -size.z() * reverse);
	glTexCoord2f(0.f, parTextureRepeat.y());
	glVertex3f(size.x(), size.y(),  -size.z() * reverse);
	glTexCoord2f(0.f, 0.f);
	glVertex3f(size.x(), -size.y(),  -size.z() * reverse);
	glTexCoord2f(parTextureRepeat.x(), 0.f);
	glVertex3f(-size.x(), -size.y(),  -size.z() * reverse);
	
    //---------------
    // Left Face

	if (parTextureId && currentTextureId != parTextureId[4])
	{
		currentTextureId = parTextureId[4];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f( -1.0f, 0.f, 0.f);

	glTexCoord2f(0.f, 0.f);
	glVertex3f(-size.x() * reverse, -size.y(), -size.z());	// Bottom Left Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.z(), 0.f);
	glVertex3f(-size.x() * reverse, -size.y(), size.z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.z(), parTextureRepeat.y());
	glVertex3f(-size.x() * reverse, size.y(), size.z());	// Top Right Of The Texture and Quad
	glTexCoord2f(0.f, parTextureRepeat.y());
	glVertex3f(-size.x() * reverse, size.y(), -size.z());	// Top Left Of The Texture and Quad
	
	//---------------
    // Right face
    
	if (parTextureId && currentTextureId != parTextureId[5])
	{
		currentTextureId = parTextureId[5];
		glEnd();
		glBindTexture(GL_TEXTURE_2D, currentTextureId);
		glBegin(GL_QUADS);
	}

	glNormal3f( 1.0f, 0.0f, 0.0f);

	glTexCoord2f(parTextureRepeat.z(), 0.f);
	glVertex3f(size.x() * reverse, -size.y(), -size.z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.z(), parTextureRepeat.y());
	glVertex3f(size.x() * reverse, size.y(), -size.z());	// Top Right Of The Texture and Quad
	glTexCoord2f(0.f, parTextureRepeat.y());
	glVertex3f(size.x() * reverse, size.y(), size.z());	// Top Left Of The Texture and Quad
	glTexCoord2f(0.f, 0.f);
	glVertex3f(size.x() * reverse, -size.y(), size.z());	// Bottom Left Of The Texture and Quad
	
	//---------------
	// finished

    glEnd();

	if (alphaTestEffect)
		glAlphaFunc(GL_GREATER, 0.1f);

	if (!parTextureId) setWireframeMode(false);
}

//---------------------------------------------------------------------
// look at top if angle=(0,0,0)

void Scene3DPrivate::drawPlane(const Float2& parSize, unsigned int parTextureId, const Float2& parTextureRepeat, const Float3& parAngle, bool reverse)
{
	Float2 size = parSize / 2;
	
	if (parTextureId != -1)
	{
		glBindTexture(GL_TEXTURE_2D, parTextureId);
	}
	glBegin(GL_QUADS);

	glRotatef(-parAngle.x(), 1.f, 0.f, 0.f);
	glRotatef(-parAngle.y(), 0.f, 1.f, 0.f);
	glRotatef(-parAngle.z(), 0.f, 0.f, 1.f);
	
	if (!reverse)
	{
		glNormal3f(0.f, 1.f, 0.f);
		glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.y());
		glVertex3f(-size.x(), 0, -size.y());	// Top Left Of The Texture and Quad
		glTexCoord2f(parTextureRepeat.x(), 0.f);
		glVertex3f(-size.x(), 0,  size.y());	// Bottom Left Of The Texture and Quad
		glTexCoord2f(0.f, 0.f);
		glVertex3f(size.x(), 0,  size.y());	// Bottom Right Of The Texture and Quad
		glTexCoord2f(0.f, parTextureRepeat.y());
		glVertex3f(size.x(), 0, -size.y());	// Top Right Of The Texture and Quad
	}
	else
	{
		glNormal3f(0.f, -1.f, 0.f);
		glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.y());
		glVertex3f(-size.x(), 0, -size.y());	// Top Left Of The Texture and Quad
		glTexCoord2f(0.f, parTextureRepeat.y());
		glVertex3f(size.x(), 0, -size.y());	// Top Right Of The Texture and Quad
		glTexCoord2f(0.f, 0.f);
		glVertex3f(size.x(), 0,  size.y());	// Bottom Right Of The Texture and Quad
		glTexCoord2f(parTextureRepeat.x(), 0.f);
		glVertex3f(-size.x(), 0,  size.y());	// Bottom Left Of The Texture and Quad
	}
	glEnd();
}

//---------------------------------------------------------------------

void Scene3DPrivate::drawQuad(const Float2& parSize, const Float3 pts[4], unsigned int parTextureId, const Float2& parTextureRepeat)
{
	Float2 size = parSize / 2;
	
	if (parTextureId != -1)
	{
		glBindTexture(GL_TEXTURE_2D, parTextureId);
	}
	glBegin(GL_QUADS);

	glNormal3f(0.f, -1.f, 0.f);
	glTexCoord2f(parTextureRepeat.x(), parTextureRepeat.y());
	glVertex3f(pts[0].x(), pts[0].y(), pts[0].z());	// Top Left Of The Texture and Quad
	glTexCoord2f(parTextureRepeat.x(), 0.f);
	glVertex3f(pts[1].x(), pts[1].y(), pts[1].z());	// Bottom Left Of The Texture and Quad
	glTexCoord2f(0.f, 0.f);
	glVertex3f(pts[2].x(), pts[2].y(), pts[2].z());	// Bottom Right Of The Texture and Quad
	glTexCoord2f(0.f, parTextureRepeat.y());
	glVertex3f(pts[3].x(), pts[3].y(), pts[3].z());	// Bottom Right Of The Texture and Quad

	glEnd();
}

//---------------------------------------------------------------------

void Scene3DPrivate::pushPosition()
{
	glPushMatrix();
}

//---------------------------------------------------------------------

void Scene3DPrivate::popPosition()
{
	glPopMatrix();
}

//---------------------------------------------------------------------

Float3 Scene3DPrivate::getPosition() const
{
	GLfloat matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	return Float3(matrix[12], matrix[13], matrix[14]);
}

//---------------------------------------------------------------------
void Scene3DPrivate::rotatePosition(float angleX, float angleY, float angleZ)
{
	glRotatef(angleX, 1.f, 0.f, 0.f);
	glRotatef(angleY, 0.f, 1.f, 0.f);
	glRotatef(angleZ, 0.f, 0.f, 1.f);
}
//---------------------------------------------------------------------
void Scene3DPrivate::scalePosition(float x, float y, float z)
{
	glScalef(x,y,z);
}
//---------------------------------------------------------------------
void Scene3DPrivate::setPosition(const Float3& parNewPosition)
{
	glLoadIdentity();
	glLoadMatrixf(eyeMatrix);
	glTranslatef(parNewPosition.x(),parNewPosition.y(),parNewPosition.z());
}

//---------------------------------------------------------------------

void Scene3DPrivate::movePosition(const Float3& parMovement)
{
	glTranslatef(parMovement.x(),parMovement.y(),parMovement.z());
}

//---------------------------------------------------------------------

unsigned int Scene3DPrivate::createAndBeginDrawList()
{
	GLuint list;
	list = glGenLists(1);
	glNewList(list, GL_COMPILE_AND_EXECUTE);
	return list;
}

//---------------------------------------------------------------------

void Scene3DPrivate::endDrawList()
{
	glEndList();
}

//---------------------------------------------------------------------

void Scene3DPrivate::callDrawList(unsigned int dl)
{
	glCallList(dl);
}

//---------------------------------------------------------------------

void Scene3DPrivate::removeDrawList(unsigned int dl)
{
	glDeleteLists(dl, 1);
}

//---------------------------------------------------------------------

int sizeRenderToTexture = 1024;

void Scene3DPrivate::startRenderToTexture(float aspectRatio, bool lightingEnable, int parSizeRenderToTexture, Camera& camera)
{
	sizeRenderToTexture = parSizeRenderToTexture;
	
	glViewport (0, 0, sizeRenderToTexture, sizeRenderToTexture);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(camera.getFieldOfView(), 1.f, 0.1f, maxVisibleDistance);

	glMatrixMode(GL_MODELVIEW);

	glReadBuffer(GL_BACK_LEFT);
}

void Scene3DPrivate::stopRenderToTexture(unsigned int textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glFlush();
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, sizeRenderToTexture, sizeRenderToTexture, 0);
	Int2 winSize = Engine::instance().getScene2DMgr().getWindowSize();
	glViewport(0, 0, winSize.width(), winSize.height());
	
	//return (int)renderTex;
}

Int2 Scene3DPrivate::get2DPosFrom3D(const Float3& pos, const Camera& camera) const
{
	glPushMatrix();

	//updateCamera();

	GLdouble model_view[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);	// get 3D coordinates based on window coordinates

	GLdouble pos3D_x = (double)pos.x();
	GLdouble pos3D_y = (double)pos.y();
	GLdouble pos3D_z = (double)pos.z();

	GLdouble pos2D_x;
	GLdouble pos2D_y;
	GLdouble pos2D_z;

	gluProject(pos3D_x, pos3D_y, pos3D_z, model_view, projection, viewport, &pos2D_x, &pos2D_y, &pos2D_z);
	glPopMatrix();

	Float3 diff = camera.getPosition() - pos;
	float angleObj = MathUtils::getAngleInPiRange(atan2(diff.z(), diff.x())-MathUtils::PI*0.5f);
	float angleCam = MathUtils::getAngleInPiRange(camera.getAngleY()*MathUtils::PI/180.f);
	float diffAngle = fabs(angleObj - angleCam);
	if (diffAngle < MathUtils::PI*0.5f)
		return Int2((int)pos2D_x, Engine::instance().getScene2DMgr().getWindowSize().height()-(int)pos2D_y);
	else
		return Int2(-10000,-10000);
}

Float3 Scene3DPrivate::get3DPosFrom2D(const Int2& pos) const
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble posX, posY, posZ;
 
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );
 
    GLfloat winX = (float)pos.x();
    GLfloat winY = (float)viewport[3] - (float)pos.y();
	GLfloat winZ = 0.f;
    glReadPixels( pos.x(), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
 
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
 
    return Float3((float)posX, (float)posY, (float)posZ);
}

bool Scene3DPrivate::isLightingEnabled()
{
	GLboolean param;
	glGetBooleanv(GL_LIGHTING, &param);
	return param ? true: false;
}

void Scene3DPrivate::setPositionAndScaleAndAngle(const Float3& pos, const Float3& scale_, const Float3& angle)//angle in degree
{
	setPosition(pos);
	rotatePosition(angle.x(), angle.y(), angle.z());
	scalePosition(scale_.x(), scale_.y(), scale_.z());
}

//---------------------------------------------------------------------

Scene3DPrivate::~Scene3DPrivate()
{
    gluDeleteQuadric((GLUquadricObj*)quadric);
}

//---------------------------------------------------------------------
