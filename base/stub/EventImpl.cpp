//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Script Event Handling and Dispatching
// TVPSystemControl �� Application�A�e�탆�[�e�B���e�B�N���X�ɂ���Ē��ۉ�����
// �Ă���̂ŁA���̃\�[�X�R�[�h�̓v���b�g�t�H�[�����ʂƂ��Ĉ�����B
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "EventImpl.h"

#include "SystemControl.h"
#include "ThreadIntf.h"
#include "TickCount.h"
#include "TimerIntf.h"
#include "SysInitIntf.h"
#include "DebugIntf.h"
#include "Application.h"
#include "NativeEventQueue.h"
#include "UserEvent.h"

//---------------------------------------------------------------------------
// TVPInvokeEvents for EventIntf.cpp
// �e�v���b�g�t�H�[���Ŏ�������B
// �A�v���P�[�V�����̏������o���Ă��鎞�A�C�x���g�̔z�M���m���ɂ��邽�߂ɕK�v
// �g���g���̃C�x���g��Post���ꂽ��ɃR�[�������B
// Windows�ł̓��b�Z�[�W�f�B�X�p�b�`���ŃC�x���g���n���h�����O���邽�߂�
// �_�~�[���b�Z�[�W�����C���E�B���h�E�֑����Ă���(::PostMessage)
//---------------------------------------------------------------------------
bool TVPEventInvoked = false;
void TVPInvokeEvents()
{
	if(TVPEventInvoked) return;
	TVPEventInvoked = true;
	// Windows�ł̓_�~�[���b�Z�[�W�����C���E�B���h�E�֑����Ă���
	if(TVPSystemControl) TVPSystemControl->InvokeEvents();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPEventReceived for EventIntf.cpp
// �e�v���b�g�t�H�[���Ŏ�������B
// �g���g���̃C�x���g�z�M��A���̃C�x���g�̏����̂��߂ɌĂяo�����B
// Windows�ł͓��ɉ������Ă��Ȃ�
//---------------------------------------------------------------------------
void TVPEventReceived()
{
	TVPEventInvoked = false;
	// Windows�ł̓J�E���^�����������Ă��邪�A���̒l�͖��g�p(�����폜)
	if( TVPSystemControl ) TVPSystemControl->NotifyEventDelivered();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPCallDeliverAllEventsOnIdle for EventIntf.cpp
// �e�v���b�g�t�H�[���Ŏ�������B
// ��xOS�ɐ����߂��A���̌�TVPInvokeEvents�i�j���Ăяo���悤�ɐݒ�B
// Windows�ł̓_�~�[���b�Z�[�W�����C���E�B���h�E�֑����Ă���(::PostMessage)
//---------------------------------------------------------------------------
void TVPCallDeliverAllEventsOnIdle()
{
	// Windows�ł̓_�~�[���b�Z�[�W�����C���E�B���h�E�֑����Ă���
	if( TVPSystemControl ) TVPSystemControl->CallDeliverAllEventsOnIdle();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TVPBreathe
// implement this in each platform
// to handle OS's message events
// this should be called when in a long time processing, something like a
// network access, to reply to user's Windowing message, repainting, etc...
// in TVPBreathe, TVP events must not be invoked. ( happened events over the
// long time processing are pending until next timing of message delivering. )
// �e�v���b�g�t�H�[���Ŏ�������B
// �����ԃ��C���X���b�h���~�߂Ă��܂�����GUI�����ł܂�Ȃ��悤�ɒ���I�ɌĂяo��
// �v���O�C���p�ɒ񋟂���Ă���
//---------------------------------------------------------------------------
static bool TVPBreathing = false;
void TVPBreathe()
{
	TVPEventDisabled = true; // not to call TVP events...
	TVPBreathing = true;
	try
	{
		// Windows�ł̓��b�Z�[�W�|���v����
		Application->ProcessMessages(); // do Windows message pumping
	}
	catch(...)
	{
		TVPBreathing = false;
		TVPEventDisabled = false;
		throw;
	}

	TVPBreathing = false;
	TVPEventDisabled = false;
}
//---------------------------------------------------------------------------
// breathing���������ǂ���
//---------------------------------------------------------------------------
bool TVPGetBreathing()
{
	// return whether now is in event breathing
	return TVPBreathing;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPSystemEventDisabledState
// sets whether system overall event handling is enabled.
// this works distinctly from TVPEventDisabled.
// �C�x���g�n���h�����O�̗L��/������ݒ肷��
//---------------------------------------------------------------------------
void TVPSetSystemEventDisabledState(bool en)
{
	TVPSystemControl->SetEventEnabled( !en );
	if(!en) TVPDeliverAllEvents();
}
//---------------------------------------------------------------------------
bool TVPGetSystemEventDisabledState()
{
	return !TVPSystemControl->GetEventEnabled();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPContinuousHandlerCallLimitThread
//---------------------------------------------------------------------------
class tTVPContinuousHandlerCallLimitThread : public tTVPThread
{
	tjs_uint64 NextEventTick;
	tjs_uint64 Interval;
	tTVPThreadEvent Event;
	tTJSCriticalSection CS;

	bool Enabled;

	NativeEventQueue<tTVPContinuousHandlerCallLimitThread> EventQueue;

public:
	tTVPContinuousHandlerCallLimitThread();
	~tTVPContinuousHandlerCallLimitThread();

protected:
	void Execute();

	void WndProc(NativeEvent& ev) {
		EventQueue.HandlerDefault(ev);
	}

public:
	void SetEnabled(bool enabled);

	void SetInterval(tjs_uint64 interval) { Interval = interval; }


} static * TVPContinuousHandlerCallLimitThread = NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPContinuousHandlerCallLimitThread::tTVPContinuousHandlerCallLimitThread()
	 : tTVPThread(true), EventQueue(this,&tTVPContinuousHandlerCallLimitThread::WndProc)
{
	NextEventTick = 0;
	Interval = (1<<TVP_SUBMILLI_FRAC_BITS)*1000/60; // default 60Hz
	Enabled = false;
	EventQueue.Allocate();
	Resume();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPContinuousHandlerCallLimitThread::~tTVPContinuousHandlerCallLimitThread()
{
	Terminate();
	Resume();
	Event.Set();
	WaitFor();
	EventQueue.Deallocate();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPContinuousHandlerCallLimitThread::Execute()
{
	while(!GetTerminated())
	{
		tjs_uint64 curtick = TVPGetTickCount() << TVP_SUBMILLI_FRAC_BITS;
		DWORD sleeptime;

		{	// thread-protected
			tTJSCriticalSectionHolder holder(CS);

			if(Enabled)
			{
				if(NextEventTick <= curtick)
				{
					TVPProcessContinuousHandlerEventFlag = true; // set flag to process event on next idle
					EventQueue.PostEvent( NativeEvent(TVP_EV_CONTINUE_LIMIT_THREAD) );
					while(NextEventTick <= curtick) NextEventTick += Interval;
				}
				tjs_uint64 sleeptime_64 = NextEventTick - curtick;
				sleeptime = (DWORD)(sleeptime_64 >> TVP_SUBMILLI_FRAC_BITS) +
						((sleeptime_64 & ((1<<TVP_SUBMILLI_FRAC_BITS)-1))?1:0);
							// add 1 if fraction exists
			}
			else
			{
				sleeptime = 10000; // how long to sleep when disabled does not matter
			}


		}	// end-of-thread-protected

		if(sleeptime == 0) sleeptime = 1; // 0 will let thread sleeping forever ...
		Event.WaitFor(sleeptime);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPContinuousHandlerCallLimitThread::SetEnabled(bool enabled)
{
	tTJSCriticalSectionHolder holder(CS);

	Enabled = enabled;
	if(enabled)
	{
		tjs_uint64 curtick = TVPGetTickCount() << TVP_SUBMILLI_FRAC_BITS;
		NextEventTick = ((curtick + 1) / Interval) * Interval;
		Event.Set();
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static tjs_int TVPContinousHandlerLimitFrequency = 0;
//---------------------------------------------------------------------------
void TVPBeginContinuousEvent()
{
	// read commandline options
	static tjs_int ArgumentGeneration = 0;
	if(ArgumentGeneration != TVPGetCommandLineArgumentGeneration())
	{
		ArgumentGeneration = TVPGetCommandLineArgumentGeneration();

		tTJSVariant val;
		if( TVPGetCommandLine(TJS_W("-contfreq"), &val) )
		{
			TVPContinousHandlerLimitFrequency = (tjs_int)val;
		}
	}

	if(!TVPIsWaitVSync())
	{
		if(TVPContinousHandlerLimitFrequency == 0)
		{
			// no limit
			// this notifies continuous calling of TVPDeliverAllEvents.
			if(TVPSystemControl) TVPSystemControl->BeginContinuousEvent();
		}
		else
		{
			// has limit
			if(!TVPContinuousHandlerCallLimitThread)
				TVPContinuousHandlerCallLimitThread = new tTVPContinuousHandlerCallLimitThread();
			TVPContinuousHandlerCallLimitThread->SetInterval(
				 (1<<TVP_SUBMILLI_FRAC_BITS)*1000/TVPContinousHandlerLimitFrequency);
			TVPContinuousHandlerCallLimitThread->SetEnabled(true);
		}
	}


	// TVPEnsureVSyncTimingThread();
	// if we wait vsync, the continuous handler will be executed at the every timing of
	// vsync.
}
//---------------------------------------------------------------------------
void TVPEndContinuousEvent()
{
	// anyway
	if(TVPContinuousHandlerCallLimitThread)
		TVPContinuousHandlerCallLimitThread->SetEnabled(false);

	// anyway
	if(TVPSystemControl) TVPSystemControl->EndContinuousEvent();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static void TVPReleaseContinuousHandlerCallLimitThread()
{
	if(TVPContinuousHandlerCallLimitThread)
		delete TVPContinuousHandlerCallLimitThread,
			TVPContinuousHandlerCallLimitThread = NULL;
}
// to release TVPContinuousHandlerCallLimitThread at exit
static tTVPAtExit TVPTimerThreadUninitAtExit(TVP_ATEXIT_PRI_SHUTDOWN,
	TVPReleaseContinuousHandlerCallLimitThread);
//---------------------------------------------------------------------------
