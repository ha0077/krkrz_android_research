
#include "tjsCommHead.h"

#include "TVPScreen.h"
#include "Application.h"


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "krkrz", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "krkrz", __VA_ARGS__))

//���_�V�F�[�_�̃R�[�h
const char tTVPScreen::VERTEX_CODE[] =
	"attribute vec4 a_Position;\n"
	"attribute vec2 a_UV;\n"
	"varying vec2 v_UV;\n"
	"void main(){\n"
		"gl_Position=a_Position;\n"
		"v_UV=a_UV;\n"
	"}\n";

//�t���O�����g�V�F�[�_�̃R�[�h
const char tTVPScreen::FRAGMENT_CODE[] =
	"precision mediump float;\n"
	"varying vec2 v_UV;\n"
	"uniform sampler2D u_Tex;\n"
	"void main(){\n"
		"gl_FragColor=texture2D(u_Tex,v_UV);\n"
	"}\n";


tTVPScreen::tTVPScreen()
: display_(EGL_NO_DISPLAY), context_(EGL_NO_CONTEXT), surface_(EGL_NO_SURFACE),
  width_(0), height_(0), format_(0) {
}

tTVPScreen::~tTVPScreen() {
}

GLuint tTVPScreen::loadShader(GLenum shaderType, const char* source) {
	GLuint shader = glCreateShader( shaderType );
	glShaderSource( shader, 1, &source, nullptr );
	glCompileShader(shader );
	printShaderCompileLog( shader );

	GLint compiled;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
	if( compiled == GL_FALSE ) {
		// error
	}
	return shader;
}

GLuint tTVPScreen::createProgram( const char* vertexCode, const char* fragmentCode ) {
	GLuint vertexShader = loadShader( GL_VERTEX_SHADER, vertexCode );
	GLuint pixelShader = loadShader( GL_FRAGMENT_SHADER, fragmentCode );
	GLuint program = glCreateProgram();
	glAttachShader( program, vertexShader );
	glAttachShader( program, pixelShader );
    glDeleteShader( vertexShader );
    glDeleteShader( pixelShader );
	glLinkProgram( program );
	printShaderLinkLog( program );
	GLint linkStatus = GL_FALSE;
	glGetProgramiv( program, GL_LINK_STATUS, &linkStatus );
	if( linkStatus == GL_FALSE ) {
		// error
	}
	return program;
}
// �V�F�[�_�R���p�C���G���[�̏o��
void tTVPScreen::printShaderCompileLog( GLuint shader ) {
	GLint logSize;
	// ���O�̒����́A�Ō��NULL�������܂�
	glGetShaderiv( shader, GL_INFO_LOG_LENGTH , &logSize );
	if( logSize > 1 ) {
		std::vector<char> log(logSize);
		GLint length;
		glGetShaderInfoLog( shader, logSize, &length, &(log[0]) );
		LOGW( "OpenGL ES2.0 Shader Compile Log\n%s\n", &(log[0]) );
	}
}
void tTVPScreen::printShaderLinkLog( GLuint program ) {
	GLint logSize;
	// ���O�̒����́A�Ō��NULL�������܂�
	glGetProgramiv( program, GL_INFO_LOG_LENGTH , &logSize );
	if( logSize > 1 ) {
		std::vector<char> log(logSize);
		GLint length;
		glGetProgramInfoLog( program, logSize, &length, &(log[0]) );
		LOGW( "OpenGL ES2.0 Shader Link Log\n%s\n", &(log[0]) );
	}
}

void tTVPScreen::setupProgram() {
	program_ = createProgram( VERTEX_CODE, FRAGMENT_CODE );
	positionHandle_ = glGetAttribLocation( program_, "a_Position" );
	glEnableVertexAttribArray( positionHandle_ );

	uvHandle_ = glGetAttribLocation( program_, "a_UV" );
	glEnableVertexAttribArray( uvHandle_ );

	texHandle_ = glGetUniformLocation( program_, "u_Tex" );
}

