#| Exercise 1.5
    Ben Bitdiddle has invented a test to determine whether the
    interpreter he is faced with is using applicative-order
    evaluation or normal-order evaluation. He defines the
    following two procedures:
|#

#| With normal-order evaluation, the (p) statement is
   "expanded" into (p) again. Assuing the interpreter
   doesn't notice, this causes an infinite loop, and
   the program halts.
   With applicative-order, because of lazy evaluation
   of the 'if' statement, (p) is never expanded.
|#

(define (p) (p))

(define (test x y)
  (if (= x 0)
      0
      y))

(test 0 (p))