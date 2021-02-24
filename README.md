# fstl

`fstl` is a very fast [.stl file](http://en.wikipedia.org/wiki/STL_\(file_format\)) viewer.

It was originally written by [Matt Keeter](https://mattkeeter.com),
and is now primarily maintained by [@DeveloperPaul123](https://github.com/DeveloperPaul123).

It is designed to quickly load and render very high-polygon models;
showing 2 million triangles at 60+ FPS on a mid-range laptop.

For more details, see the [project page](http://mattkeeter.com/projects/fstl).

Issues and minor pull requests are welcome;
the project is under 1K lines of code and should be fairly approachable.

## Screenshot

![Eiffel tower](http://mattkeeter.com/projects/fstl/eiffel.png)
(credit to [Pranav Panchal](https://grabcad.com/pranav.panchal))

## Setting `fstl` as the Default STL Viewer

### Windows

1. Right-click an STL file
2. Select `Open With` >>> `Choose another app`
3. Select `More Apps` and `Look for another app on this PC`
4. Enter the path to the `fstl` EXE file

### MacOS

1. Ctrl+click an STL file
2. Select `Get Info`
3. Navigate to the `Open with` section
4. Select `fstl` in the dropdown
5. Click `Change All`

### Linux

If `mimeopen` is available on your system, it can be used to set `fstl` as the default viewer for STL files.
Run the following in your terminal:

```bash
# replace example.stl with an actual file
mimeopen -d example.stl
```

The following output will result:

```
Please choose a default application for files of type model/stl

	1) Other...

use application #
```

Select the `Other` option and type `fstl` as the desired command to open STL files.
This will now become the system default, even when opening files from the file manager.

## Building

The only dependency for `fstl` is [Qt 5](https://www.qt.io),
plus [`cmake`](https://cmake.org/) for building.

### macOS

Install Qt from their website or [Homebrew](brew.sh).

Install `cmake` through Homebrew or equivalent.

Then, run through the following set of commands in a shell:

```
git clone https://github.com/fstl-app/fstl
cd fstl
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt/5.15.0/ ..
make -j8
./fstl.app/Contents/MacOS/fstl
```

You may need to edit the Qt path depending on your installation.

To package a standalone app, go to the app directory and run `package.sh`

```
cd ../app
./package.sh
```

This should produce two new files in the root directory:
- `fstl.app` is a standalone application that can be copied to `/Applications`
- `fstl.dmg` is a disk image that can be given to a friend

### Linux

Install Qt with your distro's package manager (required libraries are Core, Gui,
Widgets and OpenGL, e.g. `qt5-default` and `libqt5opengl5-dev` on Debian).

You can build fstl with CMake:
```
git clone https://github.com/fstl-app/fstl
cd fstl
mkdir build
cd build
cmake ..
make -j8
./fstl
```

--------------------------------------------------------------------------------

# License

Copyright (c) 2014-2017 Matthew Keeter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
