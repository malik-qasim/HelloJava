section .bss
charBuffer      	resb 150 ;Buffer array to hold characters 
outfileDescriptor	resq 1 	 ;Used to hold the file descriptor for the output file

section .data
BUFFER_MAX       equ 150
CAESAR_OFFSET    equ 9 ;How much to offset each char by in the Caesar cypher

;Messages and message constants
LF              equ  10 ;ASCII char const for Linux linefeed
NULL            equ  0  ;ASCII char const for NULL
welcomeMessage  db  "Reading from file...", LF, NULL
doneMessage     db  "Done! See cypher_text.txt", LF, NULL
errorMessage	db	"Could not open output file", LF, NULL
writeError      db  "Could not write to file!", LF, NULL
outfileName		db	"cypher_text.txt", NULL
lineFeed        db  LF, NULL
welcomeLength   equ 21
doneLength      equ 26
errorLength     equ 27
writeErrLength	equ	25

;------------- Constants -------------
EXIT_SUCCESS    equ     0   ;Exit code for success
EXIT_FAIL       equ     1   ;Exit code for error
TRUE            equ     1
FALSE           equ     0

;File descriptors for common files
STDIN           equ     0 ;File descriptor for standard in
STDOUT          equ     1 ;File descriptor for standard out
STDERR          equ     2

;Syscall codes
SYS_read        equ     0
SYS_write       equ     1
SYS_open        equ     2
SYS_close       equ     3
SYS_chmod       equ     15
SYS_exit        equ     60  ;Syscall code for terminating program
SYS_creat       equ     85

;Arguments for file I/O related syscalls
O_CREATE        equ     0x40
O_APPEND        equ     1024
O_RDONLY        equ     000000q
O_WRONLY        equ     000001q
O_RDWR	        equ     000002q
O_RDWREX        equ     777o

section .text
global _start
global populateBuffer
global translateCharsInBuffer
global writeBuffer
global translateFile
global printDoneMessage
global printWelcomeMessage
global openOutputFile
_start:

