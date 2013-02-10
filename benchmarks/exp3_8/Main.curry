data Nat = Z | S Nat

-- First arg is a Nat, second is an Int.
myadd Z y = y
myadd (S x) y = S (myadd x y)

-- First arg is an Int, second is a Nat.
mymul x Z = Z
mymul x (S y) = myadd (mymul x y) x
fromInteger_ x = if x < 1 then Z else S (fromInteger_ (x-1))

myint Z = 0
myint (S x) = 1 + myint x

mypow x Z = S Z
mypow x (S y) = mymul x (mypow x y)

main = do
  print $ myint (mypow (fromInteger_ 3) (fromInteger_ 8))
