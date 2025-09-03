using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using Microsoft.EntityFrameworkCore;
using PhotoShare.Data;
using PhotoShare.Models;
using PhotoShare.ViewModels;

namespace PhotoShare.Pages
{
    public class MapSearchModel : PageModel
    {
        private readonly PhotoShareContext _context;

        public MapSearchModel(PhotoShareContext context)
        {
            _context = context;
        }

        [BindProperty]
        public MapSearchViewModel SearchInput { get; set; } = new();

        public List<Photo> Photos { get; set; } = new();
        public bool HasSearched { get; set; }

        public async Task<IActionResult> OnGetAsync(double? lat, double? lng, int? radius, string? city, string? country, PhotoCategory? category)
        {
            if (lat.HasValue || lng.HasValue || !string.IsNullOrEmpty(city) || !string.IsNullOrEmpty(country) || category.HasValue)
            {
                HasSearched = true;
                SearchInput.Latitude = lat;
                SearchInput.Longitude = lng;
                SearchInput.Radius = radius ?? 10;
                SearchInput.City = city;
                SearchInput.Country = country;
                SearchInput.Category = category;

                await SearchPhotosAsync();
            }

            return Page();
        }

        public async Task<IActionResult> OnPostAsync()
        {
            HasSearched = true;
            await SearchPhotosAsync();
            return Page();
        }

        public async Task<IActionResult> OnGetPhotosJsonAsync(double? lat, double? lng, int? radius, string? city, string? country, PhotoCategory? category)
        {
            var query = _context.Photos
                .Include(p => p.User)
                .Where(p => true);

            // Apply filters
            if (category.HasValue)
            {
                query = query.Where(p => p.Category == category);
            }

            if (!string.IsNullOrEmpty(city))
            {
                query = query.Where(p => p.City != null && p.City.Contains(city));
            }

            if (!string.IsNullOrEmpty(country))
            {
                query = query.Where(p => p.Country != null && p.Country.Contains(country));
            }

            // Geographic filter
            if (lat.HasValue && lng.HasValue && radius.HasValue)
            {
                var radiusKm = radius.Value;
                query = query.Where(p => p.Latitude.HasValue && p.Longitude.HasValue);
                // Simple distance calculation (not precise but works for demo)
                var photos = await query.ToListAsync();
                photos = photos.Where(p => CalculateDistance(lat.Value, lng.Value, p.Latitude!.Value, p.Longitude!.Value) <= radiusKm).ToList();
                
                var result = photos.Select(p => new
                {
                    id = p.Id,
                    title = p.Title,
                    fileName = p.FileName,
                    latitude = p.Latitude,
                    longitude = p.Longitude,
                    userName = p.User.Name,
                    category = p.Category.ToString(),
                    likes = p.Likes,
                    views = p.Views
                });

                return new JsonResult(result);
            }

            var allPhotos = await query.Take(50).ToListAsync();
            var jsonResult = allPhotos.Select(p => new
            {
                id = p.Id,
                title = p.Title,
                fileName = p.FileName,
                latitude = p.Latitude,
                longitude = p.Longitude,
                userName = p.User.Name,
                category = p.Category.ToString(),
                likes = p.Likes,
                views = p.Views
            });

            return new JsonResult(jsonResult);
        }

        private async Task SearchPhotosAsync()
        {
            var query = _context.Photos
                .Include(p => p.User)
                .Where(p => true);

            // Apply filters
            if (SearchInput.Category.HasValue)
            {
                query = query.Where(p => p.Category == SearchInput.Category);
            }

            if (!string.IsNullOrEmpty(SearchInput.City))
            {
                query = query.Where(p => p.City != null && p.City.Contains(SearchInput.City));
            }

            if (!string.IsNullOrEmpty(SearchInput.Country))
            {
                query = query.Where(p => p.Country != null && p.Country.Contains(SearchInput.Country));
            }

            // Geographic filter
            if (SearchInput.Latitude.HasValue && SearchInput.Longitude.HasValue)
            {
                query = query.Where(p => p.Latitude.HasValue && p.Longitude.HasValue);
                var allPhotos = await query.ToListAsync();
                Photos = allPhotos.Where(p => CalculateDistance(
                    SearchInput.Latitude.Value, 
                    SearchInput.Longitude.Value, 
                    p.Latitude!.Value, 
                    p.Longitude!.Value) <= SearchInput.Radius).ToList();
            }
            else
            {
                Photos = await query.Take(50).ToListAsync();
            }
        }

        private static double CalculateDistance(double lat1, double lon1, double lat2, double lon2)
        {
            var R = 6371; // Earth's radius in kilometers
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
            return degrees * (Math.PI / 180);
        }
    }
}
