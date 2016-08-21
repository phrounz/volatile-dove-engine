
## The Volatile Dove Engine 

... is a video game engine, and includes Makefiles, Visual Studio projects, etc and a script to manage your multi-plaftorm video games projects easily. See http://www.volatiledove.com/Engine for more informations.

See [LICENSE.txt](./LICENSE.txt) to know the engine license. Read it before use!

### Engine source code and dependancies
   
 * [include](./include) are the Volatile Dove Engine "include" files

 * [shaders](./shaders) are the default source shaders files used in the application
   you build, in HLSL (DirectX) and GLSL (OpenGL) languages.

 * [source](./source) are the Volatile Dove Engine "source" files
 
 * [common](./common) contains files common to several projects (Application_... directories), 
   for example compilation procedures for a given target platform common to several projects
   and stuff used by generate_project.pl. This include all the dependancies for Windows (includes, lib and dll already compiled) in [common/dependancy_libraries](./common/dependancy_libraries).


 * [Doxyfile](./Doxyfile) is the Doxygen file, which requires 
  [Doxygen](http://www.stack.nl/~dimitri/doxygen/),
  This will automatically generates the documentation in the directory "html".
  It is currently set up to browse the "include" directory only.

### generate_project.pl

The script [generate_project.pl](./generate_project.pl) is a Perl script which helps you to create a new empty project and maintain existing ones.
 * Installation:
  * On Linux, Perl should be already installed (if not, type: `apt-get install perl` or `yum install perl`)
  * On Windows, install http://strawberryperl.com/ . When Perl is installed, just double-click on the .pl file.
 * How it works: This script is used to:
  * create a new project. A new directory with the name of your project will then be been created with the file [setup.ini](./PROJECTS.md#setupini) in it and the source file code/MainClass.cpp. You can then [take notice](PROJECTS.md#app_vs2008_opengl) of the particularities of each target platforms within this project.
  * update an existing project. This can be useful if you pull changes from GitHub and new sources files are available, for example, or if there are new cpp/h files to take into consideration in code/, or to take into consideration the configuration you made manually in [setup.ini](./PROJECTS.md#setupini). If the .sln,.vcproj, and any other file which was created by the script is going to change, it will ask for confirmation in order to choose between the new generated version and the local version (note: for some items like code/MainClass.cpp, WorkDir/data/default_font.png, copy_work_dir_to_appx.bat, and files in App_VS2013_DX_Store/Assets/ , local version will always be kept if existing).

### Example projects

See [PROJECTS.md](./PROJECTS.md) for description of how each example project is organized.

 * [Application_EmptyProject](./Application_EmptyProject) is a small example tutorial program.

 * [Application_AnUnconventionalWeapon](./Application_AnUnconventionalWeapon) contains the stuff 
   required to make and run the game "Big Bertha vs alien robots!". 

 * [Application_LD34_TwoButtons_Growing](./Application_LD34_TwoButtons_Growing) contains the stuff 
   required to make and run the game "The Mad Circle For The Glorious Dice Tower". 
