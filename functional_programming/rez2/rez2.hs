import Test.QuickCheck

type Pos = Int

data Elem = Head | Tail | Food

data GamePosition = GamePos {
	xPos :: Pos,
	yPos :: Pos
}

data GameBoard = GameBoard [(Elem, GamePosition)]

--1 luam maxim

instance Show Elem where
	show Head = "Head";
	show Tail = "Tail";
	show Food = "Food"

instance Eq Elem where
	Head == Head = True
	Tail == Tail = True
	Food == Food = True
	_ == _ = False;

instance Show GamePosition where
	show (GamePos x y) =
		"xPos: " ++ show x ++ ", yPos: " ++ show y

instance Eq GamePosition where
	(GamePos x1 y1) == (GamePos x2 y2) =
		x1 == x2 && y1 == y2

-- pentru test daca nu vrei sa scrii in terminal 
-- Funcție de test pentru GamePosition
--testGamePosition :: IO ()
--testGamePosition = do
  --  putStrLn "=== Test GamePosition ==="
    --let pos1 = GamePos 5 10
  --  let pos2 = GamePos (-2) 15
  --  let pos3 = GamePos 0 0
    
   -- putStrLn $ "pos1: " ++ show pos1
 --   putStrLn $ "pos2: " ++ show pos2
 --   putStrLn $ "pos3: " ++ show pos3
 --   putStrLn $ "pos1 == pos1: " ++ show (pos1 == pos1)
 --   putStrLn $ "pos1 == pos2: " ++ show (pos1 == pos2)

-- Implementare prin recursie
getHead :: GameBoard -> Maybe GamePosition
getHead (GameBoard []) = Nothing
getHead (GameBoard ((Head, pos):_)) = Just pos
getHead (GameBoard (_:xs)) = getHead (GameBoard xs)

-- Implementare prin foldr
getHeadFoldr :: GameBoard -> Maybe GamePosition
getHeadFoldr (GameBoard board) = 
    foldr (\(elem, pos) acc -> case elem of
                                Head -> Just pos
                                _    -> acc) Nothing board

--comprehension
getHeadComprehension :: GameBoard -> Maybe GamePosition
getHeadComprehension (GameBoard board) =
    case [pos | (Head, pos) <- board] of
		[] -> Nothing
		(head:_) -> Just head

--cv show 
instance Show GameBoard where
	show (GameBoard []) = "GameBoard is empty"
	show (GameBoard board) = "GameBoard: " ++ show (map (\(elem, pos) -> (show elem, show pos)) board)

--Arbitrary
instance Arbitrary Elem where
	arbitrary = elements [Head, Tail, Food]

instance Arbitrary GamePosition where
	arbitrary = do
		x <- arbitrary
		y <- arbitrary
		return (GamePos x y)

instance Arbitrary GameBoard where
    arbitrary = do
        board <- arbitrary
        return (GameBoard board)

prop_getHeadEquiv :: GameBoard -> Bool
prop_getHeadEquiv gb = getHead gb == getHeadFoldr gb

-- Proprietate QuickCheck pentru echivalența getHead și getHeadComprehension
prop_getHeadComprehensionEquiv :: GameBoard -> Bool
prop_getHeadComprehensionEquiv gb = getHead gb == getHeadComprehension gb

-- Funcția principală pentru testare
testGetHeadProperties :: IO ()
testGetHeadProperties = do
	
    putStrLn "Testing getHead vs getHeadFoldr:"
    quickCheck prop_getHeadEquiv
    
    putStrLn "\nTesting getHead vs getHeadComprehension:"
    quickCheck prop_getHeadComprehensionEquiv