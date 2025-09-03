using System.ComponentModel.DataAnnotations;

namespace PhotoShare.Models
{
    public class User
    {
        public int Id { get; set; }
        
        [Required, MaxLength(100)]
        public string Name { get; set; } = string.Empty;
        
        [Required, EmailAddress, MaxLength(255)]
        public string Email { get; set; } = string.Empty;
        
        [Required]
        public string PasswordHash { get; set; } = string.Empty;
        
        public UserType Type { get; set; } = UserType.Registered;
        
        [MaxLength(500)]
        public string? Bio { get; set; }
        
        [MaxLength(255)]
        public string? Avatar { get; set; }
        
        public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
        public DateTime LastLogin { get; set; }
        
        // Navigation properties
        public ICollection<Photo> Photos { get; set; } = new List<Photo>();
        public ICollection<Album> Albums { get; set; } = new List<Album>();
        public ICollection<Comment> Comments { get; set; } = new List<Comment>();
    }
    
    public enum UserType
    {
        Visitor = 0,
        Registered = 1,
        Moderator = 2
    }
}