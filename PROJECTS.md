## Conventions for each example project

Each example project "Application_XXXXXX" may contain the following directories, which correspond to different targets/environments:

### App_VS2008GL or App_VS2008_OpenGL
 * Should work just by opening the solution with **Visual Studio 2008 Express** and 
   compiling+running it (with F5).
 * Working directory is set to "WorkDir" (Included on Git. Contains the data file.)

### App_VS2013_DX_Desktop
 * Should work just by opening the solution with **Visual Studio 2013 Express for Desktop** 
   and compiling+running it (with F5).
 * Working directory is set to "WorkDir" (Included on Git. Contains the data file.)
 * Note: if your Visual Studio is not in "C:\Program files (x86)\", you have to change
   the option C/C++->General->"Additional #using directory".

### App_VS2013_DX_WinStore
 * Requires to compile+run the solution with **Visual Studio 2013 Express For Windows**,
   * then stop debugging after it crashes,
   * then 
     * for "Application_EmptyProject": copy WorkDir\default_font.png into the working directory 
     * for other projects: run copy_work_dir_to_appx.sh,
   * then run the solution again.
 * Working directory is:
   * for "Application_EmptyProject":  the default one (App_VS2013_DX_Store\Debug[or Release]\App_VS2013_DX_Store\AppX)
   * for other projects: WorkDirStore\AppX\

### App_Linux (Experimental)
 * Should work on **Ubuntu 14.04 Desktop LTS 32 bits**.
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