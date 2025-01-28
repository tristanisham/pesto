# pesto
Pesto is a tool for converting HEIF images to PNGs.


## Usage
```sh
./pesto directory_of_heic other_dir dir_three file.heic more.heic put_as_many_dirs_or_files_as_you_want_for_args.heif
```
Use the `--outdir <dir>` flag to specify where it should dump its output, otherwise it'll pollute the input directories.

## Building
Pesto can be compiled on Windows, Linux, and MacOS. You must have [libheif](https://github.com/strukturag/libheif) installed on your system, or use the include `vcpkg.json` to install the dependency for Windows. All other dependencies are included in the `vendor` directory.

You can install the dependency on Windows using: `vcpkg install`


