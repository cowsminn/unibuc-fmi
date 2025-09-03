// Services/PhotoService.cs - FIXED VERSION
using Microsoft.EntityFrameworkCore;
using PhotoShare.Data;
using PhotoShare.Models;

namespace PhotoShare.Services
{
    public class PhotoService : IPhotoService
    {
        private readonly PhotoShareContext _context;
        private readonly IWebHostEnvironment _environment;
        private readonly string _uploadsPath;

        public PhotoService(PhotoShareContext context, IWebHostEnvironment environment)
        {
            _context = context;
            _environment = environment;
            _uploadsPath = Path.Combine(_environment.WebRootPath, "uploads");
            
            // Create uploads directory if it doesn't exist
            Directory.CreateDirectory(_uploadsPath);
            Directory.CreateDirectory(Path.Combine(_uploadsPath, "thumbs"));
        }

        public async Task<IEnumerable<Photo>> GetAllPhotosAsync()
        {
            return await _context.Photos
                .Include(p => p.User)
                .Include(p => p.Comments)
                .OrderByDescending(p => p.UploadedAt)
                .ToListAsync();
        }

        public async Task<IEnumerable<Photo>> GetPhotosByCategoryAsync(PhotoCategory category)
        {
            return await _context.Photos
                .Include(p => p.User)
                .Include(p => p.Comments)
                .Where(p => p.Category == category)
                .OrderByDescending(p => p.UploadedAt)
                .ToListAsync();
        }

        public async Task<IEnumerable<Photo>> GetUserPhotosAsync(int userId)
        {
            return await _context.Photos
                .Include(p => p.User)
                .Include(p => p.Comments)
                .Where(p => p.UserId == userId)
                .OrderByDescending(p => p.UploadedAt)
                .ToListAsync();
        }

        public async Task<Photo?> GetPhotoByIdAsync(int id)
        {
            return await _context.Photos
                .Include(p => p.User)
                .Include(p => p.Comments)
                .ThenInclude(c => c.User)
                .FirstOrDefaultAsync(p => p.Id == id);
        }

        public async Task<Photo> SavePhotoAsync(IFormFile file, string title, string description, PhotoCategory category, int userId, double? latitude = null, double? longitude = null, string? locationName = null, string? city = null, string? country = null)
        {
            // Enhanced logging for uploaded files
            Console.WriteLine($"Processing file upload: {file.FileName}");
            Console.WriteLine($"File size: {file.Length} bytes");
            Console.WriteLine($"Content type: {file.ContentType}");
            
            // Save file to disk with enhanced error handling
            var fileName = await SaveFileAsync(file);
            var filePath = Path.Combine("uploads", fileName);

            // Create photo record with additional metadata
            var photo = new Photo
            {
                Title = title,
                Description = description,
                FileName = fileName,
                FilePath = filePath,
                FileSize = file.Length,
                ContentType = file.ContentType,
                Category = category,
                UserId = userId,
                Latitude = latitude,
                Longitude = longitude,
                LocationName = locationName,
                City = city,
                Country = country,
                UploadedAt = DateTime.UtcNow
            };

            _context.Photos.Add(photo);
            await _context.SaveChangesAsync();
            
            Console.WriteLine($"Photo saved successfully with ID: {photo.Id}");

            return photo;
        }

        public async Task<bool> DeletePhotoAsync(int id, int userId)
        {
            var photo = await _context.Photos.FindAsync(id);
            if (photo == null || photo.UserId != userId) return false;

            // Delete related PhotoLikes first
            var photoLikes = await _context.PhotoLikes.Where(pl => pl.PhotoId == id).ToListAsync();
            _context.PhotoLikes.RemoveRange(photoLikes);

            // Delete related CommentLikes
            var commentIds = await _context.Comments.Where(c => c.PhotoId == id).Select(c => c.Id).ToListAsync();
            var commentLikes = await _context.CommentLikes.Where(cl => commentIds.Contains(cl.CommentId)).ToListAsync();
            _context.CommentLikes.RemoveRange(commentLikes);

            // Delete comments
            var comments = await _context.Comments.Where(c => c.PhotoId == id).ToListAsync();
            _context.Comments.RemoveRange(comments);

            // Delete album associations
            var albumPhotos = await _context.AlbumPhotos.Where(ap => ap.PhotoId == id).ToListAsync();
            _context.AlbumPhotos.RemoveRange(albumPhotos);

            // Delete file from disk
            await DeleteFileAsync(photo.FilePath);

            // Delete from database
            _context.Photos.Remove(photo);
            await _context.SaveChangesAsync();

            return true;
        }

        public async Task IncrementViewsAsync(int photoId)
        {
            var photo = await _context.Photos.FindAsync(photoId);
            if (photo != null)
            {
                photo.Views++;
                await _context.SaveChangesAsync();
            }
        }

        // FIXED: Nu mai folosim această metodă pentru like-uri
        // Like-urile se gestionează direct în controller
        public Task IncrementLikesAsync(int photoId)
        {
            // Această metodă nu mai este folosită pentru like-uri
            // Like-urile se gestionează prin PhotoLikes table
            return Task.CompletedTask;
        }

        // ADDED: Metodă pentru actualizarea contorului de like-uri
        public async Task UpdateLikesCountAsync(int photoId)
        {
            var photo = await _context.Photos.FindAsync(photoId);
            if (photo != null)
            {
                var likesCount = await _context.PhotoLikes.CountAsync(pl => pl.PhotoId == photoId);
                photo.Likes = likesCount;
                await _context.SaveChangesAsync();
            }
        }

