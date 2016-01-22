
#ifndef __APPLICATION_INTERFACE_H__
#define __APPLICATION_INTERFACE_H__

class iTVPApplication {
	// struct android_app*
	// ASensorManager* sensorManager;
	// const ASensor* accelerometerSensor;
	// ASensorEventQueue* sensorEventQueue;
public:
	virtual void startApplication( struct android_app* state ) = 0;
	
	virtual void setApplicationState( struct android_app* state ) = 0;

	// ���O�ɕۑ����ꂽSaveState�𓾂āA����������
	virtual void loadSaveState( void* state ) = 0;

	virtual void handleSensorEvent() = 0;
	/*
if( engine.accelerometerSensor != NULL ) {
    ASensorEvent event;
    while (ASensorEventQueue_getEvents(engine.sensorEventQueue, &event, 1) > 0) {
        // LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
    }
}
	*/
	// �I�����ɌĂ΂��
	virtual void tarminateProcess() = 0;
	// 
	virtual void handleIdle() = 0;
	/**
	 * �A�v���P�[�V�������ꎞ�I�ɏI�����鎞�A����\�����ɕ��A�ł���悤�ɌĂяo�����
	 * android_app->savedState �ɕۑ�����f�[�^�� (malloc�Ń������m�ۂ��ēn��)
	 * android_app->savedStateSize �ɕۑ�����f�[�^�T�C�Y������
	 * �������AonSaveInstanceState �Ȃ̂ŁA100%�Ăяo�����Ƃ͌���Ȃ�
	 * onPause �̃^�C�~���O�Ńf�[�^��ۑ���������m��
	 */
	virtual void saveState() = 0;
	/*
		// The system has asked us to save our current state.  Do so.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
	*/
	virtual void initializeWindow() = 0;
	/*
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
		    engine_init_display(engine);
		    engine_draw_frame(engine);
		}
	*/
	virtual void tarminateWindow() = 0;
	/*
		// The window is being hidden or closed, clean it up.
		engine_term_display(engine);
	*/
	virtual void gainedFocus() = 0;
	/*
		// When our app gains focus, we start monitoring the accelerometer.
		if (engine->accelerometerSensor != NULL) {
		    ASensorEventQueue_enableSensor(engine->sensorEventQueue,
		            engine->accelerometerSensor);
		    // We'd like to get 60 events per second (in us).
		    ASensorEventQueue_setEventRate(engine->sensorEventQueue,
		            engine->accelerometerSensor, (1000L/60)*1000);
		}
	*/
	virtual void lostFocus() = 0;
	/*
		// When our app loses focus, we stop monitoring the accelerometer.
		// This is to avoid consuming battery while not being used.
		if (engine->accelerometerSensor != NULL) {
		    ASensorEventQueue_disableSensor(engine->sensorEventQueue,
		            engine->accelerometerSensor);
		}
		// Also stop animating.
		engine->animating = 0;
		engine_draw_frame(engine);
	*/
	virtual void inputChanged() = 0;
	virtual void windowResized() = 0;
	virtual void windowRedrawNeeded() = 0;
	virtual void contentRectChanged() = 0;
	virtual void configChanged() = 0;
	virtual void lowMemory() = 0;
	virtual void onStart() = 0;
	virtual void onResume() = 0;
	virtual void onPause() = 0;
	virtual void onStop() = 0;
	virtual void onDestroy() = 0;

	virtual void OnTouchDown( float x, float y, float cx, float cy, int32_t id, float pressure, int32_t meta ) = 0;
	virtual void OnTouchMove( float x, float y, float cx, float cy, int32_t id, float pressure,int32_t meta ) = 0;
	virtual void OnTouchUp( float x, float y, float cx, float cy, int32_t id, float pressure,int32_t meta ) = 0;
};


/*
iTVPApplication* CreateApplication();
void DestroyApplication( iTVPApplication* app );
*/

#endif // __APPLICATION_INTERFACE_H__
