package poo.database;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class DatabaseManager {
    private static DatabaseManager instance;
    private Connection connection;
    
    // Configurația pentru MySQL - SCHIMBĂ CU DATELE TALE
    private final String URL = "jdbc:mysql://localhost:3306/thrift_auction?useSSL=false&serverTimezone=UTC";
    private final String USERNAME = "root"; // Schimbă cu username-ul tău
    private final String PASSWORD = "parola123"; // Schimbă cu parola ta
    
    private DatabaseManager() {
        try {
            // Încarcă driver-ul MySQL
            Class.forName("com.mysql.cj.jdbc.Driver");
            
            // Creează conexiunea
            this.connection = DriverManager.getConnection(URL, USERNAME, PASSWORD);
            
            System.out.println("Conectat cu succes la MySQL!");
            
        } catch (ClassNotFoundException e) {
            System.err.println("Driver-ul MySQL nu a fost găsit!");
            e.printStackTrace();
        } catch (SQLException e) {
            System.err.println("Eroare la conectarea cu baza de date!");
            e.printStackTrace();
        }
    }
    
    public static DatabaseManager getInstance() {
        if (instance == null) {
            synchronized (DatabaseManager.class) {
                if (instance == null) {
                    instance = new DatabaseManager();
                }
            }
        }
        return instance;
    }
    
    public Connection getConnection() {
        try {
            // Verifică dacă conexiunea e încă validă
            if (connection == null || connection.isClosed()) {
                // Reconectează dacă e nevoie
                connection = DriverManager.getConnection(URL, USERNAME, PASSWORD);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return connection;
    }
    
    public void closeConnection() {
        try {
            if (connection != null && !connection.isClosed()) {
                connection.close();
                System.out.println("Conexiunea cu baza de date a fost închisă.");
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }
}