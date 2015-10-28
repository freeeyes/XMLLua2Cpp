#ifndef _PARSE_C_API_FILE_H
#define _PARSE_C_API_FILE_H

#include "Common.h"
#include <string>
using namespace std ;

//记录引用的头文件
struct _FileInclude
{
	string m_strIncoude;
};
typedef vector<_FileInclude> vecFileInclude;

//记录引用的函数体代码
struct _FunctionCode
{
	char   m_szFuncName[200];
	string m_strCode;

	_FunctionCode()
	{
		m_szFuncName[0] = '\0';
	}
};
typedef vector<_FunctionCode> vecFunctionCode;

//文件拆解信息
struct _File_Info
{
	vecFileInclude  m_vecFileInclude;
	vecFunctionCode m_vecFunctionCode;
};

//解析出函数名
bool Parse_Function_Name(char* pLine, char* pFunctionName);

//解析包含的头文件
void Parse_File_Include(char* pData, int nFileSize, _File_Info& obj_File_Info);

//解析包含的所有函数名和函数体
void Parse_File_Function_Info(char* pData, int nFileSize, _File_Info& obj_File_Info);

//解析文件中所包含的头文件
bool Parse_CAPI_H_File(const char* pFileName, _File_Info& obj_File_Info);

#endif