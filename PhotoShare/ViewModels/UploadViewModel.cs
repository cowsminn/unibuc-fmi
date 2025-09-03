using PhotoShare.Models;
using System.ComponentModel.DataAnnotations;

namespace PhotoShare.ViewModels
{
    public class UploadViewModel
    {
        [Required(ErrorMessage = "Titlul este obligatoriu")]
        [MaxLength(200, ErrorMessage = "Titlul nu poate depăși 200 de caractere")]
        public string Title { get; set; } = string.Empty;

        [MaxLength(1000, ErrorMessage = "Descrierea nu poate depăși 1000 de caractere")]
        public string? Description { get; set; }

        [Required(ErrorMessage = "Categoria este obligatorie")]
        public PhotoCategory Category { get; set; }

        [Required(ErrorMessage = "Te rog selectează o imagine")]
        public IFormFile Photo { get; set; } = null!;
        
        // Location properties
        public double? Latitude { get; set; }
        public double? Longitude { get; set; }
        
        [MaxLength(255)]
        public string? LocationName { get; set; }
        
        [MaxLength(100)]
        public string? City { get; set; }
        
        [MaxLength(100)]
        public string? Country { get; set; }
        
        // Flag to indicate if image was edited in frontend
        public bool ImageWasEdited { get; set; } = false;
    }
}