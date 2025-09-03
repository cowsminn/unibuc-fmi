using System.ComponentModel.DataAnnotations;

namespace PhotoShare.Models
{
    public class Album
    {
        public int Id { get; set; }
        
        [Required, MaxLength(100)]
        public string Name { get; set; } = string.Empty;
        
        [MaxLength(500)]
        public string? Description { get; set; }
        
        public bool IsPublic { get; set; } = true;
        
        public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
        public DateTime UpdatedAt { get; set; } = DateTime.UtcNow;
        
        // Foreign Keys
        public int UserId { get; set; }
        public User User { get; set; } = null!;
        
        // Navigation properties
        public ICollection<AlbumPhoto> AlbumPhotos { get; set; } = new List<AlbumPhoto>();
    }
}