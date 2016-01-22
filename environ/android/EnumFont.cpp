
#include "tjsCommHead.h"

#include <stdio.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <android/log.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_UNPATENTED_H
#include FT_SYNTHESIS_H
#include FT_BITMAP_H
#include FT_SYSTEM_H


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "krkrz", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "krkrz", __VA_ARGS__))


FT_Library FreeTypeLibrary = NULL;	//!< FreeType ���C�u����
void TVPInitializeFont() {
	if( FreeTypeLibrary == NULL ) {
		FT_Error err = FT_Init_FreeType( &FreeTypeLibrary );
	}
}
void TVPUninitializeFreeFont() {
	if( FreeTypeLibrary ) {
		FT_Done_FreeType( FreeTypeLibrary );
		FreeTypeLibrary = NULL;
	}
}
static unsigned long read_func( FT_Stream stream, unsigned long offset, unsigned char* buffer, unsigned long count ) {
	FILE* fp = static_cast<FILE*>(stream->descriptor.pointer);
	unsigned long result;
	if( count == 0 ) {
		result = 0;
		fseek( fp, offset, SEEK_SET );
	} else {
		fseek( fp, offset, SEEK_SET );
		fread( buffer, 1, count, fp );
		result = count;
	}
	return result;
}
static void close_func( FT_Stream stream ) {
	fclose( static_cast<FILE*>(stream->descriptor.pointer) );
}
//---------------------------------------------------------------------------
/**
 * �w��C���f�b�N�X��Face���J��
 * @param index	�J��index
 * @param face	FT_Face �ϐ��ւ̎Q��
 * @return	Face���J����� true �����łȂ���� false
 * @note	���߂� Face ���J���ꍇ�� face �Ŏw�肷��ϐ��ɂ� null �����Ă�������
 */
bool OpenFaceByIndex( tjs_uint index, FT_Face& face, FT_StreamRec& stream )
{
	if( face ) FT_Done_Face(face), face = NULL;

	FT_Parameter parameters[1];
	parameters[0].tag = FT_PARAM_TAG_UNPATENTED_HINTING; // Apple�̓���������s��
	parameters[0].data = NULL;

	FT_Open_Args args;
	memset(&args, 0, sizeof(args));
	args.flags = FT_OPEN_STREAM;
	args.stream = &stream;
	args.driver = 0;
	args.num_params = 1;
	args.params = parameters;
	FT_Error err = FT_Open_Face( FreeTypeLibrary, &args, index, &face );
	return err == 0;
}
struct Face {
	std::string file_name_;
	std::string family_name_;
	std::string style_name_;
	long face_flags_;
	long style_flags_;
	int face_index_;
	int num_glyphs_;
};
static void read_face_names( const char* filename ) {
	std::string path(std::string("/system/fonts/") + std::string(filename));
	FILE* fp = fopen( path.c_str(), "rb" );
	if( fp == NULL ) return;

	fseek( fp, 0, SEEK_END );
	long len = ftell( fp );

	FT_StreamRec fsr;
	fsr.base = 0;
	fsr.size = static_cast<unsigned long>(len);
	fsr.pos = 0;
	fsr.descriptor.pointer = fp;
	fsr.pathname.pointer = NULL;
	fsr.read = read_func;
	fsr.close = close_func;

	std::vector<std::string> FaceNames;
	// Face �����ꂼ��J���AFace�����擾���� FaceNames �Ɋi�[����
	tjs_uint face_num = 1;
	FT_Face face = NULL;
	for( tjs_uint i = 0; i < face_num; i++ ) {
		if( !OpenFaceByIndex( i, face, fsr ) ) {
			FaceNames.push_back(std::string());
		} else {
			const char * name = face->family_name;
			FaceNames.push_back( std::string(name) );
			face_num = face->num_faces;
		}
	}
	if( face ) FT_Done_Face(face), face = NULL;

	LOGI( "Font file : %s\n", filename );
	std::vector<std::string>::iterator i = FaceNames.begin();
	for( ; i != FaceNames.end(); ++i ) {
		LOGI( "  Face name : %s\n", i->c_str() );
	}
	// FT_ENCODING_SJIS or FT_ENCODING_UNICODE
	/*
	�񊿎��F1183���� �{ 39�������̖��g�p�̈�
	�E��P���������F2965����
	�E��Q���������F3390����
	7538 ���̕ӂ�̃O���t���ȏオ���
	���̒��œ��{��O���t����������̂���ŐU�蕪����̂����
	�������̕����ŃO���t�擾�ł��邩�����̂�����Ȃ�
	
	�t�H���g���X�g�擾���鎞�Ƀ��X�g�A�b�v���Ă��Ȃ������炻���ŏ��߂ă��X�g�A�b�v�B
	�f�t�H���g�́Aassets�ɓ���Ă�����g���̂����
	*/
}

/**
 * �ȉ��̂悤�ɂ��āAttf�t�@�C�����擾����΁A�t�H���g���g�������ł͂���B
 * �����A�S���J���ăt�F�C�X���L���b�V�����ā`�ƂȂ�ƁA�������Ԃ����肻���B
 * �A�[�J�C�u���Ƀt�H���g����āA�����ǂ�ŕ\������̂��m�����B
 * ����N�����ɒ��ׂāA�ȍ~�L���b�V������̂��m������
 */
void print_font_files() {
	TVPInitializeFont();
	DIR *dir;
	if( (dir=opendir("/system/fonts")) != NULL) {
		struct dirent *dp;
		for( dp = readdir(dir); dp != NULL; dp = readdir(dir) ) {
			if( dp->d_type == DT_REG ) {
				read_face_names( dp->d_name );
			}
		}
		closedir(dir);
	}
	TVPUninitializeFreeFont();
}



