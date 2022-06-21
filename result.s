.data

.globl main
.text
li.s $f0, 5.6

s.s $f0, 0($sp) 
li.s $f0, 6.7

s.s $f0, 4($sp) 
l.s $f0, 0($sp)

l.s $f1, 4($sp)

sub.s $f0, $f0, $f1
l.s $f0, 0($sp)

l.s $f1, 4($sp)

div.s $f0, $f0, $f1
l.s $f0, 0($sp)

l.s $f1, 4($sp)

c.lt.s $f1, $f0
l.s $f0, 0($sp)

l.s $f1, 4($sp)

c.lt.s $f1, $f0
l.s $f0, 0($sp)

l.s $f1, 4($sp)

c.eq.s $f1, $f0

