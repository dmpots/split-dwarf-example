# Overview
This repo is a simple example of how to generate and examine [split-dwarf][1]
debug info.

The example program uses two source files `main.cc` and `foo.cc`.  The `main`
function simply calls the `foo` function in `foo.cc`.  The `foo` function uses a
number of c++ standard library types so that it will generate some interesting
debug info.

The example assumes you have a clang++, lldb, llvm-dwarfdump, and llvm-dwp
executables in your path.

# Tutorial

First build the program with split dwarf contained in the dwo files.
```
$ ninja
[3/3] Linking a.out
```

## Debugging with dwo files
Then load the program into lldb and dump the split dwarf info.

```
$ lldb a.out
(lldb) target create "a.out"
(lldb) target modules dump separate-debug-info
Symbol file: a.out
Type: "dwo"
Dwo ID             Err Dwo Path
------------------ --- -----------------------------------------
0xf9904caec8f2c0d5     main.dwo
0xbcc16eddceb662b2     foo.dwo
```

Now delete the dwo files and try again. We get an error that the dwo files are
missing.

```
$ rm *.dwo
$ lldb a.out
(lldb) target create "a.out"
warning: (x86_64) a.out unable to locate separate debug file (dwo, dwp). Debugging will be degraded.

(lldb) target modules dump separate-debug-info
Symbol file: a.out
Type: "dwo"
Dwo ID             Err Dwo Path
------------------ --- -----------------------------------------
0xf9904caec8f2c0d5 E   unable to locate .dwo debug file "main.dwo" for skeleton DIE 0x0000000000000014
0xbcc16eddceb662b2 E   unable to locate .dwo debug file "foo.dwo" for skeleton DIE 0x000000000000003c
```

## Debugging with dwp files
Now build the dwp file and delete the dwo files.  We don't actually need to
delete the dwo files because lldb will default to the dwp file for the symbols,
but doing so make it extra clear that the dwp file is being used.

```
$ ninja a.dwp
[3/3] Generating DWP file a.dwp
$ rm *.dwo
$ lldb a.out
(lldb) target create "a.out"
(lldb) target modules dump separate-debug-info
Symbol file: a.out
Type: "dwo"
Dwo ID             Err Dwo Path
------------------ --- -----------------------------------------
0xf9904caec8f2c0d5     a.dwp(main.dwo)
0xbcc16eddceb662b2     a.dwp(foo.dwo)

```

## Examining the debug info

The debug info in the `a.out` executable will have `DW_UT_skeleton` units as the
compile unit type. The `DWO_id` is used to locate the corresponding dwo file
that contains the actual debug info.

Dump the debug info in the `a.out` executable and note the DWO_ids.

```
$ llvm-dwarfdump a.out | grep "Compile Unit"
0x00000000: Compile Unit: length = 0x00000024, format = DWARF32, version = 0x0005, unit_type = DW_UT_skeleton, abbr_offset = 0x0000, addr_size = 0x08, DWO_id = 0xf9904caec8f2c0d5 (next unit at 0x00000028)
0x00000028: Compile Unit: length = 0x0000002c, format = DWARF32, version = 0x0005, unit_type = DW_UT_skeleton, abbr_offset = 0x0017, addr_size = 0x08, DWO_id = 0xbcc16eddceb662b2 (next unit at 0x00000058)
```

Now dump the debug info in the `a.dwp` file and note the matching DWO_ids.
```
$ llvm-dwarfdump a.dwp | grep "Compile Unit"
0x00000000: Compile Unit: length = 0x00016624, format = DWARF32, version = 0x0005, unit_type = DW_UT_split_compile, abbr_offset = 0x0000, addr_size = 0x08, DWO_id = 0xbcc16eddceb662b2 (next unit at 0x00016628)
0x00016628: Compile Unit: length = 0x0000002a, format = DWARF32, version = 0x0005, unit_type = DW_UT_split_compile, abbr_offset = 0x0a0b, addr_size = 0x08, DWO_id = 0xf9904caec8f2c0d5 (next unit at 0x00016656)
```

[1]: https://gcc.gnu.org/wiki/DebugFission
