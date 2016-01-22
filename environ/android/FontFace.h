
#ifndef __FONT_FACE_H__
#define __FONT_FACE_H__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H

class FontFace {
	FT_Face face_;
	tjs_uint32 options_; //!< �t���O

	tjs_uint (*UnicodeToLocalChar)(tjs_char in); //!< SJIS�Ȃǂ�Unicode�ɕϊ�����֐�
	tjs_char (*LocalCharToUnicode)(tjs_uint in); //!< Unicode��SJIS�Ȃǂɕϊ�����֐�

private:
	static unsigned long asset_read_func( FT_Stream stream, unsigned long offset, unsigned char* buffer, unsigned long count );
	static void asset_close_func( FT_Stream stream );
	bool openFaceByIndex( tjs_uint index, FT_Face& face, FT_StreamRec& stream );
	bool loadGlyphSlotFromCharcode( tjs_char code );

public:
	FontFace();

	/**
	 * asset ����t�@�C���w��ŊJ��
	 * @param fontfile �t�H���g�t�@�C����
	 * @param index : �t�H���g�t�@�C�����̃t�F�C�X�C���f�b�N�X
	 */
	bool loadFaceFromAsset( const char* fontfile, tjs_uint index = 0 );

	/**
	 * �ڍׂȃT�C�Y���w�肷��B
	 * �Œ菭���Ŏw��ł���̂ŉ�ʃT�C�Y�Ƒz��T�C�Y���Ⴄ�ꍇ�Ɏg���₷��
	 * @param height : 26.6�Œ菭���ł̍���(point)���w��
	 * @param hresolution : ���𑜓x(dpi)
	 * @param vresolution : �c�𑜓x(dpi)
	 */
	bool setSize( tjs_int height, tjs_uint hresolution=300, tjs_uint vresolution=300 ) {
		return FT_Set_Char_Size( face_, 0, height, hresolution, vresolution ) == 0;
	}

	/**
	 * �����������_�����O���āA�����̃O���t�r�b�g�}�b�v�𓾂���悤�ɂ���
	 * �A���`�G�C���A�X�Ȃ��Ń����_�����O���Ă���
	 * @param code : �����R�[�h
	 */
	bool renderGlyph( tjs_char code );

	/**
	 * �����_�����O���������̉摜�𓾂�
	 * ���O��renderGlyph���R�[�����Ă�������
	 */
	const FT_Bitmap* getBitmap() const { return &(face_->glyph->bitmap); }

	/**
	 * �����_�����O�����O���t�𓾂�
	 * ���O��renderGlyph���R�[�����Ă�������
	 */
	const FT_GlyphSlot* getGlyph() const { return &(face_->glyph); }

	/**
	 * Face�𓾂�
	 */
	const FT_Face getFace() const { return face_; }

	// 26.6�Œ菭���𐮐��ɂ���
	static inline tjs_int posToInt( tjs_int x ) { return (((x) + (1 << 5)) >> 6); }
	// ������26.6�Œ菭���ɂ���
	static inline tjs_int intToPos( tjs_int x ) { return x << 6; }
};

#endif // __FONT_FACE_H__
