# wildfire

![GitHub](https://img.shields.io/github/license/mmore21/wildfire)

A metamorphic virus written in C.

This virus attaches itself to executable files and replicates itself.

## Design

The virus is first activated by running the compiled bytecode. It then searches and embeds itself into all executable files in the same directory.

## Usage

Temporarily, run the program by compiling from source with gcc. Scripts listed below will be implemented later.

### Unix:

Run with the following Makefile:
<pre>
TODO
</pre>

## Libraries

* dirent.h - Used for retrieving file and directory information

## Outlook

The goal is to transform this project into metamorphic malware in order to better comprehend this challenging topic.

Some tasks to be completed include:

* Encrypted ping payloads
* Propagating into executables
* Refactor code and free variables from memory

## License

wildfire is available under the [MIT License](https://github.com/mmore21/wildfire/blob/master/LICENSE).
