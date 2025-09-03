// Controllers/PhotosApiController.cs - ENHANCED WITH TOGGLE LIKE & ADMIN DELETE
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using PhotoShare.Data;
using PhotoShare.Models;
using PhotoShare.Services;
using PhotoShare.Extensions;

namespace PhotoShare.Controllers
{
    [ApiController]
    [Route("api/photos")]
    public class PhotosApiController : ControllerBase
    {
        private readonly IPhotoService _photoService;
        private readonly PhotoShareContext _context;
        private readonly ILogger<PhotosApiController> _logger;

        public PhotosApiController(IPhotoService photoService, PhotoShareContext context, ILogger<PhotosApiController> logger)
        {
            _photoService = photoService;
            _context = context;
            _logger = logger;
        }

        [HttpPost("{id}/like")]
        public async Task<IActionResult> ToggleLikePhoto(int id)
        {
            _logger.LogInformation("ToggleLikePhoto called for photo {PhotoId}", id);
            
            // Verifică dacă utilizatorul este autentificat
            if (!HttpContext.Session.IsLoggedIn())
            {
                _logger.LogWarning("User not authenticated for like request");
                return Unauthorized(new { success = false, message = "Trebuie sa te autentifici pentru a aprecia!" });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;
            _logger.LogInformation("User {UserId} attempting to toggle like for photo {PhotoId}", userId, id);

            try
            {
                // Verifică dacă poza există
                var photo = await _context.Photos.FindAsync(id);
                if (photo == null)
                {
                    _logger.LogWarning("Photo {PhotoId} not found", id);
                    return NotFound(new { success = false, message = "Poza nu a fost gasita!" });
                }

                // Verifică dacă utilizatorul a dat deja like
                var existingLike = await _context.PhotoLikes
                    .FirstOrDefaultAsync(pl => pl.PhotoId == id && pl.UserId == userId);

                bool isLiked;
                string message;

                if (existingLike != null)
                {
                    // RETRAGE LIKE-UL
                    _context.PhotoLikes.Remove(existingLike);
                    photo.Likes = Math.Max(0, photo.Likes - 1); // Evită valorile negative
                    isLiked = false;
                    message = "Like retras!";
                    
                    _logger.LogInformation("User {UserId} removed like from photo {PhotoId}", userId, id);
                }
                else
                {
                    // ADAUGĂ LIKE
                    var photoLike = new PhotoLike
                    {
                        PhotoId = id,
                        UserId = userId,
                        CreatedAt = DateTime.UtcNow
                    };

                    _context.PhotoLikes.Add(photoLike);
                    photo.Likes++;
                    isLiked = true;
                    message = "Poza apreciata!";
                    
                    _logger.LogInformation("User {UserId} added like to photo {PhotoId}", userId, id);
                }

                await _context.SaveChangesAsync();

                _logger.LogInformation("Successfully toggled like for photo {PhotoId} by user {UserId}. New count: {LikesCount}, IsLiked: {IsLiked}", 
                    id, userId, photo.Likes, isLiked);

                return Ok(new { 
                    success = true, 
                    message = message,
                    newLikesCount = photo.Likes,
                    isLiked = isLiked
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error toggling like for photo {PhotoId} by user {UserId}", id, userId);
                return StatusCode(500, new { success = false, message = "Eroare la apreciere. Te rog incearca din nou!" });
            }
        }

        [HttpPost("comments/{commentId}/like")]
        public async Task<IActionResult> ToggleLikeComment(int commentId)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return Unauthorized(new { success = false, message = "Trebuie sa te autentifici pentru a aprecia!" });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;

            try
            {
                // Verifică dacă comentariul există
                var comment = await _context.Comments.FindAsync(commentId);
                if (comment == null)
                {
                    return NotFound(new { success = false, message = "Comentariul nu a fost gasit!" });
                }

                // Verifică dacă a dat deja like
                var existingLike = await _context.CommentLikes
                    .FirstOrDefaultAsync(cl => cl.CommentId == commentId && cl.UserId == userId);

                bool isLiked;
                string message;

                if (existingLike != null)
                {
                    // RETRAGE LIKE-UL
                    _context.CommentLikes.Remove(existingLike);
                    comment.Likes = Math.Max(0, comment.Likes - 1);
                    isLiked = false;
                    message = "Like la comentariu retras!";
                }
                else
                {
                    // ADAUGĂ LIKE
                    var commentLike = new CommentLike
                    {
                        CommentId = commentId,
                        UserId = userId,
                        CreatedAt = DateTime.UtcNow
                    };

                    _context.CommentLikes.Add(commentLike);
                    comment.Likes++;
                    isLiked = true;
                    message = "Comentariu apreciat!";
                }
                
                await _context.SaveChangesAsync();

                return Ok(new { 
                    success = true, 
                    message = message, 
                    newLikes = comment.Likes,
                    isLiked = isLiked
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error toggling like for comment {CommentId} by user {UserId}", commentId, userId);
                return StatusCode(500, new { success = false, message = "Eroare la apreciere." });
            }
        }

        [HttpDelete("comments/{commentId}")]
        public async Task<IActionResult> DeleteComment(int commentId)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return Unauthorized(new { success = false, message = "Trebuie sa te autentifici!" });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;
            var userType = HttpContext.Session.GetString("UserType");

            try
            {
                var comment = await _context.Comments
                    .Include(c => c.User)
                    .FirstOrDefaultAsync(c => c.Id == commentId);

                if (comment == null)
                {
                    return NotFound(new { success = false, message = "Comentariul nu a fost gasit!" });
                }

                // Verifică permisiunile (autorul comentariului sau moderator)
                if (comment.UserId != userId && userType != "Moderator")
                {
                   return StatusCode(403, new { success = false, message = "Nu ai permisiunea sa stergi acest comentariu!" });
                }

                // Șterge like-urile la comentariu
                var commentLikes = await _context.CommentLikes
                    .Where(cl => cl.CommentId == commentId)
                    .ToListAsync();
                _context.CommentLikes.RemoveRange(commentLikes);

                // Șterge comentariul
                _context.Comments.Remove(comment);
                await _context.SaveChangesAsync();

                return Ok(new { success = true, message = "Comentariu sters cu succes!" });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error deleting comment {CommentId} by user {UserId}", commentId, userId);
                return StatusCode(500, new { success = false, message = "Eroare la stergere." });
            }
        }

        [HttpDelete("{id}")]
        public async Task<IActionResult> DeletePhoto(int id)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return Unauthorized(new { success = false, message = "Trebuie sa fii autentificat!" });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;
            var userType = HttpContext.Session.GetString("UserType");

            try
            {
                // Verifică dacă poza există
                var photo = await _context.Photos
                    .Include(p => p.User)
                    .FirstOrDefaultAsync(p => p.Id == id);

                if (photo == null)
                {
                    return NotFound(new { success = false, message = "Poza nu a fost gasita!" });
                }

                // Verifică permisiunile - ADMIN SAU OWNER
                bool canDelete = photo.UserId == userId || userType == "Moderator";
                
                if (!canDelete)
                {
                    _logger.LogWarning("User {UserId} (Type: {UserType}) attempted to delete photo {PhotoId} owned by {OwnerId}", 
                        userId, userType, id, photo.UserId);
                    return StatusCode(403, new { success = false, message = "Nu ai permisiunea sa stergi aceasta poza!" });
                }

                // Admin poate șterge orice poză
                if (userType == "Moderator")
                {
                    _logger.LogInformation("Moderator {UserId} deleting photo {PhotoId} owned by {OwnerId}", userId, id, photo.UserId);
                }

                var success = await _photoService.DeletePhotoAsync(id, photo.UserId);
                if (success)
                {
                    _logger.LogInformation("Photo {PhotoId} deleted successfully by user {UserId} (Type: {UserType})", id, userId, userType);
                    return Ok(new { success = true, message = "Poza stearsa cu succes!" });
                }
                else
                {
                    return BadRequest(new { success = false, message = "Eroare la stergerea pozei!" });
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error deleting photo {PhotoId} by user {UserId} (Type: {UserType})", id, userId, userType);
                return StatusCode(500, new { success = false, message = "Eroare la stergere." });
            }
        }

        [HttpGet("{id}/info")]
        public async Task<IActionResult> GetPhotoInfo(int id)
        {
            try
            {
                var photo = await _photoService.GetPhotoByIdAsync(id);
                if (photo == null)
                {
                    return NotFound(new { success = false, message = "Poza nu a fost găsită!" });
                }

                return Ok(new
                {
                    success = true,
                    data = new
                    {
                        id = photo.Id,
                        title = photo.Title,
                        description = photo.Description,
                        likes = photo.Likes,
                        views = photo.Views,
                        author = photo.User.Name,
                        uploadedAt = photo.UploadedAt,
                        category = photo.Category.ToString()
                    }
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting photo info for {PhotoId}", id);
                return StatusCode(500, new { success = false, message = "Eroare la încărcarea informațiilor." });
            }
        }

        // Endpoint pentru a verifica dacă user-ul a dat like la un comentariu
        [HttpGet("comments/{commentId}/like-status")]
        public async Task<IActionResult> GetCommentLikeStatus(int commentId)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return Ok(new { success = true, isLiked = false, likes = 0 });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;

            try
            {
                var comment = await _context.Comments.FindAsync(commentId);
                if (comment == null)
                {
                    return NotFound(new { success = false, message = "Comentariul nu a fost găsit!" });
                }

                var isLiked = await _context.CommentLikes
                    .AnyAsync(cl => cl.CommentId == commentId && cl.UserId == userId);

                return Ok(new { 
                    success = true, 
                    isLiked = isLiked,
                    likes = comment.Likes
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting like status for comment {CommentId}", commentId);
                return StatusCode(500, new { success = false, message = "Eroare la verificarea statusului." });
            }
        }

        // Endpoint pentru a verifica dacă user-ul a dat like la o poză
        [HttpGet("{id}/like-status")]
        public async Task<IActionResult> GetPhotoLikeStatus(int id)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return Ok(new { success = true, isLiked = false, likes = 0 });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;

            try
            {
                var photo = await _context.Photos.FindAsync(id);
                if (photo == null)
                {
                    return NotFound(new { success = false, message = "Poza nu a fost gasita!" });
                }

                var isLiked = await _context.PhotoLikes
                    .AnyAsync(pl => pl.PhotoId == id && pl.UserId == userId);

                return Ok(new { 
                    success = true, 
                    isLiked = isLiked,
                    likes = photo.Likes
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting like status for photo {PhotoId}", id);
                return StatusCode(500, new { success = false, message = "Eroare la verificarea statusului." });
            }
        }

        // ADDED: Endpoint pentru debug
        [HttpGet("{id}/likes")]
        public async Task<IActionResult> GetPhotoLikes(int id)
        {
            try
            {
                var likes = await _context.PhotoLikes
                    .Include(pl => pl.User)
                    .Where(pl => pl.PhotoId == id)
                    .Select(pl => new { 
                        userId = pl.UserId, 
                        userName = pl.User.Name, 
                        createdAt = pl.CreatedAt 
                    })
                    .ToListAsync();

                var photo = await _context.Photos.FindAsync(id);
                
                return Ok(new { 
                    success = true, 
                    photoLikes = photo?.Likes ?? 0,
                    actualLikes = likes.Count,
                    likes = likes 
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting likes for photo {PhotoId}", id);
                return StatusCode(500, new { success = false, message = "Eroare la încărcarea like-urilor." });
            }
        }

        [HttpPost("comments/{commentId}/approve")]
        public async Task<IActionResult> ApproveComment(int commentId)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return Unauthorized(new { success = false, message = "Trebuie sa te autentifici!" });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;

            try
            {
                var comment = await _context.Comments
                    .Include(c => c.Photo)
                    .FirstOrDefaultAsync(c => c.Id == commentId);

                if (comment == null)
                {
                    return NotFound(new { success = false, message = "Comentariul nu a fost gasit!" });
                }

                // Only photo owner can approve comments
                if (comment.Photo.UserId != userId)
                {
                    return StatusCode(403, new { success = false, message = "Nu ai permisiunea sa aprobi acest comentariu!" });
                }

                comment.IsApproved = true;
                await _context.SaveChangesAsync();

                return Ok(new { success = true, message = "Comentariu aprobat cu succes!" });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error approving comment {CommentId} by user {UserId}", commentId, userId);
                return StatusCode(500, new { success = false, message = "Eroare la aprobat." });
            }
        }

        [HttpPost("comments/{commentId}/reject")]
        public async Task<IActionResult> RejectComment(int commentId)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return Unauthorized(new { success = false, message = "Trebuie sa te autentifici!" });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;

            try
            {
                var comment = await _context.Comments
                    .Include(c => c.Photo)
                    .FirstOrDefaultAsync(c => c.Id == commentId);

                if (comment == null)
                {
                    return NotFound(new { success = false, message = "Comentariul nu a fost gasit!" });
                }

                // Only photo owner can reject comments
                if (comment.Photo.UserId != userId)
                {
                    return StatusCode(403, new { success = false, message = "Nu ai permisiunea sa respingi acest comentariu!" });
                }

                // Delete comment likes first
                var commentLikes = await _context.CommentLikes
                    .Where(cl => cl.CommentId == commentId)
                    .ToListAsync();
                _context.CommentLikes.RemoveRange(commentLikes);

                // Delete the comment
                _context.Comments.Remove(comment);
                await _context.SaveChangesAsync();

                return Ok(new { success = true, message = "Comentariu respins si sters!" });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error rejecting comment {CommentId} by user {UserId}", commentId, userId);
                return StatusCode(500, new { success = false, message = "Eroare la respingere." });
            }
        }

        [HttpPost("{photoId}/comments")]
        public async Task<IActionResult> AddComment(int photoId, [FromBody] AddCommentRequest request)
        {
            if (!HttpContext.Session.IsLoggedIn())
            {
                return Unauthorized(new { success = false, message = "Trebuie sa te autentifici!" });
            }

            if (string.IsNullOrWhiteSpace(request.Content))
            {
                return BadRequest(new { success = false, message = "Comentariul nu poate fi gol!" });
            }

            var userId = HttpContext.Session.GetCurrentUserId()!.Value;

            try
            {
                // Check if photo exists
                var photo = await _context.Photos.FindAsync(photoId);
                if (photo == null)
                {
                    return NotFound(new { success = false, message = "Poza nu a fost gasita!" });
                }

                // Anyone can comment, but approval depends on ownership
                bool isAutoApproved = photo.UserId == userId;

                var comment = new Comment
                {
                    PhotoId = photoId,
                    UserId = userId,
                    Content = request.Content.Trim(),
                    CreatedAt = DateTime.UtcNow,
                    IsApproved = isAutoApproved
                };

                _context.Comments.Add(comment);
                await _context.SaveChangesAsync();

                // Return the new comment data
                var user = await _context.Users.FindAsync(userId);
                return Ok(new
                {
                    success = true,
                    message = isAutoApproved ? "Comentariu adaugat cu succes!" : "Comentariu trimis pentru aprobare!",
                    comment = new
                    {
                        id = comment.Id,
                        content = comment.Content,
                        createdAt = comment.CreatedAt,
                        likes = 0,
                        isApproved = comment.IsApproved,
                        user = new { name = user?.Name }
                    }
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error adding comment to photo {PhotoId} by user {UserId}", photoId, userId);
                return StatusCode(500, new { success = false, message = "Eroare la adaugarea comentariului." });
            }
        }

    }

    public class AddCommentRequest
    {
        public string Content { get; set; } = string.Empty;
    }
}