# findrep
A tool to easily find and replace from files

This is just ```grep``` and ``sed`` put together !!

###Usage
```
$ findrep [-h] <FIND-PATTERN> <REPLACE-WITH> [LOCATION]
```
###Examples
```
#replace all instances of 'foo' with 'bar' in the current directory

$ findrep foo bar
```
```
#replace all instances of 'foo' with 'bar' in the directory 'my-dir'

$ findrep foo bar my-dir/
```

###Building from source
```
$ make
```
* move `findrep` to any location in your `$PATH`. eg: `/usr/local/bin`

###TODO
* Enhance `escape_spl_chars()` to give correct input for sed (and grep).
* get `main_sed()` to always propagate correct return code.

###Contributions
All contributions are most welcome.
