# Single-event Upset Simulator (SUS)
 This is an experimental program that simulates single-event upsets by bit flipping random data in memory. It's available for Windows x86 and x64, and is intended to be used for educational purposes, to show what can happen to a system when random parts of memory are modified.

 WARNING!!! THIS PROGRAM HAS THE ABILITY TO MODIFY THE MEMORY OF PRETTY MUCH ANY ACTIVELY RUNNING PROCESS ON YOUR PC, INCLUDING SYSTEM SERVICES! PLEASE BE RESPONSIBLE AND ONLY USE IT IN CONTROLLED ENVIRONMENTS WHERE DAMAGE IS EASILY REVERSIBLE! I AM NOT RESPONSIBLE FOR ANY DAMAGES OR OTHER ISSUES THAT MAY OCCUR AS A RESULT OF USING THIS SOFTWARE!

# How it Works
 The program first gathers a list of currently running processes. This includes both system processes and user applications. It then picks a random process from that list and reads its memory. Then, it gathers a list of memory pages from that process that contain data that is able to be written to, and picks a random address from a random page to modify. The address is then modified by taking its existing data and inverting a single bit from it. The actual effects of this flip can range from nothing to a complete system crash and potential corruption or other major issues.

# File Version
 There is also a Python script included that works similarly to the main program except it modifies a random bit from a random file inside a specified directory. This comes with the same warnings as the main program. It may be a simple Python script but it can still cause corruption that could be difficult or impossible to fix. Please use it responsibly.

# Issues & Pull Requests
 If you would like to contribute to this repository by creating an issue or pull request, please refer to the [contributing guidelines.](https://lambdagaming.github.io/contributing.html)
