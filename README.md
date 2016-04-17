# ZIPWalk
[ONGOING PROJECT]</br>
<b>View and analyze the structure of ZIP file</b>

The main goal of the program is to detect steganography in ZIP files.

Structure of the ZIP archive is not strictly defined so it can contain hidden malware.</br>
Most of AV software implements support of ZIP in End-first approach meaning that it looks for the End Of Central Directory(EOCD) located at the end of the file. EOCD keeps pointer on the CDH records which should contain information about each file in ZIP. However there are some steganography techniques that can make file undetectable.<br>

With this piece of software I try to solve that issue.

To do :<br>
1) implement different steganography detectors in ZIP archives<br>
2) create a real built-in ZIP structure and search for multiple file signatures in "free space"<br>
3) make GUI
