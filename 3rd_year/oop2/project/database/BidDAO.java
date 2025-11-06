package poo.database;

import poo.obj.Bid;
import poo.obj.Auction;
import poo.obj.User;
import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class BidDAO implements GenericDAO<Bid, Integer> {
    private DatabaseManager dbManager;
    private UserDAO userDAO;
    private AuctionDAO auctionDAO;
    
    public BidDAO() {
        this.dbManager = DatabaseManager.getInstance();
        this.userDAO = new UserDAO();
        this.auctionDAO = new AuctionDAO();
    }
    
    @Override
    public Integer create(Bid bid) {
        String sql = "INSERT INTO bids (auction_id, bidder_id, bid_amount) VALUES (?, ?, ?)";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql, Statement.RETURN_GENERATED_KEYS)) {
            
            stmt.setInt(1, bid.getAuctionId());
            stmt.setInt(2, bid.getBidderId());
            stmt.setBigDecimal(3, bid.getBidAmount());
            
            int rowsAffected = stmt.executeUpdate();
            
            if (rowsAffected > 0) {
                try (ResultSet generatedKeys = stmt.getGeneratedKeys()) {
                    if (generatedKeys.next()) {
                        Integer bidId = generatedKeys.getInt(1);
                        bid.setBidId(bidId);
                        return bidId;
                    }
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la crearea ofertei: " + e.getMessage());
        }
        
        return null;
    }
    
    @Override
    public Bid read(Integer bidId) {
        String sql = "SELECT * FROM bids WHERE bid_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, bidId);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return mapResultSetToBid(rs);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la citirea ofertei: " + e.getMessage());
        }
        
        return null;
    }
    
    @Override
    public boolean update(Bid bid) {
        String sql = "UPDATE bids SET bid_amount = ? WHERE bid_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setBigDecimal(1, bid.getBidAmount());
            stmt.setInt(2, bid.getBidId());
            
            int rowsAffected = stmt.executeUpdate();
            return rowsAffected > 0;
            
        } catch (SQLException e) {
            System.err.println("Eroare la actualizarea ofertei: " + e.getMessage());
            return false;
        }
    }
    
    @Override
    public boolean delete(Integer bidId) {
        String sql = "DELETE FROM bids WHERE bid_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, bidId);
            
            int rowsAffected = stmt.executeUpdate();
            return rowsAffected > 0;
            
        } catch (SQLException e) {
            System.err.println("Eroare la ștergerea ofertei: " + e.getMessage());
            return false;
        }
    }
    
    @Override
    public List<Bid> getAll() {
        List<Bid> bids = new ArrayList<>();
        String sql = "SELECT * FROM bids ORDER BY bid_time DESC";
        
        try (Connection conn = dbManager.getConnection();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery(sql)) {
            
            while (rs.next()) {
                Bid bid = mapResultSetToBid(rs);
                if (bid != null) {
                    bids.add(bid);
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la citirea tuturor ofertelor: " + e.getMessage());
        }
        
        return bids;
    }
    
    // Metodă pentru găsirea ofertelor unui utilizator
    public List<Bid> getBidsByUser(Integer bidderId) {
        List<Bid> bids = new ArrayList<>();
        String sql = "SELECT * FROM bids WHERE bidder_id = ? ORDER BY bid_time DESC";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, bidderId);
            ResultSet rs = stmt.executeQuery();
            
            while (rs.next()) {
                Bid bid = mapResultSetToBid(rs);
                if (bid != null) {
                    bids.add(bid);
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la căutarea ofertelor utilizatorului: " + e.getMessage());
        }
        
        return bids;
    }
    
    // Metodă pentru găsirea ofertelor pentru o licitație
    public List<Bid> getBidsByAuction(Integer auctionId) {
        List<Bid> bids = new ArrayList<>();
        String sql = "SELECT * FROM bids WHERE auction_id = ? ORDER BY bid_amount DESC";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, auctionId);
            ResultSet rs = stmt.executeQuery();
            
            while (rs.next()) {
                Bid bid = mapResultSetToBid(rs);
                if (bid != null) {
                    bids.add(bid);
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la căutarea ofertelor pentru licitație: " + e.getMessage());
        }
        
        return bids;
    }
    
    // Metodă pentru găsirea celei mai mari oferte pentru o licitație
    public Bid getHighestBidForAuction(Integer auctionId) {
        String sql = "SELECT * FROM bids WHERE auction_id = ? ORDER BY bid_amount DESC LIMIT 1";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, auctionId);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return mapResultSetToBid(rs);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la căutarea celei mai mari oferte: " + e.getMessage());
        }
        
        return null;
    }
    
    // Metodă pentru găsirea ofertelor câștigătoare ale unui utilizator
    public List<Bid> getWinningBidsByUser(Integer bidderId) {
        List<Bid> winningBids = new ArrayList<>();
        String sql = """
            SELECT b.* FROM bids b 
            JOIN auctions a ON b.auction_id = a.auction_id 
            WHERE b.bidder_id = ? AND a.highest_bidder_id = ? AND a.status = 'Ended'
            ORDER BY b.bid_time DESC
            """;
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, bidderId);
            stmt.setInt(2, bidderId);
            ResultSet rs = stmt.executeQuery();
            
            while (rs.next()) {
                Bid bid = mapResultSetToBid(rs);
                if (bid != null) {
                    winningBids.add(bid);
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la căutarea ofertelor câștigătoare: " + e.getMessage());
        }
        
        return winningBids;
    }
    
    // Metodă pentru statistici - numărul de oferte ale unui utilizator
    public int getBidCountByUser(Integer bidderId) {
        String sql = "SELECT COUNT(*) FROM bids WHERE bidder_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, bidderId);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return rs.getInt(1);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la numărarea ofertelor utilizatorului: " + e.getMessage());
        }
        
        return 0;
    }
    
    // Metodă pentru statistici - numărul de oferte pentru o licitație
    public int getBidCountByAuction(Integer auctionId) {
        String sql = "SELECT COUNT(*) FROM bids WHERE auction_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, auctionId);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return rs.getInt(1);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la numărarea ofertelor pentru licitație: " + e.getMessage());
        }
        
        return 0;
    }
    
    // Helper method pentru maparea ResultSet la Bid
    private Bid mapResultSetToBid(ResultSet rs) throws SQLException {
        Bid bid = new Bid(
            rs.getInt("bid_id"),
            rs.getInt("auction_id"),
            rs.getInt("bidder_id"),
            rs.getBigDecimal("bid_amount"),
            rs.getTimestamp("bid_time").toLocalDateTime()
        );
        
        // Opțional: încarcă și informațiile despre bidder (fără auction pentru a evita dependența circulară)
        User bidder = userDAO.read(bid.getBidderId());
        bid.setBidder(bidder);
        
        return bid;
    }
}