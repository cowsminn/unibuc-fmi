using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using PhotoShare.Services;
using PhotoShare.ViewModels;
using PhotoShare.Extensions;

namespace PhotoShare.Pages
{
    public class LoginModel : PageModel
    {
        private readonly IUserService _userService;

        public LoginModel(IUserService userService)
        {
            _userService = userService;
        }

        [BindProperty]
        public LoginViewModel Input { get; set; } = new();

        public string? ErrorMessage { get; set; }

        public IActionResult OnGet(string? returnUrl = null)
        {
            if (HttpContext.Session.IsLoggedIn())
            {
                return RedirectToPage("/Index");
            }

            Input.ReturnUrl = returnUrl;
            return Page();
        }

        public async Task<IActionResult> OnPostAsync()
        {
            if (!ModelState.IsValid)
            {
                return Page();
            }

            var isValid = await _userService.ValidatePasswordAsync(Input.Email, Input.Password);
            if (!isValid)
            {
                ErrorMessage = "Email sau parolă incorectă!";
                return Page();
            }

            var user = await _userService.GetByEmailAsync(Input.Email);
            if (user != null)
            {
                // Set session
                HttpContext.Session.SetCurrentUserId(user.Id);
                HttpContext.Session.SetString("UserName", user.Name);
                HttpContext.Session.SetString("UserType", user.Type.ToString());

                // Update last login
                await _userService.UpdateLastLoginAsync(user.Id);

                // Redirect
                if (!string.IsNullOrEmpty(Input.ReturnUrl) && Url.IsLocalUrl(Input.ReturnUrl))
                {
                    return Redirect(Input.ReturnUrl);
                }
                return RedirectToPage("/Index");
            }

            ErrorMessage = "Eroare la autentificare!";
            return Page();
        }
    }
}
