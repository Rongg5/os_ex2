part 1:
1.a
Q.
 Describe what the functions sigsetjmp and siglongjmp do
A.
 sigsetjmp - sigsetjmp is like a bookmark, it's save the current line in the code for use in the future.
siglongjmp - siglongjmp is jump to the "bookmark" that was set in the sigsetjmp call.
1.b
Q.
 Describe how sigsetjmp and siglongjmp affect masking.
A.
 sigsetjmp affect the masking that if it gets (savemask != 0) so it saves the current masking, else it
will not. so if we will use siglongjmp to a sigsetjmp that did not save the current masking it will
behave differently than if it did save.

2.
 Q.
  Describe one general use of user-level threads and explain why user-level threads are a
   reasonable choice for your example.
A.
 when i use a mp4 video so the presses need to simultaneously run the audio and the screen. so becuse it's
easyer to make threads simultaneously in the ULT we will prefer that.

3. Q.
 Google’s Chrome browser creates a new process for each tab. What are the advantages
   and disadvantages of creating the new process (instead of creating a kernel-level
   thread)?
   A.
   creates a new process for each tab handle more well in security problem like that one tab will not able
   to get a private information from other tab, like if the bank occunt is open in that tab.

4. Q.
   Interrupts and signals:
   a. Open an application (for example, “Shotwell” on one of the CS computers). Use
   the “ps -A” command to extract the application’s pid (process ID).
   b. Open a shell and type “kill pid”.
   c. Explain which interrupts and signals are involved during the command execution,
   what triggered them and who should handle them. In your answer, refer to the
   keyboard, OS, shell, and the application you just killed.
A.
   First when i click the keyboard the OS get interrupt to check the input and then it starts a process by
   the shell that interprets the input.
