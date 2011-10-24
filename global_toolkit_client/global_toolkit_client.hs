module Main where

import Primality
import System.Random
import Control.Monad

testPrimalitySimple x = all (\d -> x `mod` d /= 0) [2..(floor . sqrt . fromIntegral $ x)]

main :: IO ()
main = do
    gen <- getStdGen
    let probPrimes = testPrimalityR [1000000..2000000] gen
    print (last $! probPrimes)
    return ()
