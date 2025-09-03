namespace PhotoShare.Models
{
    public class AlbumPhoto
    {
        public int AlbumId { get; set; }
        public Album Album { get; set; } = null!;
        
        public int PhotoId { get; set; }
        public Photo Photo { get; set; } = null!;
        
        public DateTime AddedAt { get; set; } = DateTime.UtcNow;
    }
}