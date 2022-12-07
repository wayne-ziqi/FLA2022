#Q = {empty,yes,no,start,judge,addM,cnt2head,una2head,at_head0,at_head1,out_yes1,out_yes2,out_yes3,out_yes4,out_no1,out_no2,out_no3,out_no4,out_no5,halt}

#S = {1}

#G = {_,1,t,r,u,e,f,a,l,s,e,M}

#q0 = empty

#B = _

#F = {yes,no}

#N = 2

; empty: if the input is empty, goto yes, else put a M on tape1
empty __ __ ** yes
empty 1_ 1M ** start
start 1M ** rr judge

; judge: if tape0 is _ and tape1 is M, reject, if tape1 and tape2 are both _, accept
judge _M ** l* no
judge __ ** l* yes
judge 1_ 1M *r addM

; add one more M to the end of tape1
addM 1_ 1M ** cnt2head

; cnt2head: move the head on tape1 to head
cnt2head 1M ** *l cnt2head
cnt2head 1_ ** *r at_head1

; at_head1: the count head is at the front
at_head1 1M ** rr at_head1
at_head1 _M ** ** judge
at_head1 __ ** ** judge
at_head1 1_ ** ** judge

;yes both head0 and head1 point to _, keep head1 and move head0 to the front
yes 1_ _* l* yes
yes __ ** r* out_yes1

;no head0 points to _ and head1 points to M
no 1M _* l* no
no _M ** r* out_no1

; out_yes prints true
out_yes1 __ t* r* out_yes2
out_yes2 __ r* r* out_yes3
out_yes3 __ u* r* out_yes4
out_yes4 __ e* ** halt

; out_no prints false
out_no1 _M f* r* out_no2
out_no2 _M a* r* out_no3
out_no3 _M l* r* out_no4
out_no4 _M s* r* out_no5
out_no5 _M e* r* halt
