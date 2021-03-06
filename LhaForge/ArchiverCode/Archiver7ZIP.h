/*
 * Copyright (c) 2005-2012, Claybird
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:

 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Claybird nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#pragma once
#include "arc_interface.h"

typedef int    (WINAPI *COMMON_ARCHIVER_GETARCHIVETYPE)(LPCSTR);

struct CConfigZIP;
struct CConfig7Z;

class CArchiver7ZIP:public CArchiverDLL{
protected:
	void WriteResponceFile(HANDLE,LPCTSTR);
	COMMON_ARCHIVER_SETUNICODEMODE ArchiverSetUnicodeMode;
	COMMON_ARCHIVER_GETARCHIVETYPE ArchiverGetArchiveType;

	virtual bool ExtractDirectoryEntry(LPCTSTR lpszArcFile,CConfigManager&,const ARCHIVE_ENTRY_INFO_TREE* lpBase,const ARCHIVE_ENTRY_INFO_TREE* lpDir,LPCTSTR lpszOutputBaseDir,bool bCollapseDir,CString &strLog);

	bool FormatCompressCommandZIP(const CConfigZIP&,CString &Param,bool bSFX,int Options,LPCTSTR lpszMethod,LPCTSTR lpszLevel,CString &strLog);
	bool FormatCompressCommand7Z(const CConfig7Z&,CString &Param,int Options,LPCTSTR lpszMethod,CString &strLog);
public:
	CArchiver7ZIP();
	virtual ~CArchiver7ZIP();
	virtual LOAD_RESULT LoadDLL(CConfigManager&,CString &strErr)override;
	virtual void FreeDLL()override;
	virtual bool IsUnicodeCapable()const override{return true;}	//UNICODE対応DLLならtrueを返す
	virtual bool Compress(LPCTSTR,std::list<CString>&,CConfigManager&,const PARAMETER_TYPE,int,LPCTSTR,LPCTSTR,LPCTSTR,CString &)override;
	virtual bool Extract(LPCTSTR,CConfigManager&,const CConfigExtract&,bool,LPCTSTR,CString &)override;
	virtual bool ExtractSpecifiedOnly(LPCTSTR ArcFileName,CConfigManager&,LPCTSTR OutputDir,std::list<CString>&,CString &,bool bUsePath=false)override;
	virtual bool ExamineArchive(LPCTSTR,CConfigManager&,bool,bool&,bool&,CString&,CString &strErr)override;

	//アーカイブからファイルを削除
	virtual bool DeleteItemFromArchive(LPCTSTR ArcFileName,CConfigManager&,const std::list<CString>&,CString &)override;
	virtual bool QueryDeleteItemFromArchiveSupported(LPCTSTR ArcFileName)const override{return true;}		//DeleteFileがサポートされているかどうか

	virtual ARCRESULT TestArchive(LPCTSTR,CString &)override;	//アーカイブが正しいかどうかチェックする

	//アーカイブに指定したファイルを追加
	virtual bool AddItemToArchive(LPCTSTR ArcFileName,const std::list<CString>&,CConfigManager&,LPCTSTR lpDestDir,CString&)override;
	virtual bool QueryAddItemToArchiveSupported(LPCTSTR ArcFileName)const override{return true;}

	//-------------------------------
	//---UNICODE版をオーバーライド---
	//-------------------------------
	virtual BOOL CheckArchive(LPCTSTR)override;
	virtual int GetFileCount(LPCTSTR)override;	//アーカイブ中のファイル数を返す

	// 書庫内検査用メソッド
	virtual bool InspectArchiveBegin(LPCTSTR,CConfigManager&)override;				//書庫内調査開始
	virtual bool InspectArchiveGetFileName(CString&)override;		//書庫内ファイル名取得
	virtual bool InspectArchiveGetMethodString(CString&)override;			//書庫内ファイル格納モード取得
};


enum SEVEN_ZIP_COMPRESS_TYPE{
	SEVEN_ZIP_COMPRESS_LZMA,
	SEVEN_ZIP_COMPRESS_PPMD,
	SEVEN_ZIP_COMPRESS_BZIP2,
	SEVEN_ZIP_COMPRESS_DEFLATE,
	SEVEN_ZIP_COMPRESS_COPY,
	SEVEN_ZIP_COMPRESS_LZMA2,

	ENUM_COUNT_AND_LASTITEM(SEVEN_ZIP_COMPRESS_TYPE),
};
enum SEVEN_ZIP_COMPRESS_LEVEL{
	SEVEN_ZIP_COMPRESS_LEVEL5,
	SEVEN_ZIP_COMPRESS_LEVEL0,
	SEVEN_ZIP_COMPRESS_LEVEL1,
	SEVEN_ZIP_COMPRESS_LEVEL7,
	SEVEN_ZIP_COMPRESS_LEVEL9,

	ENUM_COUNT_AND_LASTITEM(SEVEN_ZIP_COMPRESS_LEVEL),
};
enum SEVEN_ZIP_LZMA_MODE{
	SEVEN_ZIP_LZMA_MODE1,
	SEVEN_ZIP_LZMA_MODE0,

	ENUM_COUNT_AND_LASTITEM(SEVEN_ZIP_LZMA_MODE)
};

#define SEVEN_ZIP_PPMD_MODEL_SIZE_LOWEST	2
#define SEVEN_ZIP_PPMD_MODEL_SIZE_HIGHEST	32

//----------
enum ZIP_COMPRESS_TYPE{
	ZIP_COMPRESS_DEFLATE,
	ZIP_COMPRESS_DEFLATE64,
	ZIP_COMPRESS_BZIP2,
	ZIP_COMPRESS_COPY,
	ZIP_COMPRESS_LZMA,
	ZIP_COMPRESS_PPMD,

	ENUM_COUNT_AND_LASTITEM(ZIP_COMPRESS_TYPE),
};
enum ZIP_COMPRESS_LEVEL{
	ZIP_COMPRESS_LEVEL5,
	ZIP_COMPRESS_LEVEL0,
	ZIP_COMPRESS_LEVEL9,

	ENUM_COUNT_AND_LASTITEM(ZIP_COMPRESS_LEVEL)
};

enum ZIP_CRYPTO_MODE{
	ZIP_CRYPTO_ZIPCRYPTO,
	ZIP_CRYPTO_AES128,
	ZIP_CRYPTO_AES192,
	ZIP_CRYPTO_AES256,

	ENUM_COUNT_AND_LASTITEM(ZIP_CRYPTO_MODE)
};

#define ZIP_DEFLATE_MEMORY_SIZE_LOWEST	3
#define ZIP_DEFLATE_MEMORY_SIZE_HIGHEST	255
#define ZIP_DEFLATE_PASS_NUMBER_LOWEST	1
#define ZIP_DEFLATE_PASS_NUMBER_HIGHEST	4

