// Form = new TTVPWindowForm

// ���z�E�B���h�E�Ƃ��Ď������Ă��܂��̂�������
// �V���O���E�B���h�E���ł́A1���1�E�B���h�E�݂̂Ƃ��āA1�̃E�B���h�E�����A�N�e�B�u�ɂȂ�Ȃ��B
// �E�B���h�E�̐؂�ւ����@���񋟂���Ă��Ȃ��ꍇ�́A�i�s�s�\�ɂȂ��Ă��܂������߂Ă��炤�H
// DrawDevice �̓e�N�X�`���ɕ`�悵�āA�����Ԃ������̎������������ȁH ���̃e�N�X�`��������Window�Ƃ��āB
// ������̓}���`�E�B���h�E���\�ɂȂ�悤��
/*
 * �V���O���E�B���h�E�n�V�X�e���̂��߂ɁA�}���`�E�B���h�E�͎g���Ȃ���������ς肢�����ȁB
 * Window ���قڂ��̂܂܃f�B�X�v���C�ƕR�Â��Ă���悤��
 */
class tTVPWindowForm {
	// Window���L�����ǂ����A�������ƃC�x���g���z�M����Ȃ�
	bool GetFormEnabled();

	// ����
	void InvalidateClose();
	void SendCloseMessage()
	void Close();
	void OnCloseQueryCalled(bool b);

	// ����I�ɌĂяo�����̂ŁA�������������Ύ��s����
	void TickBeat()

	// �A�N�e�B�u/�f�A�N�e�B�u�����ꂽ���ɁAWindow���A�N�e�B�u���ǂ����`�F�b�N�����
	bool GetWindowActive()

	// DrawDevice
	void ResetDrawDevice()

	// �L�[����
	void InternalKeyDown(WORD key, tjs_uint32 shift);
	void OnKeyUp( WORD vk, int shift );
	void OnKeyPress( WORD vk, int repeat, bool prevkeystate, bool convertkey );

	// �v���C�}���[���C���[�̃T�C�Y�ɍ����悤�ɌĂяo�����Bw/h��LayerWidth/LayerHeight�ɓ�����A�Y�[�����l�����ĕ\���T�C�Y��ݒ肷��
	void SetPaintBoxSize(tjs_int w, tjs_int h);

	// �}�E�X�J�[�\��
	void SetDefaultMouseCursor();
	void SetMouseCursor(tjs_int handle);
	void HideMouseCursor();
	void SetMouseCursorState(tTVPMouseCursorState mcs);
	tTVPMouseCursorState GetMouseCursorState() const
	// void ReleaseCapture();
	// �}�E�X�J�[�\�����W��Layer.cursorX/cursorY�œ�����悤�ɂȂ��Ă��邪�A�^�b�`�X�N���[�����Ɩ��Ӗ��Ȃ̂Ŏg���Ȃ��Ȃ�
	void GetCursorPos(tjs_int &x, tjs_int &y);
	void SetCursorPos(tjs_int x, tjs_int y);

	// �q���g�\��(Android�ł͖�����)
	void SetHintText(iTJSDispatch2* sender, const ttstr &text);
	void SetHintDelay( tjs_int delay );
	tjs_int GetHintDelay() const;

	// IME ���͊֌W�AEditView���őO�ʂɓ\��t���āA�����œ��͂�����̂������I���ȁA�D���Ȉʒu�ɕ\���͉�ʋ����Ƃ��܂茻���I����Ȃ�����
	// ���̓^�C�g�����w�肵�āA���͎�t�A�m�蕶�����Ԃ��Ă���X�^�C���̕����������A���[�_���ɂ͂Ȃ�Ȃ�����A�m���C�x���g�ʒm����
	void SetAttentionPoint(tjs_int left, tjs_int top, const struct tTVPFont * font );
	void DisableAttentionPoint();
	void SetImeMode(tTVPImeMode mode);
	tTVPImeMode GetDefaultImeMode() const;
	void ResetImeMode();

	// Window�n���h���n���\�b�h�͂��ׂĕs�v
	// VideoOverlay �� SetMessageDrainWindow �ɓn�� Window �n���h��
	HWND GetSurfaceWindowHandle();
	// VideoOverlay �� OwnerWindow �Ƃ��Đݒ肳��� Window �n���h��
	HWND GetWindowHandle();
	// Window::HWND �v���p�e�B�l�Ƃ��Ďg�p�����B��Windows���ł̓v���O�C���ɓn�����Əo���Ȃ�����s�v���B�_�~�[��NULL�Ԃ��΂���
	HWND GetWindowHandleForPlugin();

