# Single-event Upset Simulator (SUS)
 Program that simulates single-event upsets by bit flipping random data in memory. It's available for Windows x86 and x64, and is intended to be used for educational purposes, to show what can happen to a system when random parts of memory are modified.

 WARNING: This program has the ability to modify the memory of pretty much any actively running process on your PC, including system processes. Please be responsible and only use it in controlled environments where damage is easily reversible. I am not responsible for any damages or other issues that may occur as a result of using this software.

# How it Works
 The program first gathers a list of currently running processes. This includes both system processes and anything executed by a user. It then picks a random process from that list and reads its memory. It keeps a list of any memory that contains data that is able to be written to, and then picks a random address to modify. The address is modified by taking its existing data and inverting a single bit from it. The actual effects of this flip can range from nothing to a complete system crash and potential corruption or other permanent damage.

# Issues & Pull Requests
 If you would like to contribute to this repository by creating an issue or pull request, please refer to the [contributing guidelines.](https://lambdagaming.github.io/contributing.html)
