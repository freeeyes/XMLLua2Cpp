#ifndef _XMLOPRATION_H
#define _XMLOPRATION_H

#include "tinyxml.h"
#include "tinystr.h"

#include "Common.h"

using namespace std;

//将XML转换为数据结构
//add by freeeyes


class CXmlOpeation
{
public:
  CXmlOpeation(void);
  ~CXmlOpeation(void);

  bool Init(const char* pFileName);
  bool Init_String(const char* pXMLText);

  char* GetData(const char* pName, const char* pAttrName);
  char* GetData(const char* pName, const char* pAttrName, TiXmlElement*& pNextTiXmlElement);

  char* GetData_Text(const char* pName);
  char* GetData_Text(const char* pName, TiXmlElement*& pNextTiXmlElement);

  void Close();

  bool Parse_XML_File_Lua(const char* pName, _Project_Lua_Info* pLuaProject);
  bool Parse_XML_File_Cpp(const char* pName, _Project_Cpp_Info* pCppProject);
  bool Parse_XML_File_Test(const char* pName, _Test_API* pTestAPI);
  bool Parse_XML_File_BaseData(const char* pName, _Base_Data_Group* pBaseDataGroup);

private:
  TiXmlDocument* m_pTiXmlDocument;
  TiXmlElement*  m_pRootElement;
};
#endif
