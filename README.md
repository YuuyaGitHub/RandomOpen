# Random Open
A simple Windows tool that opens one random file from a specified folder.

# Features
- Open a random file from a folder
- Filter by file type (`--type png,jpg`)
- Prevents self-recursion
- Confirms before opening risky file types
- No administrator privileges required

# Usage
```bat
random_open <folder_path> [--type ext1,ext2]
````

## Examples
```bat
random_open C:\Images --type png,jpg
random_open C:\Music --type mp3,wav
```

# Build commands
## MinGW
```bat
windres src/version.rc build/version.o
g++ -std=gnu++17 -O2 src/random_open.cpp build/version.o -o random_open.exe
```
## Visual Studio Developer Command Prompt
```bat
rc src\version.rc
cl /std:c++17 /O2 src\random_open.cpp version.res shell32.lib
```

# License
This program is licensed under the MIT License.<br>
See the `LICENSE` file for details.
