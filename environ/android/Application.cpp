
#include "tjsCommHead.h"

#include "tjsError.h"
#include "tjsDebug.h"

#include <jni.h>
#include <errno.h>
#include <dirent.h>
#include <dlfcn.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/looper.h>
#include "Application.h"

#include "ScriptMgnIntf.h"
#include "SystemIntf.h"
#include "DebugIntf.h"
#include "TickCount.h"
#include "NativeEventQueue.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "krkrz", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "krkrz", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "krkrz", __VA_ARGS__))

tTVPApplication* Application;


tTVPApplication::tTVPApplication()
: app_state_(NULL)
{
}
tTVPApplication::~tTVPApplication() {
}
//-------------------------------------------------------------------
#if 0
class AssetCache {
	class AssetDirectory {
		bool is_file_;
		std::string name_;
		std::map<std::string,AssetDirectory*> files_;
	public:
		AssetDirectory() : is_file_(false) {}
		AssetDirectory( bool isfile, const char* name ) : is_file_(isfile), name_(name) {}
		AssetDirectory( bool isfile, const std::string& name ) : is_file_(isfile), name_(name) {}
		~AssetDirectory() {
			std::map<std::string,AssetDirectory*>::iterator i = files_.begin();
			for( ; i != files_.end(); ++i ) {
				delete (*i).second;
			}
			files_.clear();
		}
		void pushFile( const std::string& name, AssetDirectory* dir ) {
			files_.insert( std::map<std::string,AssetDirectory*>::value_type( name, dir ) );
		}
		AssetDirectory* getFile( const std::string& name ) {
			std::map<std::string,AssetDirectory*>::iterator i = files_.find( name );
			if( i != files_.end() ) {
				return (*i).second;
			} else {
			}
		}
		const std::string& getName() const { return name_; }
		bool isFile() const { return is_file_; }
	};
	AssetDirectory root_;

private:
	void searchDir( AAssetManager* mgr, AAssetDir* dir, AssetDirectory& current, const std::string& base ) {
		LOGI( "find path %s\n", base.c_str() );
		const char* filename = NULL;
		do {
			filename = AAssetDir_getNextFileName( dir );
			if( filename ) {
				std::string curfile(filename);
				std::string path( base + curfile );
				//AAssetDir* newdir = AAssetManager_openDir( mgr, path.c_str() );
				// �t�@�C���ƃf�B���N�g���̎��ʕ��@�͂Ȃ��H
				// ���������t�@�C�����������Ȃ��l�q
				// Asset �ɗ����������͖����ہAAsset���烍�[�J���Ƀt�@�C���R�s�[���ē������������ǂ���
				// �X�N���v�g�����āA���s�p�X�w�肵������������
				AAssetDir* newdir = NULL;
				if( newdir ) {
					LOGI( "find dir %s\n", path.c_str() );
					AssetDirectory* finddir = new AssetDirectory( false, curfile );
					current.pushFile( curfile, finddir );
					// �ċA
					searchDir( mgr, newdir, *finddir, path + std::string("/") );
					AAssetDir_close( newdir );
				} else {
					LOGI( "find file %s\n", path.c_str() );
					// �t�@�C��������
					current.pushFile( curfile, new AssetDirectory( true, curfile ) );
				}
			}
		} while( filename );
	}
public:
	AssetCache() {}
	void initialize() {
		AAssetManager* mgr = Application->getAssetManager();
		AAssetDir* dir = AAssetManager_openDir( mgr, "" );
		if( dir ) {
			searchDir( mgr, dir, root_, std::string("") );
			AAssetDir_close( dir );
		}
		
		AAsset* root = AAssetManager_open( mgr, "", AASSET_MODE_STREAMING );
		if( root ) {
			off_t start, length;
			int fd = AAsset_openFileDescriptor( root, start, length );
			if( fd >= 0 ) {
				
			}
		}
	}
};
//-------------------------------------------------------------------
#endif
extern void print_font_files();
bool tTVPApplication::initCommandPipe() {
	int msgpipe[2];
	//if( pipe(msgpipe) ) {
	if( pipe2(msgpipe, O_NONBLOCK | O_CLOEXEC) == -1 ) {
		LOGE("could not create pipe: %s", strerror(errno));
		return false;
	}
	user_msg_read_ = msgpipe[0];
	user_msg_write_ = msgpipe[1];
	ALooper_addFd( app_state_->looper, user_msg_read_, LOOPER_ID_USER, ALOOPER_EVENT_INPUT, tTVPApplication::messagePipeCallBack, this );

	return true;
}
void tTVPApplication::finalCommandPipe() {
	ALooper_removeFd( app_state_->looper, user_msg_read_ );
	close( user_msg_read_ );
	close( user_msg_write_ );
}
	// �R�}���h�����C���̃��b�Z�[�W���[�v�ɓ�����
