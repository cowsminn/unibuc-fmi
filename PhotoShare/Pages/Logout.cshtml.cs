using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using PhotoShare.Extensions;

namespace PhotoShare.Pages
{
    public class LogoutModel : PageModel
    {
        private readonly ILogger<LogoutModel> _logger;

        public LogoutModel(ILogger<LogoutModel> logger)
        {
            _logger = logger;
        }

        public IActionResult OnGet()
        {
            _logger.LogInformation("Logout GET request - clearing session");
            
            // Clear all session data
            HttpContext.Session.Clear();
            
            _logger.LogInformation("Session cleared, redirecting to Index");
            TempData["Message"] = "Te-ai deconectat cu succes!";
            return RedirectToPage("/Index");
        }

        public IActionResult OnPost()
        {
            _logger.LogInformation("Logout POST request - User attempting to logout");
            
            var userId = HttpContext.Session.GetCurrentUserId();
            var userName = HttpContext.Session.GetString("UserName");
            
            _logger.LogInformation("Logging out user: ID={UserId}, Name={UserName}", userId, userName);
            
            // Clear all session data completely
            HttpContext.Session.Clear();
            
            // Also clear any cookies if they exist
            foreach (var cookie in Request.Cookies.Keys)
            {
                if (cookie.StartsWith("PhotoShare") || cookie.Contains("Session"))
                {
                    Response.Cookies.Delete(cookie);
                }
            }
            
            _logger.LogInformation("User logged out successfully, session cleared");
            
            TempData["Message"] = "Te-ai deconectat cu succes!";
            return RedirectToPage("/Index");
        }
    }
}