        public async Task<string> SaveFileAsync(IFormFile file)
        {
            // Generate unique filename with timestamp to avoid conflicts
            var extension = Path.GetExtension(file.FileName);
            var nameWithoutExtension = Path.GetFileNameWithoutExtension(file.FileName);
            var timestamp = DateTime.UtcNow.ToString("yyyyMMdd_HHmmss");
            var fileName = $"{nameWithoutExtension}_{timestamp}_{Guid.NewGuid().ToString("N")[..8]}{extension}";
            var fullPath = Path.Combine(_uploadsPath, fileName);

            Console.WriteLine($"Saving file to: {fullPath}");

            try
            {
                // Save original file
                using (var stream = new FileStream(fullPath, FileMode.Create))
                {
                    await file.CopyToAsync(stream);
                }

                // Create simple thumbnail by copying (no resizing for now)
                await CreateSimpleThumbnailAsync(fullPath, fileName);
                
                Console.WriteLine($"File saved successfully: {fileName}");
                return fileName;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error saving file: {ex.Message}");
                throw;
            }
        }

        public Task<bool> DeleteFileAsync(string filePath)
        {
            return Task.Run(() =>
            {
                try
                {
                    var fullPath = Path.Combine(_environment.WebRootPath, filePath);
                    if (File.Exists(fullPath))
                    {
                        File.Delete(fullPath);
                    }

                    // Delete thumbnail too
                    var thumbPath = Path.Combine(_uploadsPath, "thumbs", Path.GetFileName(filePath));
                    if (File.Exists(thumbPath))
                    {
                        File.Delete(thumbPath);
                    }

                    return true;
                }
                catch
                {
                    return false;
                }
            });
        }

        private async Task CreateSimpleThumbnailAsync(string originalPath, string fileName)
        {
            try
            {
                var thumbsPath = Path.Combine(_uploadsPath, "thumbs");
                Directory.CreateDirectory(thumbsPath);

                var thumbnailPath = Path.Combine(thumbsPath, fileName);
                
                // For now, just copy the original file as thumbnail
                // Later we can implement proper resizing with a different library
                File.Copy(originalPath, thumbnailPath, true);
            }
            catch (Exception ex)
            {
                // Log error but don't fail the upload
                Console.WriteLine($"Error creating thumbnail: {ex.Message}");
            }
        }

        public async Task<IEnumerable<Photo>> GetPhotosByLocationAsync(double latitude, double longitude, double radiusKm = 10)
        {
            // Using Haversine formula to calculate distance
            var photos = await _context.Photos
                .Include(p => p.User)
                .Include(p => p.Comments)
                .Where(p => p.Latitude.HasValue && p.Longitude.HasValue)
                .ToListAsync();

            return photos.Where(p => CalculateDistance(latitude, longitude, p.Latitude!.Value, p.Longitude!.Value) <= radiusKm)
                         .OrderByDescending(p => p.UploadedAt);
        }

        public async Task<IEnumerable<Photo>> GetPhotosByCityAsync(string city)
        {
            return await _context.Photos
                .Include(p => p.User)
                .Include(p => p.Comments)
                .Where(p => !string.IsNullOrEmpty(p.City) && p.City.ToLower().Contains(city.ToLower()))
                .OrderByDescending(p => p.UploadedAt)
                .ToListAsync();
        }

        public async Task<IEnumerable<Photo>> GetPhotosByCountryAsync(string country)
        {
            return await _context.Photos
                .Include(p => p.User)
                .Include(p => p.Comments)
                .Where(p => !string.IsNullOrEmpty(p.Country) && p.Country.ToLower().Contains(country.ToLower()))
                .OrderByDescending(p => p.UploadedAt)
                .ToListAsync();
        }

        public async Task<IEnumerable<Photo>> SearchPhotosOnMapAsync(double? latitude, double? longitude, double? radiusKm, string? city, string? country, PhotoCategory? category)
        {
            var query = _context.Photos
                .Include(p => p.User)
                .Include(p => p.Comments)
                .AsQueryable();

            // Filter by category if specified
            if (category.HasValue)
            {
                query = query.Where(p => p.Category == category.Value);
            }

            // Filter by city if specified
            if (!string.IsNullOrEmpty(city))
            {
                query = query.Where(p => !string.IsNullOrEmpty(p.City) && p.City.ToLower().Contains(city.ToLower()));
            }

            // Filter by country if specified
            if (!string.IsNullOrEmpty(country))
            {
                query = query.Where(p => !string.IsNullOrEmpty(p.Country) && p.Country.ToLower().Contains(country.ToLower()));
            }

            var photos = await query.ToListAsync();

            // Filter by location radius if coordinates are provided
            if (latitude.HasValue && longitude.HasValue && radiusKm.HasValue)
            {
                photos = photos.Where(p => p.Latitude.HasValue && p.Longitude.HasValue &&
                                    CalculateDistance(latitude.Value, longitude.Value, p.Latitude!.Value, p.Longitude!.Value) <= radiusKm.Value)
                            .ToList();
            }

            return photos.OrderByDescending(p => p.UploadedAt);
        }

        private static double CalculateDistance(double lat1, double lon1, double lat2, double lon2)
        {
            // Haversine formula to calculate distance between two points on Earth
            const double R = 6371; // Earth's radius in km

            var dLat = ToRadians(lat2 - lat1);
            var dLon = ToRadians(lon2 - lon1);

            var a = Math.Sin(dLat / 2) * Math.Sin(dLat / 2) +
                    Math.Cos(ToRadians(lat1)) * Math.Cos(ToRadians(lat2)) *
                    Math.Sin(dLon / 2) * Math.Sin(dLon / 2);

            var c = 2 * Math.Atan2(Math.Sqrt(a), Math.Sqrt(1 - a));

            return R * c;
        }

        private static double ToRadians(double degrees)
        {
            return degrees * Math.PI / 180;
        }
    }
}