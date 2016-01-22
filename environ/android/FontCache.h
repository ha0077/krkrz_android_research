
#ifndef __FONT_CACHE_H__
#define __FONT_CACHE_H__

#include "GLTexture.h"

struct GlyphData {
	tTVPRect position_;	// Texture ���ł̊�ʒu�A���̒��Ɏ��܂�悤�ɕ����摜���e�N�X�`���ɓ����

	tjs_char glyph_;		// �i�[����Ă��镶��
	tjs_int cellIncX; //!< �ꕶ���i�߂�̕K�v��X�����̃s�N�Z���� 1/64�Œ菭��
	tjs_int cellIncY; //!< �ꕶ���i�߂�̕K�v��Y�����̃s�N�Z���� 1/64�Œ菭��
	tjs_uint BlackBoxX; //!< ���摜�� 1/64�Œ菭��
	tjs_uint BlackBoxY; //!< ���摜���� 1/64�Œ菭��
	tjs_int OriginX; //!< ����Bitmap��`�悷��ascent�ʒu�Ƃ̉��I�t�Z�b�g 1/64�Œ菭��
	tjs_int OriginY; //!< ����Bitmap��`�悷��ascent�ʒu�Ƃ̏c�I�t�Z�b�g 1/64�Œ菭��

	tjs_uint index_;	//!< �L���b�V�����C���f�b�N�X
	GlyphData* prev_;	//!< �O�̗v�f
	GlyphData* next_;	//!< ���̗v�f

	GlyphData() : glyph_(-1) {}
};


// �L���b�V���Ƃ����Ă��A�`��O�ɂ̓e�N�X�`���ɕ`���Ȃ��Ƃ����Ȃ��̂ŁA�t�H���g�ƈ�̂Ŏg�����ƂɂȂ�
// �L���b�V���Ǘ������́A�P���ɍŌ�Ɏg��ꂽ���̂����X�g�̐擪�ɗ���
// ���X�g�̖����ɂ�����̂���폜(�ė��p)����Ă����V���v���ȕ���
class FontCache {
private:
	GLTexture* texture_;	// �����摜��ێ�����e�N�X�`��
	std::vector<GlyphData>		glyph_cache_;	// �O���t�f�[�^�L���b�V��
	std::map<tjs_char,tjs_uint>	glyph_index_;	// �L���b�V�����̃C���f�b�N�X�ƕ����R�[�h�̑Ή�

	GlyphData* use_list_;		// �g�p���A�C�e�����X�g
	GlyphData* last_item_;		// �g�p���̒��ň�Ԑ̂ɃA�N�Z�X���ꂽ�v�f
	GlyphData* remain_list_;	// ���g�p�A�C�e�����X�g

	FontFace face_;	// �t�F�C�X
private:
	/**
	 * �L���b�V���ɂȂ������������_�����O���Ċi�[����
	 */
	int pushCh( tjs_int ch );
	/**
	 * ���g�p�L���b�V���𓾂�
	 */
	int findEmpty();
	/**
	 * ��ԌÂ��L���b�V���𓾂�
	 */
	int getLastItem();
	/**
	 * �g�p���ɂ���
	 */
	void toUseList( GlyphData* data );
	/**
	 * �w��C���f�b�N�X�̃O���t���𓾂�
	 */
	const GlyphData& getGlyph( tjs_uint index ) const {
		if( index < glyph_cache_.size() ) {
			return glyph_cache_[index];
		} else {
			throw "Not found glyph";
			return glyph_cache_[0];
		}
	}

public:
	FontCache();
	~FontCache();

	bool loadFaceFromAsset( const char* fontfile, tjs_uint index = 0 );

	void initialize();

	const GlyphData& getGlyphData( tjs_char ch );
};

#endif // __FONT_CACHE_H__
