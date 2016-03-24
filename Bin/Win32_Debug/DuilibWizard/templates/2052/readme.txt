[!if WIN_APP]
========================================================================
    Duilib 界面库应用程序：[!output PROJECT_NAME] 项目概述
========================================================================

Duilib界面库应用程序向导已为您创建了此 [!output PROJECT_NAME] 界面库应用程序。
[!endif]

[!if PRE_COMPILED_HEADER]
本文件概要介绍组成 [!output PROJECT_NAME] 应用程序的每个文件的内容。
[!else]
没有为此项目创建源文件。
[!endif]


[!output PROJECT_NAME].vcproj
    这是使用Duilib界面库应用程序向导生成的 VC++ 项目的主项目文件，
    其中包含生成该文件的 Visual C++ 的版本信息，以及有关使用应用程序向导选择的平台、配置和项目功能的信息。

[!if !EMPTY_PROJECT]
[!output PROJECT_NAME].cpp
    这是主应用程序源文件。
[!endif]

[!if WIN_APP]

/////////////////////////////////////////////////////////////////////////////
应用程序向导创建了下列资源：

[!output RC_FILE_NAME]
这是程序使用的所有 Microsoft Windows 资源的列表。它包括 RES 子目录中存储的图标、位图和光标。
此文件可以直接在 Microsoft Visual C++ 中进行编辑。

Resource.h
    这是标准头文件，可用于定义新的资源 ID。
    Microsoft Visual C++ 将读取并更新此文件。
[!if WIN_APP]

[!output PROJECT_NAME].ico
    这是一个图标文件，可用作应用程序的图标 (32x32)。
    此图标包括在主资源文件 [!output RC_FILE_NAME] 中。

[!endif]

/////////////////////////////////////////////////////////////////////////////
其他标准文件：

StdAfx.h, StdAfx.cpp
    这些文件用于生成名为 [!output PROJECT_NAME].pch 的预编译头 (PCH) 文件和名为 StdAfx.obj 的预编译类型文件。

[!endif]
[!if PRE_COMPILED_HEADER]

/////////////////////////////////////////////////////////////////////////////

StdAfx.h, StdAfx.cpp
    这些文件用于生成名为 [!output PROJECT_NAME].pch 的预编译头 (PCH) 文件和名为 StdAfx.obj 的预编译类型文件。

[!endif]

/////////////////////////////////////////////////////////////////////////////
其他注释：

应用程序向导使用“TODO:”注释来指示应添加或自定义的源代码部分。

/////////////////////////////////////////////////////////////////////////////