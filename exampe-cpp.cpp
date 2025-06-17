lisp> (define square (lambda (x) (* x x)))
<expr>
lisp> (square 5)
25
lisp> (if (> 5 3) 42 0)
42

(define x 10)
(define y 15)
(define sum (lambda (a b) (+ a b)))
(sum x y)

./lisp
lisp> (load "myfile.lisp")
25

lisp> (define square (lambda (x) (* x x))) ; defines square
<lambda>
lisp> (square 5)
25
lisp> *   ; get last result
25
lisp> (list 1 2 3)
(1 2 3)

