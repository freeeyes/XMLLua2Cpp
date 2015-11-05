#include "XmlOpeation.h"

CXmlOpeation::CXmlOpeation(void)
{
	m_pTiXmlDocument = NULL;
	m_pRootElement   = NULL;
}

CXmlOpeation::~CXmlOpeation(void)
{
	Close();
}

bool CXmlOpeation::Init(const char* pFileName)
{
	Close();
	m_pTiXmlDocument = new TiXmlDocument(pFileName);
	if(NULL == m_pTiXmlDocument)
	{
		return false;
	}

	if(false == m_pTiXmlDocument->LoadFile())
	{
		return false;
	}

	//获得根元素
	m_pRootElement = m_pTiXmlDocument->RootElement();

	return true;
}

bool CXmlOpeation::Init_String(const char* pXMLText)
{
	//解析字符串
	Close();
	m_pTiXmlDocument = new TiXmlDocument();
	m_pTiXmlDocument->Parse(pXMLText, 0, TIXML_DEFAULT_ENCODING); 

	//获得根元素
	m_pRootElement = m_pTiXmlDocument->RootElement();

	return true;
}

void CXmlOpeation::Close()
{
	if(NULL != m_pTiXmlDocument)
	{
		delete m_pTiXmlDocument;
		m_pTiXmlDocument = NULL;
		m_pRootElement   = NULL;
	}
}

char* CXmlOpeation::GetData(const char* pName, const char* pAttrName)
{
	if(m_pRootElement == NULL)
	{
		return NULL;
	}

	TiXmlElement* pTiXmlElement = m_pRootElement->FirstChildElement(pName);
	if(NULL != pTiXmlElement)
	{
		return (char* )pTiXmlElement->Attribute(pAttrName);
	}

	return NULL;
}

char* CXmlOpeation::GetData(const char* pName, const char* pAttrName, TiXmlElement*& pNextTiXmlElement)
{
	if(m_pRootElement == NULL)
	{
		return NULL;
	}

	TiXmlElement* pTiXmlElement = NULL;
	if(NULL == pNextTiXmlElement)
	{
		pTiXmlElement = m_pRootElement->FirstChildElement(pName);
		pNextTiXmlElement = pTiXmlElement;
	}
	else
	{
		pTiXmlElement  = pNextTiXmlElement->NextSiblingElement();
		pNextTiXmlElement = pTiXmlElement;
	}

	if(NULL != pTiXmlElement)
	{
		return (char* )pTiXmlElement->Attribute(pAttrName);
	}

	return NULL;
}

char* CXmlOpeation::GetData_Text(const char* pName)
{
	if(m_pRootElement == NULL)
	{
		return NULL;
	}

	TiXmlElement* pTiXmlElement = m_pRootElement->FirstChildElement(pName);
	if(NULL != pTiXmlElement)
	{
		return (char* )pTiXmlElement->GetText();
	}

	return NULL;
}

char* CXmlOpeation::GetData_Text(const char* pName, TiXmlElement*& pNextTiXmlElement)
{
	if(m_pRootElement == NULL)
	{
		return NULL;
	}

	TiXmlElement* pTiXmlElement = NULL;
	if(NULL == pNextTiXmlElement)
	{
		pTiXmlElement = m_pRootElement->FirstChildElement(pName);
		pNextTiXmlElement = pTiXmlElement;
	}
	else
	{
		pTiXmlElement  = pNextTiXmlElement->NextSiblingElement();
		pNextTiXmlElement = pTiXmlElement;
	}

	if(NULL != pTiXmlElement)
	{
		return (char* )pTiXmlElement->GetText();
	}

	return NULL;
}

