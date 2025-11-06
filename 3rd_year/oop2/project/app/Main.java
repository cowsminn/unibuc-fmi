package poo.app;

import poo.obj.*;
import poo.service.*;

import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.*;

public class Main {
    private static Scanner scanner = new Scanner(System.in);
    private static UserService userService = new UserService();
    private static ThriftItemService itemService = new ThriftItemService();
    private static AuctionService auctionService = new AuctionService();
    private static User currentUser = null;
    
    public static void main(String[] args) {
       // initializeDemoData();
        mainMenu();
    }
    
    private static void mainMenu() {
        boolean exit = false;
        
        while (!exit) {
            System.out.println("\n===== THRIFT ITEM BIDDING SYSTEM =====");
            
            if (currentUser == null) {
                // Opțiuni pentru utilizatori neautentificați
                System.out.println("1. Inregistrare utilizator nou");
                System.out.println("2. Autentificare");
                System.out.println("3. Vizualizare obiecte disponibile");
                System.out.println("4. Cautare obiecte");
                System.out.println("5. Iesire");
            } else {
                // Opțiuni pentru utilizatori autentificați
                System.out.println("Utilizator curent: " + currentUser.getUsername());
                System.out.println("1. Vizualizare obiecte disponibile");
                System.out.println("2. Cautare obiecte");
                System.out.println("3. Adauga obiect pentru licitatie");
                System.out.println("4. Plaseaza oferta");
                System.out.println("5. Vizualizare ofertele mele");
                System.out.println("6. Vizualizare obiectele mele");
                System.out.println("7. Deconectare");
                System.out.println("8. Iesire");
            }
            
            System.out.print("\nSelecteaza o optiune: ");
            int option = getIntInput();
            
            if (currentUser == null) {
                handleUnauthenticatedUser(option);
            } else {
                handleAuthenticatedUser(option);
            }
        }
    }
    
    private static void handleUnauthenticatedUser(int option) {
        switch (option) {
            case 1:
                registerUser();
                break;
            case 2:
                login();
                break;
            case 3:
                viewAvailableItems();
                break;
            case 4:
                searchItemsMenu();
                break;
            case 5:
                System.out.println("La revedere!");
                System.exit(0);
                break;
            default:
                System.out.println("Optiune invalida! Te rog sa incerci din nou.");
        }
    }
    
    private static void handleAuthenticatedUser(int option) {
        switch (option) {
            case 1:
                viewAvailableItems();
                break;
            case 2:
                searchItemsMenu();
                break;
            case 3:
                addItemMenu();
                break;
            case 4:
                placeBid();
                break;
            case 5:
                viewMyBids();
                break;
            case 6:
                viewMyItems();
                break;
            case 7:
                currentUser = null;
                System.out.println("Te-ai deconectat cu succes!");
                break;
            case 8:
                System.out.println("La revedere!");
                System.exit(0);
                break;
            default:
                System.out.println("Optiune invalida! Te rog sa incerci din nou.");
        }
    }
    
    private static void registerUser() {
        System.out.println("\n===== INREGISTRARE UTILIZATOR NOU =====");
        
        System.out.print("Nume utilizator: ");
        String username = scanner.nextLine();
        
        System.out.print("Email: ");
        String email = scanner.nextLine();
        
        System.out.print("Parola: ");
        String password = scanner.nextLine();
        
        System.out.print("Numar de telefon: ");
        String phone = scanner.nextLine();
        
        System.out.print("Adresa: ");
        String address = scanner.nextLine();
        
        try {
            User newUser = userService.registerUser(username, email, password, phone, address);
            System.out.println("Utilizator inregistrat cu succes: " + newUser.getUsername());
            currentUser = newUser; 
        } catch (IllegalArgumentException e) {
            System.out.println("Eroare la inregistrare: " + e.getMessage());
        }
    }
    
    private static void login() {
        System.out.println("\n===== AUTENTIFICARE =====");
        
        System.out.print("Nume utilizator: ");
        String username = scanner.nextLine();
        
        System.out.print("Parola: ");
        String password = scanner.nextLine();
        
        User user = userService.authenticateUser(username, password);
        if (user != null) {
            currentUser = user;
            System.out.println("Autentificare reusita! Bine ai venit, " + user.getUsername() + "!");
        } else {
            System.out.println("Nume utilizator sau parola incorecte.");
        }
    }
    
    private static void viewAvailableItems() {
        System.out.println("\n===== OBIECTE DISPONIBILE PENTRU LICITATIE =====");
        
        List<Auction> activeAuctions = auctionService.getActiveAuctions();
        if (activeAuctions.isEmpty()) {
            System.out.println("Nu exista licitatii active in acest moment.");
            return;
        }
        
        displayAuctions(activeAuctions);
        
        System.out.println("\nApasa Enter pentru a continua...");
        scanner.nextLine();
    }
    
