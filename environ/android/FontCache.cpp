

#include "tjsCommHead.h"

#include "FontCache.h"



FontCache::FontCache() : use_list_(NULL), remain_list_(NULL), last_item_(NULL) {
	// �e�N�X�`������
	int maxsize = GLTexture::getMaxTextureSize();
	tjs_uint8* temp = new tjs_uint8[maxsize*maxsize];
	texture_ = new GLTexture( maxsize, maxsize, temp, GL_ALPHA );
	delete[] temp;
}
FontCache::~FontCache() {
	delete texture_;
	texture_ = NULL;
}


// �A���`�G�C���A�X�̓V�F�[�_�[�ɔC����
void FontCache::initialize() {
	// �c�Ɖ��̊i�[�\�����v�Z����
	const FT_Face f = face_.getFace();
	const FT_BBox& box = f.bbox;
	tjs_int gw = FontFace::posToInt( box.xMax - box.xMin ) + 1;
	tjs_int gh = FontFace::posToInt( box.yMax - box.yMin ) + 1;
	int num_x = texture_->width() / gw;
	int num_y = texture_->height() / gh;

	// bool Antialiased;
	// int style_;	// underline/strikeout

	// ������
	// �e�v�f�̈ʒu�̊m��ƃ����N�̐ڑ����s��
	int cacheCount = num_x * num_y;
	glyph_cache_.resize(cacheCount);
	int idx = 0;
	GlyphData* prev = NULL;
	for( int y = 0; y < num_y; y++ ) {
		int posy = y * gh;
		for( int x = 0; x < num_x; x++ ) {
			int posx = x * gw;
			int l = posx;
			int r = l + gw;
			int t = posy;
			int b = t + gh;
			glyph_cache_[idx].position_.set( l, t, r, b );
			glyph_cache_[idx].index_ = idx;
			glyph_cache_[idx].prev_ = prev;
			glyph_cache_[idx].next_ = glyph_cache_[idx+1];
			prev = &(glyph_cache_[idx]);
			idx++;
		}
	}
	glyph_cache_[cacheCount-1].next_ = NULL;
	remain_list_ = &(glyph_cache_[0]);
	use_list_ = NULL;
	last_item_ = NULL;
}
void FontCache::toUseList( GlyphData* data ) {
	if( use_list_ == NULL ) {
		use_list_ = data;
		data->next_ = NULL
		data->prev_ = NULL;
	} else {
		use_list_->prev_ = data;
		data->next_ = use_list_;
		data->prev_ = NULL;
		use_list_ = data;
	}
	if( last_item_ == NULL ) {
		// �g�p���ꂽ�A�C�e�����܂��Ȃ����́A����̂�����
		last_item_ = data;
	}
}
// ��ԌÂ��v�f�𓾂�
// ��ԌÂ��v�f�́A�g���邱�ƂɂȂ�̂ŁA��ԐV�����v�f�ƂȂ�
int FontCache::getLastItem() {
	if( last_item_ ) {
		int result = last_item_->index_;
		assert( last_item_->next_ );
		// �Ō�̎�O�̗v�f���Ō�̗v�f�ɂȂ�
		GlyphData* prev = last_item_->prev_;
		prev->next_ = NULL;

		// ���݂̍Ō�̃A�C�e����擪�Ɉړ�����
		GlyphData* first = last_item_;
		first->prev_ = NULL;
		first->next_ = use_list_;
		use_list_->prev_ = first;
		use_list_ = first;

		last_item_ = prev;	// �Ō�̎�O�������v�f���Ō�̗v�f
		return result;
	} else {
		// �g�p���Ă���Â����̂��Ȃ��A�v���O�����G���[�H
		return -1;
	}
}
int FontCache::findEmpty() {
	int result = -1;
	if( remain_list_ != NULL ) {
		GlyphData* first = remain_list_;
		result = remain_list_->index_;

		if( remain_list_->next_ ) {
			// ���̗v�f������ꍇ�͂����擪��
			remain_list_ = remain_list_->next_;
			remain_list_->prev_ = NULL;
		} else {
			remain_list_ = NULL;
		}
		toUseList( first );
	}
	return result;
}
int FontCache::pushCh( tjs_int ch ) {
	int index = findEmpty();
	if( index < 0 ) {
		// ���g�p�̂��Ȃ����́A��ԌÂ��̂��g��
		index = getLastItem();
		if( index >= 0 ) {
			// �g���Ă������̂�map�������
			tjs_char ch = glyph_cache_[index].glyph_;
			glyph_index_.erase( ch );
		}
	}
	if( face_.renderGlyph( ch ) ) {
		GlyphData& glyph = glyph_cache_[index];
		
		face_.
		
		// �����_�����O�������̂�map��
		glyph_index_.insert( std::map<tjs_char,tjs_int>::value_type( ch, index ) );
	}
}
const GlyphData& FontCache::getGlyphData( tjs_char ch ) {
	std::map<tjs_char,tjs_int>::iterator i = glyph_index_.find( ch );
	if( i != glyph_index_.end() ) {
		// �L���b�V���ɂ���̂ŁA��������̂܂܎g��
		return getGlyph( i->second );
	} else {
		// �L���b�V���ɂȂ��̂ŐV���Ƀ����_�����O
		int index = pushCh( ch );
		if( index < 0 ) {
			throw "font rendaring error";
		}
		return getGlyph( (tjs_uint)index );
	}
}

bool FontCache::loadFaceFromAsset( const char* fontfile, tjs_uint index = 0 ) {
	if( face_.loadFaceFromAsset( fontfile, index ) ) {
		return true;
	}
	return false;
}

/*

	// ���ۂ̉�ʃT�C�Y�Ƒz��T�C�Y������Ă��A�ł��邾������ʃT�C�Y���Y��ɕ`�悷��
	// realsize : ���ۂ̉�ʃT�C�Y(��or����)
	// referencesize : �z�肵�Ă����ʃT�C�Y(��or����)
	realsize, referencesize, x, y, fontsize


*/

