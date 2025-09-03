using System.Diagnostics;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;

namespace PhotoShare.Pages;

[ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
[IgnoreAntiforgeryToken]
public class ErrorModel : PageModel
{
    public string? RequestId { get; set; }
    public int StatusCode { get; set; }
    public string? ErrorMessage { get; set; }

    public bool ShowRequestId => !string.IsNullOrEmpty(RequestId);

    private readonly ILogger<ErrorModel> _logger;

    public ErrorModel(ILogger<ErrorModel> logger)
    {
        _logger = logger;
    }

    public void OnGet(int? statusCode = null)
    {
        RequestId = Activity.Current?.Id ?? HttpContext.TraceIdentifier;
        StatusCode = statusCode ?? Response.StatusCode;

        ErrorMessage = StatusCode switch
        {
            404 => "Pagina pe care o căutați nu a fost găsită.",
            403 => "Nu aveți permisiunea să accesați această resursă.",
            500 => "A apărut o eroare internă pe server.",
            _ => "A apărut o eroare neașteptată."
        };

        _logger.LogWarning("Error page accessed with status code {StatusCode}", StatusCode);
    }
}

