using PhotoShare.Models;
using System.ComponentModel.DataAnnotations;

namespace PhotoShare.ViewModels
{
    public class MapSearchViewModel
    {
        [Display(Name = "Latitudine")]
        public double? Latitude { get; set; }

        [Display(Name = "Longitudine")]
        public double? Longitude { get; set; }

        [Display(Name = "Raza (km)")]
        [Range(1, 1000, ErrorMessage = "Raza trebuie să fie între 1 și 1000 km")]
        public int Radius { get; set; } = 10;

        [Display(Name = "Oraș")]
        public string? City { get; set; }

        [Display(Name = "Țară")]
        public string? Country { get; set; }

        [Display(Name = "Categorie")]
        public PhotoCategory? Category { get; set; }
    }
}