void tTVPApplication::postEvent( const NativeEvent* ev ) {
	if( write( user_msg_write_, ev, sizeof(NativeEvent)) != sizeof(NativeEvent) ) {
		LOGE("Failure writing pipe event: %s\n", strerror(errno));
	}
}
int8_t tTVPApplication::readCommand() {
	int8_t cmd;
	if( read(user_msg_read_, &cmd, sizeof(cmd)) == sizeof(cmd) ) {
		return cmd;
	} else {
		LOGE("No data on command pipe!");
	}
	return -1;
}
int tTVPApplication::messagePipeCallBack(int fd, int events, void* user) {
	if( user != NULL ) {
		tTVPApplication* app = (tTVPApplication*)user;
		NativeEvent msg;
		while( read(fd, &msg, sizeof(NativeEvent)) == sizeof(NativeEvent) ) {
			app->HandleMessage(msg);
		}
	}
	return 1;
}
void tTVPApplication::HandleMessage( NativeEvent& ev ) {
	for( std::vector<NativeEventQueueIntarface*>::iterator it = event_handlers_.begin(); it != event_handlers_.end(); it++ ) {
		if( (*it) != NULL ) (*it)->Dispatch( ev );
	}
}
// for iTVPApplication
void tTVPApplication::startApplication( struct android_app* state ) {
	assert( state );
	app_state_ = state;

	state->userData = this;
	state->onAppCmd = tTVPApplication::handleCommand;
	state->onInputEvent = tTVPApplication::handleInput;
	initCommandPipe();

	if( state->savedState != NULL ) {
		// We are starting with a previous saved state; restore from it.
		loadSaveState( state->savedState );
	}

	//print_font_files();
	// �������珉����
	
	// try starting the program!
	bool engine_init = false;
	try {
		// TJS2 �X�N���v�g�G���W��������������startup.tjs���ĂԁB
		TVPInitScriptEngine();
		engine_init = true;

		// banner
		TVPAddImportantLog( TVPFormatMessage(TVPProgramStartedOn, TVPGetOSName(), TVPGetPlatformName()) );
		
		// main loop
		tjs_uint32 tick = TVPGetRoughTickCount32();
		while( 1 ) { // Read all pending events.
			int ident;
			int events;
			//struct android_poll_source* source;
			void* source;
			int timeout = 16;	//16msec�����œ��삷��悤�ɂ���
			while( (ident = ALooper_pollAll( timeout/* msec */, NULL, &events, (void**)&source)) != ALOOPER_POLL_TIMEOUT ) {
				// Process this event.
				if( source != NULL ) {
					if( (tTVPApplication*)source == this ) {
						// user_msg_write_ �֓�����ꂽ�R�}���h
					} else {
						struct android_poll_source* ps = (struct android_poll_source*)source;
						ps->process(state, ps);
					}
				}

				// If a sensor has data, process it now.
				/*
				if( ident == LOOPER_ID_USER ) {
					handleSensorEvent();
				}
				*/

				// Check if we are exiting.
				if( state->destroyRequested != 0 ) {
					tarminateProcess();
					return;
				}
				tjs_uint32 curtick = TVPGetRoughTickCount32();
				if( tick > curtick ) {	// 1������Ă��܂����ꍇ
					curtick += 0xffffffffUL - tick;
					tick = 0;
				}
				timeout = 16 - (curtick - tick);
				if( timeout < 0 ) timeout = 0;
			}
			handleIdle();
			tick = TVPGetRoughTickCount32();
		}
	} catch(...) {
	}
}

