# fstl

`fstl` is a viewer for [.stl files](http://en.wikipedia.org/wiki/STL_\(file_format\)).

It is designed to quickly load and render very high-polygon models;
showing 2 million triangles at 60+ FPS on a mid-range laptop.

For more details, see the [project page](http://mattkeeter.com/projects/fstl).

Issues and pull requests are welcome;
the project is under 1K lines of code and should be fairly approachable.

## Screenshot
![Eiffel tower](http://mattkeeter.com/projects/fstl/eiffel.png)
(credit to [Pranav Panchal](https://grabcad.com/pranav.panchal))

## Building

You need to have Qt installed to build the application.

### macOS

Install [Qt](https://www.qt.io/developers/) and add the Qt binaries to your path.

To build, first create the build directory and enter it:

```
mkdir build
cd build
```

Then run `qmake`:

```
qmake ../qt/fstl.pro
```

Now you can build a binary with `make`:

```
make
```

If you got no errors, you can run the binary:

```
./fstl.app/Contents/MacOS/fstl
```

To build the app, go to the app directory and run the `package.sh`

```
cd ../app
./package.sh
```

Now you have a brand new fstl.app in the project root directory, which you put to /Applications/

--------------------------------------------------------------------------------

# License

Copyright (c) 2014 Matthew Keeter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.