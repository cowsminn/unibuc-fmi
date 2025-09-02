package poo.database;

import poo.obj.ThriftItem;
import poo.obj.User;
import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class ThriftItemDAO implements GenericDAO<ThriftItem, Integer> {
    private DatabaseManager dbManager;
    private UserDAO userDAO;
    
    public ThriftItemDAO() {
        this.dbManager = DatabaseManager.getInstance();
        this.userDAO = new UserDAO();
    }
    
    @Override
    public Integer create(ThriftItem item) {
        String sql = "INSERT INTO items (seller_id, title, description, condition_type, starting_price) VALUES (?, ?, ?, ?, ?)";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql, Statement.RETURN_GENERATED_KEYS)) {
            
            stmt.setInt(1, item.getSellerId());
            stmt.setString(2, item.getTitle());
            stmt.setString(3, item.getDescription());
            stmt.setString(4, item.getConditionType());
            stmt.setBigDecimal(5, item.getStartingPrice());
            
            int rowsAffected = stmt.executeUpdate();
            
            if (rowsAffected > 0) {
                try (ResultSet generatedKeys = stmt.getGeneratedKeys()) {
                    if (generatedKeys.next()) {
                        Integer itemId = generatedKeys.getInt(1);
                        item.setItemId(itemId);
                        return itemId;
                    }
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la crearea obiectului: " + e.getMessage());
        }
        
        return null;
    }
    
    @Override
    public ThriftItem read(Integer itemId) {
        String sql = "SELECT * FROM items WHERE item_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, itemId);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return mapResultSetToThriftItem(rs);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la citirea obiectului: " + e.getMessage());
        }
        
        return null;
    }
    
    @Override
    public boolean update(ThriftItem item) {
        String sql = "UPDATE items SET title = ?, description = ?, condition_type = ?, starting_price = ? WHERE item_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setString(1, item.getTitle());
            stmt.setString(2, item.getDescription());
            stmt.setString(3, item.getConditionType());
            stmt.setBigDecimal(4, item.getStartingPrice());
            stmt.setInt(5, item.getItemId());
            
            int rowsAffected = stmt.executeUpdate();
            return rowsAffected > 0;
            
        } catch (SQLException e) {
            System.err.println("Eroare la actualizarea obiectului: " + e.getMessage());
            return false;
        }
    }
    
    @Override
    public boolean delete(Integer itemId) {
        String sql = "DELETE FROM items WHERE item_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, itemId);
            
            int rowsAffected = stmt.executeUpdate();
            return rowsAffected > 0;
            
        } catch (SQLException e) {
            System.err.println("Eroare la ștergerea obiectului: " + e.getMessage());
            return false;
        }
    }
    
    @Override
    public List<ThriftItem> getAll() {
        List<ThriftItem> items = new ArrayList<>();
        String sql = "SELECT * FROM items ORDER BY created_at DESC";
        
        try (Connection conn = dbManager.getConnection();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery(sql)) {
            
            while (rs.next()) {
                ThriftItem item = mapResultSetToThriftItem(rs);
                if (item != null) {
                    items.add(item);
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la citirea tuturor obiectelor: " + e.getMessage());
        }
        
        return items;
    }
    
    // Metodă pentru găsirea obiectelor unui vânzător
    public List<ThriftItem> getItemsBySeller(Integer sellerId) {
        List<ThriftItem> items = new ArrayList<>();
        String sql = "SELECT * FROM items WHERE seller_id = ? ORDER BY created_at DESC";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, sellerId);
            ResultSet rs = stmt.executeQuery();
            
            while (rs.next()) {
                ThriftItem item = mapResultSetToThriftItem(rs);
                if (item != null) {
                    items.add(item);
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la căutarea obiectelor vânzătorului: " + e.getMessage());
        }
        
        return items;
    }
    
    // Metodă pentru căutarea obiectelor după titlu/descriere - VERSIUNEA FIXATĂ
    public List<ThriftItem> searchItems(String keyword) {
        List<ThriftItem> items = new ArrayList<>();
        String sql = "SELECT * FROM items WHERE title LIKE ? OR description LIKE ? ORDER BY created_at DESC";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            String searchPattern = "%" + keyword + "%";
            stmt.setString(1, searchPattern);
            stmt.setString(2, searchPattern);
            ResultSet rs = stmt.executeQuery();
            
            // Primul pas: extrage toate datele de bază din ResultSet
            List<ItemData> itemDataList = new ArrayList<>();
            while (rs.next()) {
                ItemData data = new ItemData();
                data.itemId = rs.getInt("item_id");
                data.sellerId = rs.getInt("seller_id");
                data.title = rs.getString("title");
                data.description = rs.getString("description");
                data.conditionType = rs.getString("condition_type");
                data.startingPrice = rs.getBigDecimal("starting_price");
                data.createdAt = rs.getTimestamp("created_at").toLocalDateTime();
                itemDataList.add(data);
            }
            
            // Al doilea pas: creează item-urile și încarcă seller-ii
            for (ItemData data : itemDataList) {
                ThriftItem item = new ThriftItem(
                    data.itemId, data.sellerId, data.title, data.description,
                    data.conditionType, data.startingPrice, data.createdAt
                );
                
                // Încarcă seller-ul cu conexiune separată
                try {
                    User seller = userDAO.read(data.sellerId);
                    item.setSeller(seller);
                } catch (Exception e) {
                    System.err.println("Avertisment: Nu s-au putut încărca detaliile seller-ului pentru item " + data.itemId);
                }
                
                items.add(item);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la căutarea obiectelor: " + e.getMessage());
        }
        
        return items;
    }
    
    // Clasa helper pentru a stoca datele din ResultSet
    private static class ItemData {
        Integer itemId;
        Integer sellerId;
        String title;
        String description;
        String conditionType;
        BigDecimal startingPrice;
        LocalDateTime createdAt;
    }
    
    // Metodă pentru filtrarea după preț - VERSIUNEA FIXATĂ
    public List<ThriftItem> getItemsByPriceRange(BigDecimal minPrice, BigDecimal maxPrice) {
        List<ThriftItem> items = new ArrayList<>();
        String sql = "SELECT * FROM items WHERE starting_price BETWEEN ? AND ? ORDER BY starting_price ASC";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setBigDecimal(1, minPrice);
            stmt.setBigDecimal(2, maxPrice);
            ResultSet rs = stmt.executeQuery();
            
            // Primul pas: extrage toate datele de bază din ResultSet
            List<ItemData> itemDataList = new ArrayList<>();
            while (rs.next()) {
                ItemData data = new ItemData();
                data.itemId = rs.getInt("item_id");
                data.sellerId = rs.getInt("seller_id");
                data.title = rs.getString("title");
                data.description = rs.getString("description");
                data.conditionType = rs.getString("condition_type");
                data.startingPrice = rs.getBigDecimal("starting_price");
                data.createdAt = rs.getTimestamp("created_at").toLocalDateTime();
                itemDataList.add(data);
            }
            
            // Al doilea pas: creează item-urile și încarcă seller-ii
            for (ItemData data : itemDataList) {
                ThriftItem item = new ThriftItem(
                    data.itemId, data.sellerId, data.title, data.description,
                    data.conditionType, data.startingPrice, data.createdAt
                );
                
                // Încarcă seller-ul cu conexiune separată
                try {
                    User seller = userDAO.read(data.sellerId);
                    item.setSeller(seller);
                } catch (Exception e) {
                    System.err.println("Avertisment: Nu s-au putut încărca detaliile seller-ului pentru item " + data.itemId);
                }
                
                items.add(item);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la filtrarea după preț: " + e.getMessage());
        }
        
        return items;
    }
    
    // Metodă pentru filtrarea după condiție - VERSIUNEA FIXATĂ
    public List<ThriftItem> getItemsByCondition(String condition) {
        List<ThriftItem> items = new ArrayList<>();
        String sql = "SELECT * FROM items WHERE condition_type = ? ORDER BY created_at DESC";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setString(1, condition);
            ResultSet rs = stmt.executeQuery();
            
            // Primul pas: extrage toate datele de bază din ResultSet
            List<ItemData> itemDataList = new ArrayList<>();
            while (rs.next()) {
                ItemData data = new ItemData();
                data.itemId = rs.getInt("item_id");
                data.sellerId = rs.getInt("seller_id");
                data.title = rs.getString("title");
                data.description = rs.getString("description");
                data.conditionType = rs.getString("condition_type");
                data.startingPrice = rs.getBigDecimal("starting_price");
                data.createdAt = rs.getTimestamp("created_at").toLocalDateTime();
                itemDataList.add(data);
            }
            
            // Al doilea pas: creează item-urile și încarcă seller-ii
            for (ItemData data : itemDataList) {
                ThriftItem item = new ThriftItem(
                    data.itemId, data.sellerId, data.title, data.description,
                    data.conditionType, data.startingPrice, data.createdAt
                );
                
                // Încarcă seller-ul cu conexiune separată
                try {
                    User seller = userDAO.read(data.sellerId);
                    item.setSeller(seller);
                } catch (Exception e) {
                    System.err.println("Avertisment: Nu s-au putut încărca detaliile seller-ului pentru item " + data.itemId);
                }
                
                items.add(item);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la filtrarea după condiție: " + e.getMessage());
        }
        
        return items;
    }
    
    // Helper method pentru maparea ResultSet la ThriftItem
    private ThriftItem mapResultSetToThriftItem(ResultSet rs) throws SQLException {
        Integer sellerId = rs.getInt("seller_id");
        
        ThriftItem item = new ThriftItem(
            rs.getInt("item_id"),
            sellerId,
            rs.getString("title"),
            rs.getString("description"),
            rs.getString("condition_type"),
            rs.getBigDecimal("starting_price"),
            rs.getTimestamp("created_at").toLocalDateTime()
        );
        
        // Opțional: încarcă și informațiile despre seller
        User seller = userDAO.read(sellerId);
        item.setSeller(seller);
        
        return item;
    }
}