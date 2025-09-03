using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using Microsoft.EntityFrameworkCore;
using PhotoShare.Data;
using PhotoShare.Models;
using PhotoShare.Services;
using PhotoShare.Extensions;

namespace PhotoShare.Pages
{
    public class PhotoDetailsModel : PageModel
    {
        private readonly IPhotoService _photoService;
        private readonly PhotoShareContext _context;

        public PhotoDetailsModel(IPhotoService photoService, PhotoShareContext context)
        {
            _photoService = photoService;
            _context = context;
        }

        public Photo? Photo { get; set; }
        public int UserPhotosCount { get; set; }
        public int TotalLikes { get; set; }
        public Photo? PreviousPhoto { get; set; }
        public Photo? NextPhoto { get; set; }
        public List<Comment> PendingComments { get; set; } = new();

        public async Task<IActionResult> OnGetAsync(int id)
        {
            Photo = await _photoService.GetPhotoByIdAsync(id);
            
            if (Photo == null)
            {
                return RedirectToPage("/NotFound", new { 
                    message = $"Poza cu ID-ul {id} nu a fost găsită. Poate a fost ștearsă sau nu există.",
                    path = $"/Photo/{id}"
                });
            }

            // Increment views
            await _photoService.IncrementViewsAsync(id);

            // Get user stats
            var userPhotos = await _photoService.GetUserPhotosAsync(Photo.UserId);
            UserPhotosCount = userPhotos.Count();
            TotalLikes = userPhotos.Sum(p => p.Likes);

            // Get navigation photos
            var allPhotos = await _photoService.GetAllPhotosAsync();
            var photosList = allPhotos.ToList();
            var currentIndex = photosList.FindIndex(p => p.Id == id);
            
            if (currentIndex > 0)
                PreviousPhoto = photosList[currentIndex - 1];
            if (currentIndex < photosList.Count - 1)
                NextPhoto = photosList[currentIndex + 1];

            // Get pending comments if user is photo owner
            if (HttpContext.Session.IsLoggedIn() && HttpContext.Session.GetCurrentUserId() == Photo.UserId)
            {
                PendingComments = await _context.Comments
                    .Include(c => c.User)
                    .Where(c => c.PhotoId == id && !c.IsApproved)
                    .OrderBy(c => c.CreatedAt)
                    .ToListAsync();
            }

            return Page();
        }

        public async Task<IActionResult> OnPostAddCommentAsync(int photoId, string content)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Login");
            }

            if (string.IsNullOrWhiteSpace(content))
            {
                return RedirectToPage(new { id = photoId });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;
            
            // Check if the photo exists
            var photo = await _context.Photos.FindAsync(photoId);
            if (photo == null)
            {
                return NotFound();
            }
            
            // Determine if comment should be auto-approved (only owner's comments)
            bool isAutoApproved = photo.UserId == userId;
            
            var comment = new Comment
            {
                PhotoId = photoId,
                UserId = userId,
                Content = content.Trim(),
                CreatedAt = DateTime.UtcNow,
                IsApproved = isAutoApproved // Only auto-approve if it's the photo owner
            };

            _context.Comments.Add(comment);
            await _context.SaveChangesAsync();

            if (isAutoApproved)
            {
                TempData["Message"] = "Comentariu adăugat cu succes!";
            }
            else
            {
                TempData["Message"] = "Comentariu trimis pentru aprobare!";
            }

            return RedirectToPage(new { id = photoId });
        }

        public async Task<IActionResult> OnPostApproveCommentAsync(int commentId)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Login");
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;
            var comment = await _context.Comments
                .Include(c => c.Photo)
                .FirstOrDefaultAsync(c => c.Id == commentId);

            if (comment == null || comment.Photo.UserId != userId)
            {
                return NotFound();
            }

            comment.IsApproved = true;
            await _context.SaveChangesAsync();

            return RedirectToPage(new { id = comment.PhotoId });
        }

        public async Task<IActionResult> OnPostRejectCommentAsync(int commentId)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Login");
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;
            var comment = await _context.Comments
                .Include(c => c.Photo)
                .FirstOrDefaultAsync(c => c.Id == commentId);

            if (comment == null || comment.Photo.UserId != userId)
            {
                return NotFound();
            }

            // Delete comment and its likes
            var commentLikes = await _context.CommentLikes
                .Where(cl => cl.CommentId == commentId)
                .ToListAsync();
            _context.CommentLikes.RemoveRange(commentLikes);
            _context.Comments.Remove(comment);
            await _context.SaveChangesAsync();

            return RedirectToPage(new { id = comment.PhotoId });
        }
    }
}