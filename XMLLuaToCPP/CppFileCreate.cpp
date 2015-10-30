#include "CppFileCreate.h"

char* delete_space(char src[])
{
    char *ret = src;
    char *p1 = src;
    char *p2;
     
    if (NULL == src)
    {
        return NULL;
    }
         
    while (*p1 == ' ' || *p1 == '\t')//跳过最前面空白字符(这里增加了2楼提到的'\t'字符)
    {
        p1++;
    }

    p2 = p1;
    while (*p2)
    {
        p2++;
    }

    while (--p2 >= src && (*p2== ' ' || *p2 == '\t'))//跳过最后面空白字符
    {
        ;
    }

    while (p1 <= p2)
    {
        *src++ = *p1++;//拷贝中间部分
    }

    *src = 0;  
    return ret;
}

char* delete_star(char src[])
{
    char *ret = src;
    char *p1 = src;
    char *p2;
     
    if (NULL == src)
    {
        return NULL;
    }
         
    while (*p1 == ' ' || *p1 == '\t')//跳过最前面空白字符(这里增加了2楼提到的'\t'字符)
    {
        p1++;
    }

    p2 = p1;
    while (*p2)
    {
        p2++;
    }

    while (--p2 >= src && (*p2== '*' || *p2== ' ' || *p2 == '\t'))//跳过最后面空白字符
    {
        ;
    }

    while (p1 <= p2)
    {
        *src++ = *p1++;//拷贝中间部分
    }

    *src = 0; 
    return ret;
}

void Tranfile( const char* pFileSrc, const char* pFileDes )
{
	fstream fsCopee( pFileSrc, ios::binary | ios::in ) ;
	fstream fsCoper( pFileDes, ios::binary | ios::out ) ;
	fsCoper << fsCopee.rdbuf() ;
}

