#include "CppFileCreate.h"

void Tranfile( const char* pFileSrc, const char* pFileDes )
{
	fstream fsCopee( pFileSrc, ios::binary | ios::in ) ;
	fstream fsCoper( pFileDes, ios::binary | ios::out ) ;
	fsCoper << fsCopee.rdbuf() ;
}

void Create_Lua_Environment( _Project_Lua_Info* pLuaProject )
{
	char szLuaInclude[50] = {"LuaInclude"};
	char szLuaLib[50]     = {"LuaLib"};
	char szTempPath[50]   = {'\0'};
	char szTempFile[100]  = {'\0'};

	//创建LuaIncode目录
	sprintf_safe(szTempPath, 50, "%s/%s", pLuaProject->m_szProjectName, szLuaInclude);
#ifdef WIN32
	_mkdir(szTempPath);
#else
	mkdir(szTempPath, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
#endif

	//把Lua文件拷贝过去
	sprintf_safe(szTempFile, 100, "%s/lua.h", szTempPath);
	Tranfile("../LuaInclude/lua.h", szTempFile);
	sprintf_safe(szTempFile, 100, "%s/lauxlib.h", szTempPath);
	Tranfile("../LuaInclude/lauxlib.h", szTempFile);
	sprintf_safe(szTempFile, 100, "%s/lua.hpp", szTempPath);
	Tranfile("../LuaInclude/lua.hpp", szTempFile);
	sprintf_safe(szTempFile, 100, "%s/luaconf.h", szTempPath);
	Tranfile("../LuaInclude/luaconf.h", szTempFile);
	sprintf_safe(szTempFile, 100, "%s/lualib.h", szTempPath);
	Tranfile("../LuaInclude/lualib.h", szTempFile);

	//创建Lib目录
	sprintf_safe(szTempPath, 50, "%s/%s", pLuaProject->m_szProjectName, szLuaLib);
#ifdef WIN32
	_mkdir(szTempPath);
#else
	mkdir(szTempPath, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
#endif

	//拷贝LuaCommon.h文件
	sprintf_safe(szTempPath, 50, "%s", pLuaProject->m_szProjectName);
	sprintf_safe(szTempFile, 100, "%s/LuaCommon.h", szTempPath);
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
		sprintf_safe(szPathFile, 200, "%s/%s.h", 
			pCppProject->m_szProjectName,
			pCppProject->m_vecCppFileList[i].m_szFileName);

		FILE* pFile = fopen(szPathFile, "w");
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
		sprintf_safe(szTemp, 200, "#include <stdio.h>\n\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

		sprintf_safe(szTemp, 200, "extern \"C\"\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		sprintf_safe(szTemp, 200, "{\n");
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
		//编写函数
		for(int j = 0; j <  (int)pCppProject->m_vecCppFileList[i].m_vecFunctionList.size(); j++)
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
		sprintf_safe(szPathFile, 200, "%s/%s.cpp", 
			pCppProject->m_szProjectName,
			pCppProject->m_vecCppFileList[i].m_szFileName);

		pFile = fopen(szPathFile, "w");
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

			sprintf_safe(szTemp, 200, "\t//add your API code at here.\n\n", obj_Function_Info.m_szFunctionName);
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

	//sprintf_safe(szPathFile, 200, "%s/Test_%s.cpp", 
	//	pCppProject->m_szProjectName,
	//	pCppProject->m_szProjectName);

	sprintf_safe(szPathFile, 200, "%s/API_%s.h", 
		pCppProject->m_szProjectName,
		pCppProject->m_szProjectName);

	FILE* pFile = fopen(szPathFile, "w");
	if(NULL == pFile)
	{
		return false;
	}

	sprintf_safe(szTemp, 200, "//API Lua Project.\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	//编写引用头文件
	sprintf_safe(szTemp, 200, "#include \"LuaCommon.h\"\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		//首先生成引用文件。
		sprintf_safe(szTemp, 200, "#include \"%s.h\"\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}
	sprintf_safe(szTemp, 200, "\n");
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
	sprintf_safe(szTemp, 200, "\tRegedit_ToLua_Function(L);\n\n");
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
						sprintf_safe(szTemp, 200, ", %s* %s", 
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
							nIndex);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else if(obj_Function_Info.m_vecParamList[k].m_emParamClass == PARAM_CLASS_STRING)
					{
						sprintf_safe(szTemp, 200, "\tsprintf(obj_%s.%s, \"%s\", (char*)lua_tostring(L, %d));\n", 
							obj_Function_Info.m_szFunctionName,
							obj_Function_Info.m_vecParamList[k].m_szParamName,
							nIndex);
						fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
					}
					else
					{
						sprintf_safe(szTemp, 200, "\tobj_%s.%s = lua_tolightuserdata(L, %d);\n",
							obj_Function_Info.m_szFunctionName,
							obj_Function_Info.m_vecParamList[k].m_szParamName,
							nIndex);
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

	//生辰测试工程文件
	sprintf_safe(szPathFile, 200, "%s/Test_%s.cpp", 
		pCppProject->m_szProjectName,
		pCppProject->m_szProjectName);

	pFile = fopen(szPathFile, "w");
	if(NULL == pFile)
	{
		return false;
	}

	sprintf_safe(szTemp, 200, "//Test Lua Project.\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	//编写引用头文件
	sprintf_safe(szTemp, 200, "#include \"API_%s.h\"\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "#include <string.h>\n\n", pCppProject->m_szProjectName);
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
				if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_emParamClass != PARAM_CLASS_INT)
				{
					sprintf_safe(szTempl, MAX_BUFF_50, "\"%s\"", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
				}
				else
				{
					sprintf_safe(szTempl, MAX_BUFF_50, "%s", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
				}
				strTemp += szTempl;
			}
			else
			{
				if(pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_emParamClass != PARAM_CLASS_INT)
				{
					sprintf_safe(szTempl, MAX_BUFF_50, ",\"%s\"", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
				}
				else
				{
					sprintf_safe(szTempl, MAX_BUFF_50, ",%s", pTestAPI->m_vecTestAPIInfo[i].m_vecTestLuaParamIn[j].m_szValue);
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
		sprintf_safe(szTemp, 200, "}\n");
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
	for(int i = 0; i < (int)pLuaProject->m_vecLuaFileList.size(); i++)
	{
		sprintf_safe(szTemp, 200, "\t\tOpen_Lua_File(pLuaState, \"%s.lua\");\n", 
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

bool CreateMakefile( _Project_Cpp_Info* pCppProject )
{
	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	//自动生成makefile.define文件
	sprintf_safe(szPathFile, 200, "%s/Makefile.define", 
		pCppProject->m_szProjectName);

	FILE* pFile = fopen(szPathFile, "w");
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
	sprintf_safe(szTemp, 200, "INCLUDES = -I./ -I../ -I/usr/include  -I./Include\n");
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
	sprintf_safe(szTemp, 200, "\t@echo '----- '$*.cpp' is compiled ok!'\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "# Here are some rules for converting .c -> .o\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, ".SUFFIXES: .c .o\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, ".c.o:\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t@$(CC) $(CFLAGS) -c $*.c \n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t@echo '----- '$*.c' is compiled ok!'\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	fclose(pFile);

	sprintf_safe(szPathFile, 200, "%s/Makefile", 
		pCppProject->m_szProjectName);

	pFile = fopen(szPathFile, "w");
	if(NULL == pFile)
	{
		return false;
	}

	sprintf_safe(szTemp, 200, "include Makefile.define\n\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "PATS = ");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		sprintf_safe(szTemp, 200, "\t%s.o \\\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}
	sprintf_safe(szTemp, 200, "\tTest_%s.o\n\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "OBJS = ");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	for(int i = 0; i < (int)pCppProject->m_vecCppFileList.size(); i++)
	{
		sprintf_safe(szTemp, 200, "\t%s.o \\\n", pCppProject->m_vecCppFileList[i].m_szFileName);
		fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	}
	sprintf_safe(szTemp, 200, "\tTest_%s.o\n\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

	sprintf_safe(szTemp, 200, "APP_NAME = %s\n\n", pCppProject->m_szProjectName);
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "$(APP_NAME):$(PATS)\n");
	fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
	sprintf_safe(szTemp, 200, "\t$(CC) -rdynamic -o $(APP_NAME) $(OBJS) $(LIBS)\n\n");
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
