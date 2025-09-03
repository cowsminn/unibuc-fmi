using System.ComponentModel.DataAnnotations;

namespace PhotoShare.Models
{
    public class Photo
    {
        public int Id { get; set; }
        
        [Required, MaxLength(200)]
        public string Title { get; set; } = string.Empty;
        
        [MaxLength(1000)]
        public string? Description { get; set; }
        
        [Required, MaxLength(255)]
        public string FileName { get; set; } = string.Empty;
        
        [Required, MaxLength(255)]
        public string FilePath { get; set; } = string.Empty;
        
        public long FileSize { get; set; }
        
        [MaxLength(50)]
        public string ContentType { get; set; } = string.Empty;
        
        public PhotoCategory Category { get; set; }
        
        public int Likes { get; set; } = 0;
        public int Views { get; set; } = 0;
        
        // Geolocation properties
        public double? Latitude { get; set; }
        public double? Longitude { get; set; }
        
        [MaxLength(255)]
        public string? LocationName { get; set; }
        
        [MaxLength(100)]
        public string? City { get; set; }
        
        [MaxLength(100)]
        public string? Country { get; set; }
        
        public DateTime UploadedAt { get; set; } = DateTime.UtcNow;
        
        // Foreign Keys
        public int UserId { get; set; }
        public User User { get; set; } = null!;
        
        // Navigation properties
        public ICollection<Comment> Comments { get; set; } = new List<Comment>();
        public ICollection<AlbumPhoto> AlbumPhotos { get; set; } = new List<AlbumPhoto>();
        
        // Helper property to check if photo has location data
        public bool HasLocation => Latitude.HasValue && Longitude.HasValue;
    }
    
    public enum PhotoCategory
    {
        Natura = 0,
        Cladiri = 1,
        Animale = 2,
        Portrete = 3,
        Abstract = 4
    }
}