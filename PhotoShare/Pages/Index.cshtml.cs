using Microsoft.AspNetCore.Mvc.RazorPages;
using PhotoShare.Models;
using PhotoShare.Services;

namespace PhotoShare.Pages
{
    public class IndexModel : PageModel
    {
        private readonly IPhotoService _photoService;

        public IndexModel(IPhotoService photoService)
        {
            _photoService = photoService;
        }

        public IEnumerable<Photo> PopularPhotos { get; set; } = new List<Photo>();

        public async Task OnGetAsync()
        {
            var allPhotos = await _photoService.GetAllPhotosAsync();
            PopularPhotos = allPhotos
                .OrderByDescending(p => p.Likes + p.Views)
                .Take(6)
                .ToList();
        }
    }
}