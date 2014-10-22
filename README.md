DebugThingsIntegratedCLRExample
===============================

Code example for SSRS HAP blog post. In order to reliably recreate the HostAccessProtectionException without SSRS I needed to recreate a integrated CLR hosting environment. This code is based off of [ADMHost](http://www.microsoft.com/en-us/download/details.aspx?id=7325).

You can find out more about ADMHost here:
http://blogs.msdn.com/b/shawnfa/archive/2006/01/05/509440.aspx
http://blogs.msdn.com/b/shawnfa/archive/2005/10/06/478009.aspx

You can find out more about HostAccessProtection here:
http://blogs.msdn.com/b/shawnfa/archive/2005/10/12/480186.aspx

#How to build!
You **SHOULD** be able to just build this with VisualStudio 2013. It's old enough to be compiled in VS2008. 

#Issues
If your having issues with the build step you can manually export the TLB that the ADM host is looking for.

If you try to debug this application and it fails make sure it's building properly. Also make sure you have not changed any of the underlying files.

#Debugging
I use WinDbg in my examples and this is known to work. However if you want to use it in Visual Studio you can try. However, I didn't have any success.
