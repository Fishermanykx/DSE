; 
(set-info :status unknown)
(declare-fun X0 () Int)
(declare-fun X2 () Int)
(declare-fun X1 () Int)
(assert
 (let (($x13 (= X0 0)))
 (= $x13 true)))
(assert
 (let (($x15 (= X1 X2)))
 (= $x15 false)))
(check-sat)
