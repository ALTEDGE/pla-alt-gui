# pla-gui

This is the GUI used with the PLA gaming controller. It's based on Qt5, and is
compatible with 64-bit Windows and Linux-based systems.

`Pla_GUI` is a QtCreator project folder. This project requires `wwwidgets`,
which may also be opened as a QtCreator project. The provided version of
`wwwidgets` is taken from [here](https://github.com/TheDZhon/wwwidgets).

`Runtime Files` contains files that should be in the directory of the Pla_GUI executable when it is executed. Qt Creator creates a `build.../` folder; find the executable in there and copy the files to there.

# Building

Building these projects on Linux can be simply done with the GNU compiler.

On Windows, building should be done with MSVC 2015. You should also use a static Qt library; one is available [here](https://bitgloo.com/files/msvc2015--static.zip) (64-bit).
