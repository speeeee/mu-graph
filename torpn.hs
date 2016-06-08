-- This program simply rewrites the program in RPN changing names to numeric codes and
--   assigning an ID for each type.  Actual processing of the program happens in another program.

import Text.Read (readMaybe)

main = do
  (x:_) <- getArgs
  y <- readFile $ x ++ ".mug"
  writeFile (x ++ ".muo") $ typify $ toRPN $ words y

toRPN :: [String] -> [String]
toRPN = reverse . filter (\x -> not $ elem x ["[","]"])

typify = intersperse ' ' . concat . map (\x -> case readMaybe x :: Maybe Double of
                                                 Nothing -> ["1", case findIndex (x==) f of Nothing -> (-1)
                                                                                            Just a -> a]
                                                 Just _  -> ["0", x])

f = ["line","load","dup","save"]