    private static void searchItemsMenu() {
        boolean back = false;
        
        while (!back) {
            System.out.println("\n===== CAUTARE OBIECTE =====");
            System.out.println("1. Cautare dupa titlu/descriere");
            System.out.println("2. Filtrare dupa conditie");
            System.out.println("3. Filtrare dupa interval de pret");
            System.out.println("4. Inapoi la meniul principal");
            
            System.out.print("\nSelecteaza o optiune: ");
            int option = getIntInput();
            
            switch (option) {
                case 1:
                    searchByKeyword();
                    break;
                case 2:
                    filterByCondition();
                    break;
                case 3:
                    filterByPriceRange();
                    break;
                case 4:
                    back = true;
                    break;
                default:
                    System.out.println("Optiune invalida! Te rog sa incerci din nou.");
            }
        }
    }
    
    private static void searchByKeyword() {
        System.out.println("\n===== CAUTARE DUPA CUVANT CHEIE =====");
        
        System.out.print("Introdu cuvantul cheie: ");
        String keyword = scanner.nextLine();
        
        List<ThriftItem> results = itemService.searchItems(keyword);
        if (results.isEmpty()) {
            System.out.println("Nu s-au gasit obiecte care sa corespunda cautarii.");
            return;
        }
        
        System.out.println("\nRezultate pentru \"" + keyword + "\":");
        displayItems(results);
        
        System.out.println("\nApasa Enter pentru a continua...");
        scanner.nextLine();
    }
    
    private static void filterByCondition() {
        System.out.println("\n===== FILTRARE DUPA CONDITIE =====");
        System.out.println("1. Poor");
        System.out.println("2. Good");
        System.out.println("3. Excellent");
        
        System.out.print("\nSelecteaza o conditie: ");
        int option = getIntInput();
        
        String condition = "";
        switch (option) {
            case 1: condition = "Poor"; break;
            case 2: condition = "Good"; break;
            case 3: condition = "Excellent"; break;
            default:
                System.out.println("Conditie invalida!");
                return;
        }
        
        List<ThriftItem> results = itemService.getItemsByCondition(condition);
        if (results.isEmpty()) {
            System.out.println("Nu exista obiecte cu conditia " + condition + ".");
            return;
        }
        
        System.out.println("\nObiecte cu conditia " + condition + ":");
        displayItems(results);
        
        System.out.println("\nApasa Enter pentru a continua...");
        scanner.nextLine();
    }
    
    private static void filterByPriceRange() {
        System.out.println("\n===== FILTRARE DUPA INTERVAL DE PRET =====");
        
        System.out.print("Pret minim: ");
        BigDecimal minPrice = getBigDecimalInput();
        
        System.out.print("Pret maxim: ");
        BigDecimal maxPrice = getBigDecimalInput();
        
        if (minPrice.compareTo(maxPrice) > 0) {
            System.out.println("Pretul minim nu poate fi mai mare decat pretul maxim!");
            return;
        }
        
        List<ThriftItem> results = itemService.getItemsByPriceRange(minPrice, maxPrice);
        if (results.isEmpty()) {
            System.out.println("Nu exista obiecte in intervalul de pret specificat.");
            return;
        }
        
        System.out.println("\nObiecte cu pret intre " + minPrice + " si " + maxPrice + ":");
        displayItems(results);
        
        System.out.println("\nApasa Enter pentru a continua...");
        scanner.nextLine();
    }
    
    private static void addItemMenu() {
        System.out.println("\n===== ADAUGA OBIECT PENTRU LICITATIE =====");
        
        System.out.print("Titlu obiect: ");
        String title = scanner.nextLine();
        
        System.out.print("Descriere: ");
        String description = scanner.nextLine();
        
        System.out.print("Pret de pornire: ");
        BigDecimal startingPrice = getBigDecimalInput();
        
        System.out.println("Conditie:");
        System.out.println("1. Poor");
        System.out.println("2. Good");
        System.out.println("3. Excellent");
        System.out.print("Selecteaza conditia: ");
        int conditionOption = getIntInput();
        
        String condition = "";
        switch (conditionOption) {
            case 1: condition = "Poor"; break;
            case 2: condition = "Good"; break;
            case 3: condition = "Excellent"; break;
            default:
                System.out.println("Conditie invalida!");
                return;
        }
        
        try {
            ThriftItem newItem = itemService.addItem(currentUser.getUserId(), title, description, condition, startingPrice);
            System.out.println("Obiect adaugat cu succes!");
            
            System.out.print("Numar de zile pentru licitatie: ");
            int days = getIntInput();
            
            LocalDateTime endTime = LocalDateTime.now().plusDays(days);
            auctionService.createAuction(newItem.getItemId(), endTime);
            
            System.out.println("Licitatie creata cu succes! Se va incheia in " + days + " zile.");
        } catch (Exception e) {
            System.out.println("Eroare: " + e.getMessage());
        }
    }
    
