# fstl

`fstl` is the fastest [.stl file](http://en.wikipedia.org/wiki/STL_\(file_format\)) viewer.

It is designed to quickly load and render very high-polygon models;
showing 2 million triangles at 60+ FPS on a mid-range laptop.

For more details, see the [project page](http://mattkeeter.com/projects/fstl).

Issues and minor pull requests are welcome;
the project is under 1K lines of code and should be fairly approachable.

## Screenshot
![Eiffel tower](http://mattkeeter.com/projects/fstl/eiffel.png)  
(credit to [Pranav Panchal](https://grabcad.com/pranav.panchal))

## Building

The only dependency for `fstl` is [Qt](https://www.qt.io).

### macOS

Install Qt from their website or [Homebrew](brew.sh),
making sure `qmake` is on your shell's path.

Then, run through the following set of commands in a shell:
```
git clone https://github.com/mkeeter/fstl
cd fstl
mkdir build
cd build
qmake ../qt/fstl.pro
make -j8
./fstl.app/Contents/MacOS/fstl
```

To package a standalone app, go to the app directory and run `package.sh`

```
cd ../app
./package.sh
```

This should produce two new files in the root directory:
- `fstl.app` is a standalone application that can be copied to `/Applications`
- `fstl.dmg` is a disk image that can be given to a friend

--------------------------------------------------------------------------------

# License

Copyright (c) 2014 Matthew Keeter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
