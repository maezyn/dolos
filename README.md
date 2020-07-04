# wildfire

![GitHub](https://img.shields.io/github/license/mmore21/wildfire)

A metamorphic virus written in C.

## Description

The malware is first activated by running the compiled bytecode. It copies the */etc/passwd* and */etc/shadow* files from the user's system and sends them to an external IP address. This is achieved by embedding these sensitive file contents in the payloads of ICMP pings. The contents of these pings can then be intercepted using a packet analyzer such as Wireshark and then fed into a password cracker such as John the Ripper. In the next stage, the malware scans the current directory and overwrites all executable files that have not been previously infected with its morphed code. Because each propagation uses a unique version of the morphed code, it makes it difficult for antivirus software to detect it. Afterwards, the original executable is run from a hidden file it was copied to during the propagation phase to disguise the fact that the actual executable was infected. Finally, a Perl reverse shell is opened to give the attacker access to the system with the cracked passwords.

## Design

![Malware Flowchart](https://github.com/mmore21/wildfire/blob/master/img/flow_malware.png)

## Usage

**Caution, running the executable will propagate the virus into all other files in the current directory that have executable permissions. This can cause permanent damage to your system if misused.**

Run the program using the following Makefile or by compiling from source with gcc.

### Unix:

Run with the following Makefile:
<pre>
git clone https://github.com/mmore21/wildfire.git
cd wildfire/
make
sudo ./wildfire
make clean
</pre>

Afterwards, go to a protocol analyzer such as Wireshark and filter by ICMP traffic. The default IP address that is pinged is 127.0.0.1 (loopback address). Copy the payload as printable ASCII text for each respective ping and save them into two files that represent the */etc/passwd* and */etc/shadow* files. Please note, you will have to remove the spaces from the *$* along with any extraneous payload data.

![Wireshark Screenshot](https://github.com/mmore21/wildfire/blob/master/img/screen_wireshark.png)

Finally, use a password cracker such as John The Ripper to attempt to crack the unshadowed combination of these files. Please fill in *PASSWD_FILENAME* and *SHADOW_FILENAME* with the respective names of the files created in the protocol analyzer step. Please note, these passwords may take a long time to crack.

<pre>
sudo unshadow PASSWD_FILENAME SHADOW_FILENAME > mypasswd
sudo john mypasswd
</pre>

After the malware sends the ICMP pings, it then propagates, runs the original executable, and finally attempts to fire a Perl reverse shell. In order to successfully establish a connection, the attacker must listen on a specific port. By default, the IP address is set to 127.0.0.1 (loopback address) and the port is set to 1234. Please note, netcat must be listening on the specific port prior to running the malware.

<pre>
nc 127.0.0.1 -lp 1234
</pre>

## Outlook

* Refactor code and ensure memory is being freed
* Add more metamorphic components
* Encrypted ping payloads
* Reducing trail of evidence by embedding virus and executable in one file rather than creating a hidden file for the executable that is called after virus execution

## Disclaimer

This project is intended solely for educational purposes to better understand various malware techniques. It is not to be distributed, modified, or used for any malicious intent. As stated in the license below, I take no responsibility for any malicious use of this malware.

## License

wildfire is available under the [MIT License](https://github.com/mmore21/wildfire/blob/master/LICENSE).

## References

* [Wikipedia Metamorphic Code](https://en.wikipedia.org/wiki/Metamorphic_code)
* [Stack Overflow Metamorphic Code Examples](https://stackoverflow.com/questions/10113254/metamorphic-code-examples)
* [Secret messages in a ping: creation and prevention](http://www.wseas.us/e-library/conferences/2012/SaintMalo/SITE/SITE-03.pdf)
