module Primality 
(
  testPrimality,
  testPrimalityR,
) where

import System.Random

-- find2km (2^k * m) = (k,m)
find2km :: Integral a => a -> (a,a)
find2km n = f 0 n
    where 
        f k m
          | r == 1 = (k,m)
          | otherwise = f (k+1) q
          where (q,r) = quotRem m 2  

-- n is the number to test; a is the (presumably randomly chosen) witness
millerRabinPrimality :: RandomGen g => Integer -> Int -> g -> (Bool, g)
millerRabinPrimality n round gen
    | round == 0          = (True,  gen)
    | b0 `elem` [1, n']   = millerRabinPrimality n (round - 1) gen'
    | otherwise           = iter (tail b)
    where
        n' = n - 1
        (a, gen') = randomR (2, n - 2) gen
        (k, m) = find2km n'
        b0 = powMod n a m
        b = take (fromIntegral $! k) $ iterate (squareMod n) b0
        iter [] = (False, gen')
        iter (x:xs)
            | x == 1 = (False, gen')
            | x == n' = millerRabinPrimality n (round - 1) gen'
            | otherwise = iter xs

testPrimality :: RandomGen g => Integer -> g -> (Bool, g)
testPrimality 1 gen = (True, gen)
testPrimality 2 gen = (True, gen)
testPrimality n gen | n < 2 || even n = (False, gen)
	            | otherwise = millerRabinPrimality n (floor . logBase 2 . fromInteger $ n) gen

testPrimalityR :: RandomGen g => [Integer] -> g -> [Integer]
testPrimalityR [] g = []
testPrimalityR (x:xs) g | p = x : (testPrimalityR xs g')
                        | otherwise = testPrimalityR xs g'
                        where (p, g') = testPrimality x g
    
mulMod :: Integral a => a -> a -> a -> a
mulMod a b c = (b * c) `mod` a

squareMod :: Integral a => a -> a -> a
squareMod a b = (b * b) `rem` a

-- (eq. to) pow' (*) (^2) n k = n^k
pow' :: (Num a, Integral b) => (a->a->a) -> (a->a) -> a -> b -> a
pow' _ _ _ 0 = 1
pow' mul sq x' n' = f x' n' 1
    where 
        f x n y
            | n == 1 = x `mul` y
            | r == 0 = f x2 q y
            | otherwise = f x2 q (x `mul` y)
            where
                (q, r) = quotRem n 2
                x2 = sq x

-- (eq. to) powMod m n k = n^k `mod` m
powMod :: Integral a => a -> a -> a -> a
powMod m = pow' (mulMod m) (squareMod m)

