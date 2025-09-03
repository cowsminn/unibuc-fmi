using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using Microsoft.EntityFrameworkCore;
using PhotoShare.Data;
using PhotoShare.Models;
using PhotoShare.Extensions;

namespace PhotoShare.Pages
{
    public class ProfileModel : PageModel
    {
        private readonly PhotoShareContext _context;

        public ProfileModel(PhotoShareContext context)
        {
            _context = context;
        }

        public User UserProfile { get; set; } = null!;
        public List<Photo> UserPhotos { get; set; } = new();
        public int TotalLikes { get; set; }
        public int TotalViews { get; set; }
        public int TotalComments { get; set; }
        public bool IsOwnProfile { get; set; }

        [BindProperty]
        public string Bio { get; set; } = string.Empty;

        public string? SuccessMessage { get; set; }
        public string? ErrorMessage { get; set; }

        public async Task<IActionResult> OnGetAsync(int? id)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Login");
            }

            var currentUserId = HttpContext.Session.GetCurrentUserId()!.Value;
            var targetUserId = id ?? currentUserId;
            IsOwnProfile = (targetUserId == currentUserId);

            // Get user profile
            UserProfile = await _context.Users
                .FirstOrDefaultAsync(u => u.Id == targetUserId);

            if (UserProfile == null)
            {
                return RedirectToPage("/NotFound", new { 
                    message = $"Utilizatorul cu ID-ul {targetUserId} nu a fost găsit.",
                    path = $"/Profile/{targetUserId}"
                });
            }

            // Get user photos
            UserPhotos = await _context.Photos
                .Where(p => p.UserId == targetUserId)
                .OrderByDescending(p => p.UploadedAt)
                .ToListAsync();

            // Calculate statistics
            TotalLikes = UserPhotos.Sum(p => p.Likes);
            TotalViews = UserPhotos.Sum(p => p.Views);
            TotalComments = await _context.Comments
                .Where(c => UserPhotos.Select(p => p.Id).Contains(c.PhotoId) && c.IsApproved)
                .CountAsync();

            Bio = UserProfile.Bio ?? string.Empty;

            if (TempData["SuccessMessage"] != null)
            {
                SuccessMessage = TempData["SuccessMessage"].ToString();
            }

            if (TempData["ErrorMessage"] != null)
            {
                ErrorMessage = TempData["ErrorMessage"].ToString();
            }

            return Page();
        }

        public async Task<IActionResult> OnPostUpdateBioAsync()
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Login");
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;

            try
            {
                var user = await _context.Users.FindAsync(userId);
                if (user == null)
                {
                    TempData["ErrorMessage"] = "Utilizatorul nu a fost găsit!";
                    return RedirectToPage();
                }

                user.Bio = Bio?.Trim() ?? string.Empty;
                await _context.SaveChangesAsync();

                TempData["SuccessMessage"] = "Bio-ul a fost actualizat cu succes!";
            }
            catch (Exception)
            {
                TempData["ErrorMessage"] = "Eroare la actualizarea bio-ului!";
            }

            return RedirectToPage();
        }
    }
}