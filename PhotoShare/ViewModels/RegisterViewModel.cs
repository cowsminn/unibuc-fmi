using System.ComponentModel.DataAnnotations;

namespace PhotoShare.ViewModels
{
    public class RegisterViewModel
    {
        [Required(ErrorMessage = "Numele este obligatoriu")]
        [MinLength(2, ErrorMessage = "Numele trebuie să aibă cel puțin 2 caractere")]
        [MaxLength(100, ErrorMessage = "Numele nu poate depăși 100 de caractere")]
        public string Name { get; set; } = string.Empty;

        [Required(ErrorMessage = "Email-ul este obligatoriu")]
        [EmailAddress(ErrorMessage = "Format email invalid")]
        public string Email { get; set; } = string.Empty;

        [Required(ErrorMessage = "Parola este obligatorie")]
        [MinLength(6, ErrorMessage = "Parola trebuie să aibă cel puțin 6 caractere")]
        public string Password { get; set; } = string.Empty;

        [Required(ErrorMessage = "Confirmarea parolei este obligatorie")]
        [Compare("Password", ErrorMessage = "Parolele nu se potrivesc")]
        public string ConfirmPassword { get; set; } = string.Empty;
    }
}