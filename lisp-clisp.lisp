(defparameter *global-env* (make-hash-table))

(defun make-env (parent)
  (list (make-hash-table) parent))

(defun env-lookup (symbol env)
  (let ((table (first env))
        (parent (second env)))
    (or (gethash symbol table)
        (and parent (env-lookup symbol parent))
        (error "Unbound variable: ~A" symbol))))

(defun env-define (symbol value env)
  (setf (gethash symbol (first env)) value))

(defun env-extend (params args env)
  (let ((new-env (make-env env)))
    (loop for p in params
          for a in args
          do (env-define p a new-env))
    new-env))

(defun is-lambda? (expr)
  (and (consp expr) (eq (car expr) 'lambda)))

(defun eval-exp (exp env)
  (cond
    ((symbolp exp) (env-lookup exp env))
    ((atom exp) exp)

    ((eq (car exp) 'quote)
     (cadr exp))

    ((eq (car exp) 'if)
     (let ((test (eval-exp (second exp) env)))
       (if test
           (eval-exp (third exp) env)
           (eval-exp (fourth exp) env))))

    ((eq (car exp) 'define)
     (let ((name (second exp))
           (value (eval-exp (third exp) env)))
       (env-define name val

