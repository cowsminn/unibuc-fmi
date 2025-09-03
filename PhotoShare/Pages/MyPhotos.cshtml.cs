using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using Microsoft.EntityFrameworkCore;
using PhotoShare.Data;
using PhotoShare.Models;
using PhotoShare.Services;
using PhotoShare.Extensions;

namespace PhotoShare.Pages
{
    public class MyPhotosModel : PageModel
    {
        private readonly IPhotoService _photoService;
        private readonly PhotoShareContext _context;

        public MyPhotosModel(IPhotoService photoService, PhotoShareContext context)
        {
            _photoService = photoService;
            _context = context;
        }

        public IEnumerable<Photo> Photos { get; set; } = new List<Photo>();
        public int TotalLikes { get; set; }
        public int TotalViews { get; set; }
        public int TotalComments { get; set; }



         public async Task<IActionResult> OnGetAsync()
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Login", new { returnUrl = "/MyPhotos" });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;
            Photos = await _photoService.GetUserPhotosAsync(userId);

            // Calculate stats
            TotalLikes = Photos.Sum(p => p.Likes);
            TotalViews = Photos.Sum(p => p.Views);
            TotalComments = Photos.Sum(p => p.Comments.Count);

            return Page();
        }

        public async Task<IActionResult> OnPostEditPhotoAsync(int photoId, string title, string description)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Login");
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;
            var photo = await _context.Photos.FirstOrDefaultAsync(p => p.Id == photoId && p.UserId == userId);

            if (photo == null)
            {
                return NotFound();
            }

            photo.Title = title?.Trim() ?? photo.Title;
            photo.Description = description?.Trim();

            await _context.SaveChangesAsync();

            return RedirectToPage();
        }
    }
}