#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <vector>
using namespace std;

//自动生成Lua文件
//add by freeeyes

#define MAX_BUFF_50 50

//参数类型
enum EM_PARAM_TYPE
{
	PARAM_TYPE_IN  = 0,    //入口参数
	PARAM_TYPE_OUT,        //出口参数  
};

//参数种类
enum EM_PARAM_CLASS
{
	PARAM_CLASS_INT = 0,   //整数类型
	PARAM_CLASS_STRING,    //字符串类型 
	PARAM_CLASS_VOID,      //指针类型
};

//参数类型
struct _Function_Param
{
	char           m_szParamName[MAX_BUFF_50];   //参数名称
	char           m_szParamType[MAX_BUFF_50];   //参数类型（字符串） 
	EM_PARAM_TYPE  m_emParamType;                //参数类型
	EM_PARAM_CLASS m_emParamClass;               //参数种类
	int            m_nLength;                    //参数长度

	_Function_Param()
	{
		m_szParamName[0] = '\0';
		m_emParamType    = PARAM_TYPE_IN;
		m_emParamClass   = PARAM_CLASS_INT;
		m_nLength        = 0;
	}
};
typedef vector<_Function_Param> vecParamList;

//Lua函数信息
struct _Function_Info
{
	char m_szFunctionName[MAX_BUFF_50];
	char m_szDesc[MAX_BUFF_50];
	vecParamList m_vecParamList;

	_Function_Info()
	{
		m_szFunctionName[0] = '\0';
		m_szDesc[0]         = '\0';
	}
};
typedef vector<_Function_Info> vecFunctionList;

//Lua文件
struct _LuaFile_Info
{
	char m_szFileName[MAX_BUFF_50];
	char m_szDesc[MAX_BUFF_50];
	vecFunctionList m_vecFunctionList;

	_LuaFile_Info()
	{
		m_szFileName[0] = '\0';
		m_szDesc[0]     = '\0';
	}
};
typedef vector<_LuaFile_Info> vecLuaFileList;

//Lua工程信息
struct _Project_Lua_Info
{
	char m_szProjectName[MAX_BUFF_50];
	vecLuaFileList m_vecLuaFileList;

	_Project_Lua_Info()
	{
		m_szProjectName[0] = '\0';
	}
};

//Lua文件
struct _CppFile_Info
{
	char m_szFileName[MAX_BUFF_50];
	char m_szDesc[MAX_BUFF_50];
	vecFunctionList m_vecFunctionList;

	_CppFile_Info()
	{
		m_szFileName[0] = '\0';
		m_szDesc[0]     = '\0';
	}
};
typedef vector<_CppFile_Info> vecCppFileList;

//Cpp工程信息
struct _Project_Cpp_Info
{
	char m_szProjectName[MAX_BUFF_50];
	vecCppFileList m_vecCppFileList;

	_Project_Cpp_Info()
	{
		m_szProjectName[0] = '\0';
	}
};

//测试参数数据结构体
struct _Test_API_Param_Info
{
	char           m_szParamName[MAX_BUFF_50];
	EM_PARAM_CLASS m_emParamClass;
	char           m_szParamClass[MAX_BUFF_50];
	char           m_szValue[MAX_BUFF_50];

	_Test_API_Param_Info()
	{
		m_szParamName[0] = '\0';
		m_emParamClass   = PARAM_CLASS_VOID;
		m_szParamClass[0] = '\0';
		m_szValue[0]     = '\0';
	}
};
typedef vector<_Test_API_Param_Info> vecTestLuaParamIn;
typedef vector<_Test_API_Param_Info> vecTestLuaParamOut;

//测试Lua API函数
struct _Test_API_Lua_Function_Info
{
	char               m_szName[MAX_BUFF_50];
	char               m_szLuaFuncName[MAX_BUFF_50];
	int                m_nTestCount;
	vecTestLuaParamIn  m_vecTestLuaParamIn;
	vecTestLuaParamOut m_vecTestLuaParamOut;

	_Test_API_Lua_Function_Info()
	{
		m_szName[0]        = '\0';
		m_szLuaFuncName[0] = '\0';
		m_nTestCount       = 0;
	}
};
typedef vector<_Test_API_Lua_Function_Info> vecTestAPIInfo;

struct _Test_API
{
	char m_szProjectName[MAX_BUFF_50];
	vecTestAPIInfo m_vecTestAPIInfo;

	_Test_API()
	{
		m_szProjectName[0] = '\0';
	}
};

//记录基础数据类型的数据模型
struct _Base_Data_Info
{
	char m_szDataName[MAX_BUFF_50];
	char m_szDataType[MAX_BUFF_50];
	int  m_nDataLen;
	char m_szDataDesc[MAX_BUFF_50];

	_Base_Data_Info()
	{
		m_szDataName[0] = '\0';
		m_szDataType[0] = '\0';
		m_szDataDesc[0] = '\0';
		m_nDataLen      = 0;
	}
};
typedef vector<_Base_Data_Info> vecBaseDataInfo;

//结构对象命名结构体
struct _Base_Data_Struct
{
	char m_szStructName[MAX_BUFF_50];
	vecBaseDataInfo m_vecBaseDataInfo;

	_Base_Data_Struct()
	{
		m_szStructName[0] = '\0';
	}
};
typedef vector<_Base_Data_Struct> vecBaseDataStruct;

struct _Base_Data_Group
{
	char m_szProjectName[MAX_BUFF_50];
	vecBaseDataStruct m_vecBaseDataStruct;

	_Base_Data_Group()
	{
		m_szProjectName[0] = '\0';
	}
};

static void sprintf_safe(char* szText, int nLen, const char* fmt ...)
{
	if(szText == NULL)
	{
		return;
	}

	va_list ap;
	va_start(ap, fmt);

	vsnprintf(szText, nLen, fmt, ap);
	szText[nLen - 1] = '\0';

	va_end(ap);
};

#endif
