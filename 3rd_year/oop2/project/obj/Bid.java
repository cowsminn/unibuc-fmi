package poo.obj;

import java.math.BigDecimal;
import java.time.LocalDateTime;

public class Bid implements Comparable<Bid> {
    private Integer bidId;
    private Integer auctionId;
    private Integer bidderId;
    private BigDecimal bidAmount;
    private LocalDateTime bidTime;
    
    // Referințe pentru convenience
    private User bidder;
    private Auction auction;

    public Bid(Integer auctionId, Integer bidderId, BigDecimal bidAmount) {
        this.auctionId = auctionId;
        this.bidderId = bidderId;
        this.bidAmount = bidAmount;
        this.bidTime = LocalDateTime.now();
    }

    // Constructor cu ID pentru obiectele din baza de date
    public Bid(Integer bidId, Integer auctionId, Integer bidderId, BigDecimal bidAmount, LocalDateTime bidTime) {
        this.bidId = bidId;
        this.auctionId = auctionId;
        this.bidderId = bidderId;
        this.bidAmount = bidAmount;
        this.bidTime = bidTime;
    }

    public Integer getBidId() {
        return bidId;
    }

    public void setBidId(Integer bidId) {
        this.bidId = bidId;
    }

    public Integer getAuctionId() {
        return auctionId;
    }

    public void setAuctionId(Integer auctionId) {
        this.auctionId = auctionId;
    }

    public Integer getBidderId() {
        return bidderId;
    }

    public void setBidderId(Integer bidderId) {
        this.bidderId = bidderId;
    }

    public BigDecimal getBidAmount() {
        return bidAmount;
    }

    public void setBidAmount(BigDecimal bidAmount) {
        this.bidAmount = bidAmount;
    }

    public LocalDateTime getBidTime() {
        return bidTime;
    }

    public void setBidTime(LocalDateTime bidTime) {
        this.bidTime = bidTime;
    }

    public User getBidder() {
        return bidder;
    }

    public void setBidder(User bidder) {
        this.bidder = bidder;
    }

    public Auction getAuction() {
        return auction;
    }

    public void setAuction(Auction auction) {
        this.auction = auction;
    }

    @Override
    public int compareTo(Bid other) {
        // Sort bids by amount (descending)
        return other.bidAmount.compareTo(this.bidAmount);
    }

    @Override
    public String toString() {
        return "Bid{" +
                "bidId=" + bidId +
                ", auctionId=" + auctionId +
                ", bidderId=" + bidderId +
                ", bidAmount=" + bidAmount +
                ", bidTime=" + bidTime +
                '}';
    }
}