(define global-env
  (let ((env (make-hash-table)))
    ;; Add built-in procedures
    (for-each (lambda (pair)
                (hash-table-set! env (car pair) (cdr pair)))
              `((+ . ,+)
                (- . ,-)
                (* . ,*)
                (/ . ,/)
                (= . ,=)
                (< . ,<)
                (> . ,>)
                (cons . ,cons)
                (car . ,car)
                (cdr . ,cdr)
                (list . ,list)
                (null? . ,null?)
                (not . ,not)))
    env))

(define (eval-exp exp env)
  (cond
    ;; Self-evaluating
    ((number? exp) exp)
    ((string? exp) exp)
    ((boolean? exp) exp)

    ;; Variable reference
    ((symbol? exp)
     (hash-table-ref env exp (lambda () (error "Unbound variable" exp))))

    ;; Special forms
    ((pair? exp)
     (case (car exp)
       ((quote) (cadr exp))
       ((if)
        (if (eval-exp (cadr exp) env)
            (eval-exp (caddr exp) env)
            (eval-exp (cadddr exp) env)))
       ((define)
        (let ((name (cadr exp))
              (val (eval-exp (caddr exp) env)))
          (hash-table-set! env name val)
          name))
       ((lambda)
        (let ((params (cadr exp))
              (body (caddr exp)))
          (lambda args
            (let ((new-env (make-hash-table)))
              (for-each (lambda (param arg)
                          (hash-table-set! new-env param arg))
                        params args)
              (set! new-env (merge-envs env new-env))
              (eval-exp body new-env)))))
       (else
        ;; Function application
        (let ((proc (eval-exp (car exp) env))
              (args (map (lambda (e) (eval-exp e env)) (cdr exp))))
          (apply proc args)))))
    (else
     (error "Unknown expression type" exp))))

(define (merge-envs parent child)
  (let ((new-env (make-hash-table)))
    ;; Copy parent bindings
    (hash-table-for-each parent
      (lambda (k v) (hash-table-set! new-env k v)))
    ;; Override with child bindings
    (hash-table-for-each child
      (lambda (k v) (hash-table-set! new-env k v)))
    new-env))


