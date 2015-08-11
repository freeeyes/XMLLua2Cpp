#ifndef _LUAFILECREATE_H
#define _LUAFILECREATE_H

#include "Common.h"
#include "XmlOpeation.h"
#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h> 
#endif

bool Read_Lua_File_XML(const char* pXMLName, _Project_Lua_Info* pLuaProject);

bool Creat_Lua_Files(_Project_Lua_Info* pLuaProject);

#endif