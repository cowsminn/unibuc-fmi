package poo.service;

import poo.obj.Auction;
import poo.obj.Bid;
import poo.obj.ThriftItem;
import poo.database.AuctionDAO;
import poo.database.BidDAO;
import poo.database.ThriftItemDAO;

import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.util.List;

public class AuctionService {
    private AuctionDAO auctionDAO;
    private BidDAO bidDAO;
    private ThriftItemDAO itemDAO;
    private AuditService auditService;

    public AuctionService() {
        this.auctionDAO = new AuctionDAO();
        this.bidDAO = new BidDAO();
        this.itemDAO = new ThriftItemDAO();
        this.auditService = AuditService.getInstance();
    }

    public Auction createAuction(Integer itemId, LocalDateTime endTime) {
        // Verifică că item-ul există
        ThriftItem item = itemDAO.read(itemId);
        if (item == null) {
            auditService.logAction("AUCTION_CREATE_FAILED", "Item not found: " + itemId);
            throw new IllegalArgumentException("Item-ul nu există");
        }
        
        // Verifică că nu există deja o licitație pentru acest item
        if (getAuctionByItemId(itemId) != null) {
            auditService.logAction("AUCTION_CREATE_FAILED", "Auction already exists for item: " + itemId);
            throw new IllegalArgumentException("Există deja o licitație pentru acest obiect");
        }
        
        // Verifică că data de sfârșit este în viitor
        if (endTime.isBefore(LocalDateTime.now())) {
            auditService.logAction("AUCTION_CREATE_FAILED", "End time in past for item: " + itemId);
            throw new IllegalArgumentException("Data de sfârșit trebuie să fie în viitor");
        }
        
        Auction auction = new Auction(itemId, LocalDateTime.now(), endTime, item.getStartingPrice());
        
        Integer auctionId = auctionDAO.create(auction);
        if (auctionId == null) {
            auditService.logAction("AUCTION_CREATE_FAILED", "Database error for item: " + itemId);
            throw new RuntimeException("Eroare la crearea licitației");
        }
        
        auditService.logAction("AUCTION_CREATE", "Auction created for item: " + item.getTitle() + " (ID: " + itemId + ")");
        return auction;
    }
    
    public Bid placeBid(Integer auctionId, Integer bidderId, BigDecimal bidAmount) {
        Auction auction = auctionDAO.read(auctionId);
        if (auction == null) {
            auditService.logAction("BID_PLACE_FAILED", "Auction not found: " + auctionId);
            throw new IllegalArgumentException("Licitația nu există");
        }
        
        if (!auction.isActive()) {
            auditService.logAction("BID_PLACE_FAILED", "Auction not active: " + auctionId);
            throw new IllegalStateException("Licitația nu este activă");
        }
        
        if (LocalDateTime.now().isAfter(auction.getEndTime())) {
            auditService.logAction("BID_PLACE_FAILED", "Auction ended: " + auctionId);
            throw new IllegalStateException("Licitația s-a încheiat");
        }
        
        // Verifică că suma oferită este mai mare decât prețul curent
        if (bidAmount.compareTo(auction.getCurrentPrice()) <= 0) {
            auditService.logAction("BID_PLACE_FAILED", "Bid amount too low: " + bidAmount + " for auction: " + auctionId);
            throw new IllegalArgumentException("Oferta trebuie să fie mai mare decât prețul curent: " + auction.getCurrentPrice());
        }
        
        // Verifică că utilizatorul nu încearcă să liciteze pe propriul obiect
        ThriftItem item = itemDAO.read(auction.getItemId());
        if (item != null && item.getSellerId().equals(bidderId)) {
            auditService.logAction("BID_PLACE_FAILED", "Self-bidding attempt by user: " + bidderId);
            throw new IllegalArgumentException("Nu poți licita pe propriul obiect");
        }
        
        // Creează oferta
        Bid bid = new Bid(auctionId, bidderId, bidAmount);
        Integer bidId = bidDAO.create(bid);
        
        if (bidId == null) {
            auditService.logAction("BID_PLACE_FAILED", "Database error for bid on auction: " + auctionId);
            throw new RuntimeException("Eroare la plasarea ofertei");
        }
        
        // Actualizează licitația cu noul preț și cel mai mare ofertant
        auction.setCurrentPrice(bidAmount);
        auction.setHighestBidderId(bidderId);
        auctionDAO.update(auction);
        
        auditService.logAction("BID_PLACE", "Bid placed: " + bidAmount + " on auction: " + auctionId + " by user: " + bidderId);
        return bid;
    }