	// VideoOverlay�ŕ\���T�C�Y�����߂邽�߂ɃY�[���l��p���Ĉ����l���g��k������
	void ZoomRectangle( tjs_int & left, tjs_int & top, tjs_int & right, tjs_int & bottom);
	// VideoOverlay�ŕ\���ʒu�����߂邽�߁A�t���X�N���[�����̍��ӂ����l������left/top�ʒu�𓾂�
	void GetVideoOffset(tjs_int &ofsx, tjs_int &ofsy);

	// �v���O�C���Ń��b�Z�[�W���V�[�o��o�^����Ɏg����AAndroid�ł͖���
	void RegisterWindowMessageReceiver(tTVPWMRRegMode mode, void * proc, const void *userdata);


	// ���e�X�V
	void UpdateWindow(tTVPUpdateType type = utNormal);

	// �\��/��\��
	bool GetVisible() const;
	void SetVisibleFromScript(bool b);
	void ShowWindowAsModal();

	// �^�C�g���AActivity�̃^�C�g���ɐݒ�ł��邪�A���Ӗ�����
	std::wstring GetCaption();
	void GetCaption( std::wstring& v ) const;
	void SetCaption( const std::wstring& v );

	// �T�C�Y��ʒu�Ȃ�
	// �ʒu��Android�ł͖������A���0��Ԃ��A�ݒ���X���[�Ȃ�
	void SetLeft( int l );
	int GetLeft() const;
	void SetTop( int t );
	int GetTop() const;
	void SetPosition( int l, int t );
	// �T�C�Y
	void SetWidth( int w );
	int GetWidth() const;
	void SetHeight( int h );
	int GetHeight() const;
	void SetSize( int w, int h );
	// �ŏ��A�ő�T�C�Y�֌W�AAndroid�Ȃǃ��T�C�Y���Ȃ��Ƃ����疳����
	void SetMinWidth( int v )
	int GetMinWidth() const
	void SetMinHeight( int v )
	int GetMinHeight()
	void SetMinSize( int w, int h )
	void SetMaxWidth( int v )
	int GetMaxWidth()
	void SetMaxHeight( int v )
	int GetMaxHeight()
	void SetMaxSize( int w, int h )

	// �����̃T�C�Y�A�����I�ɂ��ꂪ�\���̈�T�C�Y
	void SetInnerWidth( int w );
	int GetInnerWidth() const;
	void SetInnerHeight( int h );
	int GetInnerHeight() const;
	void SetInnerSize( int w, int h );

	// ���E�T�C�Y�A����
	void SetBorderStyle( enum tTVPBorderStyle st);
	enum tTVPBorderStyle GetBorderStyle() const;

	// ��ɍőO�ʕ\���A����
	void SetStayOnTop( bool b );
	bool GetStayOnTop() const;
	// �őO�ʂֈړ�
	void BringToFront();

	// �t���X�N���[���A�����ƌ�������ɐ^
	void SetFullScreenMode(bool b);
	bool GetFullScreenMode() const;

	//�}�E�X�L�[(�L�[�{�[�h�ł̃}�E�X�J�[�\������)�͖���
	void SetUseMouseKey(bool b);
	bool GetUseMouseKey() const;

	// ���E�B���h�E�̃L�[���͂��g���b�v���邩�A����
	void SetTrapKey(bool b);
	bool GetTrapKey() const;

	// �E�B���h�E�}�X�N���[�W�����͖���
	void SetMaskRegion(HRGN threshold);
	void RemoveMaskRegion();

	// �t�H�[�X�͏�ɐ^
	void SetFocusable(bool b);
	bool GetFocusable() const;

	// �\���Y�[���֌W
	void SetZoom(tjs_int numer, tjs_int denom, bool set_logical = true);
	void SetZoomNumer( tjs_int n );
	tjs_int GetZoomNumer() const;
	void SetZoomDenom(tjs_int d);
	tjs_int GetZoomDenom() const;

	// �^�b�`���͊֌W
	void SetTouchScaleThreshold( double threshold )
	double GetTouchScaleThreshold() const
	void SetTouchRotateThreshold( double threshold )
	double GetTouchRotateThreshold() const
	tjs_real GetTouchPointStartX( tjs_int index ) const;
	tjs_real GetTouchPointStartY( tjs_int index ) const;
	tjs_real GetTouchPointX( tjs_int index ) const;
	tjs_real GetTouchPointY( tjs_int index ) const;
	tjs_int GetTouchPointID( tjs_int index ) const;
	tjs_int GetTouchPointCount() const;

	// �^�b�`���͂̃}�E�X�G�~�����[�gON/OFF
	void SetEnableTouch( bool b );
	bool GetEnableTouch() const;

	// �^�b�`�A�}�E�X�����x
	bool GetTouchVelocity( tjs_int id, float& x, float& y, float& speed ) const
	bool GetMouseVelocity( float& x, float& y, float& speed ) const
	void ResetMouseVelocity()

	// ��ʕ\�������擾
	int GetDisplayOrientation();
	int GetDisplayRotate();
};

