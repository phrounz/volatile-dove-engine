
## The Volatile Dove Engine 

... is a lightweight video game engine. See http://www.volatiledove.com/Engine for more informations.

See [LICENSE.txt](./LICENSE.txt) to know the engine license. Read it before use!

### Engine source code and dependancies
   
 * [dependancy_libraries](./dependancy_libraries) are all the dependancies for Windows
   (includes, lib and dll already compiled)

 * [include](./include) are the Volatile Dove Engine "include" files

 * [shaders](./shaders) are the default source shaders files used in the application
   you build, in HLSL (DirectX) and GLSL (OpenGL) languages.

 * [source](./source) are the Volatile Dove Engine "source" files
 
 * [common](./common) contains files common to several projects (Application_... directories), 
   for example compilation procedures for a given target platform common to several projects.

 * [Doxyfile](./Doxyfile) is the Doxygen file, which requires 
  [Doxygen](http://www.stack.nl/~dimitri/doxygen/),
  This will automatically generates the documentation in the directory "html".
  It is currently set up to browse the "include" directory only.

 * [generate_project.pl](./generate_project.pl) is a Perl script which helps you to create a new empty project.
	* On Linux, Perl should be already installed (if not, type: `apt-get install perl` or `yum install perl`)
    * On Windows, install http://strawberryperl.com/ . When Perl is installed, just double-click on the .pl file.

### Example projects

See [PROJECTS.md](./PROJECTS.md) for description of how each example project is organized.

 * [Application_EmptyProject](./Application_EmptyProject) is a small example tutorial program.

 * [Application_AnUnconventionalWeapon](./Application_AnUnconventionalWeapon) contains the stuff 
   required to make and run the game "Big Bertha vs alien robots!". 

 * [Application_LD34_TwoButtons_Growing](./Application_LD34_TwoButtons_Growing) contains the stuff 
   required to make and run the game "The Mad Circle For The Glorious Dice Tower". 
