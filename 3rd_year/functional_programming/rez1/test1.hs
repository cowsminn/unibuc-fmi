import Data.Char
import Data.Maybe
import Control.Monad
import Control.Applicative
import System.Random
import Data.List

-- examen 
type IPAddress = String 
data EquipmentType = Router | Switch | Host | Server 
data NetworkEquipment = NetworkEquipment 
    { eqName :: String, 
      eqType :: EquipmentType,
      ipAddresses :: [IPAddress]
    }

eq1 :: NetworkEquipment 
eq1 = NetworkEquipment { eqName = "R-CTI", eqType = Router, ipAddresses = ["139.208.0.1"] }

eq2 :: NetworkEquipment 
eq2 = NetworkEquipment { eqName = "SW-CTI", eqType = Switch, ipAddresses = ["139.208.0.2", "139.208.0.3"] }

eq3 :: NetworkEquipment
eq3 = NetworkEquipment { eqName = "HOST-CTI", eqType = Host, ipAddresses = ["139.208.0.10", "139.208.0.11", "139.208.0.12"] }

eq4 :: NetworkEquipment
eq4 = NetworkEquipment { eqName = "SERVER", eqType = Server, ipAddresses = ["139.208.15.254"] }

-- Exercitiul 1. instante pentru Eq si Show

instance Show EquipmentType where 
    show Router = "Router"
    show Switch = "Switch"
    show Host = "Host"
    show Server = "Server"

instance Eq EquipmentType where 
    Router == Router = True 
    Switch == Switch = True 
    Host == Host = True 
    Server == Server = True 
    _ == _ = False 

instance Show NetworkEquipment where 
    show (NetworkEquipment name eqType ipAddresses) = "NE { name = " ++ show name 
        ++ ", type = " ++ show eqType 
        ++ ", ipAddresses = " ++ show ipAddresses ++ " }"

instance Eq NetworkEquipment where 
    (NetworkEquipment name1 eqType1 ipAddresses1) == (NetworkEquipment name2 eqType2 ipAddresses2) = 
        name1 == name2 && eqType1 == eqType2 && ipAddresses1 == ipAddresses2

-- Exercitiul 2. Scrieti o functie care sa verifice daca o anumita adresa IP a fost deja utilizata pe unul dintre echipamente.
-- trebuie sa implementati cu foldr (din proprietatea de universalitate) si cu comprehensiune
-- si apoi sa verificati, folosind QuickCheck, ca cele doua implementari sunt echivalente (aparent)


ipExists :: IPAddress -> [NetworkEquipment] -> Bool 
ipExists _ [] = False 
ipExists ip (x:xs) 
    | ip `elem` ipAddresses x = True 
    | otherwise = ipExists ip xs 


-- vrem sa implementam cu un foldr 
{-
g [] = i 
g (x:xs) = f x (g xs) 
atunci g = foldr f i 

Pasul 1.
inversam, in implementarea foldr-ului (auxiliar) cele doua argumente ale functiei 

redenumim si functia cu g 
si schimbam din guards in if-then-else

g [] _ = False 
g (x:xs) ip = if (ip `elem` ipAddresses x) then True else g xs ip

Pasul 2.
aplicam currying / uncurrying astfel incat sa avem fix forma din proprietatea de universalitate

sum x y = x + y atunci sum x = \y -> x + y sau chiar sum = \x y -> x + y sau sum = \x -> \y -> x + y 

g [] = \_ -> False 
g (x:xs) = \ip -> if (ip `elem` ipAddresses x) then True else g xs ip

Pasul 3.
aplicam tranzitivitatea g (x:xs) = \ip -> if (ip `elem` ipAddresses x) then True else g xs ip
 si        f x (g xs) = g (x:xs)
f x (g xs) = \ip -> if (ip `elem` ipAddresses x) then True else g xs ip

Pasul 4.
Notam g xs = u 
f x u = \ip -> if (ip `elem` ipAddresses x) then True else u ip
=> f = \x u ip -> if (ip `elem` ipAddresses x) then True else u ip
   i = \_ -> False 

=> g = foldr f i cu f si i de mai sus 
-}

ipExistsFoldr' :: [NetworkEquipment] -> IPAddress -> Bool 
ipExistsFoldr' = foldr (\x u ip -> if (ip `elem` ipAddresses x) then True else u ip) (\_ -> False)

ipExistsFoldr :: IPAddress -> [NetworkEquipment] -> Bool 
ipExistsFoldr ip list = ipExistsFoldr' list ip 