    public Auction getAuctionById(Integer auctionId) {
        auditService.logAction("AUCTION_READ", "Read auction by ID: " + auctionId);
        return auctionDAO.read(auctionId);
    }
    
    public Auction getAuctionByItemId(Integer itemId) {
        auditService.logAction("AUCTION_SEARCH", "Search auction by item ID: " + itemId);
        return auctionDAO.getByItemId(itemId);
    }

    public List<Auction> getAllAuctions() {
        auditService.logAction("AUCTION_LIST", "Retrieved all auctions");
        return auctionDAO.getAll();
    }

    public List<Auction> getActiveAuctions() {
        auditService.logAction("AUCTION_LIST_ACTIVE", "Retrieved active auctions");
        return auctionDAO.getActiveAuctions();
    }
    
    public List<Auction> getEndingSoonAuctions(int hours) {
        auditService.logAction("AUCTION_LIST_ENDING_SOON", "Retrieved auctions ending in " + hours + " hours");
        return auctionDAO.getEndingSoonAuctions(hours);
    }

    public List<Bid> getBidsByUser(Integer bidderId) {
        auditService.logAction("BID_LIST_BY_USER", "Retrieved bids for user: " + bidderId);
        return bidDAO.getBidsByUser(bidderId);
    }
    
    public List<Bid> getBidsByAuction(Integer auctionId) {
        auditService.logAction("BID_LIST_BY_AUCTION", "Retrieved bids for auction: " + auctionId);
        return bidDAO.getBidsByAuction(auctionId);
    }
    
    public Bid getHighestBidForAuction(Integer auctionId) {
        auditService.logAction("BID_GET_HIGHEST", "Retrieved highest bid for auction: " + auctionId);
        return bidDAO.getHighestBidForAuction(auctionId);
    }
    
    public List<Bid> getWinningBidsByUser(Integer bidderId) {
        auditService.logAction("BID_LIST_WINNING", "Retrieved winning bids for user: " + bidderId);
        return bidDAO.getWinningBidsByUser(bidderId);
    }

    public void endAuction(Integer auctionId) {
        Auction auction = auctionDAO.read(auctionId);
        if (auction == null) {
            auditService.logAction("AUCTION_END_FAILED", "Auction not found: " + auctionId);
            throw new IllegalArgumentException("Licitația nu există");
        }
        
        auction.setStatus("Ended");
        boolean result = auctionDAO.update(auction);
        
        if (result) {
            auditService.logAction("AUCTION_END", "Auction ended: " + auctionId);
        } else {
            auditService.logAction("AUCTION_END_FAILED", "Failed to end auction: " + auctionId);
        }
    }
    
    public int updateExpiredAuctions() {
        int count = auctionDAO.updateExpiredAuctions();
        auditService.logAction("AUCTION_UPDATE_EXPIRED", "Updated " + count + " expired auctions");
        return count;
    }

    public Bid getWinningBid(Integer auctionId) {
        Auction auction = auctionDAO.read(auctionId);
        if (auction == null) {
            auditService.logAction("BID_GET_WINNING_FAILED", "Auction not found: " + auctionId);
            throw new IllegalArgumentException("Licitația nu există");
        }
        
        if (auction.isActive()) {
            auditService.logAction("BID_GET_WINNING_FAILED", "Auction still active: " + auctionId);
            throw new IllegalStateException("Licitația este încă activă");
        }
        
        auditService.logAction("BID_GET_WINNING", "Retrieved winning bid for auction: " + auctionId);
        return bidDAO.getHighestBidForAuction(auctionId);
    }
    
    // Metodă utilă pentru statistici
    public int getBidCountForAuction(Integer auctionId) {
        auditService.logAction("BID_COUNT_AUCTION", "Count bids for auction: " + auctionId);
        return bidDAO.getBidCountByAuction(auctionId);
    }
    
    public int getBidCountForUser(Integer bidderId) {
        auditService.logAction("BID_COUNT_USER", "Count bids for user: " + bidderId);
        return bidDAO.getBidCountByUser(bidderId);
    }
}