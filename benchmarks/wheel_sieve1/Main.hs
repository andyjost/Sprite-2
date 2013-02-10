data Wheel = Wheel Int [Int]

primes :: [Int]
primes = sieve wheels primes squares

sieve ((Wheel s ns):ws) ps qs =
  (gen s ns ps qs) ++ (sieve ws (tail ps) (tail qs))
noFactor s ps qs = if s<=2 then const True else notDivBy ps qs
gen s ns ps qs = concatMap (lambda_4 ns (noFactor s ps qs)) (gen_2 s ps)
gen_2 s ps = s:[s*2,s*3..((head ps)-1)*s]
lambda_4 ns pred o = concatMap (lambda_5 pred o) ns
lambda_5 pred o n = filter pred [n+o]

notDivBy (p:ps) (q:qs) n =
  q > n || ((n `mod` p > 0) && (notDivBy ps qs n))

squares :: [Int]
squares = map lambda_0 primes
lambda_0 p = p*p

wheels :: [Wheel]
wheels = (Wheel 1 [1]) : zipWith nextSize wheels primes

nextSize (Wheel s ns) p = Wheel (s*p) (ns' s ns p)
ns' s ns p = concatMap (lambda_1 ns p) [0,s .. (p-1)*s]
lambda_1 ns p o = concatMap (lambda_2 p o) ns
lambda_2 p o n = filter (lambda_3 p) [n+o]
lambda_3 p n' = n'`mod`p > 0

main = do
  print $ primes !! 20000
