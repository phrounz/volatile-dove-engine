This is a simple tutorial program.
See http://www.volatiledove.com/Engine/tutorial.html
All solutions use the same source file "MainClass.cpp".

App_VS2008GL
	Should work just by opening the solution with Visual Studio 2008 Express 
	and compiling+running it (with F5).
	Working directory is set to "WorkDir" (Included on Git. Contains the data file.)

App_VS2013_DX_Desktop
	Should work just by opening the solution with Visual Studio 2013 Express for Desktop
	and compiling+running it (with F5).
	Working directory is set to "WorkDir" (Included on Git. Contains the data file.)

App_VS2013_DX_WinStore
	Requires to compile+run the solution with Visual Studio 2013 Express For Windows, 
	then stop debugging after it crashes, 
	then copy WorkDir\default_font.png into the working directory, 
	then run the solution again.
	Working directory is the default one:
		App_VS2013_DX_Store\Debug[or Release]\App_VS2013_DX_Store\AppX
