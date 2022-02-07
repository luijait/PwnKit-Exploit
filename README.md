# PwnKit-Exploit 
CVE-2021-4034
<div id="image" align="center">
 <img src="https://user-images.githubusercontent.com/60628803/151507343-b49df170-c853-47c9-aac1-740302e435f9.png" height="300" width="300">
                                                                                                                   
  <img src="https://linuxiac.b-cdn.net/wp-content/uploads/2022/01/polkit-bug.png" alt="PolKit" height="300">
</div>

---

<div id="badges" align="center">
  <img src="https://img.shields.io/badge/CVE-%20%20CVE--2021--4034%20-critical">
  <img src="https://img.shields.io/badge/%40author-luijait.es-informational">
  <img src="https://img.shields.io/github/repo-size/luijait/PwnKit-Exploit?label=Size">
  <img src="https://img.shields.io/github/languages/top/luijait/PwnKit-Exploit?label=C">
</div>
@c0br40x help to make this section in README!!

### Proof of Concept


![Proof of Concept](https://j.gifs.com/XQpL88.gif)


```
debian@debian:~/PwnKit-Exploit$ make
cc -Wall    exploit.c   -o exploit
debian@debian:~/PwnKit-Exploit$ whoami
debian
debian@debian:~/PwnKit-Exploit$ ./exploit
Current User before execute exploit
hacker@victim$whoami: debian
Exploit written by @luijait (0x6c75696a616974)
[+] Enjoy your root if exploit was completed succesfully
root@debian:/home/debian/PwnKit-Exploit# whoami
root
root@debian:/home/debian/PwnKit-Exploit# 
```
### Fix
| Command | Use |
| ------- | ----------- |
| `sudo chmod 0755 pkexec` | Fix CVE 2021-4034 |



### Installation & Use
`git clone https://github.com/luijait/PwnKit-Exploit`

`cd PwnKit-Exploit` 


`make` 


`./exploit` 


`whoami`
| Command | Utility |
| ------- | ------------- |
|`make clean` | Clean build to test code modified |
### Explanation 
Based blog.qualys.com


The beginning of pkexec’s main() function processes the command-line arguments (lines 534-568), and searches for the program to be executed, if its path is not absolute, in the directories of the PATH environment variable (lines 610-640):
```C

435 main (int argc, char *argv[])
436 {
...
534   for (n = 1; n < (guint) argc; n++)
535     {
...
568     }
...
610   path = g_strdup (argv[n]);
...
629   if (path[0] != '/')
630     {
...
632       s = g_find_program_in_path (path);
...
639       argv[n] = path = s;
640     }
```
unfortunately, if the number of command-line arguments argc is 0 – which means if the argument list argv that we pass to execve() is empty, i.e. {NULL} – then argv[0] is NULL. This is the argument list’s terminator. Therefore:

 at line 534, the integer n is permanently set to 1;
 at line 610, the pointer path is read out-of-bounds from argv[1];
 at line 639, the pointer s is written out-of-bounds to argv[1].
But what exactly is read from and written to this out-of-bounds argv[1]?

To answer this question, we must digress briefly. When we execve() a new program, the kernel copies our argument, environment strings, and pointers (argv and envp) to the end of the new program’s stack; for example:
```
|---------+---------+-----+------------|---------+---------+-----+------------| 
| argv[0] | argv[1] | ... | argv[argc] | envp[0] | envp[1] | ... | envp[envc] | 
|----|----+----|----+-----+-----|------|----|----+----|----+-----+-----|------| 
V         V                V           V         V                V 

"program" "-option"           NULL      "value" "PATH=name"          NULL 
```
Clearly, because the argv and envp pointers are contiguous in memory, if argc is 0, then the out-of-bounds argv[1] is actually envp[0], the pointer to our first environment variable, “value”. Consequently:

 At line 610, the path of the program to be executed is read out-of-bounds from argv[1] (i.e. envp[0]), and points to “value”;
 At line 632, this path “value” is passed to g_find_program_in_path()  (because “value” does not start with a slash, at line 629);
 Then, g_find_program_in_path() searches for an executable file named “value” in the directories of our PATH environment variable;
 If such an executable file is found, its full path is returned to pkexec’s main() function (at line 632);
 Finally, at line 639, this full path is written out-of-bounds to argv[1] (i.e. envp[0]), thus overwriting our first environment variable.
So, stated more precisely:

 If our PATH environment variable is “PATH=name”, and if the directory “name” exists (in the current working directory) and contains an  executable file named “value”, then a pointer to the string  “name/value” is written out-of-bounds to envp[0];
OR

 If our PATH is “PATH=name=.”, and if the directory “name=.” exists and contains an executable file named “value”, then a pointer to the  string “name=./value” is written out-of-bounds to envp[0].
In other words, this out-of-bounds write allows us to re-introduce an “unsecure” environment variable (for example, LD_PRELOAD) into pkexec’s environment. These “unsecure” variables are normally removed (by ld.so) from the environment of SUID programs before the main() function is called. We will exploit this powerful primitive in the following section.

Last-minute note: polkit also supports non-Linux operating systems such as Solaris and *BSD, but we have not investigated their exploitability. However, we note that OpenBSD is not exploitable, because its kernel refuses to execve() a program if argc is 0.

