# pesto
Pesto is a tool for converting HEIF images to PNGs.

Just clone the project, open in Visual Studio, and *pesto*, you've got yourself an easy image converter. I built pesto because I wanted an easy way to convert all the images I 
took on my vacation into something I could edit without having to upload a couple gigabytes to an online converter, or use a tool that may steal my pictures. Pesto is stupid. It only knows 
how to convert images. 

## Usage
```sh
./pesto.exe directory_of_heic other_dir dir_three file.heic more.heic put_as_many_dirs_or_files_as_you_want_for_args.heif
```
Use the `--outdir <dir>` flag to specify where it should dump its output, otherwise it'll pollute the input directories.

## Building
I wrote Pesto for Windows because that's where I do most of my photo editing, but it tries to exclusivly use C++20 and portable libraries that should compile without trouble on Linux/MacOS using 
Clang++ or G++. 

## Contributin
Feel free to contribute a CMakeList.txt or Makefile for all the Linux/Mac chads out there!