ipExistsComprehension :: IPAddress -> [NetworkEquipment] -> Bool 
ipExistsComprehension ip list = length [neteq | neteq <- list, ip `elem` ipAddresses neteq] > 0

-- verificam daca ipExistsFoldr si ipExistsComprehension sunt egale dpdv statistic 
-- QuickCheck 


-- facem instante Arbitrary pentru tipurile noastre de date - EquipmentType, respectiv NetworkEquipment 

-- instance Arbitrary EquipmentType where 
--     arbitrary = elements [Router, Switch, Host, Server]

instance Arbitrary NetworkEquipment where 
    arbitrary = do 
        eqName <- arbitrary 
        eqType <- elements [Router, Switch, Host, Server]
        ipAddresses <- arbitrary 
        return (NetworkEquipment eqName eqType ipAddresses)


prop_qc :: IPAddress -> [NetworkEquipment] -> Bool
prop_qc ip list = ipExistsFoldr ip list == ipExistsComprehension ip list 

test_prop_qc = quickCheck prop_qc 

-- Exercitiul 3. Fie Log si NetworkWriter, spor la facut monade si exemple 

type Log = [String]
data NetworkWriter a = NetworkWriter { runNetworkWriter :: (a, Log) }
    deriving Show 

-- Functor -> Applicative -> Monad 
-- Functor: generalizare a functiei map 
-- map (\x -> x + 1) [1,2,3] => [2,3,4]
-- Applicative: ne permite sa aplicam functii din contexte monadice pe argumente din contexte monadice, cu rezultate in contexte monadice 
-- Monad: ne permite sa inlantuim actiuni in contexte monadice 

instance Functor NetworkWriter where 
    fmap f (NetworkWriter (a, log)) = NetworkWriter (f a, log)

instance Applicative NetworkWriter where 
    pure x = NetworkWriter (x, [])
    (NetworkWriter (f, log1)) <*> (NetworkWriter (a, log2)) = NetworkWriter (f a, log1 ++ log2)

instance Monad NetworkWriter where 
    return = pure 
    (NetworkWriter (a, log1)) >>= f =
        let (NetworkWriter (b, log2)) = f a 
        in NetworkWriter (b, log1 ++ log2)

addEquipment :: NetworkEquipment -> [NetworkEquipment] -> NetworkWriter [NetworkEquipment]
addEquipment eq eqs = NetworkWriter (eq : eqs, ["Added equipment: " ++ show eq])

removeEquipment :: String -> [NetworkEquipment] -> NetworkWriter [NetworkEquipment]
removeEquipment name eqs = 
    let newEqs = filter (\ne -> name /= eqName ne) eqs 
    in NetworkWriter (newEqs, ["Removed equipment with name: " ++ show name])

runExample :: [NetworkEquipment] -> NetworkWriter [NetworkEquipment]
runExample equipments = do 
    eqs1 <- addEquipment eq1 equipments 
    eqs2 <- addEquipment eq2 eqs1 
    eqs3 <- removeEquipment "R-CTI" eqs2 
    return eqs3 

main :: IO ()
main = do 
    let equipments = []
    let (newEquipments, log) = runNetworkWriter (runExample equipments)
    mapM_ putStrLn log 


-----------------------------------------------------------------------------------------------------------------------
-- nu schimbati nimic mai jos daca vreti sa mearga ceva 

 ------Module QuickCheck-----------------------------------
 --- sursa: https://www.cse.chalmers.se/~rjmh/QuickCheck/QuickCheck.hs

infixr 0 ==>
infix  1 `classify`

--------------------------------------------------------------------
-- Generator

newtype Gen a
  = Gen (Int -> StdGen -> a)

sized :: (Int -> Gen a) -> Gen a
sized fgen = Gen (\n r -> let Gen m = fgen n in m n r)

resize :: Int -> Gen a -> Gen a
resize n (Gen m) = Gen (\_ r -> m n r)

rand :: Gen StdGen
rand = Gen (\n r -> r)

promote :: (a -> Gen b) -> Gen (a -> b)
promote f = Gen (\n r -> \a -> let Gen m = f a in m n r)

variant :: Int -> Gen a -> Gen a
variant v (Gen m) = Gen (\n r -> m n (rands r !! (v+1)))
 where
  rands r0 = r1 : rands r2 where (r1, r2) = split r0

