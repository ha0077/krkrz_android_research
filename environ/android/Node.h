

#ifndef __NODE_H__
#define __NODE_H__

class tTJSNI_Node : public tTJSNativeInstance {
protected:
	iTJSDispatch2 *Owner;
	tTVPScreen* screen_;

	tjs_real x_;
	tjs_real y_;
	tjs_real width_;
	tjs_real height_;

	// ����v�Z���邩�A�ێ������܂܂ɂ��邩�c�c
	tjs_real real_x_;	// ���ۂ̈ʒuX
	tjs_real real_y_;	// ���ۂ̈ʒuY
	tjs_real real_width_;	// ���ۂ̕�
	tjs_real real_height_;	// ���ۂ̍���

	tTJSNI_Node *Parent;
	tObjectList<tTJSNI_Node> Children;

protected:
	void updateRealSize() {
		tjs_int original = screen_->getOriginalSize();
		tjs_int view = screen_->getViewSize();

		real_x_ = x_ * view / original;
		real_y_ = y_ * view / original;
		real_width_ = width_ * view / original;
		real_height_ = height_ * view / original;
	}

public:
	tTJSNI_Node();
	virtual ~tTJSNI_Node();

	virtual tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj);
	virtual void TJS_INTF_METHOD Invalidate();

	/**
	 * ���g��parent �ɒǉ�����
	 * @param parent �e�m�[�h
	 */
	void Join(tTJSNI_Node *parent);
	/**
	 * ���݂̐e���番������
	 */
	void Part();
	/**
	 * child �����g�̎q�Ƃ��Ēǉ�����
	 * @param child : �ǉ�����m�[�h
	 */
	void AddChild(tTJSNI_Node *child);
	/**
	 * child �����g�̎q����폜����
	 * @param child : �폜����m�[�h
	 */
	void RemoveChild(tTJSNI_Node *child);

	/**
	 * @param original : ��T�C�Y�B���̃T�C�Y��z�肵�č���Ă���
	 * @param view : �\���T�C�Y�B�t���X�N���[�������ő傫���Ȃ�B
	 * view / original ���g�嗦
	 * �ύX���A���ׂẴm�[�h�ɒʒm���s��
	 */
	virtual void OnChangeRealSize( tjs_int original, tjs_int view ) {
		updateRealSize();
	}
	/*
	virtual void OnPreDraw();
	virtual void OnPostDraw();
	*/

	virtual void OnDraw( NodeDrawDevice* drawdevice );

	virtual tTJSNI_Node* OnHitTest( tjs_int x, tjs_int y ) {
		return this;
	}
	virtual tTJSNI_Node* HitTest( tjs_int x, tjs_int y ) {
		if( ( x >= real_x_ && x < (real_x_+real_width_) ) &&
			( y >= real_y_ && y < (real_y_+real_height_) ) ) {
			return OnHitTest( x, y );
		}
		return NULL;
	}
};


#endif // __NODE_H__
