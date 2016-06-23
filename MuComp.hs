module MuComp where

data NArr = NArr [Int] [Int] [Double] deriving (Show,Eq)
data Fun = Fun [Char] [Int] deriving (Show,Eq)
data Item = D Double | I Int | Dw [Word8] | Iw [Word8] | Op Word8 deriving (Show,Eq)

data Queued = D Int | I Int | Sz Int {- always 8 -} | S Int | deriving (Show,Eq)

shp, ele :: [Int] -> Int
shp = product
ele   = product

frame :: [Int] -> Int
frame x = shp x * ele x

boxd :: NArr -> NArr -> NArr
boxd (NArr _ _ (a:_)) (NArr s e d) = let (s',e') = splitAt s (length s - 1 - a) in
  NArr s (e'++e) d

ind :: NArr -> Int
ind (NArr a b d) i = map (((!!) d) . (((ele b)*i%(shp a*ele b))+)) [0..ele b - 1]

{-appf_print :: [NArr] -> Fun -> IO ()
appf_print ns (Fun name dims) = do
  let bns = map (boxd . flip narri) ns dims-}

