package poo.obj;

import java.math.BigDecimal;
import java.time.LocalDateTime;

public class Auction {
    private Integer auctionId;
    private Integer itemId;
    private LocalDateTime startTime;
    private LocalDateTime endTime;
    private BigDecimal currentPrice;
    private Integer highestBidderId;
    private String status; // Active, Ended, Cancelled
    private LocalDateTime createdAt;
    
    // Referințe pentru convenience
    private ThriftItem item;
    private User highestBidder;

    public Auction(Integer itemId, LocalDateTime startTime, LocalDateTime endTime, BigDecimal currentPrice) {
        this.itemId = itemId;
        this.startTime = startTime;
        this.endTime = endTime;
        this.currentPrice = currentPrice;
        this.status = "Active";
        this.createdAt = LocalDateTime.now();
    }

    // Constructor cu ID pentru obiectele din baza de date
    public Auction(Integer auctionId, Integer itemId, LocalDateTime startTime, LocalDateTime endTime,
                   BigDecimal currentPrice, Integer highestBidderId, String status, LocalDateTime createdAt) {
        this.auctionId = auctionId;
        this.itemId = itemId;
        this.startTime = startTime;
        this.endTime = endTime;
        this.currentPrice = currentPrice;
        this.highestBidderId = highestBidderId;
        this.status = status;
        this.createdAt = createdAt;
    }

    public Integer getAuctionId() {
        return auctionId;
    }

    public void setAuctionId(Integer auctionId) {
        this.auctionId = auctionId;
    }

    public Integer getItemId() {
        return itemId;
    }

    public void setItemId(Integer itemId) {
        this.itemId = itemId;
    }

    public LocalDateTime getStartTime() {
        return startTime;
    }

    public void setStartTime(LocalDateTime startTime) {
        this.startTime = startTime;
    }

    public LocalDateTime getEndTime() {
        return endTime;
    }

    public void setEndTime(LocalDateTime endTime) {
        this.endTime = endTime;
    }

    public BigDecimal getCurrentPrice() {
        return currentPrice;
    }

    public void setCurrentPrice(BigDecimal currentPrice) {
        this.currentPrice = currentPrice;
    }

    public Integer getHighestBidderId() {
        return highestBidderId;
    }

    public void setHighestBidderId(Integer highestBidderId) {
        this.highestBidderId = highestBidderId;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public LocalDateTime getCreatedAt() {
        return createdAt;
    }

    public void setCreatedAt(LocalDateTime createdAt) {
        this.createdAt = createdAt;
    }

    public ThriftItem getItem() {
        return item;
    }

    public void setItem(ThriftItem item) {
        this.item = item;
    }

    public User getHighestBidder() {
        return highestBidder;
    }

    public void setHighestBidder(User highestBidder) {
        this.highestBidder = highestBidder;
    }

    public boolean isActive() {
        return "Active".equals(status) && LocalDateTime.now().isBefore(endTime);
    }

    public void endAuction() {
        this.status = "Ended";
    }

    @Override
    public String toString() {
        return "Auction{" +
                "auctionId=" + auctionId +
                ", itemId=" + itemId +
                ", startTime=" + startTime +
                ", endTime=" + endTime +
                ", currentPrice=" + currentPrice +
                ", highestBidderId=" + highestBidderId +
                ", status='" + status + '\'' +
                ", createdAt=" + createdAt +
                '}';
    }
}