    private static void placeBid() {
        System.out.println("\n===== PLASARE OFERTA =====");

        List<Auction> activeAuctions = auctionService.getActiveAuctions();
        if (activeAuctions.isEmpty()) {
            System.out.println("Nu exista licitatii active in acest moment.");
            return;
        }
        
        System.out.println("Licitatii disponibile:");
        displayAuctions(activeAuctions);

        System.out.print("\nIntroduceti numarul licitatiei pentru care doriti sa plasati o oferta (0 pentru a anula): ");
        int auctionIndex = getIntInput() - 1;
        
        if (auctionIndex == -1) {
            return;
        }
        
        if (auctionIndex < 0 || auctionIndex >= activeAuctions.size()) {
            System.out.println("Numar invalid!");
            return;
        }
        
        Auction selectedAuction = activeAuctions.get(auctionIndex);
        
        System.out.println("\nDetalii licitatie:");
        System.out.println("Obiect: " + selectedAuction.getItem().getTitle());
        System.out.println("Pret curent: " + selectedAuction.getCurrentPrice());
        System.out.println("Data incheierii: " + 
                         selectedAuction.getEndTime().format(DateTimeFormatter.ofPattern("dd-MM-yyyy HH:mm")));

        System.out.print("\nIntroduceti suma ofertei: ");
        BigDecimal bidAmount = getBigDecimalInput();
        
        try {
            auctionService.placeBid(selectedAuction.getAuctionId(), currentUser.getUserId(), bidAmount);
            System.out.println("Oferta plasata cu succes!");
        } catch (Exception e) {
            System.out.println("Eroare la plasarea ofertei: " + e.getMessage());
        }
    }
    
    private static void viewMyBids() {
        System.out.println("\n===== OFERTELE MELE =====");
        
        List<Bid> myBids = auctionService.getBidsByUser(currentUser.getUserId());
        if (myBids.isEmpty()) {
            System.out.println("Nu ai plasat nicio oferta pana acum.");
            return;
        }
        
        System.out.println("Ai plasat " + myBids.size() + " oferte:");
        
        for (int i = 0; i < myBids.size(); i++) {
            Bid bid = myBids.get(i);
            Auction auction = auctionService.getAuctionById(bid.getAuctionId());
            
            System.out.println("\n" + (i + 1) + ". Suma ofertei: " + bid.getBidAmount());
            System.out.println("   Data ofertei: " + bid.getBidTime().format(DateTimeFormatter.ofPattern("dd-MM-yyyy HH:mm")));
            
            if (auction != null) {
                System.out.println("   Obiect: " + auction.getItem().getTitle());
                System.out.println("   Status licitatie: " + (auction.isActive() ? "Activa" : "Incheiata"));
                
                boolean isWinning = auction.getHighestBidderId() != null && 
                                   auction.getHighestBidderId().equals(currentUser.getUserId());
                System.out.println("   Status oferta: " + (isWinning ? "Cea mai mare!" : "Depasita"));
            }
        }
        
        System.out.println("\nApasa Enter pentru a continua...");
        scanner.nextLine();
    }
    
    private static void viewMyItems() {
        System.out.println("\n===== OBIECTELE MELE =====");
        
        List<ThriftItem> myItems = itemService.getItemsBySeller(currentUser.getUserId());
        if (myItems.isEmpty()) {
            System.out.println("Nu ai adaugat niciun obiect pana acum.");
            return;
        }
        
        System.out.println("Ai adaugat " + myItems.size() + " obiecte:");
        
        for (int i = 0; i < myItems.size(); i++) {
            ThriftItem item = myItems.get(i);
            Auction auction = auctionService.getAuctionByItemId(item.getItemId());
            
            System.out.println("\n" + (i + 1) + ". " + item.getTitle());
            System.out.println("   Descriere: " + item.getDescription());
            System.out.println("   Pret pornire: " + item.getStartingPrice());
            System.out.println("   Conditie: " + item.getConditionType());
            
            if (auction != null) {
                System.out.println("   Status licitatie: " + (auction.isActive() ? "Activa" : "Incheiata"));
                System.out.println("   Pret curent: " + auction.getCurrentPrice());
                
                if (auction.getHighestBidderId() != null) {
                    System.out.println("   Exista oferte!");
                } else {
                    System.out.println("   Nicio oferta pana acum");
                }
                
                System.out.println("   Data incheierii: " + 
                                 auction.getEndTime().format(DateTimeFormatter.ofPattern("dd-MM-yyyy HH:mm")));
            } else {
                System.out.println("   Nicio licitatie asociata");
            }
        }
        
        System.out.println("\nApasa Enter pentru a continua...");
        scanner.nextLine();
    }
    
