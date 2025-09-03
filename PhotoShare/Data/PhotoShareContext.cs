// Data/PhotoShareContext.cs - UPDATED
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using PhotoShare.Models;

namespace PhotoShare.Data
{
    public class PhotoShareContext : DbContext
    {
        private readonly IConfiguration _configuration;

        public PhotoShareContext(DbContextOptions<PhotoShareContext> options, IConfiguration configuration) 
            : base(options) 
        {
            _configuration = configuration;
        }
        
        public DbSet<User> Users { get; set; }
        public DbSet<Photo> Photos { get; set; }
        public DbSet<Album> Albums { get; set; }
        public DbSet<Comment> Comments { get; set; }
        public DbSet<AlbumPhoto> AlbumPhotos { get; set; }
        public DbSet<PhotoLike> PhotoLikes { get; set; }
        public DbSet<CommentLike> CommentLikes { get; set; }
        
        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            // AlbumPhoto - Many-to-Many relationship
            modelBuilder.Entity<AlbumPhoto>()
                .HasKey(ap => new { ap.AlbumId, ap.PhotoId });
                
            modelBuilder.Entity<AlbumPhoto>()
                .HasOne(ap => ap.Album)
                .WithMany(a => a.AlbumPhotos)
                .HasForeignKey(ap => ap.AlbumId);
                
            modelBuilder.Entity<AlbumPhoto>()
                .HasOne(ap => ap.Photo)
                .WithMany(p => p.AlbumPhotos)
                .HasForeignKey(ap => ap.PhotoId);

            // PhotoLike - Many-to-Many relationship
            modelBuilder.Entity<PhotoLike>()
                .HasKey(pl => new { pl.PhotoId, pl.UserId });

            modelBuilder.Entity<PhotoLike>()
                .HasOne(pl => pl.Photo)
                .WithMany()
                .HasForeignKey(pl => pl.PhotoId)
                .OnDelete(DeleteBehavior.Cascade);

            modelBuilder.Entity<PhotoLike>()
                .HasOne(pl => pl.User)
                .WithMany()
                .HasForeignKey(pl => pl.UserId)
                .OnDelete(DeleteBehavior.Cascade);

            // CommentLike - Many-to-Many relationship
            modelBuilder.Entity<CommentLike>()
                .HasKey(cl => new { cl.CommentId, cl.UserId });

            modelBuilder.Entity<CommentLike>()
                .HasOne(cl => cl.Comment)
                .WithMany()
                .HasForeignKey(cl => cl.CommentId)
                .OnDelete(DeleteBehavior.Cascade);

            modelBuilder.Entity<CommentLike>()
                .HasOne(cl => cl.User)
                .WithMany()
                .HasForeignKey(cl => cl.UserId)
                .OnDelete(DeleteBehavior.Cascade);
            
            // User - Photo relationship
            modelBuilder.Entity<Photo>()
                .HasOne(p => p.User)
                .WithMany(u => u.Photos)
                .HasForeignKey(p => p.UserId)
                .OnDelete(DeleteBehavior.Cascade);
            
            // User - Album relationship
            modelBuilder.Entity<Album>()
                .HasOne(a => a.User)
                .WithMany(u => u.Albums)
                .HasForeignKey(a => a.UserId)
                .OnDelete(DeleteBehavior.Cascade);
            
            // Photo - Comment relationship
            modelBuilder.Entity<Comment>()
                .HasOne(c => c.Photo)
                .WithMany(p => p.Comments)
                .HasForeignKey(c => c.PhotoId)
                .OnDelete(DeleteBehavior.Cascade);
            
            // User - Comment relationship
            modelBuilder.Entity<Comment>()
                .HasOne(c => c.User)
                .WithMany(u => u.Comments)
                .HasForeignKey(c => c.UserId)
                .OnDelete(DeleteBehavior.Restrict);
            
            // Seed data
            SeedData(modelBuilder);
        }
        
        private void SeedData(ModelBuilder modelBuilder)
        {
            var adminEmail = _configuration["AdminCredentials:Email"] ?? "admin@example.com";
            var adminPassword = _configuration["AdminCredentials:Password"] ?? "changeme";
            
            // Default admin user
            modelBuilder.Entity<User>().HasData(
                new User
                {
                    Id = 1,
                    Name = "Administrator",
                    Email = adminEmail,
                    PasswordHash = BCrypt.Net.BCrypt.HashPassword(adminPassword),
                    Type = UserType.Moderator,
                    Bio = "Administrator PhotoShare",
                    CreatedAt = DateTime.UtcNow,
                    LastLogin = DateTime.UtcNow
                }
            );
        }
    }
}