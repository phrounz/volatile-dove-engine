This game was made by Francois Braud for the Ludum Dare 32 "An Unconventional Weapon".
See http://volatile-dove.itch.io/big-bertha-vs-alien-robots

All solutions use the same source files (in App_VS2008GL project)

App_VS2008GL
	Should work just by opening the solution with Visual Studio 2008 Express 
	and compiling+running it (with F5).
	Working directory is set to "WorkDir" (Included on Git. Contains the data files.)

App_VS2013_DX_WinStore
	Requires to compile+run the solution with Visual Studio 2013 Express For Windows, 
	then stop debugging after it crashes, 
	then run copy_work_dir_to_appx.bat, 
	then run the solution again.
	Working directory is set to "WorkDirStore" (Not included on Git).

App_Linux
	Ongoing work, not working yet