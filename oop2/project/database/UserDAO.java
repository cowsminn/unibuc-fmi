package poo.database;

import poo.obj.User;
import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class UserDAO implements GenericDAO<User, Integer> {
    private DatabaseManager dbManager;
    
    public UserDAO() {
        this.dbManager = DatabaseManager.getInstance();
    }
    
    @Override
    public Integer create(User user) {
        String sql = "INSERT INTO users (username, email, password, phone, address) VALUES (?, ?, ?, ?, ?)";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql, Statement.RETURN_GENERATED_KEYS)) {
            
            stmt.setString(1, user.getUsername());
            stmt.setString(2, user.getEmail());
            stmt.setString(3, user.getPassword());
            stmt.setString(4, user.getPhone());
            stmt.setString(5, user.getAddress());
            
            int rowsAffected = stmt.executeUpdate();
            
            if (rowsAffected > 0) {
                // Obține ID-ul generat automat
                try (ResultSet generatedKeys = stmt.getGeneratedKeys()) {
                    if (generatedKeys.next()) {
                        Integer userId = generatedKeys.getInt(1);
                        user.setUserId(userId); // Setează ID-ul în obiect
                        return userId;
                    }
                }
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la crearea utilizatorului: " + e.getMessage());
        }
        
        return null;
    }
    
    @Override
    public User read(Integer userId) {
        String sql = "SELECT * FROM users WHERE user_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, userId);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return mapResultSetToUser(rs);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la citirea utilizatorului: " + e.getMessage());
        }
        
        return null;
    }
    
    @Override
    public boolean update(User user) {
        String sql = "UPDATE users SET username = ?, email = ?, password = ?, phone = ?, address = ? WHERE user_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setString(1, user.getUsername());
            stmt.setString(2, user.getEmail());
            stmt.setString(3, user.getPassword());
            stmt.setString(4, user.getPhone());
            stmt.setString(5, user.getAddress());
            stmt.setInt(6, user.getUserId());
            
            int rowsAffected = stmt.executeUpdate();
            return rowsAffected > 0;
            
        } catch (SQLException e) {
            System.err.println("Eroare la actualizarea utilizatorului: " + e.getMessage());
            return false;
        }
    }
    
    @Override
    public boolean delete(Integer userId) {
        String sql = "DELETE FROM users WHERE user_id = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setInt(1, userId);
            
            int rowsAffected = stmt.executeUpdate();
            return rowsAffected > 0;
            
        } catch (SQLException e) {
            System.err.println("Eroare la ștergerea utilizatorului: " + e.getMessage());
            return false;
        }
    }
    
    @Override
    public List<User> getAll() {
        List<User> users = new ArrayList<>();
        String sql = "SELECT * FROM users ORDER BY created_at DESC";
        
        try (Connection conn = dbManager.getConnection();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery(sql)) {
            
            while (rs.next()) {
                users.add(mapResultSetToUser(rs));
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la citirea tuturor utilizatorilor: " + e.getMessage());
        }
        
        return users;
    }
    
    // Metodă specifică pentru căutarea după username
    public User getByUsername(String username) {
        String sql = "SELECT * FROM users WHERE username = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setString(1, username);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return mapResultSetToUser(rs);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la căutarea utilizatorului după username: " + e.getMessage());
        }
        
        return null;
    }
    
    // Metodă pentru autentificare
    public User authenticate(String username, String password) {
        String sql = "SELECT * FROM users WHERE username = ? AND password = ?";
        
        try (Connection conn = dbManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            
            stmt.setString(1, username);
            stmt.setString(2, password);
            ResultSet rs = stmt.executeQuery();
            
            if (rs.next()) {
                return mapResultSetToUser(rs);
            }
            
        } catch (SQLException e) {
            System.err.println("Eroare la autentificare: " + e.getMessage());
        }
        
        return null;
    }
    
    // Helper method pentru maparea ResultSet la User
    private User mapResultSetToUser(ResultSet rs) throws SQLException {
        return new User(
            rs.getInt("user_id"),
            rs.getString("username"),
            rs.getString("email"),
            rs.getString("password"),
            rs.getString("phone"),
            rs.getString("address")
        );
    }
}