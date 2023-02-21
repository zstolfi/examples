#| Exercise 1.13
    Prove that Fib(n) is the closest integer to φ^n/sqrt(5),
    where φ = (1+sqrt(5))/2.
|#


#|
    φ and ψ are the 2 solutions to x^2 = x + 1
    
    Lemma 1: φ^n = Fib(n-1) + φ*Fib(n)

    A number of the form (a + φb) will be of the same form when
    multiplied by φ. (where a and b are integers)
    We can see this algebraicly.
      (a + φb) * φ
    = (φa + φ^2*b) ; recall φ^2 = φ + 1
    = (φa + (φ+1)*b)
    = (φa + φb + b)
    = (b + φ(a+b))

    From this, we can see multiplying by φ maps:
    a -> b
    b -> a+b

    This is just the iterative rule for the fibonacci sequence.
    Because φ = (0 + 1φ), the formula φ^n can be written:
    φ^n = Fib(n-1) + φ*Fib(n)

    Also, because φ and ψ satisfy the same equation, the above
    works for ψ as well. Namely:
    ψ^n = Fib(n-1) + ψ*Fib(n)
    


    Lemma 2: Fib(n) = (φ^n - ψ^n)/sqrt(5)
    Let's do the algebra.
      (φ^n - ψ^n)/sqrt(5) ; recall lemma 1
    = ((Fib(n-1) + φ*Fib(n)) - (Fib(n-1) + ψ*Fib(n)))/sqrt(5)
    = (Fib(n-1) + φ*Fib(n) - Fib(n-1) - ψ*Fib(n))/sqrt(5)
    = (φ*Fib(n) - ψ*Fib(n))/sqrt(5)
    = Fib(n)*(φ-ψ)/sqrt(5) ; (φ-ψ) = sqrt(5)
    = Fib(n)



    Proof: Fib(n) = round(φ^n/sqrt(5))

    Let Error(n) = Fib(n) - φ^n/sqrt(5)
    This error function is taken from Lemma 2, but without the
    ψ^n term.

    Error(0) = -1/sqrt(5) (-0.4472...)
    Because this lies in the range [-0.5, 0.5),
    rounding our approximation will equal Fib(0) exactly.

    Because |ψ^n| is always decaying, Error(n) will always stay
    within the range [-0.5, 0.5) for successive values of n.
    Thus for all n >= 0, Fib(n) = round(φ^n/sqrt(5)).

    QED.                                                      █
|#
