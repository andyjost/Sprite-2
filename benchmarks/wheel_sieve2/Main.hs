primes :: [Int]
primes = spiral wheels primes squares

spiral (w@(Wheel s ms ns):ws) ps qs =
  spiral_0 w ps qs (head qs) (spiral ws (tail ps) (tail qs))

spiral_0 (Wheel s ms ns) ps qs q sp = foldr (turn0_0 q sp) (roll_0 q sp s ns ms s) ns

turn_0 q sp o n rs = turn_0_0 q sp rs (o+n)
turn_0_0 q sp rs n' =
  if n'==2 || n'<q then n':rs else dropWhile (<n') sp

roll_0 q sp s ns ms o =
  foldr (turn_0 q sp o) (foldr (turn_0 q sp o) (roll_0 q sp s ns ms (o+s)) ns) ms

turn0_0 q sp n rs =
  if n<q then n:rs else sp

squares :: [Int]
squares = map lambda_0 primes
lambda_0 p = p*p


data Wheel = Wheel Int [Int] [Int]

wheels :: [Wheel]
wheels = Wheel 1 [1] [] : zipWith3 nextSize wheels primes squares

nextSize (Wheel s ms ns) p q =
  nextSize_0 s ms p (span (<=q) (foldr (turn0_1 p) (roll_1 p s ns ms (p-1) s) ns))

nextSize_0 s ms p (xs,ns') = nextSize_1 s p ns' (foldr (turn0_1 p) xs ms)

nextSize_1 s p ns' ms' = Wheel (s*p) ms' ns'

turn0_1 p n rs =
  if n`mod`p>0 then n:rs else rs

roll_1 p s ns ms t o = if t==0 then [] else
    foldr (turn_1 p o) (foldr (turn_1 p o) (roll_1 p s ns ms (t-1) (o+s)) ns) ms

turn_1 p o n rs = turn0_1 p (o+n) rs

main = do
  print $ primes !! 8000
