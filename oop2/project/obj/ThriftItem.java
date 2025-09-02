package poo.obj;

import java.math.BigDecimal;
import java.time.LocalDateTime;

public class ThriftItem {
    private Integer itemId;
    private Integer sellerId;
    private String title;
    private String description;
    private String conditionType; // Poor, Good, Excellent
    private BigDecimal startingPrice;
    private LocalDateTime createdAt;
    
    // Referință către seller (pentru convenience)
    private User seller;

    public ThriftItem(Integer sellerId, String title, String description, String conditionType, BigDecimal startingPrice) {
        this.sellerId = sellerId;
        this.title = title;
        this.description = description;
        this.conditionType = conditionType;
        this.startingPrice = startingPrice;
        this.createdAt = LocalDateTime.now();
    }

    // Constructor cu ID pentru obiectele din baza de date
    public ThriftItem(Integer itemId, Integer sellerId, String title, String description, 
                String conditionType, BigDecimal startingPrice, LocalDateTime createdAt) {
        this.itemId = itemId;
        this.sellerId = sellerId;
        this.title = title;
        this.description = description;
        this.conditionType = conditionType;
        this.startingPrice = startingPrice;
        this.createdAt = createdAt;
    }

    public Integer getItemId() {
        return itemId;
    }

    public void setItemId(Integer itemId) {
        this.itemId = itemId;
    }

    public Integer getSellerId() {
        return sellerId;
    }

    public void setSellerId(Integer sellerId) {
        this.sellerId = sellerId;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getConditionType() {
        return conditionType;
    }

    public void setConditionType(String conditionType) {
        this.conditionType = conditionType;
    }

    public BigDecimal getStartingPrice() {
        return startingPrice;
    }

    public void setStartingPrice(BigDecimal startingPrice) {
        this.startingPrice = startingPrice;
    }

    public LocalDateTime getCreatedAt() {
        return createdAt;
    }

    public void setCreatedAt(LocalDateTime createdAt) {
        this.createdAt = createdAt;
    }

    public User getSeller() {
        return seller;
    }

    public void setSeller(User seller) {
        this.seller = seller;
    }

    @Override
    public String toString() {
        return "ThriftItem{" +
                "itemId=" + itemId +
                ", sellerId=" + sellerId +
                ", title='" + title + '\'' +
                ", description='" + description + '\'' +
                ", conditionType='" + conditionType + '\'' +
                ", startingPrice=" + startingPrice +
                ", createdAt=" + createdAt +
                '}';
    }
}