(set-logic QF_LIA)
(declare-const x Int)
(declare-const y Int)
(declare-const z Int)
(assert (not (or (> x 8) (< y 6))))
(assert (<= z 3))
(check-sat)
(exit)