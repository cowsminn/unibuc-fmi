using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using PhotoShare.Models;
using PhotoShare.Services;

namespace PhotoShare.Pages
{
    public class GalleryModel : PageModel
    {
        private readonly IPhotoService _photoService;

        public GalleryModel(IPhotoService photoService)
        {
            _photoService = photoService;
        }

        public IEnumerable<Photo> Photos { get; set; } = new List<Photo>();
        public PhotoCategory? SelectedCategory { get; set; }

        public async Task OnGetAsync(int? category)
        {
            if (category.HasValue && Enum.IsDefined(typeof(PhotoCategory), category.Value))
            {
                SelectedCategory = (PhotoCategory)category.Value;
                Photos = await _photoService.GetPhotosByCategoryAsync(SelectedCategory.Value);
            }
            else
            {
                Photos = await _photoService.GetAllPhotosAsync();
            }
        }
    }
}