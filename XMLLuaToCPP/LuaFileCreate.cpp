#include "LuaFileCreate.h"

//读取XML文件变换成pLuaFileInfo数据结构
bool Read_Lua_File_XML( const char* pXMLName, _Project_Lua_Info* pLuaProject )
{
	CXmlOpeation objXmlOpeation;
	return objXmlOpeation.Parse_XML_File_Lua(pXMLName, pLuaProject);
}

bool Creat_Lua_Files( _Project_Lua_Info* pLuaProject )
{
	//创建目录
#ifdef WIN32
	_mkdir(pLuaProject->m_szProjectName);
#else
	mkdir(pLuaProject->m_szProjectName, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
#endif

	char szTempPath[50]   = {'\0'};
	char szTempFile[100]  = {'\0'};

	//创建LuaIncode目录
	sprintf_safe(szTempPath, 50, "%s/LuaScript", pLuaProject->m_szProjectName);
#ifdef WIN32
	_mkdir(szTempPath);
#else
	mkdir(szTempPath, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH);
#endif

	char szTemp[1024]     = {'\0'};
	char szPathFile[200]  = {'\0'};

	for(int i = 0; i < (int)pLuaProject->m_vecLuaFileList.size(); i++)
	{
		sprintf_safe(szPathFile, 200, "%s/LuaScript/%s.lua", 
			pLuaProject->m_szProjectName,
			pLuaProject->m_vecLuaFileList[i].m_szFileName);

		//首先查找当前文件是否存在
		vec_FileLine obj_vec_FileLine;

		bool blIsFind = Parse_Lua_File(szPathFile, obj_vec_FileLine);

		if(false == blIsFind)
		{
			//生成声明文件。
			FILE* pFile = fopen(szPathFile, "wb");
			if(NULL == pFile)
			{
				return false;
			}

			//编写文件说明
			sprintf_safe(szTemp, 200, "--%s\n\n", pLuaProject->m_vecLuaFileList[i].m_szDesc);
			fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

			//编写函数
			for(int j = 0; j <  (int)pLuaProject->m_vecLuaFileList[i].m_vecFunctionList.size(); j++)
			{
				_Function_Info& obj_Function_Info = (_Function_Info& )pLuaProject->m_vecLuaFileList[i].m_vecFunctionList[j];
				sprintf_safe(szTemp, 200, "--%s\n", obj_Function_Info.m_szDesc);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "function %s(", obj_Function_Info.m_szFunctionName);
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

				//拼接入参
				int nPos = 0;
				for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_IN)
					{
						if(nPos == 0)
						{
							sprintf_safe(szTemp, 200, "%s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else
						{
							sprintf_safe(szTemp, 200, ", %s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						nPos++;
					}
				}
				sprintf_safe(szTemp, 200, ")\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
				sprintf_safe(szTemp, 200, "\t--add your Lua code at here.\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

				//拼接出参
				nPos = 0;
				for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
					{
						if(nPos == 0)
						{
							sprintf_safe(szTemp, 200, "\treturn ");
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);

							sprintf_safe(szTemp, 200, "%s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						else
						{
							sprintf_safe(szTemp, 200, ", %s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
						}
						nPos++;
					}
				}

				sprintf_safe(szTemp, 200, "\nend\n\n");
				fwrite(szTemp, strlen(szTemp), sizeof(char), pFile);
			}

			fclose(pFile);
		}
		else
		{
			//查找替换函数
			for(int j = 0; j <  (int)pLuaProject->m_vecLuaFileList[i].m_vecFunctionList.size(); j++)
			{
				_Function_Info& obj_Function_Info = (_Function_Info& )pLuaProject->m_vecLuaFileList[i].m_vecFunctionList[j];

				char szTempLine[200] = {'\0'};
				string strFunctionLine = "";
				string strFunctionBody = "";

				sprintf_safe(szTempLine, 200, "--%s\n", obj_Function_Info.m_szDesc);
				strFunctionBody += (string)szTempLine;
				sprintf_safe(szTempLine, 200, "function %s(", obj_Function_Info.m_szFunctionName);
				strFunctionLine += (string)szTempLine;

				//拼接入参
				int nPos = 0;
				for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_IN)
					{
						if(nPos == 0)
						{
							sprintf_safe(szTempLine, 200, "%s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							strFunctionLine += (string)szTempLine;
						}
						else
						{
							sprintf_safe(szTempLine, 200, ", %s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							strFunctionLine += (string)szTempLine;
						}
						nPos++;
					}
				}
				sprintf_safe(szTempLine, 200, ")\n");
				strFunctionLine += (string)szTempLine;

				//拼接函数体
				strFunctionBody += strFunctionLine;

				nPos = 0;
				for(int k = 0; k < (int)obj_Function_Info.m_vecParamList.size(); k++)
				{
					if(obj_Function_Info.m_vecParamList[k].m_emParamType == PARAM_TYPE_OUT)
					{
						if(nPos == 0)
						{
							sprintf_safe(szTempLine, 200, "\treturn ");
							strFunctionBody += (string)szTempLine;

							sprintf_safe(szTempLine, 200, "%s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							strFunctionBody += (string)szTempLine;
						}
						else
						{
							sprintf_safe(szTempLine, 200, ", %s", obj_Function_Info.m_vecParamList[k].m_szParamName);
							strFunctionBody += (string)szTempLine;
						}
						nPos++;
					}
				}
				sprintf_safe(szTempLine, 200, "\nend\n\n");
				strFunctionBody += (string)szTempLine;

				Searsh_Lua_File(obj_Function_Info.m_szFunctionName, strFunctionLine.c_str(), strFunctionBody, obj_vec_FileLine);
			}

			//合并替换完的文件保存
			Save_Lua_File(szPathFile, obj_vec_FileLine);
		}
	}


	return true;
}
