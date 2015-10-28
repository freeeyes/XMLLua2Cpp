#include "ParseCAPIFile.h"

char * ltrim(char * s) 
{
	while (s && *s && isspace(*s)) ++s;
	return s;
}

bool Parse_Function_Name(char* pLine, char* pFunctionName)
{
	int nLen   = (int)strlen(pLine);
	int nBegin = 0;
	int nEnd   = 0;

	for(int i = 0; i < nLen; i++)
	{
		if(pLine[i] == ' ' && nBegin == 0)
		{
			nBegin = i;
		}
		
		if(pLine[i] == '(')
		{
			nEnd = i;
			break;
		}
	}

	if(nEnd > nBegin)
	{
		memcpy(pFunctionName, (char*)pLine + nBegin, nEnd - nBegin);
		pFunctionName[nEnd - nBegin] = '\0';
		pFunctionName = ltrim(pFunctionName);
		return true;
	}
	else
	{
		return false;
	}
}

void Parse_File_Include(char* pData, int nFileSize, _File_Info& obj_File_Info)
{
	//中包含的其他.h
	char* pIncludeBegin = strstr(pData, "#include");
	while(pIncludeBegin)
	{
		int nBegin = (int)(pIncludeBegin - pData);
		int nIncludeLen = nFileSize - nBegin;
		int nPosLen = 0;
		for(int i = nBegin; i < nFileSize; i++)
		{
			if(pData[i] == '\n')
			{
				nIncludeLen = nPosLen;
				break;
			}
			nPosLen++;
		}

		char szTemp[200] = {'\0'};
		memcpy(szTemp, pIncludeBegin, nIncludeLen);
		szTemp[nIncludeLen] = '\0';

		if(strcmp(szTemp, "#include \"UserDataInterface.h\"") != 0)
		{
			_FileInclude obj_FileInclude;
			obj_FileInclude.m_strIncoude = (string)szTemp;
			obj_File_Info.m_vecFileInclude.push_back(obj_FileInclude);
		}

		pIncludeBegin = strstr(pIncludeBegin + nPosLen, "#include");
	}
}

void Parse_File_Function_Info(char* pData, int nFileSize, _File_Info& obj_File_Info)
{
	char szLine[2000]  = {'\0'};
	int nLineBegin     = 0;
	int nLineEnd       = 0;
	bool blIsLineBehin = true;
	bool blIsContent   = false;

	for(int i = 0; i < nFileSize; i++)
	{
		if(pData[i] == '\n')
		{
			nLineEnd = i;
			if(nLineEnd > nLineBegin)
			{
				//找出一行文本
				memcpy(szLine, &pData[nLineBegin], nLineEnd - nLineBegin);
				szLine[nLineEnd - nLineBegin] = '\0';

				//如果文本第一个字节不是#,\n,\r,\中的任何一个，则视为
				if(szLine[0] != '#' && szLine[0] != '\r' &&
					szLine[0] != '\n' && szLine[0] != '/')
				{
					if(szLine[0] == '{')
					{
						blIsContent = true;
						continue;
					}
					else if(szLine[0] == '}')
					{
						blIsContent = false;
						continue;
					}

					if(blIsContent == false)
					{
						//这个为函数头
						_FunctionCode obj_FunctionCode;
						Parse_Function_Name(szLine, obj_FunctionCode.m_szFuncName);
						obj_File_Info.m_vecFunctionCode.push_back(obj_FunctionCode);
					}
					else
					{
						//这个是函数体
						int nIndex = obj_File_Info.m_vecFunctionCode.size() - 1;
						_FunctionCode& obj_FunctionCode = obj_File_Info.m_vecFunctionCode[nIndex];
						obj_FunctionCode.m_strCode += (string)szLine;
					}
				}

				nLineBegin = i + 1;
			}
			else
			{
				nLineBegin++;
			}
		}
		
	}
}

bool Parse_CAPI_H_File(const char* pFileName, _File_Info& obj_File_Info)
{
	char szBakFileName[200] = {'\0'};
	char* pFileBuff = NULL;

	if(NULL == pFileName)
	{
		return false;
	}
	int nFileLen = strlen(pFileName) - 2;
	memcpy(szBakFileName, pFileName, nFileLen);
	szBakFileName[nFileLen] = '\0';
	sprintf_safe(szBakFileName, 200, "%s.hbak", szBakFileName);

	FILE* pFile = fopen(pFileName, "r");
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
	FILE* pBakFile = fopen(szBakFileName, "w");
	if(NULL == pBakFile)
	{
		return false;
	}
	fwrite(pFileBuff, nFileSize, sizeof(char), pBakFile);
	fclose(pBakFile);

	//开始分析数据
	Parse_File_Include(pFileBuff, nFileSize, obj_File_Info);

	//分析包中的函数体
	Parse_File_Function_Info(pFileBuff, nFileSize, obj_File_Info);

	delete[] pFileBuff;
	return true;
}

