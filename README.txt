
                        THE VOLATILE DOVE ENGINE

        See http://www.volatiledove.com/Engine for more informations.

- "Application_EmptyProject" contains several Visual Studio solutions, 
  for several platforms, using together the same source file MainClass.cpp, 
  and with working directories set to "WorkDir" (except ..._Store app, which 
  working directory is default one).

- "dependancy_libraries" are the dependancies, already compiled. 
  Note that OpenAL and GnuWin32 tools must currently be downloaded separately.

- "include" are the Volatile Dove Engine include files

- "shaders" are the default shaders files used in the application you build, 
  in HLSL (DirectX) and GLSL (OpenGL) languages

- "source" are the Volatile Dove Engine source files

- "Doxyfile" is the Doxygen file, which requires Doxygen 
  (http://www.stack.nl/~dimitri/doxygen/), 
  This will automatically generates the documentation in the directory "html". 
  It is currently set up to browse the "include" directory only.

- "LICENSE.txt" describe the engine license. Read it before use!
