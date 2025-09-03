using PhotoShare.Models;

namespace PhotoShare.Services
{
    public interface IPhotoService
    {
        Task<IEnumerable<Photo>> GetAllPhotosAsync();
        Task<IEnumerable<Photo>> GetPhotosByCategoryAsync(PhotoCategory category);
        Task<IEnumerable<Photo>> GetUserPhotosAsync(int userId);
        Task<Photo?> GetPhotoByIdAsync(int id);
        Task<Photo> SavePhotoAsync(IFormFile file, string title, string description, PhotoCategory category, int userId, double? latitude = null, double? longitude = null, string? locationName = null, string? city = null, string? country = null);
        Task<bool> DeletePhotoAsync(int id, int userId);
        Task IncrementViewsAsync(int photoId);
        Task IncrementLikesAsync(int photoId); // Deprecated - nu se mai folosește
        Task UpdateLikesCountAsync(int photoId); // NEW - pentru actualizarea contorului
        Task<string> SaveFileAsync(IFormFile file);
        Task<bool> DeleteFileAsync(string filePath);
        
        // New methods for location-based search
        Task<IEnumerable<Photo>> GetPhotosByLocationAsync(double latitude, double longitude, double radiusKm = 10);
        Task<IEnumerable<Photo>> GetPhotosByCityAsync(string city);
        Task<IEnumerable<Photo>> GetPhotosByCountryAsync(string country);
        Task<IEnumerable<Photo>> SearchPhotosOnMapAsync(double? latitude, double? longitude, double? radiusKm, string? city, string? country, PhotoCategory? category);
    }
}