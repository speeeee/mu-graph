-- This program simply rewrites the program in RPN changing names to numeric codes and
--   assigning an ID for each type.  Actual processing of the program happens in another program.

import qualified Data.ByteString.Lazy as B (getContents,putStr)
import qualified Data.ByteString.Char8.Lazy as BC (pack)
import Text.Read (readMaybe)
import Data.Binary (encode)

main = do
  (x:_) <- getArgs
  y <- B.getContents $ x ++ ".mug"
  writeFile (x ++ ".muo") $ out $ toRPN $ words y

toRPN :: [String] -> [String]
toRPN = reverse . filter (\x -> not $ elem x ["[","]"])

out :: [String] -> ByteString
out = concat .
  map (\x -> case readMaybe x :: Double of
                 Nothing -> case findIndex (x==) f of Nothing -> (BC.pack "1")++encode (-1)::Int32
                                                      Just a  -> (BC.pack "1")++encode a::Int32
                 Just a -> (BC.pack "0")++encode a::Double)

f = ["line","load","dup","save"]
