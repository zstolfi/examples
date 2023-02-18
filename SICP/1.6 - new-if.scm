#| Exercise 1.6
    
|#


(define (square x)
  (* x x))

(define (average x y)
  (/ (+ x y) 2))


; old version
(define (sqrt-iter guess x)
  (if (good-enough? guess x)
      guess
      (sqrt-iter (improve guess x)
		 x)))

(define (improve guess x)
  (average guess (/ x guess)))

(define (good-enough? guess x)
  (< (abs (- (square guess) x)) 0.001))

(define (sqrt x)
  (sqrt-iter 1.0 x))

#| This code will not work when the 'if' is replaced.
   The interpreter is unable to do lazy-evaluation.
   Meaning sqrt-iter is evaluated infinitely. Causing
   the program to halt.
#|

; new version
(define (sqrt-iter guess x)
  (new-if (good-enough? guess x)
	  guess
	  (sqrt-iter (improve guess x)
		     x)))

(sqrt 5)