void tTVPScreen::drawTexture( int x, int y, int w, int h, GLuint textureId ) {
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// �e�N�X�`���̎w��
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, textureId );
	glUniform1i( texHandle_, 0 );

	{	// UV�o�b�t�@�̎w��
		const GLfloat uv[] = {
			0.0f,0.0f,//����
			0.0f,1.0f,//����
			1.0f,0.0f,//�E��
			1.0f,1.0f //�E��
		};
		glVertexAttribPointer( uvHandle_, 2, GL_FLOAT, false, 0, uv );
	}

	{	// �l�p�`�̕`��
		//�E�B���h�E���W�𐳋K���f�o�C�X���W�ɕϊ�
		float left  =((float)x/(float)width_)*2.0f-1.0f;
		float top   =((float)y/(float)height_)*2.0f-1.0f;
		float right =((float)(x+w)/(float)width_)*2.0f-1.0f;
		float bottom=((float)(y+h)/(float)height_)*2.0f-1.0f;
		top   =-top;
		bottom=-bottom;

		//���_�o�b�t�@�̐���
		GLfloat vertexs[] = {
				left, top,   0.0f,//���_0
				left, bottom,0.0f,//���_1
				right,top,   0.0f,//���_2
				right,bottom,0.0f //���_3
		};
		glVertexAttribPointer( positionHandle_, 3, GL_FLOAT, false, 0, vertexs );
	}
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}

