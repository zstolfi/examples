#| Exercise 1.12
    
|#

; Slow way O(2^n) without tabulation
(define (pascal x y)
  (cond ((or (< x 0) (< y 0) (> y x)) 0)
	((or (= y 0) (= y x)) 1)
	(else (+ (pascal (- x 1) (- y 1))
		 (pascal (- x 1) y      )))))

(pascal 8 5)
(pascal 22 10)
; larger numbers are too slow to compute
;   unless I tink up a way that's O(n^2) but can't yet