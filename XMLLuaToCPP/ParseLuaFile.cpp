#include "ParseLuaFile.h"

bool Parse_Lua_File( const char* pFileName, vec_FileLine& obj_vec_FileLine )
{
	char szBakFileName[200] = {'\0'};
	char* pFileBuff = NULL;

	if(NULL == pFileName)
	{
		return false;
	}
	sprintf_safe(szBakFileName, 200, "%s.bak", pFileName);

	FILE* pFile = fopen(pFileName, "rb+");
	if(NULL == pFile)
	{
		return false;
	}

	//得到文件长度
	fseek(pFile, 0, SEEK_END);
	int nFileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	//取出文件内容，备份文件
	pFileBuff = new char[nFileSize + 1];
	fread(pFileBuff, nFileSize, sizeof(char), pFile);
	pFileBuff[nFileSize] = '\0';
	fclose(pFile);
	FILE* pBakFile = fopen(szBakFileName, "wb");
	if(NULL == pBakFile)
	{
		return false;
	}
	fwrite(pFileBuff, nFileSize, sizeof(char), pBakFile);
	fclose(pBakFile);

	//拆解出所有的行
	char szLine[2000]  = {'\0'};
	int nLineBegin     = 0;
	int nLineEnd       = 0;
	for(int i = 0; i < nFileSize; i++)
	{
		if(pFileBuff[i] == '\n')
		{
			nLineEnd = i;
			if(nLineEnd > nLineBegin)
			{
				//找出一行文本
				memcpy(szLine, &pFileBuff[nLineBegin], nLineEnd - nLineBegin + 1);
				szLine[nLineEnd - nLineBegin + 1] = '\0';

				_FileLine obj_FileLine;
				obj_FileLine.m_strLine = (string)szLine;
				obj_vec_FileLine.push_back(obj_FileLine);

				nLineBegin = i + 1;
			}
			else
			{
				nLineBegin++;
			}
		}

	}
	
	delete[] pFileBuff;

	return true;
}

bool Searsh_Lua_File(const char* pFunctionName, const char* pFunctionLine, string strFuncBody, vec_FileLine& obj_vec_FileLine)
{
	char szFindText[300] = {'\0'};
	sprintf_safe(szFindText, 300, "function %s", pFunctionName);

	for(int i = 0; i < (int)obj_vec_FileLine.size(); i++)
	{
		if(NULL != strstr(obj_vec_FileLine[i].m_strLine.c_str(), pFunctionName))
		{
			//找到了，替换之
			obj_vec_FileLine[i].m_strLine = (string)pFunctionLine;
			return true;
		}
	}

	//没有找到，这是一个新的函数
	_FileLine obj_FileLine;
	obj_FileLine.m_strLine = strFuncBody;
	obj_vec_FileLine.push_back(obj_FileLine);
	return false;
}

bool Save_Lua_File(const char* pFileName, vec_FileLine& obj_vec_FileLine)
{
	FILE* pFile = fopen(pFileName, "wb");
	if(NULL == pFile)
	{
		return false;
	}

	for(int i = 0; i < (int)obj_vec_FileLine.size(); i++)
	{
		fwrite(obj_vec_FileLine[i].m_strLine.c_str(),
			obj_vec_FileLine[i].m_strLine.length(),
			sizeof(char),
			pFile);
	}
	fclose(pFile);

	return true;
}