call printWelcomeMessage 	;Print the welcome message
call translateFile 			;Call the main function (it's void and takes no args)
call printDoneMessage ;Print the done message

;Exiting the program
last:
    mov eax, SYS_exit  ;Load the syscall code for terminating the program
    mov edi, EXIT_SUCCESS ;Load the exit value for the program into rdi
    syscall

;--------------------------- Functions go down here ---------------------------

;Main function that calls other functions. This will read and translate the entirety of the file (stdin) and write the output to another file
translateFile:
    push rbp
    mov rbp, rsp
	
	;Open the output file for writing
	call openOutputFile
	mov qword[outfileDescriptor], rax

    mainLoop:
        ;Populate the buffer using the populate buffer function
        mov rdi, STDIN
        mov rsi, charBuffer
        call populateBuffer

        ;Now that the buffer is full, check how many chars were read
        cmp rax, BUFFER_MAX
        jl lastCall

        ;Translate the entire buffer and jump back up to loop again
        mov rdi, rax
        mov rsi, charBuffer
        call translateCharsInBuffer
        jmp mainLoop

    lastCall:
    cmp rax, 0
    je doneTranslation

    ;Have the last set of characters be translated
    mov rdi, rax
    mov rsi, charBuffer
    call translateCharsInBuffer
    doneTranslation:

    ;Close the output file
    mov rax, SYS_close
    mov rdi, qword[outfileDescriptor]

    pop rbp
    ret

;Function to print the welcome message at the start of the program
;It is void and takes no arguments
printWelcomeMessage:
	mov rax, SYS_write
	mov rdi, STDOUT
	mov rsi, welcomeMessage
	mov rdx, welcomeLength
	syscall
	ret

;This function opens the output file for the program and returns its file descriptor via rax	
openOutputFile:
    mov rax, SYS_creat
    mov rdi, outfileName
    mov rsi, O_RDWREX ;Set file permissions to read, write, and execute
    syscall
	
	;Check if creation was successful and if not try an open
	cmp rax, 0
	jge noErrorCreating

    mov rax, SYS_open
    mov rdi, outfileName
    mov rsi, O_CREATE
    syscall

    ;Check if there was an error opening the file and if so, print a message and terminate
    cmp rax, 0
    jge noErrorCreating
	
	mov rax, SYS_write
	mov rdi, STDOUT
	mov rsi, errorMessage
	mov rdx, errorLength
	syscall
	
	mov eax, SYS_exit  ;Load the syscall code for terminating the program
    mov edi, EXIT_FAIL ;Load the exit value for the program into rdi
    syscall
	
	noErrorCreating:
	ret

;################## YOUR CODE BELOW ##################

;Function to print the done message at the end of the program
;It is void and takes no arguments
printDoneMessage:
	;-Finish the function to print the done message to standard out
	;This should load the arguments for a write syscall so that the doneMessage with doneLength is
	;printed to STDOUT. The syscall for writing is provided as a constant (SYS_write).
	;This function will be very similar to the provided printWelcomeMessage function
	mov rax, SYS_write
	mov rdi, STDOUT
	mov rsi, doneMessage
	mov rdx, doneLength
	syscall
	ret

;This function fills the given buffer with characters and returns how many chars were read into the buffer
;Argument 1 = File descriptor for file to read from (rdi)
;Argument 2 = Address of buffer to store chars into (rsi)
;Returns the number of characters read into the buffer
populateBuffer:
    ;-Finish the function
    ;Push rbp and update rsp
    ;rdi and rsi are already set up for the syscall, but rax and rdx need set. 
    ;rax needs to be the call code (here the SYS_read to indicate we want to read)
    ;rdx needs to be the max number of characters to read, which we have a constant for at the top called bufferMax
    ;Load the registers, make a syscall, and return the number of chars read via rax (it will already be there after the syscall, which is convenient)
    ;Pop rbp
	push rbp
	mov rbp, rsp
	mov rax, SYS_read
	mov rdx, bufferMax
	syscall
	pop rbp
    ret
	
;Writes the given number of characters from a given buffer to the output file
;Argument 1 = File descriptor (rdi)
;Argument 2 = Address of buffer array (rsi)
;Argument 3 = Number of characters to write (rdx)
writeBuffer:
    ;Finish the function to write the buffer to the given output
    ;Write to the file
    ;Check if the write was succesful, and if not, write the error message to 
	;STDOUT instead
	push rbp
	mov rbp, rsp
	mov rax, SYS_write
	syscall
	test rax, rax
	jns NoError
	mov rax, SYS_write
	mov rdi, STDOUT
	mov rsi, writeError
	mov rdx, writeErrLength
	syscall
    noError:
	pop rbp
    ret

;This function translates all characters in the buffer using a Ceasar cypher and then writes them to the output file
;Argument 1 = The number of characters in the buffer to translate, does not need error checked (rdi)
;Argument 2 = Address of the buffer (rsi)
;No return value
translateCharsInBuffer:
    ;-Finish the function
    ;Add the caeserOffset to all of the bytes in the buffer (i.e. loop through all of them, add the offset, and overwrite the chars in the buffer)
	push rbp
	mov rbp, rsp
	mov rcx, rdi
	push rsi
translateCharsInBuffer_loop:
	add byte [rsi], CAESAR_OFFSET
	inc rsi
    loop translateCharsInBuffer_loop
	;Afterwards, write the buffer out to the output file by calling the writeBuffer function
	;with the given number of characters in the buffer and using the outfileDescriptor variable
    ;Prepare to call the function for buffer writing
	mov rdi, qword[outfileDescriptor] ;Argument 1 = File descriptor (rdi)
	pop rsi							  ;Argument 2 = Address of buffer array (rsi)
	mov rdx, rdi					  ;Argument 3 = Number of characters to write (rdx)
	call writeBuffer
    pop rbp
	ret
