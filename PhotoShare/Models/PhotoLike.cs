using System.ComponentModel.DataAnnotations;

namespace PhotoShare.Models
{
    public class PhotoLike
    {
        public int PhotoId { get; set; }
        public Photo Photo { get; set; } = null!;
        
        public int UserId { get; set; }
        public User User { get; set; } = null!;
        
        public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
    }
}