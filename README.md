# GoEmbed

This is a sample showing dynamic loading of a scripting module written in Go, from a C++ main program.

This is doing the same as [V8AndGUI](https://github.com/fynv/V8AndGUI) and [CppEmbed](https://github.com/fynv/CppEmbed) demos, except that the "scripting" module is written in Go.

APIs are wrapped and exported first using extern "C", then wrapped again in Go.

