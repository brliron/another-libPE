# Another libPE

Another library to view / edit PE files.
If you don't know, the PE (Portable Executable) format is the one used in every native exe file since Windows 3.1. I don't know about the managed (.NET) ones.

This library supports only reading 32-bits PE files. I would like to implement writing, 64-bits files support, and parse some resource types.
I started working on a function to load the executable in memory and execute it, but I doubt I will finish that.

This library had 2 goals:
- Learning the PE format. I think writing a parser is a good way to understand a file format.
- Writing a GUI to quickly inspect / edit some parts of a PE file.

I already reached both goals. I wrote enough code to get a quite good understanding of the PE format, and I found [CFF Explorer](https://ntcore.com/?page_id=388), which is good and does almost everything I need.
I don't have much interest to working on it anymore. And if I ever do, I will probably break the API when doing 64-bits support. Which means you might want to ignore this library and look for another one.

Look at main.cpp for some examples.
