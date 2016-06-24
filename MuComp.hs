module MuComp where

data Item = NArr [Int] [Int] [Double] deriving (Show,Eq)
data NTyp = Any | NTyp [Char] | NArray NTyp Int | Stk NTyp
          | Fun [Char] [Int] [NTyp] [NTyp] deriving (Show,Eq)
data Lit = Lit Item NTyp deriving (Show,Eq)
--data Fun = Fun [Char] [Int] [NTyp] [NTyp] deriving (Show,Eq)

data Queued = D Int | I Int | Sz Int {- always 8 -} | S Int | deriving (Show,Eq)

prims :: [Lit]
prims = [(Fun "|" [] [Stk Any] [NArray Any 1])]

shp, ele :: [Int] -> Int
shp = product
ele = product

shape, eleN :: NArr -> Int
shape (NArr a _ _) = shp a
eleN  (NArr _ b _)  = ele b

frame :: [Int] -> Int
frame x = shp x * ele x

boxd :: NArr -> NArr -> NArr
boxd (NArr _ _ (a:_)) (NArr s e d) = let (s',e') = splitAt s (length s - 1 - a) in
  NArr s (e'++e) d

ind :: NArr -> NArr
ind (NArr a b d) i =
  NArr b [1] $ map (((!!) d) . (((ele b)*i%(shp a*ele b))+)) [0..ele b - 1]

stkType :: [Lit] -> [Ntyp]
stkType = map (\(Lit _ t) -> t)
stkRed :: [NTyp] -> [NTyp] -- Fits all duplicates into 'Stk' type.  e.g. Int Int Int -> {Int}
stkRed =
  foldr (\(x,n) -> if empty n then x:n
                   else case head n of (Stk a) -> if a == x then n else x:n
                                       a       -> if a == x then (Stk a):rest n else x:n) []

typeCheck :: [Ntyp] -> [NTyp] -> Bool
typeCheck f s = if length f > length s then False else drop (length f - length s) f == s

{-eval :: [Lit] -> [Lit]
eval = foldl (\(x,n)-}

{-appf :: [NArr] -> Fun -> NArr
appf ns (Fun name dims) =
  let bns = map (boxd . flip narri) ns dims in
      --i   = map (flip take [0..] . shape) ns in
  foldr appf_bin (last bns) (init bns)

appf_bin :: NArr -> NArr -> NArr
appf_bin (NArr aa ab ad) (NArr ba bb bd) =-}

