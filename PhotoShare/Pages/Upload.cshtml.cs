using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using PhotoShare.Services;
using PhotoShare.ViewModels;
using PhotoShare.Extensions;

namespace PhotoShare.Pages
{
    public class UploadModel : PageModel
    {
        private readonly IPhotoService _photoService;
        private const long MaxFileSize = 10 * 1024 * 1024; // 10MB
        private readonly string[] AllowedExtensions = { ".jpg", ".jpeg", ".png", ".gif", ".webp" };

        public UploadModel(IPhotoService photoService)
        {
            _photoService = photoService;
        }

        [BindProperty]
        public UploadViewModel Input { get; set; } = new();

        public string? ErrorMessage { get; set; }
        public string? SuccessMessage { get; set; }

        public IActionResult OnGet()
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Login", new { returnUrl = "/Upload" });
            }
            return Page();
        }

        public async Task<IActionResult> OnPostAsync()
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Login");
            }

            if (!ModelState.IsValid)
            {
                return Page();
            }

            // Validate file
            if (Input.Photo == null || Input.Photo.Length == 0)
            {
                ErrorMessage = "Te rog selectează o imagine!";
                return Page();
            }

            if (Input.Photo.Length > MaxFileSize)
            {
                ErrorMessage = $"Fișierul este prea mare! Mărimea maximă permisă este {MaxFileSize / 1024 / 1024}MB.";
                return Page();
            }

            var extension = Path.GetExtension(Input.Photo.FileName).ToLowerInvariant();
            if (!AllowedExtensions.Contains(extension))
            {
                ErrorMessage = "Format de fișier neacceptat! Folosește JPG, PNG, GIF sau WebP.";
                return Page();
            }

            try
            {
                var userId = HttpContext.Session.GetCurrentUserId()!.Value;
                
                // Save the photo (edited or original)
                var photo = await _photoService.SavePhotoAsync(
                    Input.Photo,
                    Input.Title,
                    Input.Description ?? string.Empty,
                    Input.Category,
                    userId,
                    Input.Latitude,
                    Input.Longitude,
                    Input.LocationName,
                    Input.City,
                    Input.Country
                );

                // Log successful upload with edit status
                if (Input.ImageWasEdited)
                {
                    Console.WriteLine($"Edited photo uploaded: {photo.FileName} by user {userId}");
                }
                else
                {
                    Console.WriteLine($"Original photo uploaded: {photo.FileName} by user {userId}");
                }
                
                // Redirect to the photo page instead of staying on upload page
                TempData["SuccessMessage"] = Input.ImageWasEdited ? 
                    "Poza editată a fost încărcată cu succes!" : 
                    "Poza a fost încărcată cu succes!";
                
                return RedirectToPage("/Photo", new { id = photo.Id });
            }
            catch (Exception ex)
            {
                ErrorMessage = "A apărut o eroare la procesarea imaginii. Te rog încearcă din nou.";
                // Enhanced error logging
                Console.WriteLine($"Upload error: {ex.Message}");
                Console.WriteLine($"Image was edited: {Input.ImageWasEdited}");
                Console.WriteLine($"File size: {Input.Photo?.Length ?? 0} bytes");
                return Page();
            }
        }
    }
}