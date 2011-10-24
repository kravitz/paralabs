import Primality
import System.Random
import Control.Monad

testPrimalitySimple x = all (\d -> x `mod` d /= 0) [2..(floor . sqrt . fromIntegral $ x)]

main = do
    probPrimes <- flip filterM [3..250000] (\num -> do
            gen <- getStdGen
            let (ans, gen') = testPrimality num gen
            setStdGen gen'
            return ans
        )
    print (length probPrimes)
    return ()
