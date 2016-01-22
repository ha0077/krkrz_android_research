/**
 * �Ăяo�����n���h�����V���O���X���b�h�œ��삷��C�x���g�L���[
 * ���C���X���b�h�ŏ������������̂����鎞�Ɏg�p����
 **/

#ifndef __NATIVE_EVENT_QUEUE_H__
#define __NATIVE_EVENT_QUEUE_H__


class NativeEvent {
public:
	int Result;
	tjs_uint Message;
	tjs_uint WParam;
	tjs_uint64 LParam;
	NativeEvent( int mes ) : Result(0), Message(mes), WParam(0), LParam(0) {}
	NativeEvent(){}
};

class NativeEventQueueIntarface {
public:
	// �f�t�H���g�n���h��
	virtual void HandlerDefault( class NativeEvent& event ) = 0;

	// Queue �̐���
	virtual void Allocate() = 0;

	// Queue �̍폜
	virtual void Deallocate() = 0;

	virtual void Dispatch( class NativeEvent& event ) = 0;

	virtual void PostEvent( const NativeEvent& event ) = 0;
};

class NativeEventQueueImplement : public NativeEventQueueIntarface {
public:
	NativeEventQueueImplement() {}

	// �f�t�H���g�n���h��
	void HandlerDefault( NativeEvent& event );

	// Queue �̐���(�������͐ڑ�)
	void Allocate();

	// Queue �̍폜(�������͐ڑ�����)
	void Deallocate();

	// ���C���X���b�h��Dispatch�����s�����邽�߂ɔ񃁃C���X���b�h������Ăяo�����
	void PostEvent( const NativeEvent& event );
};

// �R���X�g���N�^�œn���ꂽ�N���X�̊֐��|�C���^�����C���X���b�h�Ŏ��s�����邽�߂̃N���X
template<typename T>
class NativeEventQueue : public NativeEventQueueImplement {
	void (T::*handler_)(NativeEvent&);
	T* owner_;

public:
	NativeEventQueue( T* owner, void (T::*Handler)(NativeEvent&) ) : owner_(owner), handler_(Handler) {}

	void Dispatch( NativeEvent &ev ) {
		(owner_->*handler_)(ev);
	}
};

#endif // __NATIVE_EVENT_QUEUE_H__