void Create_Lua_Environment( _Project_Lua_Info* pLuaProject )
{
	char szTempPath[50]   = {'\0'};
	char szTempFile[100]  = {'\0'};

	//创建LuaIncode目录
	sprintf_safe(szTempPath, 50, "%s/LuaCppWrapper", pLuaProject->m_szProjectName);
#ifdef WIN32
	_mkdir(szTempPath);
#else
	mkdir(szTempPath, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
#endif

	//创建Lib目录
	sprintf_safe(szTempPath, 50, "%s/Test", pLuaProject->m_szProjectName);
#ifdef WIN32
	_mkdir(szTempPath);
#else
	mkdir(szTempPath, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
#endif

	//创建C API代码实现文件夹
	//创建LuaIncode目录
	sprintf_safe(szTempPath, 50, "%s/LuaCppExec", pLuaProject->m_szProjectName);
#ifdef WIN32
	_mkdir(szTempPath);
#else
	mkdir(szTempPath, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
#endif

	//拷贝LuaCommon.h文件
	sprintf_safe(szTempPath, 50, "%s", pLuaProject->m_szProjectName);
	sprintf_safe(szTempFile, 100, "%s/LuaCppWrapper/LuaCommon.h", szTempPath);
	Tranfile("../LuaCommon.h", szTempFile);
}

bool Read_Cpp_File_XML( const char* pXMLName, _Project_Cpp_Info* pCppProject )
{
	CXmlOpeation objXmlOpeation;
	return objXmlOpeation.Parse_XML_File_Cpp(pXMLName, pCppProject);
}

bool Create_Cpp_API_Files( _Project_Cpp_Info* pCppProject )
{
	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		//首先生成H文件。
		sprintf_safe(szPathFile, 200, "%s/LuaCppWrapper/%s.h", 
			pCppProject->m_szProjectName,
			pCppProject->m_vecCppFileList[i].m_szFileName);

		FILE* pFile = fopen(szPathFile, "wb");
		if(NULL == pFile)
		{
			return false;
		}

		//编写文件说明
		sprintf_safe(szTemp, 200, "//%s\n\n", pCppProject->m_vecCppFileList[i].m_szDesc);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "#ifndef _%s_h\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "#define _%s_h\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "extern \"C\"\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "{\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t#include \"lua.h\"\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t#include \"lualib.h\"\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t#include \"lauxlib.h\"\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "}\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "#include <stdio.h>\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "#include \"Exec_%s.h\"\n\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		sprintf_safe(szTemp, 200, "extern \"C\"\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "{\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		//编写Lua调用C API函数
		for(int j = 0; j < (int)pCppProject->m_vecCppFileList[i].m_vecFunctionList.size(); j++)
		{
			_Function_Info& obj_Function_Info = (_Function_Info& )pCppProject->m_vecCppFileList[i].m_vecFunctionList[j];
			sprintf_safe(szTemp, 200, "\t//%s\n", obj_Function_Info.m_szDesc);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\tint LuaFn_%s(lua_State* L);\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		}
		sprintf_safe(szTemp, 200, "}\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		sprintf_safe(szTemp, 200, "#endif\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		fclose(pFile);

		//在编写CPP文件
		sprintf_safe(szPathFile, 200, "%s/LuaCppWrapper/%s.cpp", 
			pCppProject->m_szProjectName,
			pCppProject->m_vecCppFileList[i].m_szFileName);

		pFile = fopen(szPathFile, "wb");
		if(NULL == pFile)
		{
			return false;
		}

		sprintf_safe(szTemp, 200, "#include \"%s.h\"\n\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		//编写函数
		for(int j = 0; j <  (int)pCppProject->m_vecCppFileList[i].m_vecFunctionList.size(); j++)
		{
			_Function_Info& obj_Function_Info = (_Function_Info& )pCppProject->m_vecCppFileList[i].m_vecFunctionList[j];
			sprintf_safe(szTemp, 200, "//%s\n", obj_Function_Info.m_szDesc);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "int LuaFn_%s(lua_State* L)\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "{\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			//编写获得Lua的参数代码
			int nPos = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_IN)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
					{
						sprintf_safe(szTemp, 200, "\tint %s = (int)lua_tonumber(L, %d);\n", obj_Function_Info.m_vecParamList[k].m_szParamName, nPos+1);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
					{
						sprintf_safe(szTemp, 200, "\tchar* %s =  (char* )lua_tostring(L, %d);\n", obj_Function_Info.m_vecParamList[k].m_szParamName, nPos+1);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else
					{
						sprintf_safe(szTemp, 200, "\t%s* %s = (%s* )lua_touserdata(L, %d);\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamType,
							obj_Function_Info.m_vecParamList[k].m_szParamName,
							obj_Function_Info.m_vecParamList[k].m_szParamType,
							nPos+1);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					nPos++;
				}
			}

			sprintf_safe(szTemp, 200, "\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			//编写出参声明
			nPos = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
					{
						sprintf_safe(szTemp, 200, "\tint %s = 0;\n", obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
					{
						sprintf_safe(szTemp, 200, "\tchar %s[%d] = {'\\0'};\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamName, 
							obj_Function_Info.m_vecParamList[k].m_nLength);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else
					{
						sprintf_safe(szTemp, 200, "\t%s* %s = NULL;\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamType,
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					nPos++;
				}
			}

			sprintf_safe(szTemp, 200, "\t//add Execute API code at here.\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			//编写调用函数接口(拼接入参)
			sprintf_safe(szTemp, 200, "\tExec_%s(", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			int nInPos = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_IN)
				{
					if(nInPos == 0)
					{
						sprintf_safe(szTemp, 200, "%s", obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						nInPos++;
					}
					else
					{
						sprintf_safe(szTemp, 200, ", %s", obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						nInPos++;
					}
				}
			}

			//编写调用函数接口(拼接出参)
			int nOutPos = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
				{
					if(nOutPos == 0)
					{
						if(nInPos == 0)
						{
							sprintf_safe(szTemp, 200, "%s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else
						{
							sprintf_safe(szTemp, 200, ", %s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						nOutPos++;
					}
					else
					{
						sprintf_safe(szTemp, 200, ", %s", obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						nOutPos++;
					}
				}
			}
			sprintf_safe(szTemp, 200, ");\n\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			//编写返回Lua参数代码
			nPos = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
					{
						sprintf_safe(szTemp, 200, "\tlua_pushnumber(L, %s);\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
					{
						sprintf_safe(szTemp, 200, "\tlua_pushstring(L, (char* )%s);\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else
					{
						sprintf_safe(szTemp, 200, "\tlua_pushlightuserdata(L, %s);\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					nPos++;
				}
			}

			sprintf_safe(szTemp, 200, "\treturn 1;\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "}\n\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		}
		fclose(pFile);
	}

	return true;
}

bool Create_Cpp_Test_Files( _Project_Lua_Info* pLuaProject, _Project_Cpp_Info* pCppProject, _Test_API* pTestAPI )
{
	//在编写CPP文件
	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	//生成测试工程文件
	sprintf_safe(szPathFile, 200, "%s/Test/Test_%s.cpp", 
		pCppProject->m_szProjectName,
		pCppProject->m_szProjectName);

	FILE* pFile = fopen(szPathFile, "wb");
	if(NULL == pFile)
	{
		return false;
	}

	sprintf_safe(szTemp, 200, "//Test Lua Project.\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	//编写引用头文件
	sprintf_safe(szTemp, 200, "#include \"LuaCppWrapper.h\"\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "#include <string.h>\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "#include <iostream>\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "#include <string>\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "using namespace std;\n\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	
	sprintf_safe(szTemp, 200, "bool Init_Lua_State(lua_State*& L)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tint nRet = 0;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tL = luaL_newstate();\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tif(NULL == L)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t\tLua_Print(\"[Init_Lua_State_File]lua_State is NULL.\\n\");\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t\treturn false;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t}\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tluaopen_base(L);\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tluaL_openlibs(L);\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\treturn true;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "}\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "bool Open_Lua_File(lua_State*& L, const char* pFileName)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tint nRet = luaL_dofile(L, pFileName);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tif(LUA_OK != nRet)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t\tLua_Print(\"[Init_Lua_State_File]open lua file(%%s)(%%s).\\n\", pFileName, lua_tostring(L, -1));\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t\treturn false;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t}\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\treturn true;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "}\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "void Close_Lua_State(lua_State* L)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tif(NULL != L)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t\tlua_close(L);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t}\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "}\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//生成公用测算时间代码
	sprintf_safe(szTemp, 200, "unsigned long Get_System_TickCount()\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "#ifdef WIN32\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\treturn GetTickCount();\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "#else\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tstruct timespec ts;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tclock_gettime(CLOCK_MONOTONIC, &ts);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\treturn (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "#endif\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "}\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//生成测试函数
	for(int i = 0; i < (int)pTestAPI->m_vecTestAPIInfo.size(); i++)
	{
		sprintf_safe(szTemp, 200, "void %s(lua_State* L)\n", pTestAPI->m_vecTestAPIInfo[i].m_szName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "{\n");

		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\tint nSuccess = 0;\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\tint nFail = 0;\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\tunsigned long ulbegin = Get_System_TickCount();\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		
		if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamOut.size() > 0)
		{
			sprintf_safe(szTemp, 200, "\tRe_%s obj_%s;\n", 
				pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName, 
				pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		}

		sprintf_safe(szTemp, 200, "\tfor(int i = 0; i < %d; i++)\n", pTestAPI->m_vecTestAPIInfo[i].m_nTestCount);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t{\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		string strTemp;
		for(int j = 0; j < (int)pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn.size(); j++)
		{
			char szTempl[MAX_BUFF_50] = {'\0'};
			if(j == 0)
			{
				if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_emParamClass == PARAM_CLASS_STRING)
				{
					sprintf_safe(szTemp, 200, "\t\t%s %s = \"%s\";\n", 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamClass, 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName,
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
					fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

					sprintf_safe(szTempl, MAX_BUFF_50, "%s.c_str()", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName);
				}
				else if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_emParamClass == PARAM_CLASS_INT)
				{
					sprintf_safe(szTemp, 200, "\t\t%s %s = %s;\n", 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamClass, 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName,
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
					fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

					sprintf_safe(szTempl, MAX_BUFF_50, "%s", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName);
				}
				else 
				{
					if (0 == strcmp(delete_space(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue),""))
					{
						sprintf_safe(szTemp, 200, "\t\t%s %s;\n",
							delete_star(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamClass), 
							pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

						sprintf_safe(szTempl, MAX_BUFF_50, "&%s", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName);
					}
					else
					{
						sprintf_safe(szTemp, 200, "\t\t%s %s = %s;\n", 
							delete_star(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamClass),  
							pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName,
							pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

						sprintf_safe(szTempl, MAX_BUFF_50, "&%s", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName);
					}
					
				}
				strTemp += szTempl;
			}
			else
			{
				if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_emParamClass == PARAM_CLASS_STRING)
				{
					sprintf_safe(szTemp, 200, "\t\t%s %s = \"%s\";\n", 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamClass, 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName,
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
					fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

					sprintf_safe(szTempl, MAX_BUFF_50, ", %s.c_str()", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName);
				}
				else if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_emParamClass == PARAM_CLASS_INT)
				{
					sprintf_safe(szTemp, 200, "\t\t%s %s = %s;\n", 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamClass, 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName,
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
					fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

					sprintf_safe(szTempl, MAX_BUFF_50, ", %s", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName);
				}
				else 
				{
					sprintf_safe(szTemp, 200, "\t\t%s %s = %s;\n", 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamClass, 
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName,
						pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
					fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

					sprintf_safe(szTempl, MAX_BUFF_50, ", %s", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szParamName);
				}
				strTemp += szTempl;
			}
		}

		if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamOut.size() > 0)
		{
			if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn.size() > 0)
			{
				char szTempl[MAX_BUFF_50] = {'\0'};
				sprintf_safe(szTempl, MAX_BUFF_50, ", obj_%s", pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName);
				strTemp += szTempl;
			}
			else
			{
				char szTempl[MAX_BUFF_50] = {'\0'};
				sprintf_safe(szTempl, MAX_BUFF_50, "obj_%s", pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName);
				strTemp += szTempl;
			}
		}

		sprintf_safe(szTemp, 200, "\t\tbool blState = LuaAPI_%s(L, %s);\n", pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName, strTemp.c_str());
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t\tif(blState == false)\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t\t{\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t\t\tLua_Print(\"[%s]check Return is ERROR!\\n\");\n", pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t\t\tnFail++;\n", pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t\t}\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t\telse\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t\t{\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		//开始比较出参数
		for(int j = 0; j < (int)pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamOut.size(); j++)
		{
			if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamOut[j].m_emParamClass == PARAM_CLASS_INT)
			{
				sprintf_safe(szTemp, 200, "\t\t\tif(obj_%s.%s != %s)\n", 
					pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName,
					pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamOut[j].m_szParamName,
					pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamOut[j].m_szValue);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\t\t\t{\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\t\t\t\tnFail++;\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\t\t\t\tcontinue;\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\t\t\t}\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
			else if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamOut[j].m_emParamClass == PARAM_CLASS_STRING)
			{
				sprintf_safe(szTemp, 200, "\t\t\tif(strcmp(obj_%s.%s, \"%s\") != 0)\n", 
					pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName,
					pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamOut[j].m_szParamName,
					pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamOut[j].m_szValue);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\t\t\t{\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\t\t\t\tnFail++;\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\t\t\t\tcontinue;\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\t\t\t}\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
		}

		sprintf_safe(szTemp, 200, "\t\t\tnSuccess++;\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t\t}\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\t}\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\tunsigned long ulEnd = Get_System_TickCount();\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\tchar szLog[500] = {'\\0'};\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\tsprintf(szLog, \"(%s)[%s]Success=%%d,Fail=%%d,timecost=%%d.\\n\", nSuccess, nFail, (int)(ulEnd - ulbegin));\n", 
			pTestAPI->m_vecTestAPIInfo[i].m_szName,
			pTestAPI->m_vecTestAPIInfo[i].m_szLuaFuncName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\tFILE* pFile = fopen(\"TestResult.log\", \"a+\");\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\tfwrite(szLog, strlen(szLog), sizeof(char), pFile);\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "\tfclose(pFile);\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "}\n\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}

	//生成Main文件
	sprintf_safe(szTemp, 200, "int main(int argc, char* argv[])\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tremove(\"TestResult.log\");\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tlua_State* pLuaState = NULL;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tbool blRet = false;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tblRet = Init_Lua_State(pLuaState);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tif(false == blRet)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t\tgetchar();\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t}\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\telse\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "\t\tluaopen_userdatainterface(pLuaState);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pLuaProject->m_vecLuaFileList.size(); i++)
	{
		sprintf_safe(szTemp, 200, "\t\tOpen_Lua_File(pLuaState, \"../LuaScript/%s.lua\");\n", 
			pLuaProject->m_vecLuaFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}
	sprintf_safe(szTemp, 200, "\t\t//add your test code at here.\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pTestAPI->m_vecTestAPIInfo.size(); i++)
	{
		sprintf_safe(szTemp, 200, "\t\t%s(pLuaState);\n\n", pTestAPI->m_vecTestAPIInfo[i].m_szName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}

	sprintf_safe(szTemp, 200, "\t}\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\tClose_Lua_State(pLuaState);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\treturn 0;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "}\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	fclose(pFile);
	return true;
}

bool Create_LuaCpp_Wrapper_Head_File(_Project_Lua_Info* pLuaProject, _Project_Cpp_Info* pCppProject)
{
	//在编写CPP文件
	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	sprintf_safe(szPathFile, 200, "%s/LuaCppWrapper/LuaCppWrapper.h", 
		pCppProject->m_szProjectName);

	FILE* pFile = fopen(szPathFile, "wb");
	if(NULL == pFile)
	{
		return false;
	}

	sprintf_safe(szTemp, 200, "#ifndef _LUA_CPP_WRAPPER_H_\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "#define _LUA_CPP_WRAPPER_H_\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "//API Lua Project.\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	//编写引用头文件
	sprintf_safe(szTemp, 200, "#include \"LuaCommon.h\"\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#include \"UserDataInterface.h\"\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		//首先生成引用文件。
		sprintf_safe(szTemp, 200, "#include \"%s.h\"\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}
	sprintf_safe(szTemp, 200, "\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//如果有返回数据，生成返回数据结构体
	for(int i = 0; i < (int)pLuaProject->m_vecLuaFileList.size(); i++)
	{
		for(int j = 0; j <  (int)pLuaProject->m_vecLuaFileList[i].m_vecFunctionList.size(); j++)
		{
			//寻找有没有输出参数，如果有，则需要生成对应的输出结构体
			_Function_Info& obj_Function_Info = (_Function_Info& )pLuaProject->m_vecLuaFileList[i].m_vecFunctionList[j];
			bool blIsOut = false;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
				{
					blIsOut = true;
					break;
				}
			}

			//如果有输出参数，开始组建结构体
			if(true == blIsOut)
			{
				sprintf_safe(szTemp, 200, "//this is LuaFunc %s return parem struct.\n", obj_Function_Info.m_szFunctionName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "struct Re_%s\n", obj_Function_Info.m_szFunctionName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "{\n", obj_Function_Info.m_szFunctionName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

				for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
					{
						sprintf_safe(szTemp, 200, "\t%s %s;\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamType,
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
				}
				sprintf_safe(szTemp, 200, "};\n\n", obj_Function_Info.m_szFunctionName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
		}
	}

	//编写注册绑定函数
	sprintf_safe(szTemp, 200, "void Regedit_ToLua_Function(lua_State* L);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//生成luaopen_userdatainterface函数
	sprintf_safe(szTemp, 200, "int luaopen_userdatainterface(lua_State * L);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//生成调用Lua接口函数
	for(int i = 0; i < (int)pLuaProject->m_vecLuaFileList.size(); i++)
	{
		//编写函数和函数入参
		bool blIsOut = false;
		for(int j = 0; j <  (int)pLuaProject->m_vecLuaFileList[i].m_vecFunctionList.size(); j++)
		{
			_Function_Info& obj_Function_Info = (_Function_Info& )pLuaProject->m_vecLuaFileList[i].m_vecFunctionList[j];
			sprintf_safe(szTemp, 200, "bool LuaAPI_%s(lua_State* L", 
				obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			int nInCount = 0;
			blIsOut = false;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_IN)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
					{
						sprintf_safe(szTemp, 200, ", int %s", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
					{
						sprintf_safe(szTemp, 200, ", const char* %s", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else
					{
						sprintf_safe(szTemp, 200, ", %s %s", 
							obj_Function_Info.m_vecParamList[k].m_szParamType,
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}

					nInCount++;
				}
				else
				{
					blIsOut = true;
				}
			}

			//如果有输出参数，则拼接输出参数
			if(true == blIsOut)
			{
				sprintf_safe(szTemp, 200, ", Re_%s& obj_%s", 
					obj_Function_Info.m_szFunctionName, 
					obj_Function_Info.m_szFunctionName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, ");\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
			else
			{
				sprintf_safe(szTemp, 200, ");\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
		}
	}

	sprintf_safe(szTemp, 200, "#endif\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	fclose(pFile);
	return true;
}

bool Create_LuaCpp_Wrapper_Cpp_File(_Project_Lua_Info* pLuaProject, _Project_Cpp_Info* pCppProject)
{
	//在编写CPP文件
	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	sprintf_safe(szPathFile, 200, "%s/LuaCppWrapper/LuaCppWrapper.cpp", 
		pCppProject->m_szProjectName);

	FILE* pFile = fopen(szPathFile, "wb");
	if(NULL == pFile)
	{
		return false;
	}

	//编写引用头文件
	sprintf_safe(szTemp, 200, "#include \"LuaCppWrapper.h\"\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//编写注册绑定函数
	sprintf_safe(szTemp, 200, "void Regedit_ToLua_Function(lua_State* L)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		//编写函数
		for(int j = 0; j <  (int)pCppProject->m_vecCppFileList[i].m_vecFunctionList.size(); j++)
		{
			_Function_Info& obj_Function_Info = (_Function_Info& )pCppProject->m_vecCppFileList[i].m_vecFunctionList[j];
			sprintf_safe(szTemp, 200, "\tlua_register(L, \"%s\", LuaFn_%s);\n", 
				obj_Function_Info.m_szFunctionName, 
				obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		}
	}
	sprintf_safe(szTemp, 200, "}\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//生成luaopen_userdatainterface函数
	sprintf_safe(szTemp, 200, "int luaopen_userdatainterface(lua_State * L)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "\tRegedit_ToLua_Function(L);\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//sprintf_safe(szTemp, 200, "\tluaL_requiref(L,\"userdatainterface\",luaopen_lualib,1);\n");
	//fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "\treturn 1;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	
	sprintf_safe(szTemp, 200, "}\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//生成调用Lua接口函数
	for(int i = 0; i < (int)pLuaProject->m_vecLuaFileList.size(); i++)
	{
		//编写函数和函数入参
		bool blIsOut = false;
		for(int j = 0; j <  (int)pLuaProject->m_vecLuaFileList[i].m_vecFunctionList.size(); j++)
		{
			_Function_Info& obj_Function_Info = (_Function_Info& )pLuaProject->m_vecLuaFileList[i].m_vecFunctionList[j];
			sprintf_safe(szTemp, 200, "bool LuaAPI_%s(lua_State* L", 
				obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			int nInCount = 0;
			blIsOut = false;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_IN)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
					{
						sprintf_safe(szTemp, 200, ", int %s", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
					{
						sprintf_safe(szTemp, 200, ", const char* %s", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else
					{
						sprintf_safe(szTemp, 200, ", %s %s", 
							obj_Function_Info.m_vecParamList[k].m_szParamType,
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}

					nInCount++;
				}
				else
				{
					blIsOut = true;
				}
			}

			//如果有输出参数，则拼接输出参数
			if(true == blIsOut)
			{
				sprintf_safe(szTemp, 200, ", Re_%s& obj_%s", 
					obj_Function_Info.m_szFunctionName, 
					obj_Function_Info.m_szFunctionName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, ")\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
			else
			{
				sprintf_safe(szTemp, 200, ")\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}

			sprintf_safe(szTemp, 200, "{\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\tint nRet = 0;\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\tif(NULL == L)\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t{\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t\tLua_Print(\"[Test_Lua_File_Function]lua_State is NULL.\\n\");\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t\treturn false;\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t}\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\tnRet = lua_getglobal(L, \"%s\");\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\tif(6 != nRet)\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t{\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t\tLua_Print(\"[lua_getglobal]no find lua function.\\n\");\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t\treturn false;\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t}\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			//拼装入参
			int nOutCount = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_IN)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
					{
						sprintf_safe(szTemp, 200, "\tlua_pushnumber(L, %s);\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
					{
						sprintf_safe(szTemp, 200, "\tlua_pushstring(L, (char* )%s);\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else
					{
						sprintf_safe(szTemp, 200, "\tlua_pushlightuserdata(L, (void* )%s);\n", 
							obj_Function_Info.m_vecParamList[k].m_szParamName);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
				}
				else
				{
					nOutCount++;
				}
			}

			sprintf_safe(szTemp, 200, "\tnRet = lua_pcall(L, %d, %d, 0);\n", 
				nInCount, nOutCount);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\tif (LUA_OK != nRet)\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t{\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t\tLua_Print(\"[CLuaFn::CallFileFn]call function  fail error(%%s).\\n\", lua_tostring(L, -1));\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t\treturn false;\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\t}\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			//拼接出参
			int nIndex = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
					{
						sprintf_safe(szTemp, 200, "\tobj_%s.%s = (int)lua_tonumber(L, %d);\n", 
							obj_Function_Info.m_szFunctionName,
							obj_Function_Info.m_vecParamList[k].m_szParamName, 
							nIndex + 1);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
					{
						sprintf_safe(szTemp, 200, "\tsprintf(obj_%s.%s, \"%s\", (char*)lua_tostring(L, %d));\n", 
							obj_Function_Info.m_szFunctionName,
							obj_Function_Info.m_vecParamList[k].m_szParamName,
							nIndex + 1);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else
					{
						sprintf_safe(szTemp, 200, "\tobj_%s.%s = (%s)lua_touserdata(L, %d);\n",
							obj_Function_Info.m_szFunctionName,
							obj_Function_Info.m_vecParamList[k].m_szParamName,
							obj_Function_Info.m_vecParamList[k].m_szParamType,
							nIndex + 1);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					nIndex++;
				}
			}

			sprintf_safe(szTemp, 200, "\tlua_settop(L, 0);\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "\treturn true;\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "}\n\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		}
	}

	fclose(pFile);
	return true;
}

bool CreateMakefile( _Project_Cpp_Info* pCppProject )
{
	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	//自动生成makefile.define文件
	sprintf_safe(szPathFile, 200, "%s/Test/Makefile.define", 
		pCppProject->m_szProjectName);

	FILE* pFile = fopen(szPathFile, "wb");
	if(NULL == pFile)
	{
		return false;
	}

	sprintf_safe(szTemp, 200, "# *****************************\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "# predefine\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "# *****************************\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "CC = g++\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "AR = ar\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "CFLAGS = -g -O2 -D__LINUX__\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#set Lua lib path\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "INCLUDES = -I./ -I../ -I/usr/include  -I../LuaCppWrapper -I../LuaCppExec\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "LIBS = -L/usr/lib64 -L/usr/lib -L/usr/local/lib64 -L./ -L./Lib -L${THRIFT_LIB}  -L../ -ldl -lrt -llua\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "# *****************************\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "# rule\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "# *****************************\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "# Here are some rules for converting .cpp -> .o\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, ".SUFFIXES: .cpp .o\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, ".cpp.o:\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t@$(CC) -fPIC $(CFLAGS) ${INCLUDES} -c -g $*.cpp\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t@echo '----- '$*.cpp' is compiled ok! -----'\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "# Here are some rules for converting .c -> .o\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, ".SUFFIXES: .c .o\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, ".c.o:\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t@$(CC) $(CFLAGS) -c $*.c \n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t@echo '----- '$*.c' is compiled ok! -----'\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	fclose(pFile);

	sprintf_safe(szPathFile, 200, "%s/Test/Makefile", 
		pCppProject->m_szProjectName);

	pFile = fopen(szPathFile, "wb");
	if(NULL == pFile)
	{
		return false;
	}

	sprintf_safe(szTemp, 200, "include Makefile.define\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "PATS = ");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t../LuaCppWrapper/LuaCppWrapper.o \\\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//sprintf_safe(szTemp, 200, "\t../LuaCppWrapper/UserDataInterface.o \\\n");
	//fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		sprintf_safe(szTemp, 200, "\t../LuaCppWrapper/%s.o \\\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}
	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		sprintf_safe(szTemp, 200, "\t../LuaCppExec/Exec_%s.o \\\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}
	sprintf_safe(szTemp, 200, "\t./Test_%s.o\n\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "OBJS = ");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "\tLuaCppWrapper.o \\\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//sprintf_safe(szTemp, 200, "\tUserDataInterface.o \\\n");
	//fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		sprintf_safe(szTemp, 200, "\t%s.o \\\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}
	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		sprintf_safe(szTemp, 200, "\tExec_%s.o \\\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}
	sprintf_safe(szTemp, 200, "\tTest_%s.o\n\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "APP_NAME = %s\n\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "$(APP_NAME):$(PATS)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t$(CC) -rdynamic -o $(APP_NAME) $(OBJS) $(LIBS)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\trm -rf *.o\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "clean:\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\trm -rf *.o $(APP_NAME):\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "cl:\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\trm -rf *.o\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	fclose(pFile);

	return true;
}

bool Read_Test_File_XML( const char* pXMLName, _Test_API* pTestAPI )
{
	CXmlOpeation objXmlOpeation;
	return objXmlOpeation.Parse_XML_File_Test(pXMLName, pTestAPI);
}

bool Read_StructData_File_XML(const char* pXMLName, _Base_Data_Group* pBaseDataGroup)
{
	CXmlOpeation objXmlOpeation;
	return objXmlOpeation.Parse_XML_File_BaseData(pXMLName, pBaseDataGroup);
}

bool Create_User_Data_Interface_Head_Files(_Base_Data_Group* pBaseDataGroup)
{
	//在编写H文件
	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	sprintf_safe(szPathFile, 200, "%s/LuaCppWrapper/UserDataInterface.h", 
		pBaseDataGroup->m_szProjectName);

	FILE* pFile = fopen(szPathFile, "wb");
	if(NULL == pFile)
	{
		return false;
	}

	sprintf_safe(szTemp, 200, "//struct Common Data.\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#ifndef _USER_DATA_INTERFACE_H_\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#define _USER_DATA_INTERFACE_H_\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#ifdef __cplusplus\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "extern \"C\"\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#endif\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#include \"lua.h\"\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#include \"lualib.h\"\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#include \"lauxlib.h\"\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//sprintf_safe(szTemp, 200, "int luaopen_lualib(lua_State * L);\n\n");
	//fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#ifdef __cplusplus\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "}\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#endif\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pBaseDataGroup->m_vecBaseDataStruct.size(); i++)
	{
		//生成Struct
		sprintf_safe(szTemp, 200, "struct %s\n", pBaseDataGroup->m_vecBaseDataStruct[i].m_szStructName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "{\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		for(int j = 0; j < (int)pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo.size(); j++)
		{
			if(pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_nDataLen > 0)
			{
				sprintf_safe(szTemp, 200, "\t%s %s[%d]; //%s\n", 
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataType,
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName,
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_nDataLen,
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataDesc);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
			else
			{
				sprintf_safe(szTemp, 200, "\t%s %s; //%s\n", 
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataType,
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName,
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataDesc);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
		}
		sprintf_safe(szTemp, 200, "};\n\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}

	
	sprintf_safe(szTemp, 200, "#endif  //_USER_DATA_INTERFACE_H_\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	fclose(pFile);

	return true;
}

bool Create_User_Data_Interface_Cpp_Files( _Base_Data_Group* pBaseDataGroup )
{
	//在编写H文件
	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	sprintf_safe(szPathFile, 200, "%s/LuaCppWrapper/UserDataInterface.cpp", 
		pBaseDataGroup->m_szProjectName);

	FILE* pFile = fopen(szPathFile, "wb");
	if(NULL == pFile)
	{
		return false;
	}

	sprintf_safe(szTemp, 200, "#include \"UserDataInterface.h\"\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#include <stdio.h>\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "#include <string.h>\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pBaseDataGroup->m_vecBaseDataStruct.size(); i++)
	{
		//生成Struct成员get和set方法
		for(int j = 0; j < (int)pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo.size(); j++)
		{
			//生成get方法
			sprintf_safe(szTemp, 200, "//Struct %s %s get function\n", 
				pBaseDataGroup->m_vecBaseDataStruct[i].m_szStructName,
				pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			sprintf_safe(szTemp, 200, "static int Get%s(lua_State *L)\n", 
				pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			sprintf_safe(szTemp, 200, "{\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			sprintf_safe(szTemp, 200, "\tstruct %s *pobj = (struct %s*)lua_touserdata(L, 1);\n", 
				pBaseDataGroup->m_vecBaseDataStruct[i].m_szStructName,
				pBaseDataGroup->m_vecBaseDataStruct[i].m_szStructName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			sprintf_safe(szTemp, 200, "\tluaL_argcheck(L, pobj != NULL, 1, \"Wrong Parameter\");\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			if (0 == strcmp("char",pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataType))
			{
				sprintf_safe(szTemp, 200, "\tlua_pushstring(L, pobj->%s);\n", 
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
			else if (0 == strcmp("int",pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataType))
			{
				sprintf_safe(szTemp, 200, "\tlua_pushinteger(L, pobj->%s);\n", 
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}

			sprintf_safe(szTemp, 200, "\treturn 1;\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			
			sprintf_safe(szTemp, 200, "}\n\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			//生成set方法
			sprintf_safe(szTemp, 200, "//Struct %s %s set function\n", 
				pBaseDataGroup->m_vecBaseDataStruct[i].m_szStructName,
				pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			sprintf_safe(szTemp, 200, "static int Set%s(lua_State *L)\n", 
				pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			sprintf_safe(szTemp, 200, "{\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			sprintf_safe(szTemp, 200, "\tstruct %s *pobj = (struct %s*)lua_touserdata(L, 1);\n", 
				pBaseDataGroup->m_vecBaseDataStruct[i].m_szStructName,
				pBaseDataGroup->m_vecBaseDataStruct[i].m_szStructName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			sprintf_safe(szTemp, 200, "\tluaL_argcheck(L, pobj != NULL, 1, \"Wrong Parameter\");\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			if (0 == strcmp("char",pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataType))
			{
				sprintf_safe(szTemp, 200, "\tconst char *p%s = luaL_checkstring(L, 2);\n", 
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

				sprintf_safe(szTemp, 200, "\tluaL_argcheck(L, p%s != NULL && p%s != \"\", 2, \"Wrong Parameter\");\n",
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName,
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

				if(pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_nDataLen > 0)
				{
					sprintf_safe(szTemp, 200, "\tmemcpy(pobj->%s,p%s, %d);\n",
						pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName,
						pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName,
						pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_nDataLen);
					fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				}
				else
				{
					sprintf_safe(szTemp, 200, "\tpobj->%s = p%s[0];\n",
						pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName,
						pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
					fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				}
			}
			else if (0 == strcmp("int",pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataType))
			{
				sprintf_safe(szTemp, 200, "\tint %s = luaL_checkinteger(L, 2);\n", 
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

				sprintf_safe(szTemp, 200, "\tluaL_argcheck(L, true, 2, \"Wrong Parameter\");\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

				sprintf_safe(szTemp, 200, "\tpobj->%s = %s;\n",
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName,
					pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}

			sprintf_safe(szTemp, 200, "\treturn 0;\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			
			sprintf_safe(szTemp, 200, "}\n\n");
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		}

	}

	//生成luaL_Reg数组
	sprintf_safe(szTemp, 200, "static struct luaL_Reg arrayFunc_meta[] =\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pBaseDataGroup->m_vecBaseDataStruct.size(); i++)
	{		
		for(int j = 0; j < (int)pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo.size(); j++)
		{			
			sprintf_safe(szTemp, 200, "\t{ \"get%s\", Get%s },\n", 
				pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName,
				pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			sprintf_safe(szTemp, 200, "\t{ \"set%s\", Set%s },\n", 
				pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName,
				pBaseDataGroup->m_vecBaseDataStruct[i].m_vecBaseDataInfo[j].m_szDataName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		}

	}
	sprintf_safe(szTemp, 200, "\t{ NULL, NULL }\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	
	sprintf_safe(szTemp, 200, "};\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	//生成luaopen_lualib函数
	sprintf_safe(szTemp, 200, "int luaopen_lualib(lua_State * L)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "{\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "\tluaL_newlib(L, arrayFunc_meta);\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "\treturn 1;\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	
	sprintf_safe(szTemp, 200, "}\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);	

	fclose(pFile);

	return true;
}

bool Create_Cpp_Exec_File( _Project_Cpp_Info* pCppProject )
{
	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		//首先生成H文件。
		sprintf_safe(szPathFile, 200, "%s/LuaCppExec/Exec_%s.h", 
			pCppProject->m_szProjectName,
			pCppProject->m_vecCppFileList[i].m_szFileName);

		//查找是否已存在此文件，开始分解其中的数据，并合并到新文件中去。
		_File_Info obj_H_File_Info;
		Parse_CAPI_File(szPathFile, obj_H_File_Info);

		FILE* pFile = fopen(szPathFile, "wb");
		if(NULL == pFile)
		{
			return false;
		}

		sprintf_safe(szTemp, 200, "#ifndef _EXEC_%s_H\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "#define _EXEC_%s_H\n\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		sprintf_safe(szTemp, 200, "#include \"UserDataInterface.h\"\n\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		sprintf_safe(szTemp, 200, CAPI_INCLUDE_BEGIN);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		//如果原来代码中包含头引用，复制过来
		if(obj_H_File_Info.m_strExtHead.length() > 0)
		{			
			fwrite(obj_H_File_Info.m_strExtHead.c_str(), obj_H_File_Info.m_strExtHead.length(), sizeof(char), pFile);
		}

		sprintf_safe(szTemp, 200, CAPI_INCLUDE_END);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		for(int j = 0; j < (int)pCppProject->m_vecCppFileList[i].m_vecFunctionList.size(); j++)
		{
			_Function_Info& obj_Function_Info = (_Function_Info& )pCppProject->m_vecCppFileList[i].m_vecFunctionList[j];

			//比较是否有用到的函数头
			_FunctionCode* pFunctionCode = NULL;
			Search_CAPI_Code(obj_Function_Info.m_szFunctionName, obj_H_File_Info, pFunctionCode);
			if(NULL != pFunctionCode && pFunctionCode->m_strNotes.length() > 0)
			{
				fwrite(pFunctionCode->m_strNotes.c_str(), pFunctionCode->m_strNotes.length(), sizeof(char), pFile);
			}
			else
			{
				sprintf_safe(szTemp, 200, "//Exec C_API %s\n", obj_Function_Info.m_szDesc);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
			sprintf_safe(szTemp, 200, "void Exec_%s(", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);


			int nInPos = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_IN)
				{
					if(nInPos == 0)
					{
						if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
						{
							sprintf_safe(szTemp, 200, "%s %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else
						{
							sprintf_safe(szTemp, 200, "%s* %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}

						nInPos++;
					}
					else
					{
						if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
						{
							sprintf_safe(szTemp, 200, ", %s %s",
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else
						{
							sprintf_safe(szTemp, 200, ", %s* %s",
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}

						nInPos++;
					}
				}
			}

			//编写调用函数接口(拼接出参)
			int nOutPos = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
				{
					if(nOutPos == 0)
					{
						if(nInPos == 0)
						{
							if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
							{
								sprintf_safe(szTemp, 200, "%s& %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
							else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
							{
								sprintf_safe(szTemp, 200, "%s* %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
							else
							{
								sprintf_safe(szTemp, 200, "%s*& %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
						}
						else
						{
							if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
							{
								sprintf_safe(szTemp, 200, ", %s& %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
							else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
							{
								sprintf_safe(szTemp, 200, ", %s* %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
							else
							{
								sprintf_safe(szTemp, 200, ", %s*& %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
						}
						nOutPos++;
					}
					else
					{
						if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
						{
							sprintf_safe(szTemp, 200, ", %s& %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
						{
							sprintf_safe(szTemp, 200, ", %s* %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else
						{
							sprintf_safe(szTemp, 200, ", %s*& %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}

						nOutPos++;
					}
				}
			}
			sprintf_safe(szTemp, 200, ");\n\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		}

		//把没用到的函数补充进去
		for(int m = 0; m < (int)obj_H_File_Info.m_vecFunctionCode.size(); m++)
		{
			if(obj_H_File_Info.m_vecFunctionCode[m].m_blIsUsed == false)
			{
				if(obj_H_File_Info.m_vecFunctionCode[m].m_strNotes.length() > 0)
				{
					fwrite(obj_H_File_Info.m_vecFunctionCode[m].m_strNotes.c_str(), 
						obj_H_File_Info.m_vecFunctionCode[m].m_strNotes.length(), 
						sizeof(char), pFile);
				}
				fwrite(obj_H_File_Info.m_vecFunctionCode[m].m_strFuncCode.c_str(), 
					obj_H_File_Info.m_vecFunctionCode[m].m_strFuncCode.length(), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
		}

		sprintf_safe(szTemp, 200, "#endif\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		fclose(pFile);
	}

	//编写CPP
	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		//首先生成H文件。
		sprintf_safe(szPathFile, 200, "%s/LuaCppExec/Exec_%s.cpp", 
			pCppProject->m_szProjectName,
			pCppProject->m_vecCppFileList[i].m_szFileName);

		//查找是否已存在此文件，开始分解其中的数据，并合并到新文件中去。
		_File_Info obj_CPP_File_Info;
		Parse_CAPI_File(szPathFile, obj_CPP_File_Info);

		FILE* pFile = fopen(szPathFile, "wb");
		if(NULL == pFile)
		{
			return false;
		}

		sprintf_safe(szTemp, 200, "#include \"Exec_%s.h\"\n\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		sprintf_safe(szTemp, 200, CAPI_INCLUDE_BEGIN);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		//如果原来代码中包含头引用，复制过来
		if(obj_CPP_File_Info.m_strExtHead.length() > 0)
		{			
			fwrite(obj_CPP_File_Info.m_strExtHead.c_str(), obj_CPP_File_Info.m_strExtHead.length(), sizeof(char), pFile);
		}

		sprintf_safe(szTemp, 200, CAPI_INCLUDE_END);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		for(int j = 0; j < (int)pCppProject->m_vecCppFileList[i].m_vecFunctionList.size(); j++)
		{
			_Function_Info& obj_Function_Info = (_Function_Info& )pCppProject->m_vecCppFileList[i].m_vecFunctionList[j];
			//比较是否有用到的函数头
			_FunctionCode* pFunctionCode = NULL;
			Search_CAPI_Code(obj_Function_Info.m_szFunctionName, obj_CPP_File_Info, pFunctionCode);

			if(pFunctionCode != NULL && pFunctionCode->m_strNotes.length() > 0)
			{
				fwrite(pFunctionCode->m_strNotes.c_str(), 
					pFunctionCode->m_strNotes.length(), 
					sizeof(char), pFile);
			}
			else
			{
				sprintf_safe(szTemp, 200, "//Exec C_API %s\n", obj_Function_Info.m_szDesc);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}

			sprintf_safe(szTemp, 200, "void Exec_%s(", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			int nInPos = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_IN)
				{
					if(nInPos == 0)
					{
						if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
						{
							sprintf_safe(szTemp, 200, "%s %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else
						{
							sprintf_safe(szTemp, 200, "%s* %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						nInPos++;
					}
					else
					{
						if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
						{
							sprintf_safe(szTemp, 200, ", %s %s",
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else
						{
							sprintf_safe(szTemp, 200, "%s* %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}

						nInPos++;
					}
				}
			}

			//编写调用函数接口(拼接出参)
			int nOutPos = 0;
			for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
			{
				if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
				{
					if(nOutPos == 0)
					{
						if(nInPos == 0)
						{
							if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
							{
								sprintf_safe(szTemp, 200, "%s& %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
							else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
							{
								sprintf_safe(szTemp, 200, "%s* %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
							else
							{
								sprintf_safe(szTemp, 200, "%s*& %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
						}
						else
						{
							if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
							{
								sprintf_safe(szTemp, 200, ", %s& %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
							else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
							{
								sprintf_safe(szTemp, 200, ", %s* %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}
							else
							{
								sprintf_safe(szTemp, 200, ", %s*& %s", 
									obj_Function_Info.m_vecParamList[k].m_szParamType, 
									obj_Function_Info.m_vecParamList[k].m_szParamName);
								fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
							}

						}
						nOutPos++;
					}
					else
					{
						if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_INT)
						{
							sprintf_safe(szTemp, 200, ", %s& %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
						{
							sprintf_safe(szTemp, 200, ", %s* %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else
						{
							sprintf_safe(szTemp, 200, ", %s*& %s", 
								obj_Function_Info.m_vecParamList[k].m_szParamType, 
								obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}

						nOutPos++;
					}
				}
			}
			sprintf_safe(szTemp, 200, ")\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			sprintf_safe(szTemp, 200, "{\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			if(NULL != pFunctionCode && pFunctionCode->m_strCode.length() > 0)
			{
				fwrite(pFunctionCode->m_strCode.c_str(), pFunctionCode->m_strCode.length(), sizeof(char), pFile);
			}
			else
			{
				sprintf_safe(szTemp, 200, "\t//please add your code at here.\n", obj_Function_Info.m_szFunctionName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}

			sprintf_safe(szTemp, 200, "}\n\n", obj_Function_Info.m_szFunctionName);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		}

		//把没用到的函数补充进去
		for(int m = 0; m < (int)obj_CPP_File_Info.m_vecFunctionCode.size(); m++)
		{
			if(obj_CPP_File_Info.m_vecFunctionCode[m].m_blIsUsed == false)
			{
				if(obj_CPP_File_Info.m_vecFunctionCode[m].m_strNotes.length() > 0)
				{
					fwrite(obj_CPP_File_Info.m_vecFunctionCode[m].m_strNotes.c_str(), 
						obj_CPP_File_Info.m_vecFunctionCode[m].m_strNotes.length(), sizeof(char), pFile);
				}
				fwrite(obj_CPP_File_Info.m_vecFunctionCode[m].m_strFuncCode.c_str(), 
					obj_CPP_File_Info.m_vecFunctionCode[m].m_strFuncCode.length(), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "{\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				fwrite(obj_CPP_File_Info.m_vecFunctionCode[m].m_strCode.c_str(), 
					obj_CPP_File_Info.m_vecFunctionCode[m].m_strCode.length(), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "}\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\n\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}
		}

		fclose(pFile);
	}

	return true;
}
