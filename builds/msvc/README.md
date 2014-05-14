The `CTP_Nov2013` compiler is required due to the use of **C++11/14** features. You can [download the compiler add-on here](http://www.microsoft.com/en-us/download/details.aspx?id=41151).

There are a number of NuGet packages required to compile. Several inject source code into the build, others are required for resolving references and linking.

I currently have the NuGet references to my local (relative) NuGet repo embedded in the project files.

If you don't have the packages in that location NuGet will produce errors.