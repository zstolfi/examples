#| Exercise 1.11
    
|#

; mathematical definition
(define (f n)
  (if (< n 3)
      n
      (+ (* 1 (f (- n 1)))
	 (* 2 (f (- n 2)))
	 (* 3 (f (- n 3))))))

; iterative method
(define (f n)
  (define (f-iter n a1 a2 a3)
    (if (< n 3)
	a3
        (f-iter (- n 1)
        	a2
        	a3
        	(+ (* 1 a3) (* 2 a2) (* 3 a1)))))
  (if (< n 3)
      n
      (f-iter n 0 1 2)))

(f 0) ; -> 0
(f 1) ; -> 1
(f 2) ; -> 2
(f 3) ; -> 4
(f 4) ; -> 11
(f 5) ; -> 25
(f 6) ; -> 59
(f 7) ; -> 142
(f 8) ; -> 335
(f 9) ; -> 796