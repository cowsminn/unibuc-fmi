package poo.service;

import poo.obj.ThriftItem;
import poo.database.ThriftItemDAO;

import java.math.BigDecimal;
import java.util.List;

public class ThriftItemService {
    private ThriftItemDAO itemDAO;
    private AuditService auditService;

    public ThriftItemService() {
        this.itemDAO = new ThriftItemDAO();
        this.auditService = AuditService.getInstance();
    }

    public ThriftItem addItem(Integer sellerId, String title, String description, String conditionType, BigDecimal startingPrice) {
        // Validări
        if (startingPrice.compareTo(BigDecimal.ZERO) <= 0) {
            auditService.logAction("ITEM_ADD_FAILED", "Invalid price for item: " + title);
            throw new IllegalArgumentException("Prețul de pornire trebuie să fie pozitiv");
        }
        
        if (!isValidCondition(conditionType)) {
            auditService.logAction("ITEM_ADD_FAILED", "Invalid condition for item: " + title);
            throw new IllegalArgumentException("Condiția trebuie să fie: Poor, Good, sau Excellent");
        }
        
        ThriftItem newItem = new ThriftItem(sellerId, title, description, conditionType, startingPrice);
        
        Integer itemId = itemDAO.create(newItem);
        if (itemId == null) {
            auditService.logAction("ITEM_ADD_FAILED", "Database error for item: " + title);
            throw new RuntimeException("Eroare la salvarea obiectului în baza de date");
        }
        
        auditService.logAction("ITEM_ADD", "Item added: " + title + " by seller ID: " + sellerId);
        return newItem;
    }

    public ThriftItem getItemById(Integer itemId) {
        auditService.logAction("ITEM_READ", "Read item by ID: " + itemId);
        return itemDAO.read(itemId);
    }

    public List<ThriftItem> getAllItems() {
        auditService.logAction("ITEM_LIST", "Retrieved all items");
        return itemDAO.getAll();
    }

    public List<ThriftItem> getItemsBySeller(Integer sellerId) {
        auditService.logAction("ITEM_SEARCH_BY_SELLER", "Search items by seller ID: " + sellerId);
        return itemDAO.getItemsBySeller(sellerId);
    }

    public List<ThriftItem> searchItems(String keyword) {
        auditService.logAction("ITEM_SEARCH", "Search items with keyword: " + keyword);
        return itemDAO.searchItems(keyword);
    }

    public List<ThriftItem> getItemsByPriceRange(BigDecimal minPrice, BigDecimal maxPrice) {
        auditService.logAction("ITEM_FILTER_PRICE", "Filter items by price range: " + minPrice + " - " + maxPrice);
        return itemDAO.getItemsByPriceRange(minPrice, maxPrice);
    }
    
    public List<ThriftItem> getItemsByCondition(String condition) {
        auditService.logAction("ITEM_FILTER_CONDITION", "Filter items by condition: " + condition);
        return itemDAO.getItemsByCondition(condition);
    }

    public boolean updateItem(ThriftItem item) {
        boolean result = itemDAO.update(item);
        
        if (result) {
            auditService.logAction("ITEM_UPDATE", "Item updated: " + item.getTitle());
        } else {
            auditService.logAction("ITEM_UPDATE_FAILED", "Failed to update item: " + item.getTitle());
        }
        
        return result;
    }
    
    public boolean deleteItem(Integer itemId) {
        ThriftItem item = itemDAO.read(itemId);
        boolean result = itemDAO.delete(itemId);
        
        if (result) {
            auditService.logAction("ITEM_DELETE", "Item deleted: " + (item != null ? item.getTitle() : "ID=" + itemId));
        } else {
            auditService.logAction("ITEM_DELETE_FAILED", "Failed to delete item ID: " + itemId);
        }
        
        return result;
    }
    
    // Helper method pentru validarea condiției
    private boolean isValidCondition(String condition) {
        return condition != null && 
               (condition.equals("Poor") || condition.equals("Good") || condition.equals("Excellent"));
    }
}