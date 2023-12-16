

**Introduction**

RemoveDuplicateFiles is a tool designed to remove duplicate files in a specified directory. It creates domains out of files which are then filtred using approximation of set cover problem, which is famous NP problem.

To install RemoveDuplicateFiles, clone this repository and compile the source code:

```
git clone https://github.com/cernajs/RemoveDuplicateFiles.git
cd RemoveDuplicateFiles
cmake .
```

**Usage**

On command line you can use following filtres:

For selecting only files containing specific string in name
```
--name-contains ___ 
```
For selecting files with specific extension

```
--ends-with ___
```
For excluding hidden files

```
--no-hidden-files
```

You can combine this how you want eg.
```
--name-contains file --ends-with .txt --no-hidden-files
```

After this you just run code on specific directory and it outputs which files cover all context in given directory


**License**

This project is licensed under the MIT License.