bool CXmlOpeation::Parse_XML_File_Lua( const char* pName, _Project_Lua_Info* pLuaProject )
{
	if(NULL == pLuaProject)
	{
		return false;
	}

	Close();
	m_pTiXmlDocument = new TiXmlDocument(pName);
	if(NULL == m_pTiXmlDocument)
	{
		return false;
	}

	if(false == m_pTiXmlDocument->LoadFile())
	{
		return false;
	}

	TiXmlNode* pMainNode     = NULL;
	TiXmlNode* pFunctionNode = NULL;
	TiXmlNode* pParamNode    = NULL;

	//获得根元素
	m_pRootElement = m_pTiXmlDocument->RootElement();

	if(NULL == m_pRootElement)
	{
		return false;
	}

	//获得工程名称
	sprintf_safe(pLuaProject->m_szProjectName, MAX_BUFF_50, "%s", (char* )m_pRootElement->Attribute("ProjectName"));

	//遍历Lua文件信息
	for(pMainNode = m_pRootElement->FirstChildElement();pMainNode;pMainNode = pMainNode->NextSiblingElement())
	{
		_LuaFile_Info obj_LuaFile_Info;

		//获得Lua文件信息
		int nMainType = pMainNode->Type();

		if(nMainType != TiXmlText::TINYXML_ELEMENT)
		{
			continue;
		}

		TiXmlElement* pMainElement = pMainNode->ToElement();
		sprintf_safe(obj_LuaFile_Info.m_szFileName, MAX_BUFF_50, "%s", pMainElement->Attribute("LuaFileName"));
		sprintf_safe(obj_LuaFile_Info.m_szDesc, MAX_BUFF_50, "%s", pMainElement->Attribute("desc"));
		pLuaProject->m_vecLuaFileList.push_back(obj_LuaFile_Info);

		//遍历下一级的函数信息
		for(pFunctionNode = pMainElement->FirstChildElement();pFunctionNode;pFunctionNode = pFunctionNode->NextSiblingElement())
		{
			_Function_Info obj_Function_Info;
			TiXmlElement* pFunctionElement = pFunctionNode->ToElement();

			sprintf_safe(obj_Function_Info.m_szFunctionName, MAX_BUFF_50, "%s", pFunctionElement->Attribute("FuncName"));
			sprintf_safe(obj_Function_Info.m_szDesc, MAX_BUFF_50, "%s", pFunctionElement->Attribute("desc"));
			
			int nFileIndex = pLuaProject->m_vecLuaFileList.size() - 1;
			pLuaProject->m_vecLuaFileList[nFileIndex].m_vecFunctionList.push_back(obj_Function_Info);

			//再遍历下一级参数
			for(pParamNode = pFunctionElement->FirstChildElement();pParamNode;pParamNode = pParamNode->NextSiblingElement())
			{
				_Function_Param obj_Function_Param;
				TiXmlElement* pParamElement = pParamNode->ToElement();

				sprintf_safe(obj_Function_Param.m_szParamName, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamName"));
				if(strcmp(pParamElement->Attribute("ParamType"), "in") == 0)
				{
					obj_Function_Param.m_emParamType = PARAM_TYPE_IN;
				}
				else
				{
					obj_Function_Param.m_emParamType = PARAM_TYPE_OUT;
				}

				if(strcmp(pParamElement->Attribute("ParamClass"), "int") == 0)
				{
					sprintf_safe(obj_Function_Param.m_szParamType, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamClass"));
					obj_Function_Param.m_emParamClass = PARAM_CLASS_INT;
				}
				else if(strcmp(pParamElement->Attribute("ParamClass"), "string") == 0)
				{
					sprintf_safe(obj_Function_Param.m_szParamType, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamClass"));
					obj_Function_Param.m_emParamClass = PARAM_CLASS_STRING;
				}
				else if(strcmp(pParamElement->Attribute("ParamClass"), "char*") == 0)
				{
					sprintf_safe(obj_Function_Param.m_szParamType, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamClass"));
					obj_Function_Param.m_emParamClass = PARAM_CLASS_STRING;
				}
				else
				{
					sprintf_safe(obj_Function_Param.m_szParamType, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamClass"));
					obj_Function_Param.m_emParamClass = PARAM_CLASS_VOID;
				}
				obj_Function_Param.m_nLength = atoi(pParamElement->Attribute("ParamLength"));

				int nFunctionIndex = pLuaProject->m_vecLuaFileList[nFileIndex].m_vecFunctionList.size() - 1;
				pLuaProject->m_vecLuaFileList[nFileIndex].m_vecFunctionList[nFunctionIndex].m_vecParamList.push_back(obj_Function_Param);
			}
		}
	}

	delete m_pTiXmlDocument;
	m_pTiXmlDocument = NULL;

	return true;
}

bool CXmlOpeation::Parse_XML_File_Cpp( const char* pName, _Project_Cpp_Info* pCppProject )
{
	if(NULL == pCppProject)
	{
		return false;
	}

	Close();
	m_pTiXmlDocument = new TiXmlDocument(pName);
	if(NULL == m_pTiXmlDocument)
	{
		return false;
	}

	if(false == m_pTiXmlDocument->LoadFile())
	{
		return false;
	}

	TiXmlNode* pMainNode     = NULL;
	TiXmlNode* pFunctionNode = NULL;
	TiXmlNode* pParamNode    = NULL;

	//获得根元素
	m_pRootElement = m_pTiXmlDocument->RootElement();

	if(NULL == m_pRootElement)
	{
		return false;
	}

	//获得工程名称
	sprintf_safe(pCppProject->m_szProjectName, MAX_BUFF_50, "%s", (char* )m_pRootElement->Attribute("ProjectName"));

	//遍历Lua文件信息
	for(pMainNode = m_pRootElement->FirstChildElement();pMainNode;pMainNode = pMainNode->NextSiblingElement())
	{
		_CppFile_Info obj_CppFile_Info;

		//获得Lua文件信息
		int nMainType = pMainNode->Type();

		if(nMainType != TiXmlText::TINYXML_ELEMENT)
		{
			continue;
		}

		TiXmlElement* pMainElement = pMainNode->ToElement();
		sprintf_safe(obj_CppFile_Info.m_szFileName, MAX_BUFF_50, "%s", pMainElement->Attribute("LuaFileName"));
		sprintf_safe(obj_CppFile_Info.m_szDesc, MAX_BUFF_50, "%s", pMainElement->Attribute("desc"));
		pCppProject->m_vecCppFileList.push_back(obj_CppFile_Info);

		//遍历下一级的函数信息
		for(pFunctionNode = pMainElement->FirstChildElement();pFunctionNode;pFunctionNode = pFunctionNode->NextSiblingElement())
		{
			_Function_Info obj_Function_Info;
			TiXmlElement* pFunctionElement = pFunctionNode->ToElement();

			sprintf_safe(obj_Function_Info.m_szFunctionName, MAX_BUFF_50, "%s", pFunctionElement->Attribute("FuncName"));
			sprintf_safe(obj_Function_Info.m_szDesc, MAX_BUFF_50, "%s", pFunctionElement->Attribute("desc"));

			int nFileIndex = pCppProject->m_vecCppFileList.size() - 1;
			pCppProject->m_vecCppFileList[nFileIndex].m_vecFunctionList.push_back(obj_Function_Info);

			//再遍历下一级参数
			for(pParamNode = pFunctionElement->FirstChildElement();pParamNode;pParamNode = pParamNode->NextSiblingElement())
			{
				_Function_Param obj_Function_Param;
				TiXmlElement* pParamElement = pParamNode->ToElement();

				sprintf_safe(obj_Function_Param.m_szParamName, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamName"));
				if(strcmp(pParamElement->Attribute("ParamType"), "in") == 0)
				{
					obj_Function_Param.m_emParamType = PARAM_TYPE_IN;
				}
				else
				{
					obj_Function_Param.m_emParamType = PARAM_TYPE_OUT;
				}
				if(strcmp(pParamElement->Attribute("ParamClass"), "int") == 0)
				{
					sprintf_safe(obj_Function_Param.m_szParamType, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamClass"));
					obj_Function_Param.m_emParamClass = PARAM_CLASS_INT;
				}
				else if(strcmp(pParamElement->Attribute("ParamClass"), "char") == 0)
				{
					sprintf_safe(obj_Function_Param.m_szParamType, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamClass"));
					obj_Function_Param.m_emParamClass = PARAM_CLASS_STRING;
				}
				else
				{
					sprintf_safe(obj_Function_Param.m_szParamType, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamClass"));
					obj_Function_Param.m_emParamClass = PARAM_CLASS_VOID;
				}

				obj_Function_Param.m_nLength = atoi(pParamElement->Attribute("ParamLength"));

				int nFunctionIndex = pCppProject->m_vecCppFileList[nFileIndex].m_vecFunctionList.size() - 1;
				pCppProject->m_vecCppFileList[nFileIndex].m_vecFunctionList[nFunctionIndex].m_vecParamList.push_back(obj_Function_Param);
			}
		}
	}

	delete m_pTiXmlDocument;
	m_pTiXmlDocument = NULL;

	return true;
}

bool CXmlOpeation::Parse_XML_File_Test( const char* pName, _Test_API* pTestAPI )
{
	if(NULL == pTestAPI)
	{
		return false;
	}

	Close();
	m_pTiXmlDocument = new TiXmlDocument(pName);
	if(NULL == m_pTiXmlDocument)
	{
		return false;
	}

	if(false == m_pTiXmlDocument->LoadFile())
	{
		return false;
	}

	TiXmlNode* pMainNode     = NULL;
	TiXmlNode* pParamNode    = NULL;

	//获得根元素
	m_pRootElement = m_pTiXmlDocument->RootElement();

	if(NULL == m_pRootElement)
	{
		return false;
	}

	//获得工程名称
	sprintf_safe(pTestAPI->m_szProjectName, MAX_BUFF_50, "%s", (char* )m_pRootElement->Attribute("ProjectName"));

	//分别获得测试函数名
	for(pMainNode = m_pRootElement->FirstChildElement();pMainNode;pMainNode = pMainNode->NextSiblingElement())
	{
		_Test_API_Lua_Function_Info obj_Test_API_Lua_Function_Info;

		TiXmlElement* pMainElement = pMainNode->ToElement();
		sprintf_safe(obj_Test_API_Lua_Function_Info.m_szName, MAX_BUFF_50, "%s", pMainElement->Attribute("Name"));
		sprintf_safe(obj_Test_API_Lua_Function_Info.m_szLuaFuncName, MAX_BUFF_50, "%s", pMainElement->Attribute("LuaFuncName"));
		obj_Test_API_Lua_Function_Info.m_nTestCount = atoi(pMainElement->Attribute("Count"));

		TiXmlElement* pFunctionElement = pMainNode->ToElement();

		//分别获得函数验证参数
		for(pParamNode = pFunctionElement->FirstChildElement();pParamNode;pParamNode = pParamNode->NextSiblingElement())
		{
			_Test_API_Param_Info obj_Test_API_Param_Info;
			EM_PARAM_TYPE emParamType;

			TiXmlElement* pParamElement = pParamNode->ToElement();

			if(strcmp(pParamElement->Attribute("ParamType"), "in") == 0)
			{
				emParamType = PARAM_TYPE_IN;
			}
			else
			{
				emParamType = PARAM_TYPE_OUT;
			}

			char szTemp[MAX_BUFF_50] = {'\0'};
			if(strcmp(pParamElement->Attribute("ParamClass"), "int") == 0)
			{
				obj_Test_API_Param_Info.m_emParamClass = PARAM_CLASS_INT;
			}
			else if(strcmp(pParamElement->Attribute("ParamClass"), "string") == 0)
			{
				obj_Test_API_Param_Info.m_emParamClass = PARAM_CLASS_STRING;
			}
			else
			{
				obj_Test_API_Param_Info.m_emParamClass = PARAM_CLASS_VOID;
			}

			sprintf_safe(obj_Test_API_Param_Info.m_szParamName, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamName"));
			sprintf_safe(obj_Test_API_Param_Info.m_szParamClass, MAX_BUFF_50, "%s", pParamElement->Attribute("ParamClass"));
			sprintf_safe(obj_Test_API_Param_Info.m_szValue, MAX_BUFF_50, "%s", pParamElement->Attribute("Value"));

			if(emParamType == PARAM_TYPE_IN)
			{
				obj_Test_API_Lua_Function_Info.m_vecTestLuaParamIn.push_back(obj_Test_API_Param_Info);
			}
			else
			{
				obj_Test_API_Lua_Function_Info.m_vecTestLuaParamOut.push_back(obj_Test_API_Param_Info);
			}
		}

		pTestAPI->m_vecTestAPIInfo.push_back(obj_Test_API_Lua_Function_Info);
	}

	delete m_pTiXmlDocument;
	m_pTiXmlDocument = NULL;

	return true;
}

bool CXmlOpeation::Parse_XML_File_BaseData( const char* pName, _Base_Data_Group* pBaseDataGroup )
{
	if(NULL == pBaseDataGroup)
	{
		return false;
	}

	Close();
	m_pTiXmlDocument = new TiXmlDocument(pName);
	if(NULL == m_pTiXmlDocument)
	{
		return false;
	}

	if(false == m_pTiXmlDocument->LoadFile())
	{
		return false;
	}

	TiXmlNode* pMainNode     = NULL;
	TiXmlNode* pParamNode    = NULL;

	//获得根元素
	m_pRootElement = m_pTiXmlDocument->RootElement();

	if(NULL == m_pRootElement)
	{
		return false;
	}

	//获得工程名称
	sprintf_safe(pBaseDataGroup->m_szProjectName, MAX_BUFF_50, "%s", (char* )m_pRootElement->Attribute("ProjectName"));

	//分别获得测试函数名
	for(pMainNode = m_pRootElement->FirstChildElement();pMainNode;pMainNode = pMainNode->NextSiblingElement())
	{
		TiXmlElement* pMainElement = pMainNode->ToElement();

		_Base_Data_Struct obj_Base_Data_Struct;
		sprintf_safe(obj_Base_Data_Struct.m_szStructName, MAX_BUFF_50, "%s", pMainElement->Value());

		TiXmlNode* pNode = NULL;

		for(pNode = pMainElement->FirstChildElement();pNode;pNode=pNode->NextSiblingElement())
		{
			_Base_Data_Info obj_Base_Data_Info;

			if(pNode->Type() == TiXmlText::TINYXML_ELEMENT)
			{
				sprintf_safe(obj_Base_Data_Info.m_szDataType, MAX_BUFF_50, "%s", pNode->ToElement()->GetText());
				sprintf_safe(obj_Base_Data_Info.m_szDataName, MAX_BUFF_50, "%s", pNode->ToElement()->Value());
				sprintf_safe(obj_Base_Data_Info.m_szDataDesc, MAX_BUFF_50, "%s", pNode->ToElement()->Attribute("desc"));
				if(pNode->ToElement()->Attribute("length") != NULL)
				{
					obj_Base_Data_Info.m_nDataLen = atoi(pNode->ToElement()->Attribute("length"));
				}
			}

			obj_Base_Data_Struct.m_vecBaseDataInfo.push_back(obj_Base_Data_Info);
		}

		pBaseDataGroup->m_vecBaseDataStruct.push_back(obj_Base_Data_Struct);
	}

	delete m_pTiXmlDocument;
	m_pTiXmlDocument = NULL;

	return true;
}
