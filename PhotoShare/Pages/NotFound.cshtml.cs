using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;

namespace PhotoShare.Pages
{
    public class NotFoundModel : PageModel
    {
        public string? CustomMessage { get; set; }
        public string? RequestedPath { get; set; }

        public IActionResult OnGet(string? message = null, string? path = null)
        {
            CustomMessage = message;
            RequestedPath = path;
            
            // Set appropriate status code
            Response.StatusCode = 404;
            
            return Page();
        }
    }
}
