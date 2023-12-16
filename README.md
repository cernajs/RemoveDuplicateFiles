

**Introduction**

RemoveDuplicateFiles is a tool designed to remove duplicate files in a specified directory. It creates domains out of files which are then filtred using approximation of set cover problem, which is famous NP problem.

To install RemoveDuplicateFiles, clone this repository and compile the source code:

```
git clone https://github.com/cernajs/RemoveDuplicateFiles.git
cd RemoveDuplicateFiles
cmake .
```

**Usage**

In main you can add given filtres:

```
  CompositeFilterPolicy compositeFilter;
  compositeFilter.addFilter(std::make_unique<NameContainsFilterPolicy>(""));
  compositeFilter.addFilter(std::make_unique<ExtensionFilterPolicy>(".css"));
  compositeFilter.addFilter(std::make_unique<NoHiddenFilesFilterPolicy>());
```

After this you just run code on specific directory and it outputs which files cover all context in given directory


**License**

This project is licensed under the MIT License.