generate :: Int -> StdGen -> Gen a -> a
generate n rnd (Gen m) = m size rnd'
 where
  (size, rnd') = randomR (0, n) rnd

--instance Functor Gen where
--  fmap f m = m >>= return . f

instance Monad Gen where
  Gen m >>= k =
    Gen (\n r0 -> let (r1,r2) = split r0
                      Gen m'  = k (m n r1)
                   in m' n r2)
                   
instance Applicative Gen where
  pure a = Gen (\n r -> a)
  mf <*> ma = do
    f <- mf
    a <- ma
    return (f a)       

instance Functor Gen where              
  fmap f ma = pure f <*> ma                     

-- derived

choose :: Random a => (a, a) -> Gen a
choose bounds = (fst . randomR bounds) `fmap` rand

elements :: [a] -> Gen a
elements xs = (xs !!) `fmap` choose (0, length xs - 1)

vector :: Arbitrary a => Int -> Gen [a]
vector n = sequence [ arbitrary | i <- [1..n] ]

oneof :: [Gen a] -> Gen a
oneof gens = elements gens >>= id

frequency :: [(Int, Gen a)] -> Gen a
frequency xs = choose (1, tot) >>= (`pick` xs)
 where
  tot = sum (map fst xs)

  pick n ((k,x):xs)
    | n <= k    = x
    | otherwise = pick (n-k) xs

-- general monadic

two :: Monad m => m a -> m (a, a)
two m = liftM2 (,) m m

three :: Monad m => m a -> m (a, a, a)
three m = liftM3 (,,) m m m

four :: Monad m => m a -> m (a, a, a, a)
four m = liftM4 (,,,) m m m m

--------------------------------------------------------------------
-- Arbitrary

class Arbitrary a where
  arbitrary   :: Gen a
  --coarbitrary :: a -> Gen b -> Gen b

instance Arbitrary () where
  arbitrary     = return ()
 -- coarbitrary _ = variant 0

instance Arbitrary Bool where
  arbitrary     = elements [True, False]
  --coarbitrary b = if b then variant 0 else variant 1

instance Arbitrary Char where
  arbitrary     = choose (32,255) >>= \n -> return (chr n)
 -- coarbitrary n = variant (ord n)

instance Arbitrary Int where
  arbitrary     = sized $ \n -> choose (-n,n)
 -- coarbitrary n = variant (if n >= 0 then 2*n else 2*(-n) + 1)


instance Arbitrary Integer where
  arbitrary     = sized $ \n -> choose (-fromIntegral n,fromIntegral n)
  --coarbitrary n = variant $ (fromInteger(if n >= 0 then 2*n else 2*(-n) + 1)) 

instance Arbitrary Float where
  arbitrary     = liftM3 fraction arbitrary arbitrary arbitrary 
  --coarbitrary x = coarbitrary (decodeFloat x)

instance Arbitrary Double where
  arbitrary     = liftM3 fraction arbitrary arbitrary arbitrary 
 -- coarbitrary x = coarbitrary (decodeFloat x)

fraction a b c = fromInteger a + (fromInteger b / (abs (fromInteger c) + 1))

{-
instance Arbitrary Integer where
  arbitrary     = sized $ \n -> choose (-fromInt n,fromInt n)
  coarbitrary n = variant (fromInteger (if n >= 0 then 2*n else 2*(-n) + 1))

instance Arbitrary Float where
  arbitrary     = liftM3 fraction arbitrary arbitrary arbitrary 
  coarbitrary x = coarbitrary (decodeFloat x)

instance Arbitrary Double where
  arbitrary     = liftM3 fraction arbitrary arbitrary arbitrary 
  coarbitrary x = coarbitrary (decodeFloat x)

fraction a b c = fromInteger a + (fromInteger b / (abs (fromInteger c) + 1))
-}


instance (Arbitrary a, Arbitrary b) => Arbitrary (a, b) where
  arbitrary          = liftM2 (,) arbitrary arbitrary
 -- coarbitrary (a, b) = coarbitrary a . coarbitrary b

instance (Arbitrary a, Arbitrary b, Arbitrary c) => Arbitrary (a, b, c) where
  arbitrary             = liftM3 (,,) arbitrary arbitrary arbitrary
  --coarbitrary (a, b, c) = coarbitrary a . coarbitrary b . coarbitrary c

instance (Arbitrary a, Arbitrary b, Arbitrary c, Arbitrary d)
      => Arbitrary (a, b, c, d)
 where
  arbitrary = liftM4 (,,,) arbitrary arbitrary arbitrary arbitrary
  --coarbitrary (a, b, c, d) =
  --  coarbitrary a . coarbitrary b . coarbitrary c . coarbitrary d

instance Arbitrary a => Arbitrary [a] where
  arbitrary          = sized (\n -> choose (0,n) >>= vector)
 -- coarbitrary []     = variant 0
 -- coarbitrary (a:as) = coarbitrary a . variant 1 . coarbitrary as

--instance (Arbitrary a, Arbitrary b) => Arbitrary (a -> b) where
 -- arbitrary         = promote (`coarbitrary` arbitrary)
 -- coarbitrary f gen = arbitrary >>= ((`coarbitrary` gen) . f)

--------------------------------------------------------------------
-- Testable

data Result
  = Result { ok :: Maybe Bool, stamp :: [String], arguments :: [String] }

nothing :: Result
nothing = Result{ ok = Nothing, stamp = [], arguments = [] }

newtype Property
  = Prop (Gen Result)

result :: Result -> Property
result res = Prop (return res)

evaluate :: Testable a => a -> Gen Result
evaluate a = gen where Prop gen = property a

class Testable a where
  property :: a -> Property

instance Testable () where
  property _ = result nothing

instance Testable Bool where
  property b = result (nothing{ ok = Just b })

instance Testable Result where
  property res = result res

instance Testable Property where
  property prop = prop

instance (Arbitrary a, Show a, Testable b) => Testable (a -> b) where
  property f = forAll arbitrary f

forAll :: (Show a, Testable b) => Gen a -> (a -> b) -> Property
forAll gen body = Prop $
  do a   <- gen
     res <- evaluate (body a)
     return (argument a res)
 where
  argument a res = res{ arguments = show a : arguments res }

(==>) :: Testable a => Bool -> a -> Property
True  ==> a = property a
False ==> a = property ()

label :: Testable a => String -> a -> Property
label s a = Prop (add `fmap` evaluate a)
 where
  add res = res{ stamp = s : stamp res }

classify :: Testable a => Bool -> String -> a -> Property
classify True  name = label name
classify False _    = property

trivial :: Testable a => Bool -> a -> Property
trivial = (`classify` "trivial")

collect :: (Show a, Testable b) => a -> b -> Property
collect v = label (show v)

--------------------------------------------------------------------
-- Testing

data Config = Config
  { maxTest :: Int
  , maxFail :: Int
  , size    :: Int -> Int
  , every   :: Int -> [String] -> String
  }

quick :: Config
quick = Config
  { maxTest = 100
  , maxFail = 1000
  , size    = (+ 3) . (`div` 2)
  , every   = \n args -> let s = show n in s ++ [ '\b' | _ <- s ]
  }
         
verbose :: Config
verbose = quick
  { every = \n args -> show n ++ ":\n" ++ unlines args
  }

test, quickCheck, verboseCheck :: Testable a => a -> IO ()
test         = check quick
quickCheck   = check quick
verboseCheck = check verbose
         
check :: Testable a => Config -> a -> IO ()
check config a =
  do rnd <- newStdGen
     tests config (evaluate a) rnd 0 0 []

tests :: Config -> Gen Result -> StdGen -> Int -> Int -> [[String]] -> IO () 
tests config gen rnd0 ntest nfail stamps
  | ntest == maxTest config = do done "OK, passed" ntest stamps
  | nfail == maxFail config = do done "Arguments exhausted after" ntest stamps
  | otherwise               =
      do putStr (every config ntest (arguments result))
         case ok result of
           Nothing    ->
             tests config gen rnd1 ntest (nfail+1) stamps
           Just True  ->
             tests config gen rnd1 (ntest+1) nfail (stamp result:stamps)
           Just False ->
             putStr ( "Falsifiable, after "
                   ++ show ntest
                   ++ " tests:\n"
                   ++ unlines (arguments result)
                    )
     where
      result      = generate (size config ntest) rnd2 gen
      (rnd1,rnd2) = split rnd0

done :: String -> Int -> [[String]] -> IO ()
done mesg ntest stamps =
  do putStr ( mesg ++ " " ++ show ntest ++ " tests" ++ table )
 where
  table = display
        . map entry
        . reverse
        . sort
        . map pairLength
        . group
        . sort
        . filter (not . null)
        $ stamps

  display []  = ".\n"
  display [x] = " (" ++ x ++ ").\n"
  display xs  = ".\n" ++ unlines (map (++ ".") xs)

  pairLength xss@(xs:_) = (length xss, xs)
  entry (n, xs)         = percentage n ntest
                       ++ " "
                       ++ concat (intersperse ", " xs)

  percentage n m        = show ((100 * n) `div` m) ++ "%"

--------------------------------------------------------------------
-- the end.
