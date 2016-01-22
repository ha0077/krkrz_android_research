//---------------------------------------------------------------------------
/*
	Risa [�肳]      alias �g���g��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief CP932(��SJIS)->UNICODE �ϊ�
//---------------------------------------------------------------------------
// UNICODE��SJIS(cp932)�̕ϊ��}�b�v�ɂ��Ă� unicode.org ���Q�Ƃ̂���



#ifndef _CP932_UNI_
#define _CP932_UNI_

//---------------------------------------------------------------------------

size_t SJISToUnicodeString(const char * in, tjs_char *out);
tjs_char SJISToUnicode(tjs_uint sjis);

//---------------------------------------------------------------------------

#endif

