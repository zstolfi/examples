#| Exercise 1.7
    
|#

; Setup
(define (average x y)
  (/ (+ x y) 2))
(define (square x)
  (* x x))

(define (improve guess x)
  (average guess (/ x guess)))

(define (sqrt-iter guess x)
  (if (good-enough? guess x)
      guess
      (sqrt-iter (improve guess x)
                 x)))

(define (sqrt x)
  (sqrt-iter 1.0 x))



; Naive good-enough? condition
(define (good-enough? guess x)
  (< (abs (- (square guess) x)) 0.001))

; This works pretty well for numbers close to 1
(sqrt 1.0)    ; 1.0    1.0
(sqrt 0.81)   ; .9     .90001...
(sqrt 0.64)   ; .8     .8002...

; But for small numbers it stops at 0.03125
(sqrt 0.16)   ; .4     .4009...
(sqrt 0.01)   ; .1     .1003...
(sqrt 0.0001) ; .01    .0323...
(sqrt 0.0000000001)
              ; .00001 .0312...
(sqrt 0)      ; 0.0    .0312...

; Larger numbers hold up pretty well though
;   6 decimal places of accuracy is common
(sqrt 1.21)   ; 1.1    1.10001...
(sqrt 2.25)   ; 1.5    1.500007...
(sqrt 4)      ; 2      2.000000009...
(sqrt 81)     ; 9      9.00001...
(sqrt 40000)  ; 200    200.0000005...
(sqrt 531441) ; 729    729.0000002...



; New good-enough? condition
(define (good-enough? guess x)
  (< (abs (- 1 (/ (square guess) x))) 0.001))

; Works the same for numbers close to 1
(sqrt 1.0)    ; 1.0    1.0
(sqrt 0.81)   ; .9     .90001...
(sqrt 0.64)   ; .8     .8002...

; A lot better for small numbers
(sqrt 0.16)   ; .4     .400001...
(sqrt 0.01)   ; .1     .1000005...
(sqrt 0.0001) ; .01    .0100007...
(sqrt 0.0000000001)
              ; .00001 .0000100005...
(sqrt 0)      ; 0.0    halts :(

; Worse for large numbers, but very consistantly
;   has 3 sig-figs of precision
(sqrt 1.21)   ; 1.1    1.10001...
(sqrt 2.25)   ; 1.5    1.500007...
(sqrt 4)      ; 2      2.0006...
(sqrt 81)     ; 9      9.00001...
(sqrt 40000)  ; 200    200.014...
(sqrt 531441) ; 729    729.019...



; Fix the case when x = 0
(define (good-enough? guess x)
  (if (not (= x 0))
      (< (abs (- 1 (/ (square guess) x)))
         0.001)
      (< guess 0.0000000000001))) ; ideally would always return #f

(sqrt 0)