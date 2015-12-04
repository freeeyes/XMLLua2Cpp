# XMLLua2Cpp
Generate Lua/C++ code and test code through XML automatically

本来想用英文来写此文档，但是实在是英文能力有限，先用中文吧，以后可以添加。

##功能：
* 此工具是为了规范化Lua和Cpp之间的代码关系。
* 根据指定的XML格式自动生成Lua文件框架和基于Lua的CAPI代码框架。
* 开发者可以通过设计XML来管理和生成自己的Lua代码关系。
* XML即可用于文档，也可以用于设计。

##设计目的：
* 由于自己的工程也用到了不少Lua的地方，随着工程的日益复杂。
* Lua和CAPI之间交互关系也变得复杂。
* 为了减低程序员开发和维护的成本，同时自动建立维护文档。
* 基于此目的，开发了这个工具。
* 通过XML自动生成Lua文件的自动生成。
* 并填充Lua文件内部的函数以及出入参。
* 通过XML自动生成Lua C API的文件（包含h和cpp）。
* 让开发者只要填充相关代码即可（做填空题）。
* 同时提供生成简易的代码测试工程，用以可以测试整体的工程效率。
* 程序员不必在关注如何编写胶水代码，而是注重出入参，函数的逻辑代码。

##简易说明:
### LuaFileInfo.xml
* 这个文件是用来生成所有的Lua文件。
* 你可以通过它来设计你的Lua文件接口函数以及结构。
* 支持多个Lua文件的声明
* ProjectName="autotest" 是工程名称，你可以任意起一个名字。
* XMLLua2Cpp会根据这个工程名称，自动建立一个目录以存放生成好的所有代码。
* LuaFile节点你可以设计多个，工具会自动根据这个文件属性生成不同的文件。
* LuaFunc节点同样允许多个。Param节点也是
* Param节点中的ParamType节点包含两种，一个是"in"入参，一个是"out"出参。
* ParamClass属性目前支持三种类型，int(数字类型),string(字符串类型)和void(指针类型)

### CppFileInfo.xml
* 这个文件是用来生成所有的C API。
* 也就是Lua可以调用的C API接口函数。
* 基本参数同上。
* 说明一点的是，ParamClass属性支持自定义数据结构。你可以在这里设计自己的数据结构。
* 例如：ParamClass="_PlayerInfo"。
* 意思就是 这个数据声明为_PlayerInfo类型，在自动生成代码的时候会以此问标准。
* 请在生成代码后，自动引用这个类型所在的头文件。