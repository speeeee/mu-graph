module MuGraph (out_bc) where

import qualified Data.ByteString.Lazy as B (ByteString(..),putStr,getContents,concat,append,writeFile)
import qualified Data.ByteString.Lazy.Char8 as BC (pack)
import Text.Read (readMaybe)
import Data.Binary (encode)
import Data.List (findIndex)
import Data.Int (Int64(..),Int8(..))

out_bc :: FilePath -> String -> IO ()
out_bc f str = do
  B.writeFile (f ++ ".muo") (toBC $ words str)

toBC :: [String] -> B.ByteString
toBC = B.concat .
  map (\x -> case readMaybe x :: Maybe Double of
                 Nothing -> case findIndex (x==) f of Nothing -> encode ((-1)::Int8)
                                                      Just a  -> encode (((fromIntegral a)+3)::Int8)
                 Just a -> case readMaybe x :: Maybe Int64 of
                                Nothing -> B.append (encode (1::Int8)) $ encode (a::Double)
                                Just a ->  B.append (encode (0::Int8)) $ encode (a::Int64))

f = ["T","E"]