// initialize OpenGL ES and EGL
bool tTVPScreen::initialize( tTVPApplication* app ) {
	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint attribs[] = {
		// EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
	const EGLint contextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	LOGI("eglGetDisplay\n");
	display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	LOGI("eglInitialize\n");
	eglInitialize( display_, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	EGLint numConfigs;
	EGLConfig config;
	LOGI("eglChooseConfig\n");
	eglChooseConfig( display_, attribs, &config, 1, &numConfigs );

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	LOGI("eglGetConfigAttrib\n");
	eglGetConfigAttrib( display_, config, EGL_NATIVE_VISUAL_ID, &format_ );

	ANativeWindow_setBuffersGeometry( app->getNativeWindow(), 0, 0, format_ );

	LOGI("eglCreateWindowSurface\n");
	surface_ = eglCreateWindowSurface( display_, config, app->getNativeWindow(), NULL );
	LOGI("eglCreateContext\n");
	context_ = eglCreateContext( display_, config, EGL_NO_CONTEXT, contextAttribs);

	LOGI("eglMakeCurrent\n");
	if( eglMakeCurrent( display_, surface_, surface_, context_ ) == EGL_FALSE ) {
		LOGW("Unable to eglMakeCurrent");
		return false;
	}

	LOGI("eglQuerySurface\n");
	eglQuerySurface( display_, surface_, EGL_WIDTH, &width_ );
	LOGI("eglQuerySurface\n");
	eglQuerySurface( display_, surface_, EGL_HEIGHT, &height_ );


	LOGI("glViewport\n");
	glViewport( 0, 0, width_, height_ );

	// Initialize GL state.
	//LOGI("glHint\n");
	//glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
	LOGI("glEnable\n");
	glEnable( GL_CULL_FACE );
	//LOGI("glShadeModel\n");
	//glShadeModel( GL_SMOOTH );
	LOGI("glDisable\n");
	glDisable( GL_DEPTH_TEST );

	const GLubyte* strVersion = glGetString(GL_VENDOR);
	LOGI("Vendor : %s\n",strVersion);
	strVersion = glGetString(GL_RENDERER);
	LOGI("Renderer : %s\n",strVersion);
	strVersion = glGetString(GL_VERSION);
	LOGI("Version : %s\n",strVersion);
	strVersion = glGetString (GL_EXTENSIONS);
	LOGI("Extensions : %s\n",strVersion);
	strVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	LOGI("GLSL Version : %s\n",strVersion);

	// load and set shader
	setupProgram();

	return 0;
}

void tTVPScreen::tarminate() {
	if( display_ != EGL_NO_DISPLAY ) {
		eglMakeCurrent( display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
		if( context_ != EGL_NO_CONTEXT ) {
			eglDestroyContext( display_, context_ );
		}
		if( surface_ != EGL_NO_SURFACE ) {
			eglDestroySurface( display_, surface_ );
		}
		eglTerminate( display_ );
	}
	display_ = EGL_NO_DISPLAY;
	context_ = EGL_NO_CONTEXT;
	surface_ = EGL_NO_SURFACE;
}
void tTVPScreen::drawFrame() {
	if( display_ == EGL_NO_DISPLAY ) return;

	glClearColor( 0, 0, 0, 1 ); // r, g, b, a
	glClear( GL_COLOR_BUFFER_BIT );

	eglSwapBuffers( display_, surface_ );
}
void tTVPScreen::beginDraw() {
	if( display_ == EGL_NO_DISPLAY ) return;

	// clear display
	glClearColor( 0, 0, 0, 1 ); // r, g, b, a
	glClear( GL_COLOR_BUFFER_BIT );
}
void tTVPScreen::endDraw() {
	if( display_ == EGL_NO_DISPLAY ) return;

	// swap
	eglSwapBuffers( display_, surface_ );
}

void tTVPScreen::OnActivate() {
}
void tTVPScreen::OnDeactivate() {
}
/*
void tTVPScreen::OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) {
}
void tTVPScreen::OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) {
}
void tTVPScreen::OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags) {
}
void tTVPScreen::OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) {
}
*/
void tTVPScreen::OnKeyDown(tjs_uint key, tjs_uint32 shift) {
}
void tTVPScreen::OnKeyUp(tjs_uint key, tjs_uint32 shift) {
}
void tTVPScreen::OnKeyPress(tjs_char key) {
}

void tTVPScreen::OnTouchDown( float x, float y, float cx, float cy, tjs_uint32 id ) {
}
void tTVPScreen::OnTouchUp( float x, float y, float cx, float cy, tjs_uint32 id ) {
}
void tTVPScreen::OnTouchMove( float x, float y, float cx, float cy, tjs_uint32 id ) {
}

void tTVPScreen::OnTouchScaling( float startdist, float curdist, float cx, float cy, tjs_int flag ) {
}
void tTVPScreen::OnTouchRotate( float startangle, float curangle, float dist, float cx, float cy, tjs_int flag ) {
}
void tTVPScreen::OnMultiTouch() {
}

void tTVPScreen::OnDisplayRotate( tjs_int orientation, tjs_int rotate, tjs_int bpp, tjs_int hresolution, tjs_int vresolution ) {
}

#if 0
int tTVPScreen::GetWidth() {
	HMONITOR hMonitor = ::MonitorFromWindow( Application->GetMainWindowHandle(), MONITOR_DEFAULTTOPRIMARY );
	MONITORINFO monitorinfo = {sizeof(MONITORINFO)};
	if( ::GetMonitorInfo( hMonitor, &monitorinfo ) != FALSE ) {
		return monitorinfo.rcMonitor.right - monitorinfo.rcMonitor.left;
	}
	return ::GetSystemMetrics(SM_CXSCREEN);
}
int tTVPScreen::GetHeight() {
	HMONITOR hMonitor = ::MonitorFromWindow( Application->GetMainWindowHandle(), MONITOR_DEFAULTTOPRIMARY );
	MONITORINFO monitorinfo = {sizeof(MONITORINFO)};
	if( ::GetMonitorInfo( hMonitor, &monitorinfo ) != FALSE ) {
		return monitorinfo.rcMonitor.bottom - monitorinfo.rcMonitor.top;
	}
	return ::GetSystemMetrics(SM_CYSCREEN);
}

void tTVPScreen::GetDesktopRect( RECT& r ) {
	HWND hWnd = Application->GetMainWindowHandle();
	if( hWnd != INVALID_HANDLE_VALUE ) {
		HMONITOR hMonitor = ::MonitorFromWindow( hWnd, MONITOR_DEFAULTTOPRIMARY );
		if( hMonitor != NULL ) {
			MONITORINFO monitorinfo = {sizeof(MONITORINFO)};
			if( ::GetMonitorInfo( hMonitor, &monitorinfo ) != FALSE ) {
				r = monitorinfo.rcWork;
				return;
			}
		}
	}
	::SystemParametersInfo( SPI_GETWORKAREA, 0, &r, 0 );
}
int tTVPScreen::GetDesktopLeft() {
	RECT r;
	GetDesktopRect(r);
	return r.left;
}
int tTVPScreen::GetDesktopTop() {
	RECT r;
	GetDesktopRect(r);
	return r.top;
}
int tTVPScreen::GetDesktopWidth() {
	RECT r;
	GetDesktopRect(r);
	return r.right - r.left;
}
int tTVPScreen::GetDesktopHeight() {
	RECT r;
	GetDesktopRect(r);
	return r.bottom - r.top;
}
#endif
