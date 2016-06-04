Partmod
=======

Partmod is a cross-platform partition table editor. Main features are:

 - Create/delete primary and extended partitions 
 - Create/delete GPT partitions
 - Modify flat disk images just like physical disks
 - Create partition table backups

project is not anywhere near the completion

Project structure
--------------------
Project consists of three main parts:

 - **diskio**  - a very simple disk I/O library to create platform-idependent disk I/O interface for Partmod
 - **Partmod** - the core
 - **wxGUI**   - Partmod gaphical user interface written with wxWidgets

Compilation
---------------
 - Codeblocks
 - g++ 4.8.4 or higher, although any modern C++ compiler should do the work
 - wxWidgets 3.0.2 or higher. On Linux, use`wx-config` to check that these configurations  exist:

- ``wx-config --static=no --unicode=yes --debug=no --libs``
-  `wx-config --static=no --unicode=yes --debug=no --libs`

On windows, `wx` variable must be set inside Codeblocks, and optionally `wx64` for 64-bit build if the default compiler is 32 bit

Bugs
------
- Windows disk I/O. Something happens with data pointers accross dll boundaries that makes data inaccessible for writing, so the data must be copied first inside dll, which theoretically doubles memory consumption when data is written to disk. Practically the effect is neligible
- Incomplete GUID partition table support for tables created by windows.
- Untested native 4k sector support. Theoretically all sector 512+ sector sizes are supported.
