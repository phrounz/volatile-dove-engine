## Conventions for each example project

Each example project "Application_XXXXXX" may contain the following directories. Thoses starting with App_... correspond to different targets/environments:

### code/
 * Contains your common C++ source code, used by all targets. The entry point of the application is MainClass.cpp.

### setup.ini
 * Contains the global configuration file of the project. You shall run [generate_project.pl](./README.md#generate_projectpl)    again to take into consideration any modification in this file.
  * Note that in this file, all the ```windows_*``` values (and ```visual_studio_app_guid```) must be separated by commas (if there are several values) and also they can have special optional syntaxes:
    * ```ONLY:the-directory:the-value```
    * ```EXCEPT:the-directory:the-value```
    * ```ONLY:the-directory:the-arch:the-value``` (``the-arch`` can be 64 or 32 or ARM)
    * ```EXCEPT:the-directory:the-arch:the-value``` (``the-arch`` can be 64 or 32 or ARM)
    * ```ONLY_ARCH:the-arch:the-value``` (``the-arch`` can be 64 or 32 or ARM)
    * ```EXCEPT_ARCH:the-arch:the-value``` (``the-arch`` can be 64 or 32 or ARM)
  * For example the line below would apply ```COMPILE_WITH_TEST``` on all Visual Studio projects of the project, ```_CRT_SECURE_NO_WARNINGS``` applies only to App_VS2013_DX_Desktop for 64bit, and ```COMPILE_WITH_DX``` applies on all Visual Studio projects of the project except App_VS2013_DX_Desktop.
```
   windows_additional_defines=ONLY:App_VS2013_DX_Desktop:64:_CRT_SECURE_NO_WARNINGS,COMPILE_WITH_TEST,EXCEPT:App_VS2013_DX_Desktop:COMPILE_WITH_DX
```

### App_VS2008_OpenGL
 * Should work just by opening the solution with **Visual Studio 2008 Express** and 
   compiling+running it (with F5).
 * Working directory is set to "WorkDir" (Included on Git. Contains the data file.)
 * When providing the application to a client on another machine, the [redistribuable package for VS2008](https://www.microsoft.com/en-us/download/details.aspx?id=29) or [VS2008 SP1] (https://www.microsoft.com/en-us/download/details.aspx?id=5582) should be installed to ensure it works.

### App_VS2013_DX_Desktop
 * Should work just by opening the solution with **Visual Studio 2013 Express for Desktop** 
   and compiling+running it (with F5).
 * Working directory is set to "WorkDir" (Included on Git. Contains the data file.)
 * Note: if your Visual Studio is not in "C:\Program files (x86)\", you have to change
   the option C/C++->General->"Additional #using directory".
 * When providing the application to a client on another machine, the [redistribuable package for VS2013](https://www.microsoft.com/en-us/download/details.aspx?id=40784) should be installed to ensure it works.

### App_VS2013_DX_WinStore
 * Requires to compile+run the solution with **Visual Studio 2013 Express For Windows**,
   * then stop debugging after it crashes,
   * then run copy_work_dir_to_appx.sh,
   * then run the solution again.
 * Working directory is: WorkDirStore\AppX\
   
### App_Linux
 * Should work on **Ubuntu 14.04 Desktop LTS 32 bits** and **SteamOS 2.0**.
 * Install the missing packages listed in common/Linux/install_dependancies.sh, compile with "sh compile.sh", and
   run the executable in ./WorkDir.
 * Or said otherwise, open the terminal and type the commands:
```
   cd [your-download-or-git-directory-here]/volatile-dove-engine/
   cd common/Linux/ && sudo sh install_dependancies.sh && cd ../..
   cd Application_[...]/App_Linux/
   sh compile.sh
   cd ../WorkDir/
   ./app
```
 * Working directory is set to "WorkDir" (Included on Git. Contains the data files.)
 * You can pass Makefile parameters to "sh compile.sh", for example "sh compile.sh clean" cleanups the output files
 * Note to beginners: to develop under Linux, I would suggest one of these editors:
   * Visual Studio Code for Linux (http://code.visualstudio.com)
   * Atom (http://atom.io)
   * Kate (part of KDE environment)

### App_VS2008_SDL (Highly experimental)
 * Open the solution with **Visual Studio 2008 Express** and compile (with F5).
 * Add then the SDL.dll in "dependancy_libraries/lib/SDL2_Win32_Release" into "WorkDir"
 * Working directory is set to "WorkDir" (Included on Git. Contains the data file.)

### App_JS_Emscripten (Highly Experimental)
 * Requires Perl.
 * On Windows please:
   * Install [Emscripten SDK 1.35.0](https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-1.35.0-full-64bit.exe)
   * Install http://strawberryperl.com/
   * Run compile.bat in Application_[...]/App_JS_Emscripten/
   * Open output.html in your browser
 * On Linux:
```
	cd common/JS_Emscripten/ && get_emsdk.sh && cd ../..
	cd Application_[...]/App_JS_Emscripten/ && sh compile.sh
	# Open output.html in your browser
```
