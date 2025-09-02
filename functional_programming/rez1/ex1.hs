import Test.QuickCheck


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

instance Show EquipmentType where
	show Router = "Router"
	show Switch = "Switch"
	show Host = "Host"
	show Server = "Server"

instance Eq EquipmentType where
	(==) Router Router = True
	(==) Switch Switch = True
	(==) Host Host = True
	(==) Server Server = True
	(==) _ _ = False

instance Show NetworkEquipment where
	show (NetworkEquipment name eqType ipAddrs) =
		"Name: " ++ name ++ ", Type: " ++ show eqType ++ ", IPs: " ++ show ipAddrs

instance Eq NetworkEquipment where
	(NetworkEquipment name1 eqType1 ipAddrs1) == (NetworkEquipment name2 eqType2 ipAddrs2) =
		name1 == name2 && eqType1 == eqType2 && ipAddrs1 == ipAddrs2

-- Versiunea recursivă
ipExistsRec :: IPAddress -> [NetworkEquipment] -> Bool
ipExistsRec _ [] = False  -- Lista goală -> False
ipExistsRec ip (eq:eqs) = 
    if ip `elem` (ipAddresses eq)  -- Verifică dacă IP-ul este în echipamentul curent
    then True                      -- Dacă da, returnează True
    else ipExistsRec ip eqs        -- Dacă nu, continuă cu restul listei

-- Versiunea cu list comprehension
ipExistsComprehension :: IPAddress -> [NetworkEquipment] -> Bool
ipExistsComprehension ip eqs = 
    not (null [eq | eq <- eqs, ip `elem` (ipAddresses eq)])
    -- Creează o listă cu echipamentele care conțin IP-ul
    -- Dacă lista nu e goală, înseamnă că IP-ul există


-- Versiunea cu funcții de ordin superior
ipExistsHighOrderFunctions :: IPAddress -> [NetworkEquipment] -> Bool
ipExistsHighOrderFunctions ip eqs = 
    any (\eq -> ip `elem` (ipAddresses eq)) eqs
    -- `any` verifică dacă cel puțin un element satisface condiția
    -- Lambda function verifică dacă IP-ul este în echipamentul curent

-- Versiunea cu foldr
ipExistsFoldr :: IPAddress -> [NetworkEquipment] -> Bool
ipExistsFoldr ip eqs = foldr (\eq acc -> ip `elem` (ipAddresses eq) || acc) False eqs

testFoldrEquivalence :: IO ()
testFoldrEquivalence = do
    let equipments = [eq1, eq2, eq3, eq4]
    let testIPs = ["139.208.0.1", "139.208.0.10", "192.168.1.1"]
    
    putStrLn "=== Verificare echivalență Rec vs Foldr ==="
    mapM_ (\ip -> do
        let recResult = ipExistsRec ip equipments
        let foldrResult = ipExistsFoldr ip equipments
        putStrLn $ "IP: " ++ ip ++ " | Rec: " ++ show recResult ++ " | Foldr: " ++ show foldrResult ++ " | Equal: " ++ show (recResult == foldrResult)) testIPs

-- Instanța Arbitrary pentru EquipmentType
instance Arbitrary EquipmentType where
    arbitrary = elements [Router, Switch, Host, Server]

-- Instanța Arbitrary pentru NetworkEquipment
instance Arbitrary NetworkEquipment where
    arbitrary = do
        name <- arbitrary  -- String arbitrar
        eqType <- arbitrary  -- EquipmentType arbitrar
        ipCount <- choose (1, 5)  -- Între 1 și 5 IP-uri
        ips <- vectorOf ipCount arbitrary  -- Lista de IP-uri (String-uri)
        return $ NetworkEquipment name eqType ips

-- Proprietatea pentru QuickCheck: comprehension și foldr sunt echivalente
prop_qc :: IPAddress -> [NetworkEquipment] -> Bool
prop_qc ip eqs = ipExistsComprehension ip eqs == ipExistsFoldr ip eqs

-- Funcția de test
test_prop_qc :: IO ()
test_prop_qc = quickCheck prop_qc

-- Tipul Log pentru mesaje de logging
data Log = Log { equipmentType :: String, messageLog :: String }
    deriving (Eq, Show)

-- Tipul NetworkWriter pentru operații cu logging
data NetworkWriter a = NetworkWriter { runNetworkWriter :: (a, Log) }
    deriving Show

-- Instanța Semigroup pentru Log (concatenare de mesaje)
instance Semigroup Log where
    (Log eqType1 msg1) <> (Log eqType2 msg2) = 
        Log (eqType1 ++ ", " ++ eqType2) (msg1 ++ "; " ++ msg2)

-- Instanța Monoid pentru Log (element neutru - log gol)
instance Monoid Log where
    mempty = Log "" ""

-- Instanța Semigroup pentru NetworkWriter
instance Semigroup a => Semigroup (NetworkWriter a) where
    (NetworkWriter (a1, log1)) <> (NetworkWriter (a2, log2)) = 
        NetworkWriter (a1 <> a2, log1 <> log2)

-- Instanța Monoid pentru NetworkWriter
instance Monoid a => Monoid (NetworkWriter a) where
    mempty = NetworkWriter (mempty, mempty)

-- Instanța Functor pentru NetworkWriter
instance Functor NetworkWriter where
    fmap f (NetworkWriter (a, log)) = NetworkWriter (f a, log)

-- Instanța Applicative pentru NetworkWriter
instance Applicative NetworkWriter where
    pure a = NetworkWriter (a, mempty)
    (NetworkWriter (f, log1)) <*> (NetworkWriter (a, log2)) = 
        NetworkWriter (f a, log1 <> log2)

-- Instanța Monad pentru NetworkWriter
instance Monad NetworkWriter where
    (NetworkWriter (a, log1)) >>= f = 
        let NetworkWriter (b, log2) = f a
        in NetworkWriter (b, log1 <> log2)

-- Funcție pentru a crea un log simplu
mkLog :: String -> String -> Log
mkLog eqType msg = Log eqType msg

-- Funcție pentru a rula NetworkWriter și extrage rezultatul
runNW :: NetworkWriter a -> (a, Log)
runNW = runNetworkWriter

-- Funcție pentru a adăuga logging la operații
logOperation :: String -> String -> a -> NetworkWriter a
logOperation eqType msg value = NetworkWriter (value, mkLog eqType msg)

-- Teste pentru instanțe
testLogInstances :: IO ()
testLogInstances = do
    putStrLn "=== Test Semigroup Log ==="
    let log1 = Log "Router" "Started"
    let log2 = Log "Switch" "Connected"
    print (log1 <> log2)
    
    putStrLn "\n=== Test Monoid Log ==="
    print (mempty :: Log)
    print (log1 <> mempty)
    
    putStrLn "\n=== Test NetworkWriter Monad ==="
    let nw1 = logOperation "Router" "Init" 42
    let nw2 = logOperation "Switch" "Process" 10
    let result = do
            x <- nw1
            y <- nw2
            return (x + y)
    print (runNW result)