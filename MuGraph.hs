module MuGraph where

import qualified Data.ByteString.Lazy as B (putStr,getContents)
import qualified Data.ByteString.Char8.Lazy as BC (pack)
import Text.Read (readMaybe)
import Data.Binary (encode)

out_bc :: FilePath -> IO ()
out_bc f = do
  q <- B.getContents $ f ++ ".mug"
  writeFile (f ++ ".muo") $ toBC $ words q

toBC :: [String] -> ByteString
toBC = concat .
  map (\x -> case readMaybe x :: Double of
                 Nothing -> case findIndex (x==) f of Nothing -> BC.pack "1"++encode (-1)::Int32
                                                      Just a  -> BC.pack "1"++encode a::Int32
                 Just a -> case readMaybe x :: Int32 of
                                Nothing -> BC.pack "1"++encode a::Double
                                Just a ->  BC.pack "0"++encode a::Int32)