void tTVPApplication::handleCommand( struct android_app* state, int32_t cmd ) {
	tTVPApplication* app = (tTVPApplication*)(state->userData);
	app->onCommand( state, cmd );
}
int32_t tTVPApplication::handleInput( struct android_app* state, AInputEvent* event ) {
	tTVPApplication* app = (tTVPApplication*)(state->userData);
	return app->onInput( state, event );
}
void tTVPApplication::onCommand( struct android_app* state, int32_t cmd ) {
	switch( cmd ) {
		case APP_CMD_SAVE_STATE:
			saveState();
			break;
		case APP_CMD_INIT_WINDOW:
			initializeWindow();
			break;
		case APP_CMD_TERM_WINDOW:
			tarminateWindow();
			break;
        case APP_CMD_GAINED_FOCUS:
			gainedFocus();
			break;
		case APP_CMD_LOST_FOCUS:
			lostFocus();
			break;
		case APP_CMD_INPUT_CHANGED:
			inputChanged();
			break;
		case APP_CMD_WINDOW_RESIZED:
			windowResized();
			break;
		case APP_CMD_WINDOW_REDRAW_NEEDED:
			windowRedrawNeeded();
			break;
		case APP_CMD_CONTENT_RECT_CHANGED:
			contentRectChanged();
			break;
		case APP_CMD_CONFIG_CHANGED:
			configChanged();
			break;
		case APP_CMD_LOW_MEMORY:
			lowMemory();
			break;
		case APP_CMD_START:
			onStart();
			break;
		case APP_CMD_RESUME:
			onResume();
			break;
		case APP_CMD_PAUSE:
			onPause();
			break;
		case APP_CMD_STOP:
			onStop();
			break;
		case APP_CMD_DESTROY:
			onDestroy();
			break;
	}
}
int32_t tTVPApplication::onInput( struct android_app* state, AInputEvent* event ) {
	int32_t type = AInputEvent_getType(event);
	if( type == AINPUT_EVENT_TYPE_MOTION ) {
		int32_t src = AInputEvent_getSource(event);	// ���̓f�o�C�X�̎��
		// src == AINPUT_SOURCE_TOUCHSCREEN �^�b�`�X�N���[��
		// src == AINPUT_SOURCE_MOUSE AINPUT_SOURCE_TRACKBALL AINPUT_SOURCE_TOUCHPAD
		int32_t action = AMotionEvent_getAction(event);
		int32_t meta = AMotionEvent_getMetaState(event);
		// AMotionEvent_getEventTime(event); // �C�x���g��������
		// AMotionEvent_getDownTime(event); // ������Ă�������
		// AMotionEvent_getEdgeFlags(event); // �X�N���[���[����
		float x = AMotionEvent_getX(event, 0);
		float y = AMotionEvent_getY(event, 0);
		float cy = AMotionEvent_getTouchMajor(event,0);	// �G����Ă��钷�� �w�̌`�󂩂�c���ɂ��Ă���
		float cx = AMotionEvent_getTouchMinor(event,0);	// �G����Ă���Z�� �w�̌`�󂩂牡���ɂ��Ă���
		float pressure = AMotionEvent_getPressure(event, 0);	// ����
		/*
		float size = AMotionEvent_getSize(event, 0);	// �͈�(����l) �f�o�C�X�ŗL�l����0-1�͈̔͂ɐ��K����������
		float toolmajor = AMotionEvent_getToolMajor(event,0);
		float toolminor = AMotionEvent_getToolMinor(event,0);
		LOGI( "press : %f, size: %f, major : %f, minor : %f\n", pressure, size, toolmajor, toolminor );
		*/
		int32_t id = AMotionEvent_getPointerId(event, 0);
		action &= AMOTION_EVENT_ACTION_MASK;
		switch( action ) {
		case AMOTION_EVENT_ACTION_DOWN:
			OnTouchDown( x, y, cx, cy, id, pressure, meta );
			break;
		case AMOTION_EVENT_ACTION_UP:
			OnTouchUp( x, y, cx, cy, id, pressure, meta );
			break;
		case AMOTION_EVENT_ACTION_CANCEL:	// Down/Up���������B���肦��́H
			break;
		case AMOTION_EVENT_ACTION_MOVE:
			OnTouchMove( x, y, cx, cy, id, pressure, meta );
			break;
		case AMOTION_EVENT_ACTION_POINTER_DOWN: {	// multi-touch
			size_t downidx = (action&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			size_t count = AMotionEvent_getPointerCount(event);
			if( downidx == 0 ) {
				OnTouchDown( x, y, cx, cy, id, pressure, meta );
			} else {
				OnTouchMove( x, y, cx, cy, id, pressure, meta );
			}
			for( size_t i = 1; i < count; i++ ) {
				x = AMotionEvent_getX(event, i);
				y = AMotionEvent_getY(event, i);
				cy = AMotionEvent_getTouchMajor(event,i);
				cx = AMotionEvent_getTouchMinor(event,i);
				pressure = AMotionEvent_getPressure(event, i);
				id = AMotionEvent_getPointerId(event, i);
				if( i == downidx ) {
					OnTouchDown( x, y, cx, cy, id, pressure, meta );
				} else {
					OnTouchMove( x, y, cx, cy, id, pressure, meta );
				}
			}
			break;
		}
		case AMOTION_EVENT_ACTION_POINTER_UP: {	// multi-touch
			size_t upidx = (action&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			size_t count = AMotionEvent_getPointerCount(event);
			if( upidx == 0 ) {
				OnTouchUp( x, y, cx, cy, id, pressure, meta );
			} else {
				OnTouchMove( x, y, cx, cy, id, pressure, meta );
			}
			for( size_t i = 1; i < count; i++ ) {
				x = AMotionEvent_getX(event, i);
				y = AMotionEvent_getY(event, i);
				cy = AMotionEvent_getTouchMajor(event,i);
				cx = AMotionEvent_getTouchMinor(event,i);
				pressure = AMotionEvent_getPressure(event, i);
				id = AMotionEvent_getPointerId(event, i);
				if( i == upidx ) {
					OnTouchUp( x, y, cx, cy, id, pressure, meta );
				} else {
					OnTouchMove( x, y, cx, cy, id, pressure, meta );
				}
			}
			break;
		}
		case AMOTION_EVENT_ACTION_OUTSIDE:
			break;
		}
		return 1;
	} else if( type == AINPUT_EVENT_TYPE_KEY ) { // key events
		int32_t src = AInputEvent_getSource(event);	// ���̓f�o�C�X�̎��
		// src == AINPUT_SOURCE_KEYBOARD AINPUT_SOURCE_DPAD
		return 1;
	}
	return 0;
}

#if 0
void tTVPApplication::setApplicationState( struct android_app* state ) {
	assert( state );
	app_state_ = state;
	// �����ł��낢��Ə��������Ă��܂��������悳��

	std::string internalDataPath( getInternalDataPath() );
	TVPUtf8ToUtf16( internal_data_path_, internalDataPath );

	std::string externalDataPath( getExternalDataPath() );
	TVPUtf8ToUtf16( external_data_path_, externalDataPath );

	/*
	// Prepare to monitor accelerometer
	sensor_manager_ = ASensorManager_getInstance();
	accelerometer_sensor_ = ASensorManager_getDefaultSensor( sensorManager, ASENSOR_TYPE_ACCELEROMETER );
	sensor_event_queue_ = ASensorManager_createEventQueue( sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL );
	*/
}
#endif
void tTVPApplication::loadSaveState( void* state ) {
}
void tTVPApplication::handleSensorEvent() {
}
void tTVPApplication::tarminateProcess() {
	screen_.tarminate();
}
void tTVPApplication::handleIdle() {
}
void tTVPApplication::saveState() {
	clearSaveState();
}
void tTVPApplication::initializeWindow() {
	screen_.initialize(this);
}
void tTVPApplication::tarminateWindow() {
	screen_.tarminate();
}
void tTVPApplication::gainedFocus() {
}
void tTVPApplication::lostFocus() {
}
void tTVPApplication::inputChanged() {
}
void tTVPApplication::windowResized() {
}
void tTVPApplication::windowRedrawNeeded() {
}
void tTVPApplication::contentRectChanged() {
}
void tTVPApplication::configChanged() {
}
void tTVPApplication::lowMemory() {
}
void tTVPApplication::onStart() {
}
void tTVPApplication::onResume() {
}
void tTVPApplication::onPause() {
}
void tTVPApplication::onStop() {
}
void tTVPApplication::onDestroy() {
}
void tTVPApplication::OnTouchDown( float x, float y, float cx, float cy, int32_t id, float pressure, int32_t meta ) {
	screen_.OnTouchDown( x, y, cx, cy, id );
}
void tTVPApplication::OnTouchMove( float x, float y, float cx, float cy, int32_t id, float pressure,int32_t meta ) {
	screen_.OnTouchMove( x, y, cx, cy, id );
}
void tTVPApplication::OnTouchUp( float x, float y, float cx, float cy, int32_t id, float pressure,int32_t meta ) {
	screen_.OnTouchUp( x, y, cx, cy, id );
}



extern "C" {
iTVPApplication* CreateApplication() {
	Application = new tTVPApplication();
	return Application;
}
void DestroyApplication( iTVPApplication* app ) {
	delete app;
	Application = NULL;
}
};

