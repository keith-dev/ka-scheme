(eval-exp '(define square (lambda (x) (* x x))) *global-env*)
(eval-exp '(square 6) *global-env*) ; => 36

(eval-exp '(if (> 5 3) 'yes 'no) *global-env*) ; => YES

