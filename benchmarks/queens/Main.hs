nsoln nq = length (gen nq nq)
gen r l = if l==0 then [[]]
   else filter lambda_0 (concatMap (lambda_1 r) (gen r (l-1)))

lambda_0 (q:b) = safe q 1 b
lambda_1 r b = (map (lambda_2 b) [1..r])
lambda_2 b q = q:b

safe _ _ []    = True
safe x d (q:l) = x /= q && x /= q+d && x /= q-d && safe x (d+1) l

main = do
  print $ nsoln 12

