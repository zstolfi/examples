#| Exercise 1.4
    Observe that our model of evaluation allows for
    combinations whose operators are compound expressions. Use
    this observation to describe the behavior of the following
    procedure:
|#

#| The procedure behaves differently when b > 0, but
   instead of an 'if' selecting beetween two satements,
   the 'if' selects which operation to use
|#

(define (a-plus-abs-b a b)
  ((if (> b 0) + -) a b))