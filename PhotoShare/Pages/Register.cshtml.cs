using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using PhotoShare.Services;
using PhotoShare.ViewModels;
using PhotoShare.Extensions;

namespace PhotoShare.Pages
{
    public class RegisterModel : PageModel
    {
        private readonly IUserService _userService;

        public RegisterModel(IUserService userService)
        {
            _userService = userService;
        }

        [BindProperty]
        public RegisterViewModel Input { get; set; } = new();

        public string? ErrorMessage { get; set; }

        public IActionResult OnGet()
        {
            if (HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Index");
            }
            return Page();
        }

        public async Task<IActionResult> OnPostAsync()
        {
            if (!ModelState.IsValid)
            {
                return Page();
            }

            // Check if email already exists
            if (await _userService.EmailExistsAsync(Input.Email))
            {
                ErrorMessage = "Acest email este deja înregistrat!";
                return Page();
            }

            try
            {
                var user = await _userService.CreateUserAsync(Input.Name, Input.Email, Input.Password);

                // Auto login after registration
                HttpContext.Session.SetCurrentUserId(user.Id);
                HttpContext.Session.SetString("UserName", user.Name);
                HttpContext.Session.SetString("UserType", user.Type.ToString());

                return RedirectToPage("/Index");
            }
            catch (Exception)
            {
                ErrorMessage = "Eroare la crearea contului. Te rog încearcă din nou!";
                return Page();
            }
        }
    }
}