using System.ComponentModel.DataAnnotations;

namespace PhotoShare.Models
{
    public class Comment
    {
        public int Id { get; set; }
        
        [Required, MaxLength(1000)]
        public string Content { get; set; } = string.Empty;
        
        public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
        
        public int Likes { get; set; } = 0;
        
        public bool IsApproved { get; set; } = false; // Changed to false - comments need approval
        
        // Foreign Keys
        public int PhotoId { get; set; }
        public Photo Photo { get; set; } = null!;
        
        public int UserId { get; set; }
        public User User { get; set; } = null!;
    }
}