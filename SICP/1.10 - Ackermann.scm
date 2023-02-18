#| Exercise 1.10
    
|#

; https://files.catbox.moe/5vhk0t.jpg

(define (A x y)
  (cond ((= y 0) 0)
	((= x 0) (* 2 y))
	((= y 1) 2)
	(else (A (- x 1)
		 (A x (- y 1))))))

; (A 1 10) = 2^10 = 1024)
; (A 2 4)  = 2^16 = ~64000)
; (A 3 3)  = (A 2 4) = 2^`6

; when n > 0
; (A 0 n) = 2*n
; (A 1 n) = 2^n
; (A 2 n) = 2^2^2^2^2 (n times)
; in general (A x y) seems to be 2^^^ ... ^^^2 with y arrows,
;   using Knuth's up arrow notation