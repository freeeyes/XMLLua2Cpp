#ifndef _PARSE_LUA_FILE_H
#define _PARSE_LUA_FILE_H

#include "Common.h"
#include <string>
using namespace std ;

struct _FileLine
{
	string m_strLine;
};
typedef vector<_FileLine> vec_FileLine;

//解析文件中所包含的头文件
bool Parse_Lua_File(const char* pFileName, vec_FileLine& obj_vec_FileLine);

//合并替换代码
bool Searsh_Lua_File(const char* pFunctionName, const char* pFunctionLine, string strFuncBody, vec_FileLine& obj_vec_FileLine);

//保存文件
bool Save_Lua_File(const char* pFileName, vec_FileLine& obj_vec_FileLine);

#endif
