module MuGraph (out_bc) where

import qualified Data.ByteString.Lazy as B (ByteString(..),putStr,getContents,concat,append
                                           ,writeFile,reverse)
import qualified Data.ByteString.Lazy.Char8 as BC (pack)
import Text.Read (readMaybe)
import Data.Binary (encode)
import Data.List (findIndex)
import Data.Int (Int64(..),Int8(..))
import Foreign

out_bc :: FilePath -> String -> IO ()
out_bc f str = do
  B.writeFile (f ++ ".muo") (toBC $ words str)

toBC :: [String] -> B.ByteString
toBC = B.concat .
  map (\x -> case readMaybe x :: Maybe Double of
                 Nothing -> case findIndex (x==) f of Nothing -> encode ((-1)::Int8)
                                                      Just a  -> encode (((fromIntegral a)+3)::Int8)
                 Just a -> case readMaybe x :: Maybe Int64 of
                                Nothing -> B.append (encode (1::Int8)) $ B.reverse $ encode (a::Double)
                                Just a ->  B.append (encode (0::Int8)) $ B.reverse $ encode (a::Int64))

f = ["T","E"]

------------------------------------------------------

data NArr = NArr [Int] [Int] [Double] deriving (Show,Eq)
data Item = D Double | I Int | Dw [Word8] | Iw [Word8] | Op Word8 deriving (Show,Eq)

data Queued = D Int | I Int deriving (Show,Eq)

shp, ele :: [Int] -> Int
shp = product
ele   = product

frame :: [Int] -> Int
frame x = shape x * ele x

boxd :: NArr -> NArr -> NArr
boxd (NArr _ _ (a:_)) (NArr s e d) = let (s',e') = splitAt s (length s - 1 - a) in
  NArr s (e'++e) d

ind :: NArr -> Int
ind (NArr a b d) i = map (((!!) d) . (((length a)*i%(shp a*ele b))+)) [0..shp b - 1]

lexMachine :: [Word8] -> [Item]
lexMachine = foldl (\(x,(a,b)) -> if empty $ head n then case x of 0 -> (a++[I 8],b)
                                                                   1 -> (a++[D 8],b)
                                                                   _ -> (a,b++[Op x])
                                  else let ((I n):es) = a in (if n == 1 then es else ((I (n-1)):es)
                                                             ,init b++(last b++x))) ([],[])

in_bc_h :: FilePath -> IO NArr
  a <- unpack `fmap` B.readFile
  lexMachine a
