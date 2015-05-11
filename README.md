# oncgl
Some OpenGL experiments

This is just a small experiment I wrote for a computergraphics course at my university.
It is based on the following tutorials: 

* http://ogldev.atspace.co.uk/
* http://www.tomdalling.com/blog/category/modern-opengl/
* http://learnopengl.com/

I also used this project to lern C++.
If you find any bug or improvement for my code please leave an issue.

# Git Instructions

This repository uses [git-flow](http://danielkummer.github.io/git-flow-cheatsheet/). Please make sure to always work in feature branches.

## Setup

1. Clone the repo
2. `cd` into repo directory
3. Run `git-flow init` accepting all default values

## Working with git-flow

1. Start a new feature with `git-flow feature start FEATURE_NAME` (this creates a new branch)
2. Hack on your feature
3. Finish your feature with `git-flow feature stop FEATURE_NAME` (this merges the branch into `develop`)

# Dependencies

This project includes all required libraries except assimp. You need to install assimp yourself.
On Linux you should find assimp in your package manager.
OS X users can install assimp using [homebrew](https://github.com/Homebrew/homebrew).
Since I don't really care about windows I don't know how to install assimp on windows and I won't give instructions on how to build and run this project on windows machines.

You also need cmake and gcc or clang to compile the project.
OS X users also have to install pkg-config from homebrew.

# Build Instructions

To build the project run the following:

```
cd /path/to/repository
mkdir build
cd build
cmake ..
make
```

If you run the executable anywhere else but /build (like /build/release) make sure to adjust the path in /src/misc/constants.h and rebuild the project.

# Code guidelines

The code is formatted according to [google c++ style guide](https://google-styleguide.googlecode.com/svn/trunk/cppguide.html#General_Naming_Rules)

# DONE

* Loading models using assimp
* Deferred shading
* Render fonts

# TODO

* Add shadows
* Work on performance
* Read material-coefficients like shininess from objects
* Use object colors if there is no texture
