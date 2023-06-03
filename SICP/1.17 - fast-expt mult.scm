#| Exercise 1.17
    Using the operations double, and halve, design a
    multiplication procedure analogous to fast-expt that uses a
    logarithmic number of steps.
|#

; Old version
(define (* a b)
  (if (= b 0)
      0
      (+ a (* a (- b 1)))))

; New Version
(Define (double x) (+ x x))
(define (half   x) (/ x 2))

(define (* a b)
  (cond ((= b 0) 0)
    ((even? b) (double (* a (half b))))
    (else (+ a (* a (- b 1))))))

(* 4 10)