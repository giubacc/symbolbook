# symbolbook
symbolbook is a Qt based gui tool for searching symbols in static libraries (.lib).

The main purpose of this tool is to provide a relative easy to use mechanism to find a linkable symbol inside a set of static libraries.
If your organization uses a lot of in house made libraries with a lot of undocumented symbols, a tool like this could be pretty useful and it could help you not wasting your time.

This program is designed to work under Microsoft Windows with a Microsoft Visual Studio installed.

Microsoft Visual Studio is required because of the dumpbin tool, used to search public symbols inside a static library.
You are required to set the path of the dumpbin tool inside a text file named dumpbinloc that you must put inside the working directory of symbolbook, for example, the content of this file could be: 

C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin\amd64\

according to your Visual Studio installation path.
