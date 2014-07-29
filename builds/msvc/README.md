The `CTP_Nov2013` compiler is required due to the use of **C++11/14** features. You can [download the compiler add-on here](http://www.microsoft.com/en-us/download/details.aspx?id=41151).

There are a number of NuGet packages required to compile. Several inject source code into the build, others are required for resolving references and linking. I currently have the NuGet references to my local (relative) NuGet repo embedded in the project files. If you don't have the packages in that location NuGet will produce errors.

I have packaged most of the necessary packages [here](http://www.nuget.org/profiles/evoskuil/) (you won't need all of these). You will also need various [Boost packages](http://sergey-shandar.blogspot.com/2013/08/boost-on-nugetorg.html) and [LevelDB](http://www.nuget.org/packages/LevelDB/), also available via NuGet.

It's easy to identify the necessary packages, just inspect the project's `packages.config` file.
