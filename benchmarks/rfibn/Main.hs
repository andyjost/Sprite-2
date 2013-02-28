nfib :: Int -> Int
nfib n = if n <= 1 then 1 else nfib (n-1) + nfib (n-2) + 1

main = do
  print $ nfib 35
