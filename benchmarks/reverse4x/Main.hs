data Nat = O | S Nat

add O n = n
add (S x) y = S (add x y)

double x = add x x

mult O _ = O
mult (S x) y = add y (mult x y)

two = S (S O)
four = double two
nat16 = mult four four
nat256 = mult nat16 nat16
nat4096 = mult nat256 nat16
nat16384 = mult nat4096 four
nat256k = mult nat16384 nat16
nat1M = mult nat256k four
nat16M = mult nat1M nat16
nat256M = mult nat16M nat16
nat1G = mult nat256M four

data MyBool = MyTrue | MyFalse deriving Show

append [] xs = xs
append (x:xs) ys = x : (append xs ys)

-- rev [] = []
-- rev (x:xs) = append (rev xs) [x]
rev [] = []
rev (x:[]) = [x]
rev (x:x':[]) = [x',x]
rev (x:x':x'':[]) = [x'',x',x]
rev (x:x':x'':x''':xs) = append (rev xs) [x''',x'',x',x]

natList O = []
natList (S x) = (S x) : (natList x)

-- isList [] = MyTrue
-- isList (_:xs) = isList xs
isList [] = MyTrue
isList (_:[]) = MyTrue
isList (_:_:[]) = MyTrue
isList (_:_:_:[]) = MyTrue
isList (_:_:_:xs) = isList xs

goal1 = rev (natList nat16)
goal2 = rev (natList nat256)
goal3 = isList (rev (natList nat4096))
goal4 = isList (rev (natList nat16384))

main = do
    print $ goal4
