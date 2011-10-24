import Primality
import System.Random
import Control.Monad

testPrimalitySimple x = all (\d -> x `mod` d /= 0) [2..(floor . sqrt . fromIntegral $ x)]

main = do
    gen <- getStdGen
    let probPrimes = testPrimalityR [3..250000] gen
    print (length probPrimes)
    return ()
