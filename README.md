# wildfire

![GitHub](https://img.shields.io/github/license/mmore21/wildfire)

A metamorphic virus written in C.

## Description

The malware is first activated by running the compiled bytecode. It copies the */etc/passwd* and */etc/shadow* files from the user's system and sends them to an external IP address. This is achieved by embedding these sensitive file contents in the payloads of ICMP pings. Next, the malware scans the current directory and overwrites all executable files that have not been previously infected with its morphed code. Because each propagation uses a unique version of the morphed code, it makes it difficult for antivirus software to detect it. Afterwards, the original executable is run from a hidden file it was copied to during the propagation phase to disguise the fact that the actual executable was infected.

## Usage

**Caution, running the executable will propagate the virus into all other files in the current directory that have executable permissions. This can cause permanent damage to your system if misused.**

Run the program using the following Makefile or by compiling from source with gcc.

### Unix:

Run with the following Makefile:
<pre>
git clone https://github.com/mmore21/wildfire.git
cd wildfire/
make
./wildfire
make clean
</pre>

## Libraries

* dirent.h - Used for retrieving file and directory information

## Outlook

* Encrypted ping payloads
* Refactor code and ensure memory is being freed
* Reducing trail of evidence by embedding virus and executable in one file rather than creating a hidden file for the executable that is called after virus execution

## License

wildfire is available under the [MIT License](https://github.com/mmore21/wildfire/blob/master/LICENSE).