#| Exercise 1.3
    Define a procedure that takes three numbers as arguments
    and returns the sum of the squares of the two larger
    numbers.
|#

(define (square x) (* x x))
(define (sum-of-squares x y) (+ (square x) (square y)))

(define (f x y z) (cond
    ((and (<= x y) (<= x z)) (sum-of-squares y z))
    ((and (<= y z) (<= y x)) (sum-of-squares z x))
    ((and (<= z x) (<= z y)) (sum-of-squares x y))
))

(f 1 2 3)
(f 2 3 1)
(f -4 -3 -2)