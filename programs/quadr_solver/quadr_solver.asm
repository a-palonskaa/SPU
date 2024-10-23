in
pop ax
in
pop bx
in
pop cx
push 0
push ax
je a_zero:
push 0
push bx
je b_zero:
push 0
push cx
je c_zero:
push bx
sqr
push -4
push ax
push cx
mul
mul
add
pop dx
push dx
push 0
jb no_roots:
push dx
push 0
je eq_roots:
push dx
sqrt
pop dx
push dx
push -1
push bx
mul
add
push 2
div
out
push dx
push -1
mul
push -1
push bx
mul
add
push 2
div
out
hlt
c_zero:
push 0
out
push -1
push bx
mul
push ax
div
out
hlt
b_zero:
push 0
push cx
je b_c_zero:
push -1
push cx
mul
push ax
div
pop dx
push dx
push 0
jb no_roots:
push dx
sqrt
pop dx
push dx
out
push dx
push -1
mul
out
hlt
no_roots:
push NAN
out
hlt
b_c_zero:
push 0
out
hlt
a_zero:
push bx
push 0
je a_b_zero:
push -1
push cx
mul
push bx
div
out
hlt
a_b_zero:
push 0
push cx
je a_b_c_zero:
push NAN
out
hlt
a_b_c_zero:
push INF
out
hlt
eq_roots:
push dx
sqrt
pop dx
push dx
push -1
push bx
mul
add
push 2
div
out
hlt
