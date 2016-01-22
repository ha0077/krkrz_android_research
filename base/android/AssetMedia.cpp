
#include <android/asset_manager.h>
#include "Application.h"

//---------------------------------------------------------------------------
// tTVPAssetFileStream
//---------------------------------------------------------------------------
class tTVPAssetFileStream : public tTJSBinaryStream
{
private:
	AAsset* asset_;

public:
	tTVPAssetFileStream( AAssetManager* mgr, const ttstr &origname, const ttstr & localname, tjs_uint32 flag) {
		tjs_uint32 access = flag & TJS_BS_ACCESS_MASK;
		if( access != TJS_BS_READ ) {	// asset �ւ͏������߂Ȃ�
			TVPThrowExceptionMessage(TVPCannotOpenStorage, origname);
		}
		asset_ = AAssetManager_open( mgr, localname.AsNarrowStdString().c_str(), AASSET_MODE_RANDOM );
		if( asset_ == NULL ) {
			TVPThrowExceptionMessage(TVPCannotOpenStorage, origname);
		}
	}
	~tTVPAssetFileStream() {
		if( asset_ ) {
			AAsset_close( asset_ );
			asset_ = NULL;
		}
	}

	tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence) {
		int orgin;
		switch(whence)
		{
		case TJS_BS_SEEK_SET:	orgin = SEEK_SET;	break;
		case TJS_BS_SEEK_CUR:	orgin = SEEK_CUR;	break;
		case TJS_BS_SEEK_END:	orgin = SEEK_END;	break;
		default:				orgin = SEEK_SET;	break; // may be enough
		}
		return AAsset_seek64( asset_, offset, orgin );
	}

	tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size) {
		return AAsset_read( assets_, buffer, read_size );
	}
	tjs_uint TJS_INTF_METHOD Write(const void *buffer, tjs_uint write_size) {
		// cannot write to asset
		TVPThrowExceptionMessage(TVPWriteError);
	}

	void TJS_INTF_METHOD SetEndOfStorage() {
		// cannot write to asset
		TVPThrowExceptionMessage(TVPWriteError);
	}

	tjs_uint64 TJS_INTF_METHOD GetSize() {
		return AAsset_getLength64( asset_ );
	}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// tTVPFileMedia
//---------------------------------------------------------------------------
class tTVPAssetMedia : public iTVPStorageMedia {
	tjs_uint RefCount;

public:
	tTVPAssetMedia() { RefCount = 1; }
	~tTVPAssetMedia() {;}

	void TJS_INTF_METHOD AddRef() { RefCount ++; }
	void TJS_INTF_METHOD Release() {
		if(RefCount == 1)
			delete this;
		else
			RefCount --;
	}
	void TJS_INTF_METHOD GetName(ttstr &name) { name = TJS_W("asset"); }

