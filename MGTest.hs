import MuGraph
import System.Random

dat = ["T 2 8 8",
       " 0 1 0 1 1 0 1 1 ",
       " 1 0 0 0 1 1 0 0 ",
       " 0 1 0 1 0 1 1 0 ",
       " 1 1 0 1 1 0 1 1 ",
       " 1 0 1 1 0 0 0 1 ",
       " 1 0 0 0 0 0 0 1 ",
       " 0 0 0 1 1 1 0 0 ",
       " 1 0 0 1 0 0 0 0 E"]

main = do out_bc "out" $ concat dat
