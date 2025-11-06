package poo.database;

import poo.obj.Auction;
import poo.obj.ThriftItem;
import poo.obj.User;
import java.math.BigDecimal;
import java.sql.*;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

public class AuctionDAO implements GenericDAO<Auction, Integer> {
    private DatabaseManager dbManager;
    private ThriftItemDAO itemDAO;
    private UserDAO userDAO;
    
    public AuctionDAO() {
        this.dbManager = DatabaseManager.getInstance();
        this.itemDAO = new ThriftItemDAO();
        this.userDAO = new UserDAO();
    }
    
    @Override
    public Integer create(Auction auction) {
        String sql = "INSERT INTO auctions (item_id, start_time, end_time, current_price, status) VALUES (?, ?, ?, ?, ?)";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql, Statement.RETURN_GENERATED_KEYS)) {
            
            stmt.setInt(1, auction.getItemId());
            stmt.setTimestamp(2, Timestamp.valueOf(auction.getStartTime()));
            stmt.setTimestamp(3, Timestamp.valueOf(auction.getEndTime()));
            stmt.setBigDecimal(4, auction.getCurrentPrice());
            stmt.setString(5, auction.getStatus());
            
            int rowsAffected = stmt.executeUpdate();
            
            if (rowsAffected > 0) {
                try (ResultSet generatedKeys = stmt.getGeneratedKeys()) {
                    if (generatedKeys.next()) {
                        Integer auctionId = generatedKeys.getInt(1);
                        auction.setAuctionId(auctionId);
                        return auctionId;
                    }
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la crearea licitației: " + e.getMessage());
        }
        
        return null;
    }
    
    @Override
    public Auction read(Integer auctionId) {
        String sql = "SELECT * FROM auctions WHERE auction_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, auctionId);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return mapResultSetToAuction(rs);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la citirea licitației: " + e.getMessage());
        }
        
        return null;
    }
    
    @Override
    public boolean update(Auction auction) {
        String sql = "UPDATE auctions SET end_time = ?, current_price = ?, highest_bidder_id = ?, status = ? WHERE auction_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setTimestamp(1, Timestamp.valueOf(auction.getEndTime()));
            stmt.setBigDecimal(2, auction.getCurrentPrice());
            
            if (auction.getHighestBidderId() != null) {
                stmt.setInt(3, auction.getHighestBidderId());
            } else {
                stmt.setNull(3, Types.INTEGER);
            }
            
            stmt.setString(4, auction.getStatus());
            stmt.setInt(5, auction.getAuctionId());
            
            int rowsAffected = stmt.executeUpdate();
            return rowsAffected > 0;
            
        } catch (SQLException e) {
            System.err.println("Eroare la actualizarea licitației: " + e.getMessage());
            return false;
        }
    }
    
    @Override
    public boolean delete(Integer auctionId) {
        String sql = "DELETE FROM auctions WHERE auction_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, auctionId);
            
            int rowsAffected = stmt.executeUpdate();
            return rowsAffected > 0;
            
        } catch (SQLException e) {
            System.err.println("Eroare la ștergerea licitației: " + e.getMessage());
            return false;
        }
    }
    
    @Override
    public List<Auction> getAll() {
        List<Auction> auctions = new ArrayList<>();
        String sql = "SELECT * FROM auctions ORDER BY created_at DESC";
        
        try (Connection conn = dbManager.getConnection();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery(sql)) {
            
            // Primul pas: extrage toate datele de bază din ResultSet
            List<AuctionData> auctionDataList = new ArrayList<>();
            while (rs.next()) {
                AuctionData data = new AuctionData();
                data.auctionId = rs.getInt("auction_id");
                data.itemId = rs.getInt("item_id");
                data.startTime = rs.getTimestamp("start_time").toLocalDateTime();
                data.endTime = rs.getTimestamp("end_time").toLocalDateTime();
                data.currentPrice = rs.getBigDecimal("current_price");
                data.highestBidderId = rs.getInt("highest_bidder_id");
                if (rs.wasNull()) {
                    data.highestBidderId = null;
                }
                data.status = rs.getString("status");
                data.createdAt = rs.getTimestamp("created_at").toLocalDateTime();
                auctionDataList.add(data);
            }
            
            // Al doilea pas: creează auction-urile și încarcă detaliile
            for (AuctionData data : auctionDataList) {
                Auction auction = new Auction(
                    data.auctionId, data.itemId, data.startTime, data.endTime,
                    data.currentPrice, data.highestBidderId, data.status, data.createdAt
                );
                
                // Încarcă detaliile cu conexiuni separate
                try {
                    ThriftItem item = itemDAO.read(data.itemId);
                    auction.setItem(item);
                    
                    if (data.highestBidderId != null) {
                        User highestBidder = userDAO.read(data.highestBidderId);
                        auction.setHighestBidder(highestBidder);
                    }
                } catch (Exception e) {
                    System.err.println("Avertisment: Nu s-au putut încărca detaliile pentru auction " + data.auctionId);
                }
                
                auctions.add(auction);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la citirea tuturor licitațiilor: " + e.getMessage());
        }
        
        return auctions;
    }
    
    // Metodă pentru găsirea licitației după item
    public Auction getByItemId(Integer itemId) {
        String sql = "SELECT * FROM auctions WHERE item_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, itemId);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return mapResultSetToAuction(rs);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la căutarea licitației după item: " + e.getMessage());
        }
        
        return null;
    }
    
    // Metodă pentru licitațiile active - VERSIUNEA FIXATĂ
    public List<Auction> getActiveAuctions() {
        List<Auction> auctions = new ArrayList<>();
        String sql = "SELECT * FROM auctions WHERE status = 'Active' AND end_time > NOW() ORDER BY end_time ASC";
        
        try (Connection conn = dbManager.getConnection();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery(sql)) {
            
            // Primul pas: extrage toate datele de bază din ResultSet
            List<AuctionData> auctionDataList = new ArrayList<>();
            while (rs.next()) {
                AuctionData data = new AuctionData();
                data.auctionId = rs.getInt("auction_id");
                data.itemId = rs.getInt("item_id");
                data.startTime = rs.getTimestamp("start_time").toLocalDateTime();
                data.endTime = rs.getTimestamp("end_time").toLocalDateTime();
                data.currentPrice = rs.getBigDecimal("current_price");
                data.highestBidderId = rs.getInt("highest_bidder_id");
                if (rs.wasNull()) {
                    data.highestBidderId = null;
                }
                data.status = rs.getString("status");
                data.createdAt = rs.getTimestamp("created_at").toLocalDateTime();
                auctionDataList.add(data);
            }
            
            // Al doilea pas: creează auction-urile și încarcă detaliile
            for (AuctionData data : auctionDataList) {
                Auction auction = new Auction(
                    data.auctionId, data.itemId, data.startTime, data.endTime,
                    data.currentPrice, data.highestBidderId, data.status, data.createdAt
                );
                
                // Încarcă detaliile cu conexiuni separate
                try {
                    ThriftItem item = itemDAO.read(data.itemId);
                    auction.setItem(item);
                    
                    if (data.highestBidderId != null) {
                        User highestBidder = userDAO.read(data.highestBidderId);
                        auction.setHighestBidder(highestBidder);
                    }
                } catch (Exception e) {
                    System.err.println("Avertisment: Nu s-au putut încărca detaliile pentru auction " + data.auctionId);
                }
                
                auctions.add(auction);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la citirea licitațiilor active: " + e.getMessage());
        }
        
        return auctions;
    }
    
    // Clasa helper pentru a stoca datele din ResultSet
    private static class AuctionData {
        Integer auctionId;
        Integer itemId;
        LocalDateTime startTime;
        LocalDateTime endTime;
        BigDecimal currentPrice;
        Integer highestBidderId;
        String status;
        LocalDateTime createdAt;
    }
    
    // Metodă pentru licitațiile care se termină curând - VERSIUNEA FIXATĂ
    public List<Auction> getEndingSoonAuctions(int hours) {
        List<Auction> auctions = new ArrayList<>();
        String sql = "SELECT * FROM auctions WHERE status = 'Active' AND end_time BETWEEN NOW() AND DATE_ADD(NOW(), INTERVAL ? HOUR) ORDER BY end_time ASC";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, hours);
            ResultSet rs = stmt.executeQuery();
            
            // Primul pas: extrage toate datele de bază din ResultSet
            List<AuctionData> auctionDataList = new ArrayList<>();
            while (rs.next()) {
                AuctionData data = new AuctionData();
                data.auctionId = rs.getInt("auction_id");
                data.itemId = rs.getInt("item_id");
                data.startTime = rs.getTimestamp("start_time").toLocalDateTime();
                data.endTime = rs.getTimestamp("end_time").toLocalDateTime();
                data.currentPrice = rs.getBigDecimal("current_price");
                data.highestBidderId = rs.getInt("highest_bidder_id");
                if (rs.wasNull()) {
                    data.highestBidderId = null;
                }
                data.status = rs.getString("status");
                data.createdAt = rs.getTimestamp("created_at").toLocalDateTime();
                auctionDataList.add(data);
            }
            
            // Al doilea pas: creează auction-urile și încarcă detaliile
            for (AuctionData data : auctionDataList) {
                Auction auction = new Auction(
                    data.auctionId, data.itemId, data.startTime, data.endTime,
                    data.currentPrice, data.highestBidderId, data.status, data.createdAt
                );
                
                // Încarcă detaliile cu conexiuni separate
                try {
                    ThriftItem item = itemDAO.read(data.itemId);
                    auction.setItem(item);
                    
                    if (data.highestBidderId != null) {
                        User highestBidder = userDAO.read(data.highestBidderId);
                        auction.setHighestBidder(highestBidder);
                    }
                } catch (Exception e) {
                    System.err.println("Avertisment: Nu s-au putut încărca detaliile pentru auction " + data.auctionId);
                }
                
                auctions.add(auction);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la căutarea licitațiilor care se termină curând: " + e.getMessage());
        }
        
        return auctions;
    }
    
    // Metodă pentru actualizarea statusului licitațiilor expirate
    public int updateExpiredAuctions() {
        String sql = "UPDATE auctions SET status = 'Ended' WHERE status = 'Active' AND end_time < NOW()";
        
        try (Connection conn = dbManager.getConnection();
             Statement stmt = conn.createStatement()) {
            
            return stmt.executeUpdate(sql);
            
        } catch (SQLException e) {
            System.err.println("Eroare la actualizarea licitațiilor expirate: " + e.getMessage());
            return 0;
        }
    }
    
    // Helper method pentru maparea ResultSet la Auction
    private Auction mapResultSetToAuction(ResultSet rs) throws SQLException {
        // Extrage TOATE datele din ResultSet ÎNAINTE de a face alte query-uri
        Integer auctionId = rs.getInt("auction_id");
        Integer itemId = rs.getInt("item_id");
        LocalDateTime startTime = rs.getTimestamp("start_time").toLocalDateTime();
        LocalDateTime endTime = rs.getTimestamp("end_time").toLocalDateTime();
        BigDecimal currentPrice = rs.getBigDecimal("current_price");
        Integer highestBidderId = rs.getInt("highest_bidder_id");
        if (rs.wasNull()) {
            highestBidderId = null;
        }
        String status = rs.getString("status");
        LocalDateTime createdAt = rs.getTimestamp("created_at").toLocalDateTime();
        
        // Creează auction-ul cu datele extrase
        Auction auction = new Auction(
            auctionId, itemId, startTime, endTime, currentPrice, 
            highestBidderId, status, createdAt
        );
        
        // ACUM fă query-urile separate pentru item și user (cu conexiuni noi)
        try {
            ThriftItem item = itemDAO.read(itemId);
            auction.setItem(item);
            
            if (highestBidderId != null) {
                User highestBidder = userDAO.read(highestBidderId);
                auction.setHighestBidder(highestBidder);
            }
        } catch (Exception e) {
            // Dacă nu poate încărca detaliile, continuă cu auction-ul de bază
            System.err.println("Avertisment: Nu s-au putut încărca detaliile pentru auction " + auctionId);
        }
        
        return auction;
    }
}