	void TJS_INTF_METHOD NormalizeDomainName(ttstr &name) {
		// normalize domain name
		// make all characters small
		tjs_char *p = name.Independ();
		while(*p)
		{
			if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
				*p += TJS_W('a') - TJS_W('A');
			p++;
		}
	}
	void TJS_INTF_METHOD NormalizePathName(ttstr &name) {
		// normalize path name
		// make all characters small
		tjs_char *p = name.Independ();
		while(*p)
		{
			if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
				*p += TJS_W('a') - TJS_W('A');
			p++;
		}
	}
	bool TJS_INTF_METHOD CheckExistentStorage(const ttstr &name) {
		if(name.IsEmpty()) return false;
		ttstr _name(name);
		GetLocalName(_name);
		AAsset* asset = AAssetManager_open( Application->getAssetManager(), localname.AsNarrowStdString().c_str(), AASSET_MODE_UNKNOWN);
		bool result = asset != NULL;
		if( result ) {
			AAsset_close( asset );
		}
		return result;
	}
	tTJSBinaryStream * TJS_INTF_METHOD Open(const ttstr & name, tjs_uint32 flags) {
		// open storage named "name".
		// currently only local/network(by OS) storage systems are supported.
		if(name.IsEmpty())
			TVPThrowExceptionMessage(TVPCannotOpenStorage, TJS_W("\"\""));

		ttstr origname = name;
		ttstr _name(name);
		GetLocalName(_name);
		return new tTVPAssetFileStream( Application->getAssetManager(), origname, _name, flags );
	}
	// Asset�̓L���b�V���O��������������
	void TJS_INTF_METHOD GetListAt(const ttstr &_name, iTVPStorageLister *lister) {
		ttstr name(_name);
		GetLocalName(name);

		AAssetDir* dir = AAssetManager_openDir( Application->getAssetManager(), name.AsNarrowStdString().c_str() );
		if( dir ) {
			do {
				const char* filename = AAssetDir_getNextFileName( dir );
				if( filename ) {
					ttstr file( filename );
					tjs_char *p = file.Independ();
					while(*p) {
						// make all characters small
						if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
							*p += TJS_W('a') - TJS_W('A');
						p++;
					}
					lister->Add( file );
				}
			} while( filename );
			AAssetDir_close( dir );
		}
	}
	void TJS_INTF_METHOD GetLocallyAccessibleName(ttstr &name) {
		#if 0
			public String getLocallyAccessibleName( final String name ) {
		StringBuilder newname = new StringBuilder(256);
		int start = 0;
		final int count = name.length();
		if( count > 0 && name.charAt(0) == '.' ) start++; // �ŏ��� . ������ꍇ�̓X�L�b�v
		while( count > start && (name.charAt(start) == '/' || name.charAt(start) == '\\') ) start++; // �ŏ��� / \ ������ꍇ�̓X�L�b�v

		while( start < count ) {
			char c = name.charAt(start);
			if( c != '\\' ) {
				newname.append(c);
			} else {
				newname.append('/'); // \ �� / �ɒu������
			}
			start++;
		}
		return newname.toString();
	}
		#endif
	}

	void TJS_INTF_METHOD GetLocalName(ttstr &name) {
		ttstr tmp = name;
		GetLocallyAccessibleName(tmp);
		if(tmp.IsEmpty()) TVPThrowExceptionMessage(TVPCannotGetLocalName, name);
		name = tmp;
	}
};


class AssetCache {
	class AssetDirectory {
		bool is_file_;
		std::string name_;
		std::map<std::string,AssetDirectory*> files_;
	public:
		AssetDirectory() : is_file_(false) {}
		AssetDirectory( bool isfile, const char* name ) : is_file_(isfile), name_(name) {}
		AssetDirectory( bool isfile, const std::string& name ) : is_file_(isfile), name_(name) {}
		~AssetDirectory() {
			std::map<std::string,AssetDirectory*>::iterator i = files_.begin();
			for( ; i != files_.end(); ++i ) {
				delete (*i).second;
			}
			files_.clear();
		}
		void pushFile( const std::string& name, AssetDirectory* dir ) {
			files_.insert( std::map<std::string,AssetDirectory*>::value_type( name, dir ) );
		}
		AssetDirectory* getFile( const std::string& name ) {
			std::map<std::string,AssetDirectory*>::iterator i = files_.find( const KEY_TYPE &key );
			if( i != files_.end() ) {
				return (*i).second;
			} else {
			}
		}
		const std::string& getName() const { return name_; }
		bool isFile() const { return is_file_; }
	};
	AssetDirectory root_;
public:
	void initialize() {
		AAssetManager* mgr = Application->getAssetManager();
		AAssetDir* dir = AAssetManager_openDir( mgr, "" );
		if( dir ) {
			searchDir( mgr, dir, root_, std::string("") );
			AAssetDir_close( dir );
		}
	}
	void searchDir( AAssetManager* mgr, AAssetDir* dir, AssetDirectory& current, const std::string& base ) {
		do {
			const char* filename = AAssetDir_getNextFileName( dir );
			if( filename ) {
				std::string curfile(filename);
				std::string path( base + curfile );
				AAssetDir* newdir = AAssetManager_openDir( mgr, path.c_str() );
				if( newdir ) {
					AssetDirectory* finddir = new AssetDirectory( false, curfile );
					current.pushFile( curfile, finddir );
					// �ċA
					searchDir( mgr, newdir, *finddir, path + std::string("/") );
					AAssetDir_close( newdir );
				} else {
					// �t�@�C��������
					current.pushFile( curfile, new AssetDirectory( true, curfile ) );
				}
			}
		} while( filename );
	}
};


