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

###Contributions
All contributions are most welcome.