    private static void displayItems(List<ThriftItem> items) {
        for (int i = 0; i < items.size(); i++) {
            ThriftItem item = items.get(i);
            System.out.println((i + 1) + ". " + item.getTitle());
            System.out.println("   Descriere: " + item.getDescription());
            System.out.println("   Pret pornire: " + item.getStartingPrice());
            System.out.println("   Conditie: " + item.getConditionType());
            System.out.println("   Vanzator: " + (item.getSeller() != null ? item.getSeller().getUsername() : "N/A"));
            System.out.println();
        }
    }
    
    private static void displayAuctions(List<Auction> auctions) {
        for (int i = 0; i < auctions.size(); i++) {
            Auction auction = auctions.get(i);
            
            // Încarcă item-ul separat pentru a evita problema cu ResultSet
            ThriftItem item = itemService.getItemById(auction.getItemId());
            
            if (item != null) {
                System.out.println((i + 1) + ". " + item.getTitle());
                System.out.println("   Descriere: " + item.getDescription());
                System.out.println("   Pret pornire: " + item.getStartingPrice());
                System.out.println("   Pret curent: " + auction.getCurrentPrice());
                System.out.println("   Conditie: " + item.getConditionType());
                System.out.println("   Vanzator: " + (item.getSeller() != null ? item.getSeller().getUsername() : "N/A"));
                System.out.println("   Se incheie: " + 
                                 auction.getEndTime().format(DateTimeFormatter.ofPattern("dd-MM-yyyy HH:mm")));
                System.out.println();
            }
        }
    }
    
    private static int getIntInput() {
        while (true) {
            try {
                String input = scanner.nextLine();
                return Integer.parseInt(input);
            } catch (NumberFormatException e) {
                System.out.print("Te rog sa introduci un numar valid: ");
            }
        }
    }
    
    private static BigDecimal getBigDecimalInput() {
        while (true) {
            try {
                String input = scanner.nextLine();
                return new BigDecimal(input);
            } catch (NumberFormatException e) {
                System.out.print("Te rog sa introduci o valoare numerica valida: ");
            }
        }
    }
    
    private static void initializeDemoData() {
        try {
            // Creează utilizatorii demo
            User user1 = userService.registerUser("alice", "alice@example.com", "password123", "123-456-7890", "123 Main St");
            User user2 = userService.registerUser("bob", "bob@example.com", "password123", "098-765-4321", "456 Oak Ave");
            User user3 = userService.registerUser("charlie", "charlie@example.com", "password123", "111-222-3333", "789 Pine Rd");

            // Creează obiecte demo
            ThriftItem item1 = itemService.addItem(user1.getUserId(), "Vintage T-Shirt", 
                "A rare vintage band T-shirt from the 90s", "Good", new BigDecimal("15.99"));
            
            ThriftItem item2 = itemService.addItem(user2.getUserId(), "Smartphone", 
                "Slightly used smartphone, works perfectly", "Good", new BigDecimal("150.00"));
            
            ThriftItem item3 = itemService.addItem(user3.getUserId(), "The Great Gatsby", 
                "Classic novel in excellent condition", "Excellent", new BigDecimal("8.50"));

            // Creează licitații demo
            LocalDateTime now = LocalDateTime.now();
            auctionService.createAuction(item1.getItemId(), now.plusDays(3));
            auctionService.createAuction(item2.getItemId(), now.plusDays(5));
            auctionService.createAuction(item3.getItemId(), now.plusDays(4));

            // Creează oferte demo
            auctionService.placeBid(auctionService.getAuctionByItemId(item1.getItemId()).getAuctionId(), 
                                  user2.getUserId(), new BigDecimal("20.00"));
            auctionService.placeBid(auctionService.getAuctionByItemId(item2.getItemId()).getAuctionId(), 
                                  user1.getUserId(), new BigDecimal("160.00"));
            
            System.out.println("Date demo initializate cu succes!");
            
        } catch (Exception e) {
            System.out.println("Eroare la initializarea datelor demo: " + e.getMessage());
        }
    }
}