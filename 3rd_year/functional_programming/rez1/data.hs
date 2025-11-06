import Test.QuickCheck

type IPAddress = String
data EquipmentType = Router | Switch | Host | Server
data NetworkEquipment = NetworkEquipment
	{ eqName :: String,
	  eqType :: EquipmentType,
	  ipAddresses :: [IPAddress]
	}
;;
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


ipExistsRec :: IPAddress -> [NetworkEquipment] -> Bool
ipExistsRec _ [] = False
ipExistsRec ip (eq:eqs) = 
    if ip `elem` (ipAddresses eq) 
    then True                     
    else ipExistsRec ip eqs       

-- ipExistsComprehension :: IPAddress -> [NetworkEquipment] -> Bool
-- ipExistsComprehension ip eqs = 
--     not (null [eq | eq <- eqs, ip `elem` (ipAddresses eq)])
    
ipExistsComprehension :: IPAddress -> [NetworkEquipment] -> Bool 
ipExistsComprehension ip list = length [neteq | neteq <- list, ip `elem` ipAddresses neteq] > 0

ipExistsHighOrderFunctions :: IPAddress -> [NetworkEquipment] -> Bool
ipExistsHighOrderFunctions ip eqs = 
    any (\eq -> ip `elem` (ipAddresses eq)) eqs

ipExistsFoldr :: IPAddress -> [NetworkEquipment] -> Bool
ipExistsFoldr ip eqs = foldr (\eq acc -> ip `elem` (ipAddresses eq) || acc) False eqs


instance Arbitrary EquipmentType where
    arbitrary = elements [Router, Switch, Host, Server]

instance Arbitrary NetworkEquipment where 
    arbitrary = do 
        eqName <- arbitrary 
        eqType <- arbitrary
        ipAddresses <- arbitrary 
        return (NetworkEquipment eqName eqType ipAddresses)

prop_qc :: IPAddress -> [NetworkEquipment] -> Bool
prop_qc ip list = ipExistsFoldr ip list == ipExistsComprehension ip list 

test_prop_qc = quickCheck prop_qc 

data Log = Log { equipmentType :: String, messageLog :: String }
    deriving (Eq, Show)

data NetworkWriter a = NetworkWriter { runNetworkWriter :: (a, Log) }
    deriving Show

instance Semigroup Log where
    (Log eqType1 msg1) <> (Log eqType2 msg2) = 
        Log (eqType1 ++ ", " ++ eqType2) (msg1 ++ "; " ++ msg2)

instance Monoid Log where
    mempty = Log "" ""

instance Semigroup a => Semigroup (NetworkWriter a) where
    (NetworkWriter (a1, log1)) <> (NetworkWriter (a2, log2)) = 
        NetworkWriter (a1 <> a2, log1 <> log2)

instance Monoid a => Monoid (NetworkWriter a) where
    mempty = NetworkWriter (mempty, mempty)

instance Functor NetworkWriter where
    fmap f (NetworkWriter (a, log)) = NetworkWriter (f a, log)

instance Applicative NetworkWriter where
    pure a = NetworkWriter (a, mempty)
    (NetworkWriter (f, log1)) <*> (NetworkWriter (a, log2)) = 
        NetworkWriter (f a, log1 <> log2)

instance Monad NetworkWriter where
    (NetworkWriter (a, log1)) >>= f = 
        let NetworkWriter (b, log2) = f a
        in NetworkWriter (b, log1 <> log2)
