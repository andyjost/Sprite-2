data BT = Leaf | Branch Int BT BT
insert x Leaf = Branch x Leaf Leaf
insert x (Branch y l r) | x < y = Branch y (insert x l) r
                        | y < x = Branch y l (insert x r)
			                  | x == y = Branch y l r

count Leaf = 0
count (Branch _ l r) = 1 + count l + count r

tree_loop n x t = if n==0 then t
   else tree_loop (n-1) (next x) (insert x t)

m1 = 16381
a1 = 3007
m2 = 32749
a2 = 1944
halfseed = 15809     -- must be less than m2
count_ = 200000
seed = halfseed*m2+halfseed
next x = y
  where p = div x m2
        q = mod x m2
        r = mod (a1 * p) m1
        s = mod (a2 * q) m2
        y = r * m2 + s

-- count so it does not print a big tree
main = do
  print $ count (tree_loop count_ seed Leaf)
