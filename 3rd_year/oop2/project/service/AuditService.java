package poo.service;

import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class AuditService {
    private static AuditService instance;
    private final String AUDIT_FILE = "audit.csv";
    private final DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
    
    private AuditService() {
        createAuditFileIfNotExists();
    }
    
    public static AuditService getInstance() {
        if (instance == null) {
            synchronized (AuditService.class) {
                if (instance == null) {
                    instance = new AuditService();
                }
            }
        }
        return instance;
    }
    
    /**
     * 
     * @param actionName 
     */
    public void logAction(String actionName) {
        String timestamp = LocalDateTime.now().format(dateFormatter);
        String logEntry = actionName + ", " + timestamp;
        
        try (FileWriter writer = new FileWriter(AUDIT_FILE, true)) {
            writer.append(logEntry).append("\n");
            writer.flush();
        } catch (IOException e) {
            System.err.println("Eroare la scrierea în fișierul de audit: " + e.getMessage());
        }
    }
    
    /**
     * 
     * @param actionName 
     * @param details 
     */
    public void logAction(String actionName, String details) {
        String timestamp = LocalDateTime.now().format(dateFormatter);
        String logEntry = actionName + " - " + details + ", " + timestamp;
        
        try (FileWriter writer = new FileWriter(AUDIT_FILE, true)) {
            writer.append(logEntry).append("\n");
            writer.flush();
        } catch (IOException e) {
            System.err.println("Eroare la scrierea în fișierul de audit: " + e.getMessage());
        }
    }
    
    /**
     * 
     */
    private void createAuditFileIfNotExists() {
        try {
            if (!Files.exists(Paths.get(AUDIT_FILE))) {
                try (FileWriter writer = new FileWriter(AUDIT_FILE)) {
                    writer.append("nume_actiune,timestamp\n");
                }
                System.out.println("Fișier de audit creat: " + AUDIT_FILE);
            }
        } catch (IOException e) {
            System.err.println("Eroare la crearea fișierului de audit: " + e.getMessage());
        }
    }
    
    /**
     * 
     */
    public String getAuditFilePath() {
        return AUDIT_FILE;
    }
}