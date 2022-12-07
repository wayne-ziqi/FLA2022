; This example will cyclically shift the input to the right

#Q = {empty,finding,found_tail,goto_head,start_again,roll_back,copy,finish}

#S = {0,1}

#G = {0,1,_}

#q0 = empty

#B = _

#F = {finish}

#N = 2

; transition functions

; empty: input string is empty or is waiting to be processed
empty __ __ ** finish   ;empty string can be accepted
empty *_ *_ r* finding

; finding: search for the last symbol
finding *_ *_ r* finding
finding __ __ l* found_tail

; found_tail: head of tape0 points to the last symbol, should empty it and copy it to tape2
found_tail 0_ _0 l* goto_head
found_tail 1_ _1 l* goto_head

; goto_head: head of tape0 goes to the front
goto_head ** ** l* goto_head
goto_head _* _* rr start_again

; start_again: move symbols from tape0 to tape1
start_again 0_ *0 rr start_again
start_again 1_ *1 rr start_again
start_again __ __ ll roll_back

; roll_back: move both head to the front
roll_back ** ** ll roll_back
roll_back _* ** r* copy

;copy string from tape1 to tape0
copy *0 0_ rr copy
copy *1 1_ rr copy
copy _0 0_ ** finish
copy _1 1_ ** finish