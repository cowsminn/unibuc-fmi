package poo.service;

import poo.obj.User;
import poo.database.UserDAO;

import java.util.List;

public class UserService {
    private UserDAO userDAO;
    private AuditService auditService;

    public UserService() {
        this.userDAO = new UserDAO();
        this.auditService = AuditService.getInstance();
    }

    public User registerUser(String username, String email, String password, String phone, String address) {
        // Verifică dacă username-ul există deja
        if (getUserByUsername(username) != null) {
            auditService.logAction("USER_REGISTRATION_FAILED", "Username already exists: " + username);
            throw new IllegalArgumentException("Username already exists");
        }
        
        User newUser = new User(username, email, password, phone, address);
        
        // Salvează în baza de date
        Integer userId = userDAO.create(newUser);
        if (userId == null) {
            auditService.logAction("USER_REGISTRATION_FAILED", "Database error for username: " + username);
            throw new RuntimeException("Eroare la salvarea utilizatorului în baza de date");
        }
        
        // Log successful registration
        auditService.logAction("USER_REGISTRATION", "User registered: " + username);
        return newUser;
    }

    public User getUserById(Integer userId) {
        auditService.logAction("USER_READ", "Read user by ID: " + userId);
        return userDAO.read(userId);
    }

    public User getUserByUsername(String username) {
        auditService.logAction("USER_SEARCH", "Search user by username: " + username);
        return userDAO.getByUsername(username);
    }
    
    public User authenticateUser(String username, String password) {
        User user = userDAO.authenticate(username, password);
        
        if (user != null) {
            auditService.logAction("USER_LOGIN_SUCCESS", "User logged in: " + username);
        } else {
            auditService.logAction("USER_LOGIN_FAILED", "Failed login attempt for: " + username);
        }
        
        return user;
    }

    public List<User> getAllUsers() {
        auditService.logAction("USER_LIST", "Retrieved all users");
        return userDAO.getAll();
    }

    public boolean updateUser(User user) {
        boolean result = userDAO.update(user);
        
        if (result) {
            auditService.logAction("USER_UPDATE", "User updated: " + user.getUsername());
        } else {
            auditService.logAction("USER_UPDATE_FAILED", "Failed to update user: " + user.getUsername());
        }
        
        return result;
    }
    
    public boolean deleteUser(Integer userId) {
        User user = userDAO.read(userId);
        boolean result = userDAO.delete(userId);
        
        if (result) {
            auditService.logAction("USER_DELETE", "User deleted: " + (user != null ? user.getUsername() : "ID=" + userId));
        } else {
            auditService.logAction("USER_DELETE_FAILED", "Failed to delete user ID: " + userId);
        }
        
        return result;
    }
}