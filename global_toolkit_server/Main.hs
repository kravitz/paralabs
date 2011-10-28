module Main where
import Prelude hiding (catch)
import Network (listenOn, accept, sClose, Socket, withSocketsDo, PortID(..), HostName, PortNumber)
import System.IO
import System.Environment (getArgs)
import Control.Applicative
import Control.Exception  (finally, catch, SomeException)
import Control.Concurrent
import Control.Concurrent.STM
import Control.Monad (forM, filterM, liftM, when)

type Client = (TChan String, Handle, ClientInfo)
type ClientInfo = (HostName, PortNumber)


catchAll :: IO a -> (SomeException -> IO a) -> IO a
catchAll = catch

start servSock = do
    acceptChan <- atomically newTChan
    forkIO $ acceptLoop servSock acceptChan
    mainLoop servSock acceptChan []

clientLoop :: Handle -> TChan String -> IO ()
clientLoop handle chan =
    listenLoop (hGetLine handle) chan
                    `catchAll` const (return ())
                    `finally` hClose handle

listenLoop :: IO a -> TChan a -> IO ()
listenLoop act chan =
    sequence_ (repeat (act >>= atomically . writeTChan chan))

acceptLoop :: Socket -> TChan Client -> IO ()
acceptLoop servSock chan = do
    (cHandle, host, port) <- accept servSock
    cChan <- atomically newTChan
    cTID  <- forkIO $ clientLoop cHandle cChan
    atomically $ writeTChan chan (cChan, cHandle, (host, port))
    acceptLoop servSock chan

clientInfo :: ClientInfo -> String
clientInfo (h, p) = h ++ ":" ++ (show p)

mainLoop :: Socket -> TChan Client -> [Client] -> IO ()
mainLoop servSock acceptChan clients = do
    r <- atomically $ (Left `fmap` readTChan acceptChan)
                      `orElse`
                      (Right `fmap` tselect clients)
    case r of
        Left client@(ch,h,ci) -> do
            putStrLn $ "new client [" ++ clientInfo(ci) ++ "]"
            mainLoop servSock acceptChan $ client:clients
        Right (line,_,_) -> do
            putStrLn $ "data: " ++ line
            clients' <- forM clients $
                         \client@(ch,h,_) -> do
                            hPutStrLn h line
                            hFlush h
                            return [client]
                         `catchAll` const (hClose h >> return [])
            let dropped = length $ filter null clients'
            when (dropped > 0) $
                putStrLn ("clients lost: " ++ show dropped)
            mainLoop servSock acceptChan $ concat clients'

tselect :: [(TChan a, t, c)] -> STM (a, t, c)
tselect = foldl orElse retry . map (\(ch, ty, ci) -> (,,) <$> (readTChan ch) <*> (return ty) <*> (return ci))

main :: IO ()
main = withSocketsDo $ do
    [portStr] <- getArgs
    let port = fromIntegral (read portStr :: Int)
    servSock <- listenOn (PortNumber port)
    putStrLn $ "listening on: " ++ show port
    start servSock `finally` sClose servSock
